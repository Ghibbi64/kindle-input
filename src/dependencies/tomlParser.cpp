#include <iostream>
#include <regex>
#include <fstream>
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

void changeConfig(std::string key, std::string value){
    std::ifstream in("config.toml");
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    std::regex pattern(key + R"(\s*=\s*\S+)");
    content = std::regex_replace(content, pattern, key + " = \"" + value + "\"");
    std::ofstream out("config.toml");
    out << content;
}

void changeConfig(std::string key, int value){
    std::ifstream in("config.toml");
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    std::regex pattern(key + R"(\s*=\s*\S+)");
    content = std::regex_replace(content, pattern, key + " = " + std::to_string(value));
    std::ofstream out("config.toml");
    out << content;
}

void changeConfig(std::string key, bool value){
    std::ifstream in("config.toml");
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    std::regex pattern(key + R"(\s*=\s*\S+)");
    content = std::regex_replace(content, pattern, key + " = " + (value ? "true" : "false"));
    std::ofstream out("config.toml");
    out << content;
}
