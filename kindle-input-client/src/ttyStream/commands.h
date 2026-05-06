#include <thread>
#include <atomic>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <poll.h>
#include <termios.h>

inline std::atomic<bool> close_connection{false};
inline std::thread command_thread;

void manage_commands();
