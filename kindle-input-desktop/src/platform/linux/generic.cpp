#include <filesystem>
#include <cstdlib>
#include <sys/stat.h>
#include <iostream>
#include "../../dependencies/tomlParser.h"
#include "generic.h"

bool close_program = false;

void signalHandler(int sig){
    close_program = true;
}

const char* enable_script =
    "#!/bin/bash\n"
    "echo 'SUBSYSTEM==\"input\", ATTRS{name}==\"Kindle Scribe Tablet\", ENV{ID_INPUT_TABLET}=\"0\", ENV{ID_INPUT_MOUSE}=\"1\"' > /etc/udev/rules.d/99-kindle-tablet.rules\n"
    "udevadm control --reload-rules\n";

const char* disable_script =
    "#!/bin/bash\n"
    "rm /etc/udev/rules.d/99-kindle-tablet.rules\n"
    "udevadm control --reload-rules\n";

void write_and_run(const char* script_content) {
    const char* path = "/tmp/kindle_udev.sh";
    std::ofstream f(path);
    f << script_content;
    f.close();
    chmod(path, 0755);
    system("pkexec /tmp/kindle_udev.sh");
    std::remove(path);
}

/*
This script is used to set input mode: tablet - mouse
It works by creating a script in /tmp/ and running it, the script create a udev rule associated with the virtual input so that it can
see it as a mouse. To switch back to tablet it just remove the udev rule.
*/
int switch_input_mode(int i){
    toml::table config;
    getConfig(config);
    switch(i){

        // Tablet mode
        case 0:{
            // The file is only created by switching to mouse mode, so i assume this is enough check
            if(std::filesystem::exists("/etc/udev/rules.d/99-kindle-tablet.rules")){
                std::cout<<"The input is set to mouse mode, to switch you need to input sudo password.\n";
                std::cout<<"Do you want to proceed? [y/n] ";
                char temp;
                std::cin>>temp;
                if(temp == 'y'){
                    write_and_run(disable_script);
                    std::cout<<"Switched to tablet mode\n\n";
                }else{
                    std::cout<<"Returned to mouse mode\n";
                    changeConfig("input_mode", std::string("mouse"));
                }
            }
            break;
        }

        // Mouse mode
        case 1:{
            if(!std::filesystem::exists("/etc/udev/rules.d/99-kindle-tablet.rules")){
                std::cout<<"The input is set to tablet mode, to switch you need to input sudo password.\n";
                std::cout<<"Do you want to proceed? [y/n] ";
                char temp;
                std::cin>>temp;
                if(temp == 'y'){
                    write_and_run(enable_script);
                    std::cout<<"Switched to mouse mode\n\n";
                }else{
                    std::cout<<"Returned to tablet mode\n";
                    changeConfig("input_mode", std::string("tablet"));
                }
            }
            break;
        }
        default:
            return -1;
    }
    return 0;
}
