#include <iostream>
#include <cstring> // Just for debugging
#include <libssh/libsshpp.hpp>
#include"connection.h"

//Establish ssh session and return usable stream channel
ssh::Channel* ssh_connect(ssh::Session& session, std::string ip, int port, std::string user, std::string password){
        try {
            session.setOption(SSH_OPTIONS_HOST, ip.c_str());
            session.setOption(SSH_OPTIONS_USER, user.c_str());
            session.setOption(SSH_OPTIONS_PORT, port);
            session.connect();
            std::cout << "Successfully connected to " << ip << std::endl;
            int auth_state = session.userauthPassword(password.c_str());
            if (auth_state != SSH_AUTH_SUCCESS) {
                std::cerr << "SSH Error: Authentication denied! Wrong password." << std::endl;
                return nullptr;
            }
            std::cout << "Successfully authenticated to " << ip << std::endl;
            ssh::Channel* channel = new ssh::Channel(session);
            channel->openSession();
            channel->requestExec("cat /dev/input/event2");
            return channel;
        }
        catch (ssh::SshException& e) {
            std::cerr << "SSH Error: " << e.getError() << std::endl;
            return nullptr;
        }
        return nullptr;
}
