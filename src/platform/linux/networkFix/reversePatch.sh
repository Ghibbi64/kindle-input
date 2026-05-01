#This is slop i know

#!/bin/bash
echo "Reverting Kindle Scribe USB configuration..."

# 1. Clean up NetworkManager (if it exists)
if command -v nmcli &> /dev/null; then
    if nmcli connection show "Kindle_Scribe_USB" > /dev/null 2>&1; then
        echo "Removing NetworkManager profile 'Kindle_Scribe_USB'..."
        sudo nmcli connection delete "Kindle_Scribe_USB" > /dev/null
    else
        echo "No NetworkManager profile found."
    fi
fi

# 2. Clean up udev rules and the trigger script
TRIGGER_SCRIPT="/usr/local/bin/kindle_usbnet_fix.sh"
UDEV_RULE="/etc/udev/rules.d/99-kindle-scribe.rules"
UDEV_RELOAD_NEEDED=false

if [ -f "$TRIGGER_SCRIPT" ]; then
    echo "Removing udev trigger script..."
    sudo rm -f "$TRIGGER_SCRIPT"
fi

if [ -f "$UDEV_RULE" ]; then
    echo "Removing Kindle udev rule..."
    sudo rm -f "$UDEV_RULE"
    UDEV_RELOAD_NEEDED=true
fi

# Reload the kernel's device manager if we deleted a rule
if [ "$UDEV_RELOAD_NEEDED" = true ]; then
    echo "Reloading udev rules..."
    sudo udevadm control --reload-rules
fi

# 3. Securely find the Kindle interface to clean up the live connection
KINDLE_IFACE=""
for syspath in /sys/class/net/*; do
    iface=$(basename "$syspath")

    # Skip loopback
    if [ "$iface" = "lo" ]; then continue; fi

    # Verify it's an Amazon device (Vendor ID 1949) before touching it
    if grep -q "0525" "$syspath/device/idVendor" 2>/dev/null || \
        grep -q "0525" "$syspath/device/../idVendor" 2>/dev/null || \
        grep -q "0525" "$syspath/device/../../idVendor" 2>/dev/null; then
        KINDLE_IFACE=$iface
        break
    fi
done

if [ -n "$KINDLE_IFACE" ]; then
    echo "Kindle is actively plugged in. Flushing the manual IP from $KINDLE_IFACE..."
    sudo ip addr flush dev "$KINDLE_IFACE" 2>/dev/null

    # If NetworkManager is running, tell it to take control of the port again
    if command -v nmcli &> /dev/null; then
        sudo nmcli device set "$KINDLE_IFACE" managed yes 2>/dev/null
    fi
else
    echo "No active Kindle connection found to flush (this is fine if it's unplugged)."
fi

echo "Done"
