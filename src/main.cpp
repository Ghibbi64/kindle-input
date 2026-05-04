#include <iostream>
#include <thread>
#include "platform/linux/sshConnect.h"
#include "platform/linux/injector.h"
#include "input/handler.h"
#include "dependencies/tomlParser.h"
#include "platform/linux/ttyStream.h"
#include "platform/linux/generic.h"

int main(){
    signal(SIGINT, signalHandler);

   toml::table config;
    getConfig(config);
    std::string input_mode = config["Tablet_calibration"]["input_mode"].value_or("tablet");
    // Default tablet even if mode is incorrect
    if(input_mode == "mouse")
        switch_input_mode(1);
    else
        switch_input_mode(0);
    // Open the tty stream
    int tty_fd = open_ttyStream();
    if(tty_fd!=-1){
        std::thread t1(handle, tty_fd);

        while(!close_program){}
        pthread_cancel(t1.native_handle());
        t1.join();
        std::cout<<"Closing tty stream...\n";
        close_ttyStream(tty_fd);
        std::cout<<"Destroying virtual tablet...\n";
        switch(destroy_device()){
            case -1: std::cout<<"Error while destroying the virtual tablet\n"; break;
            case -2: std::cout<<"Virtual tablet not found\n"; break;
        }
    }
}
