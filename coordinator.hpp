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
void dispatchWorkers(int width, int height, string filename);
void waitForResults(int width, int height, fd_set &master_set, int max_fd, string filename);
int readPixelData(int workerFd, Image &img);
void distributeWork(int width, int height, vector<int> &workerFds);

#endif