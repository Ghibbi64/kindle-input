#include <iostream>
#include <thread>
#include <atomic>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <poll.h>
#include <termios.h>
#include <cerrno>
#include "manage.h"

void stream_input_to_serial() {
    int fd_out = -1;

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

    int fd_in = open("/dev/input/event2", O_RDONLY);
    if (fd_in < 0) {
        std::cerr << "Failed to open /dev/input/event2\n";
        close(fd_out);
        return;
    }

    struct input_event ev;
    struct pollfd pfd;
    pfd.fd = fd_in;
    pfd.events = POLLIN;

    while (keep_streaming) {
        int poll_result = poll(&pfd, 1, 100);

        if (poll_result > 0 && (pfd.revents & POLLIN)) {
            if (read(fd_in, &ev, sizeof(struct input_event)) > 0) {

                uint8_t* ptr = reinterpret_cast<uint8_t*>(&ev);
                ssize_t bytes_left = sizeof(struct input_event);

                while (bytes_left > 0 && keep_streaming) {
                    ssize_t bytes_written = write(fd_out, ptr, bytes_left);

                    if (bytes_written <= 0) {
                        if (errno == EINTR) continue;
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        continue;
                    }

                    bytes_left -= bytes_written;
                    ptr += bytes_written;
                }
            }
        } else if (poll_result < 0) {
            if (errno != EINTR) break;
        }
    }

    close(fd_in);
    close(fd_out);
}

void stop_and_restore() {
    keep_streaming = false;

    if (stream_thread.joinable()) {
        stream_thread.join();
    }
}
