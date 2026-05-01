#include <cstring>
#include <cstdint>
#include <cerrno>
#include <libssh/libsshpp.hpp>
#include <linux/uinput.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "injector.h"
#include "../../dependencies/tomlParser.h"

int uinputid;

int setup_uinput_device() {
    // Get config data for calibration
    toml::table config;
    getConfig(config);

    // Calculate MAX and MIN absolute position based on the area percentage in the config
    int x_max = config["x_max"].value_or(20720);
    int y_max = config["y_max"].value_or(15720);
    float x_axis_area = (config["Tablet_calibration"]["x_axis_area"].value_or(100))/100.0;
    float y_axis_area = (config["Tablet_calibration"]["y_axis_area"].value_or(100))/100.0;
    int x_area = x_max*x_axis_area;
    int y_area = y_max*y_axis_area;
    int x_min = (x_max/2)-(x_area/2);
    int y_min = (y_max/2)-(y_area/2);
    x_max -= x_min;
    y_max -= y_min;
    //std::cout<<x_axis_area<<" "<<y_axis_area<<" "<<x_min<<" "<<y_min<<" "<<x_max<<" "<<y_max<<"\n";

    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) return -1;

    // Enable Pen, Absolute coordinates, and Pressure
    ioctl(fd, UI_SET_EVBIT, EV_SYN);
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_TOUCH);
    ioctl(fd, UI_SET_KEYBIT, BTN_STYLUS);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE);
    ioctl(fd, UI_SET_KEYBIT, 0x14c);
    ioctl(fd, UI_SET_KEYBIT, 0x140);
    ioctl(fd, UI_SET_KEYBIT, 0x141);
    ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);

    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    ioctl(fd, UI_SET_ABSBIT, ABS_X);
    ioctl(fd, UI_SET_ABSBIT, ABS_Y);
    ioctl(fd, UI_SET_ABSBIT, ABS_PRESSURE);
    ioctl(fd, UI_SET_ABSBIT, ABS_DISTANCE);

    // X Axis
    struct uinput_abs_setup x_setup = {0};
    x_setup.code = ABS_X;
    x_setup.absinfo.minimum = x_min;
    x_setup.absinfo.maximum = x_max;//20720 MAX
    x_setup.absinfo.resolution = 100;
    ioctl(fd, UI_ABS_SETUP, &x_setup);

    // Y Axis
    struct uinput_abs_setup y_setup = {0};
    y_setup.code = ABS_Y;
    y_setup.absinfo.minimum = y_min;
    y_setup.absinfo.maximum = y_max;//15720 MAX
    y_setup.absinfo.resolution = 100;
    ioctl(fd, UI_ABS_SETUP, &y_setup);

    struct uinput_abs_setup p_setup = {0};
    p_setup.code = ABS_PRESSURE;
    p_setup.absinfo.minimum = 0;
    p_setup.absinfo.maximum = 4095; //4095 MAX
    ioctl(fd, UI_ABS_SETUP, &p_setup);

    struct uinput_abs_setup d_setup = {0};
    d_setup.code = ABS_DISTANCE;
    d_setup.absinfo.minimum = 0;
    d_setup.absinfo.maximum = 255;
    ioctl(fd, UI_ABS_SETUP, &d_setup);

    struct uinput_setup usetup = {0};
    strcpy(usetup.name, "Kindle Scribe Tablet");
    usetup.id.bustype = BUS_USB;

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    std::cout<<"Virtual device initialized\n";
    uinputid = fd;
    return fd;
}

int destroy_device() {
    if (uinputid >= 0) {
        bool error = false;
        if (ioctl(uinputid, UI_DEV_DESTROY) < 0) {
            std::cerr << "Failed to destroy uinput device: " << std::strerror(errno) << "\n";
            error = true;
        }
        if (close(uinputid) < 0) {
            std::cerr << "Failed to close file descriptor: " << std::strerror(errno) << "\n";
            error = true;
        }
        uinputid = -1;
        if(error) return -1;

        return 0;
    }
    return -2;
}

void inject_event(int fd, uint16_t type, uint16_t code, int32_t value) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.code = code;
    ev.value = value;
    write(fd, &ev, sizeof(ev));
}
