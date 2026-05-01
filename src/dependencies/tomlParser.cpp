#include <iostream>
#include "toml.hpp"
#include "tomlParser.h"

void getConfig(toml::table& config){
    try
    {
        config = toml::parse_file("config.toml");
    }
    catch (const toml::parse_error& err)
    {
        std::cerr << "Parsing failed:\n" << err << "\n";
    }
}
