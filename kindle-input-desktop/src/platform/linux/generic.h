#include <csignal>
#include "../../dependencies/tomlParser.h"
#include <atomic>

#pragma once

// DECLARATION ONLY.
// Do NOT add "= false" here. Do not remove the 'extern' keyword.
extern bool close_program;

void signalHandler(int sig);
int switch_input_mode(int i);
