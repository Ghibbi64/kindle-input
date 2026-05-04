#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <poll.h>
#include <termios.h>
#include "setupUsb.h"

std::string old_pid = "";
bool isMtp = true;

void savePID(){
    if(old_pid==""){
        std::ifstream pid_file("/sys/kernel/config/usb_gadget/mtpgadget/idProduct");

        if (pid_file.is_open()) {
            std::getline(pid_file, old_pid);
            pid_file.close();
            std::cout<<"Successfully saved old PID: "<<old_pid<<"\n";
        }else{
            old_pid =  "0xa4a2"; //This is the one from my Scribe, worst case you need to reboot
        }
    }
}

void usbmode_serial(){
    system("echo \"\" > /sys/kernel/config/usb_gadget/mtpgadget/UDC 2>/dev/null");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    system("killall mtp-responder >/dev/null 2>&1");
    system("rm -f /sys/kernel/config/usb_gadget/mtpgadget/configs/c.1/ffs.mtp 2>/dev/null");
    system("echo 0x02 > /sys/kernel/config/usb_gadget/mtpgadget/bDeviceClass 2>/dev/null");
    savePID();
    system("echo 0x9999 > /sys/kernel/config/usb_gadget/mtpgadget/idProduct 2>/dev/null");
    system("mkdir -p /sys/kernel/config/usb_gadget/mtpgadget/functions/acm.0 2>/dev/null");
    system("ln -sf /sys/kernel/config/usb_gadget/mtpgadget/functions/acm.0 /sys/kernel/config/usb_gadget/mtpgadget/configs/c.1/ 2>/dev/null");
    system("ls /sys/class/udc/ | head -n 1 > /sys/kernel/config/usb_gadget/mtpgadget/UDC 2>/dev/null");
    isMtp = false;
}

void usbmode_mtp(){
    if(!isMtp){
        system("echo \"\" > /sys/kernel/config/usb_gadget/mtpgadget/UDC 2>/dev/null");
        system("rm -f /sys/kernel/config/usb_gadget/mtpgadget/configs/c.1/acm.* 2>/dev/null");
        std::string command = "echo " + old_pid + " > /sys/kernel/config/usb_gadget/mtpgadget/idProduct 2>/dev/null";
        system(command.c_str());
        system("echo 0x00 > /sys/kernel/config/usb_gadget/mtpgadget/bDeviceClass 2>/dev/null");
        system("ln -sf /sys/kernel/config/usb_gadget/mtpgadget/functions/ffs.mtp /sys/kernel/config/usb_gadget/mtpgadget/configs/c.1/ 2>/dev/null");
        system("killall getty agetty >/dev/null 2>&1");
        system("ls /sys/class/udc/ | head -n 1 > /sys/kernel/config/usb_gadget/mtpgadget/UDC 2>/dev/null");
        isMtp = true;
    }
}
