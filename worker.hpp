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
#include "workpool.hpp"

using namespace std;

struct worker_thread_args {
    pthread_mutex_t *sendLock;
    WorkPool *workPool;
    Renderer *renderer;
    int height;
    int coordSocketFd;
};

class Worker {
public:
    Worker();
    void setParams(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const list<Light*>& lights);
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