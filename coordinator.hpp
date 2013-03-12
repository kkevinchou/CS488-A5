#ifndef COORDINATOR_HPP
#define COORDINATOR_HPP

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "raycaster.hpp"
#include "image.hpp"

using namespace std;

class Coordinator {
public:
    Coordinator(int width, int height, string filename);
    void dispatchWorkers();
private:
    int width;
    int height;
    string filename;
    Image img;

    vector<string> getWorkerHosts();
    void distributeWork(vector<int> &workerFds);
    int readPixelData(int workerFd);
    void waitForResults(fd_set &master_set, int max_fd);
};



#endif