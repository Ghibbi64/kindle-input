#include <iostream>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <poll.h>
#include <termios.h>

void usbmode_serial();
void usbmode_mtp();