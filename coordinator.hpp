#ifndef COORDINATOR_HPP
#define COORDINATOR_HPP

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "raycaster.hpp"
#include "image.hpp"

using namespace std;

vector<string> getWorkerHosts();
void go(int width, int height, string filename);
int readPixelData(int workerFd, Image &img);

#endif