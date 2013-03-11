#ifndef WORKER_HPP
#define WORKER_HPP

#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sstream>
#include <map>
#include <vector>

class Renderer;

using namespace std;

union DataTypeConversion {
    unsigned int ui;
    int i;
    double d;
    float f;
    long l;
    short s;
    unsigned short us;
    char charArray[8];
};

typedef vector<double> (*rt_callback)(int, int);

void printServerSettings(int localSocketFd);
int acceptConnection(int localSocketFd);
void listenOnSocket(int localSocketFd);
double readDouble(int clientSocketFd);
int handleRequest(int clientSocketFd, fd_set *master_set, Renderer &r);
int wait(Renderer &r);

#endif