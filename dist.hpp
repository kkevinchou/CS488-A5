#ifndef DIST_HPP
#define DIST_HPP

#include <string>
#include "constants.hpp"

using namespace std;

void printServerSettings(int localSocketFd);
int acceptConnection(int localSocketFd);
void listenOnSocket(int localSocketFd, unsigned short port);

int setupSocketAndReturnDescriptor(const char * serverAddressString, int serverPort);
int sendDouble(int workerFd, double d);
int readDouble(int connectionFd, double &d);

#endif