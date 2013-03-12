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

int handleRequest(int coordinatorFd, Renderer &r, queue<double> &inData, int width, int height);
int wait(Renderer &r, int width, int height);

#endif