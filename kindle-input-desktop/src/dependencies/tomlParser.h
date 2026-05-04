#include "toml.hpp"
#include <iostream>

void getConfig(toml::table& config);
void changeConfig(std::string key, std::string value);
void changeConfig(std::string key, int value);
void changeConfig(std::string key, bool value);
