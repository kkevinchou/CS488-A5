#ifndef COORDINATOR_HPP
#define COORDINATOR_HPP

#include <string>
#include "worker.hpp"

int setupSocketAndReturnDescriptor(const char * serverAddressString, int serverPort);
void sendDouble(int workerFd, double d);

#endif