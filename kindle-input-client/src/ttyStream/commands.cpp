#include <iostream>
#include <thread>
#include <atomic>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <poll.h>
#include <termios.h>
#include <cerrno>
#include "commands.h"
#include "inputStream.h"

int fd_out;

void manage_commands() {
    fd_out = -1;

    for (int i = 0; i < 20 && keep_streaming; ++i) {
        fd_out = open("/dev/ttyGS0", O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd_out >= 0) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (fd_out < 0) {
        std::cerr << "Failed to open /dev/ttyGS0\n";
        return;
    }

    struct termios tty;
    if (tcgetattr(fd_out, &tty) == 0) {
        cfmakeraw(&tty);
        tcsetattr(fd_out, TCSANOW, &tty);
    }

    while (!close_connection) {

    }

    close(fd_out);
}
