#include <iostream>
#include <thread>
#include "platform/linux/connection.h"
#include "platform/linux/injector.h"
#include "input/handler.h"
#include "dependencies/tomlParser.h"

int main(){
    //Get config.toml
    toml::table config;
    getConfig(config);
    std::string ip = config["Network"]["ip_address_usb"].value_or("192.168.15.244"); // For now i'm connecting trough the usb
    int port = config["Network"]["ip_address_usb"].value_or(22);
    std::string ssh_user = config["Auth"]["ssh_user"].value_or("root");
    std::string ssh_password = config["Auth"]["ssh_password"].value_or("kindle");

    ssh::Session ssh_session;
    //Establish connection with the kindle and get the channel
    ssh::Channel* stream_channel = ssh_connect(ssh_session, ip, port, "root", "kindle");
    if(stream_channel==nullptr){
        std::cout<<"Unable to connect with the Kindle\n";
        delete stream_channel;
        return -1;
    }
    //Read the raw data stream and continue from there
    std::thread t1(handle, stream_channel);

    //Closing process
    t1.join();
    if(destroy_device()==-1) std::cout<<"Error while destroying the virtual tablet...\n";
    delete stream_channel;
}
