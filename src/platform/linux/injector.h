#include <cstring>
#include <cstdint>
#include <libssh/libsshpp.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int setup_uinput_device();
int destroy_device();
void inject_event(int fd, uint16_t type, uint16_t code, int32_t value);
