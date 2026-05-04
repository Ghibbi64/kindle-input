#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>

int open_ttyStream(){
    int tty_fd = open("/dev/ttyACM0", O_RDONLY | O_NOCTTY);
    if (tty_fd < 0) {
        std::cout<<"Failed to open /dev/ttyACM0\n";
        std::cout<<"Try disconnecting the kindle, pressing 'end' and 'start' on the kindle and reconnecting it\n";
        return -1;
    }
    struct termios tty;
    if (tcgetattr(tty_fd, &tty) != 0) {
        std::cout<<"Error getting TTY attributes\n";
        close(tty_fd);
        return -1;
    }
    cfmakeraw(&tty);
    if (tcsetattr(tty_fd, TCSANOW, &tty) != 0) {
        std::cout<<"Error setting TTY attributes\n";
        close(tty_fd);
        return -1;
    }
    return tty_fd;
}

void close_ttyStream(int tty_fd) {
    if (tty_fd >= 0) {
        close(tty_fd);
        // std::cout << "TTY stream closed successfully.\n";
    }
}
