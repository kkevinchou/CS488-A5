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
#include "constants.hpp"
#include "dist.hpp"
#include <queue>
#include "renderer.hpp"

using namespace std;

class Worker {
public:
    Worker();
    void setRenderer(Renderer *r);
    void setDimensions(int width, int height);
    void wait();
    void accept();

private:
    Renderer *r;
    int width;
    int height;
    bool firstRun;

    int localSocketFd;
    int coordSocketFd;

    int handleRequest(queue<double> &inData);
};

#endif