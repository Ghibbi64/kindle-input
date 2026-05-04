#include <csignal>
#include "../../dependencies/tomlParser.h"
#include <atomic>

#pragma once

extern bool close_program;

void signalHandler(int sig);
int switch_input_mode(int i);
