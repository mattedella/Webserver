#ifndef CLEAN_HPP
#define CLEAN_HPP

#include "webserv.hpp"
#include <signal.h>

void cleanupResources();
void signalHandler(int signum);
void initializeCleanup(conf* conf_ptr);

#endif