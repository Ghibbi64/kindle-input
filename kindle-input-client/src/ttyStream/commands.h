#include <thread>
#include <atomic>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <poll.h>
#include <termios.h>

inline std::atomic<bool> keep_running{false};
inline std::thread command_thread;
