#include <thread>
#include <atomic>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <poll.h>
#include <termios.h>

inline std::atomic<bool> keep_streaming{false};
inline std::thread stream_thread;

void stream_input_to_serial();
void stop_and_restore();
