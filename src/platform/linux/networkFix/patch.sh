#!/bin/bash
# patch.sh - Configure Kindle Scribe USB network connection

echo "Configuring Kindle Scribe USB connection..."

# 1. Find the Kindle interface by USB Vendor ID (0525 = Linux USB gadget/Netchip)
KINDLE_IFACE=""
for syspath in /sys/class/net/*; do
    iface=$(basename "$syspath")
    [ "$iface" = "lo" ] && continue

    if grep -q "0525" "$syspath/device/idVendor" 2>/dev/null || \
       grep -q "0525" "$syspath/device/../idVendor" 2>/dev/null || \
       grep -q "0525" "$syspath/device/../../idVendor" 2>/dev/null; then
        KINDLE_IFACE=$iface
        break
    fi
done

if [ -z "$KINDLE_IFACE" ]; then
    echo "Error: Could not find a plugged-in Kindle (USB Vendor ID: 0525). Is it connected and in USB network mode?"
    exit 1
fi

echo "Found Kindle on interface: $KINDLE_IFACE"

# 2. Grab the MAC address now — this is what we'll use to identify the device
# permanently, so the profile survives interface renames across reconnects
KINDLE_MAC=$(cat /sys/class/net/$KINDLE_IFACE/address)
echo "Kindle MAC address: $KINDLE_MAC"

# 3. Configure the connection
if command -v nmcli &> /dev/null; then
    echo "NetworkManager detected. Building nmcli profile..."

    # Remove any existing profile to start clean
    if nmcli connection show "Kindle_Scribe_USB" > /dev/null 2>&1; then
        sudo nmcli connection delete "Kindle_Scribe_USB" > /dev/null
    fi

    sudo nmcli connection add \
        type ethernet \
        ifname "*" \
        con-name "Kindle_Scribe_USB" \
        ethernet.mac-address "$KINDLE_MAC" \
        ipv4.method manual \
        ipv4.addresses 192.168.15.201/24 \
        ipv4.never-default yes \
        > /dev/null

    sudo nmcli connection up "Kindle_Scribe_USB" > /dev/null
    echo "NetworkManager profile created and activated."

else
    echo "No NetworkManager found. Building udev rule..."

    TRIGGER_SCRIPT="/usr/local/bin/kindle_usbnet_fix.sh"
    sudo tee $TRIGGER_SCRIPT > /dev/null << 'EOF'
#!/bin/bash
# Triggered by udev on Kindle connect. $1 is the interface name.
sleep 1
IP_CMD="/usr/bin/ip"
$IP_CMD addr flush dev $1
$IP_CMD route del default dev $1 2>/dev/null
$IP_CMD addr add 192.168.15.201/24 dev $1
$IP_CMD link set $1 up
EOF

    sudo chmod +x $TRIGGER_SCRIPT

    UDEV_RULE="/etc/udev/rules.d/99-kindle-scribe.rules"
    sudo tee $UDEV_RULE > /dev/null << EOF
ACTION=="add", SUBSYSTEM=="net", ATTRS{idVendor}=="0525", RUN+="$TRIGGER_SCRIPT %k"
EOF

    sudo udevadm control --reload-rules
    sudo $TRIGGER_SCRIPT $KINDLE_IFACE
    echo "udev rule installed and connection applied."
fi

echo "Done"
