#This is slop i know

#!/bin/bash
echo "Configuring Kindle Scribe USB Connection..."

# 1. Find the interface using Amazon's hardware Vendor ID
KINDLE_IFACE=""
for syspath in /sys/class/net/*; do
    iface=$(basename "$syspath")

    # Skip loopback interface
    if [ "$iface" = "lo" ]; then continue; fi

    # Search the device tree for Amazon's Vendor ID (1949)
    # Search the device tree for the Kindle's USBNet Vendor ID (0525)
    if grep -q "0525" "$syspath/device/idVendor" 2>/dev/null || \
        grep -q "0525" "$syspath/device/../idVendor" 2>/dev/null || \
        grep -q "0525" "$syspath/device/../../idVendor" 2>/dev/null; then
        KINDLE_IFACE=$iface
        break
    fi
done

if [ -z "$KINDLE_IFACE" ]; then
    echo "Error: Could not find a plugged-in Amazon device (Vendor ID: 1949)."
    exit 1
fi

echo "Found Kindle securely on interface: $KINDLE_IFACE"

# 2. Check for NetworkManager
if command -v nmcli &> /dev/null; then
    echo "NetworkManager detected. Building nmcli profile..."

    if nmcli connection show "Kindle_Scribe_USB" > /dev/null 2>&1; then
        sudo nmcli connection delete "Kindle_Scribe_USB" > /dev/null
    fi

    sudo nmcli connection add \
        type ethernet \
        ifname "$KINDLE_IFACE" \
        con-name "Kindle_Scribe_USB" \
        ipv4.method manual \
        ipv4.addresses 192.168.15.201/24 \
        ipv4.never-default yes \
        > /dev/null

    sudo nmcli connection up "Kindle_Scribe_USB" > /dev/null

else
    echo "No NetworkManager found. Building universal udev rule..."

    # Create a lightweight trigger script
    TRIGGER_SCRIPT="/usr/local/bin/kindle_usbnet_fix.sh"
    sudo tee $TRIGGER_SCRIPT > /dev/null << 'EOF'
#!/bin/bash
# Wait 1 second for the kernel to finish registering the interface
sleep 1
IP_CMD="/usr/bin/ip"
$IP_CMD addr flush dev $1
$IP_CMD route del default dev $1 2>/dev/null
$IP_CMD addr add 192.168.15.201/24 dev $1
$IP_CMD link set $1 up
EOF

    # Make the script executable
    sudo chmod +x $TRIGGER_SCRIPT

    # Create the udev rule
    # ATTRS{idVendor}=="1949" ensures this only fires for Amazon hardware
    UDEV_RULE="/etc/udev/rules.d/99-kindle-scribe.rules"
    sudo tee $UDEV_RULE > /dev/null << EOF
ACTION=="add", SUBSYSTEM=="net", ATTRS{idVendor}=="0525", RUN+="$TRIGGER_SCRIPT %k"
EOF

    sudo udevadm control --reload-rules

    sudo $TRIGGER_SCRIPT $KINDLE_IFACE
fi

echo "Done"
