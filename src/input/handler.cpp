#include <iostream>
#include <cstring>
#include <cstdint>
#include <libssh/libsshpp.hpp>
#include "handler.h"
#include "../platform/linux/injector.h"
#include "../dependencies/tomlParser.h"

// Linux Input Event Constants
const uint16_t EV_SYN = 0x00;
const uint16_t EV_KEY = 0x01;
const uint16_t EV_ABS = 0x03;

const uint16_t ABS_X = 0x00;
const uint16_t ABS_Y = 0x01;
const uint16_t ABS_PRESSURE = 0x18;
const uint16_t ABS_DISTANCE = 0x19;
const uint16_t BTN_TOUCH = 0x14a;

void handle(ssh::Channel* channel) {
    toml::table config;
    getConfig(config);
    int pen_compensation = config["Tablet_calibration"]["pen_compensation"].value_or(400);
    bool pen_override = config["Tablet_calibration"]["pen_override"].value_or(false);

    int uinputid = setup_uinput_device(); // This register the virtual tablet

    uint8_t buffer[16];
    int total_bytes_read = 0;

    bool tool_proximity = false;
    bool initialized = false;

    std::cout<<"Starting receiving data\n";

    while (true) {
        // Every event is 16 bytes
        int bytes_to_read = 16 - total_bytes_read;
        int bytes_read = channel->read(buffer + total_bytes_read, bytes_to_read, 0);

        // libssh is fucking hell, "oh yea i didn't receive the fucking packet in 0.000000001 ms, i will surely fuck up everything" -_-
        if (bytes_read == SSH_AGAIN || bytes_read == -2) {
            continue;
        }
        if (bytes_read < 0) {
            ssh_session c_session = ssh_channel_get_session(channel->getCChannel());
            std::cerr << "\nSSH error: " << ssh_get_error(c_session) <<"\n";
            return;
        }
        if (bytes_read == 0) {
            std::cout << "\nStream ended. Connection closed by Kindle.\n";
            return;
        }
        total_bytes_read += bytes_read;

        // Once full packet is received continue
        if (total_bytes_read == 16) {
            uint32_t tv_sec;
            uint32_t tv_usec;
            uint16_t ev_type;
            uint16_t ev_code;
            int32_t ev_value;

            std::memcpy(&tv_sec, buffer, 4);
            std::memcpy(&tv_usec, buffer + 4, 4);
            std::memcpy(&ev_type, buffer + 8, 2);
            std::memcpy(&ev_code, buffer + 10, 2);
            std::memcpy(&ev_value, buffer + 12, 4);

            if (ev_type != EV_SYN && ev_type != EV_KEY && ev_type != EV_ABS) {
                std::memmove(buffer, buffer + 1, 15);
                total_bytes_read = 15;
                continue;
            }

            /*This is to let the kernel know a stylus has been detected and to start using the virtual device
            This needed to be made because otherways the kernel would reject the virtual tablet immediately because it expected
            events packet right away*/
            if (!initialized) {
                inject_event(uinputid, EV_KEY, 0x140, 1);
                inject_event(uinputid, EV_SYN, 0, 0);
                tool_proximity = true;
                initialized = true;
            }

            if (ev_type == EV_KEY && ev_code == BTN_TOUCH) {
                inject_event(uinputid, EV_KEY, BTN_TOUCH, ev_value);
            }
            else if (ev_type == EV_KEY && ev_code == 0x140) { // 0x140 is BTN_TOOL_PEN
                tool_proximity = (ev_value == 1);
                if (tool_proximity) {
                    //std::cout << "Pen detected" << std::endl;
                } else {
                    //std::cout << "Pen removed" << std::endl;
                    inject_event(uinputid, EV_KEY, BTN_TOUCH, 0);
                }
                inject_event(uinputid, EV_KEY, 0x140, ev_value);
            }
            else if(ev_type == EV_ABS){
                uint16_t mapped_code = ev_code;
                uint16_t mapped_value = ev_value;

                //If pen_override is true the pressure will always be 0
                if(pen_override && ev_code == ABS_PRESSURE){
                    mapped_value = 0;
                }

                //Little fix because the base pressure it too small in my opinion (this can be set in the config)
                if(ev_code == ABS_PRESSURE){
                    if(mapped_value>20) mapped_value+=pen_compensation;
                    if(mapped_value>4095) mapped_value = 4095;
                }
                //This is for inverting the the coordinates bc we want the kindle to be used in landscape
                if(ev_code == ABS_Y){
                    mapped_code = ABS_X;
                    mapped_value = 20720 - ev_value;
                    //std::cout<<"COORDINATA X: "<<mapped_value<<"\t";
                }
                if(ev_code == ABS_X){
                    mapped_code = ABS_Y;
                    mapped_value = ev_value;
                    //std::cout<<"COORDINATA Y: "<<mapped_value<<"\n";
                }
                inject_event(uinputid, EV_ABS, mapped_code, mapped_value);
            }
            else {
                inject_event(uinputid, ev_type, ev_code, ev_value);
            }
        total_bytes_read = 0;
        }
    }
}
