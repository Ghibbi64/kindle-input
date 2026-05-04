/*
Just a bit of abstraction.
This is an implementation of the base functions to control usbnetLite directly from the app. Onestly it's better to have a granular control
from here instead of just making the user switching mode every time in KUAL.
It edit the config file under USBNET_CONFIG_PATH to switch mode (wifi/usb) and then call the script in USBNET_SCRIPT_PATH to toggle the changes.
It's also used to enable disable the ssh server.
*/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

const std::string USBNET_CONFIG_PATH = "/mnt/us/usbnetlite/etc/config";
const std::string USBNET_SCRIPT_PATH = "/mnt/us/usbnetlite/bin/usbnetwork";

bool setUsbNetConfigValue(const std::string& key, const std::string& newValue) {
    std::vector<std::string> lines;
    std::string line;
    bool keyFound = false;

    std::ifstream inFile(USBNET_CONFIG_PATH);
    if (!inFile.is_open()) return false;

    while (std::getline(inFile, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back(); //strip \r

        if (line.find(key + "=") == 0) {
            lines.push_back(key + "=" + newValue);
            keyFound = true;
        } else {
            lines.push_back(line);
        }
    }
    inFile.close();

    std::ofstream outFile(USBNET_CONFIG_PATH, std::ios::binary | std::ios::trunc);
    if (!outFile.is_open()) return false;

    for (const auto& l : lines) {
        outFile << l << "\n";
    }
    return keyFound;
}

bool isUsbNetworkActive() {
    return (std::system("lsmod | grep -q g_ether") == 0);
}

void enableUsbNetwork() {
    if (!isUsbNetworkActive()) {
        std::cout << "Starting USBNetlite..." << std::endl;
        std::system(USBNET_SCRIPT_PATH.c_str());
    }
}

void disableUsbNetwork() {
    if (isUsbNetworkActive()) {
        std::cout << "Stopping USBNetlite..." << std::endl;
        std::string cmd = USBNET_SCRIPT_PATH + " usbms";
        std::system(cmd.c_str());
    }
}

void restartUsbNetwork(){
    disableUsbNetwork();
    std::system("sleep 2");
    enableUsbNetwork();
}

void setUsbNetworkMode(bool useWifi) {
    std::cout << "Switching mode to " << (useWifi ? "Wi-Fi" : "USB") << " mode..." << std::endl;

    setUsbNetConfigValue("USE_WIFI", useWifi ? "\"true\"" : "\"false\"");

    if (isUsbNetworkActive()) {
        std::cout << "Restarting daemon, please wait..." << std::endl;
        restartUsbNetwork();
    }
}
