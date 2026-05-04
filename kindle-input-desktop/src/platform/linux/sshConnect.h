#include <iostream>
#include <cstdint>
#include <cstring>
#include <libssh/libsshpp.hpp>

struct kindle_input_event {
    uint32_t tv_sec;
    uint32_t tv_usec;
    uint16_t type;
    uint16_t code;
    int32_t value;
};

ssh::Channel* ssh_connect(ssh::Session& session, std::string ip, int port, std::string user, std::string password);
