#include "coordinator.hpp"

#include "a4.hpp"
#include "worker.hpp"
#include "dist.hpp"
#include "renderer.hpp"
#include "globals.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>

extern unsigned short port;

Coordinator::Coordinator(int width, int height, string filename) : width(width), height(height), filename(filename) {
    img = Image(width, height, 3);
}

vector<string> Coordinator::getWorkerHosts() {
    vector<string> hosts;
    hosts.push_back("gl01");
    hosts.push_back("gl04");
    hosts.push_back("gl05");
    hosts.push_back("gl06");
    hosts.push_back("gl07");
    hosts.push_back("gl09");
    hosts.push_back("gl10");
    hosts.push_back("gl11");
    hosts.push_back("gl12");
    hosts.push_back("gl13");
    hosts.push_back("gl14");
    hosts.push_back("gl19"); // unlimit does not exist
    hosts.push_back("gl23");
    hosts.push_back("gl24");
    hosts.push_back("gl25");
    hosts.push_back("gl26");
    hosts.push_back("gl27");

    return hosts;
}

int Coordinator::readPixelData(int workerFd) {
    double dx, dy;
    int status;

    status = (int)readDouble(workerFd, dx);
    if (status < 0) {
        return -1;
    }
    status = (int)readDouble(workerFd, dy);
    if (status < 0) {
        return -1;
    }

    double r, g, b;
    status = readDouble(workerFd, r);
    if (status < 0) {
        return -1;
    }
    status = readDouble(workerFd, g);
    if (status < 0) {
        return -1;
    }
    status = readDouble(workerFd, b);
    if (status < 0) {
        return -1;
    }

    int x = (int)dx;
    int y = (int)dy;

    img(x, y, 0) = r;
    img(x, y, 1) = g;
    img(x, y, 2) = b;

    return 0;
}

void Coordinator::waitForResults(fd_set &master_set, int max_fd) {
    int numPixels = width * height;
    int finishedPixels = 0;

    int percentage = 0;

    fd_set working_set;

    timeval timer;
    gettimeofday(&timer, NULL);
    double startTime = timer.tv_sec + timer.tv_usec/1000000.0;

    while (finishedPixels < numPixels) {
        memcpy(&working_set, &master_set, sizeof(master_set));
        int selectResult = select(max_fd + 1, &working_set, NULL, NULL, NULL);

        if (selectResult < 0) {
            cerr << "COORDINATOR - SELECT FAILED" << endl;
            return;
        }

        for (int i = 0; i < max_fd + 1; i++) {
            if (FD_ISSET(i, &working_set)) {
                int workerFd = i;

                int readResult = readPixelData(workerFd);
                if (readResult < 0) {
                    FD_CLR(workerFd, &master_set);
                    close(workerFd);
                }

                finishedPixels++;
                if (printProgress) {
                    int newPercentage = finishedPixels * 100 / numPixels;
                    if (newPercentage > percentage) {
                        percentage = newPercentage;
                        cerr << ". ";
                        if (percentage % 10 == 0) {
                            cerr << percentage << "\% done" << endl;
                        }
                    }
                }
            }
        }
    }

    gettimeofday(&timer, NULL);
    double endTime = timer.tv_sec + timer.tv_usec/1000000.0;
    double diff = endTime - startTime;

    int secs = floor(diff);
    int mins = floor(secs / 60);
    secs = secs % 60;
    int millis = (diff - secs) * 1000;

    cerr << setprecision(2) << "Completed in " << mins << "m, " << secs << "s, " << millis << "ms" << endl;

    ostringstream oss;

    string frameString;

    if (currentFrameNumber < 10) {
        ostringstream oss2;
        oss2 << "000" << currentFrameNumber;
        frameString = oss2.str();
    } else if (currentFrameNumber < 100) {
        ostringstream oss2;
        oss2 << "00" << currentFrameNumber;
        frameString = oss2.str();
    } else if (currentFrameNumber < 1000) {
        ostringstream oss2;
        oss2 << "0" << currentFrameNumber;
        frameString = oss2.str();
    } else if (currentFrameNumber < 10000) {
        ostringstream oss2;
        oss2 << currentFrameNumber;
        frameString = oss2.str();
    }

    string folderName = "frames";

    // ostringstream oss2;
    // oss2 << "exec rm -rf " << folderName;
    // system(oss2.str().c_str());
    mkdir(folderName.c_str(), S_IRWXU);

    oss << folderName << "/" << filename.substr(0, filename.length() - 4) << frameString << ".png";
    img.savePng(oss.str());
}

void Coordinator::distributeWork(vector<int> &workerFds) {
    int numWorkers = (int)workerFds.size();

    for (int i = 0; i < numWorkers; i++) {
        sendDouble(workerFds[i], (double)currentFrameNumber);
        sendDouble(workerFds[i], (double)i);
        sendDouble(workerFds[i], (double)numWorkers);
    }
}

void Coordinator::dispatchWorkers() {
    int numFrames = animLength * fps + 1;

    for (int k = 2; k < numFrames; k++) {
        cerr << "COORDINATOR - DISPATCHING WORKERS" << endl;
        int max_fd = 0;
        fd_set master_set;
        FD_ZERO(&master_set);

        vector<int> workerFds;
        vector<string> workerHosts = getWorkerHosts();

        if (workerHosts.size() == 0) {
            cerr << "No workers available... terminating" << endl;
            return;
        }

        for (unsigned int i = 0; i < workerHosts.size(); i++) {
            int workerFd = setupSocketAndReturnDescriptor(workerHosts[i].c_str(), port);
            if (workerFd < 0) {
                // cerr << "Connection failed for host: " << workerHosts[i] << endl;
                continue;
            }

            workerFds.push_back(workerFd);
            FD_SET(workerFd, &master_set);
            max_fd = max(max_fd, workerFd);
        }

        if (workerFds.size() == 0) {
            return;
        }

        currentFrameNumber = k;
        distributeWork(workerFds);
        waitForResults(master_set, max_fd);
    }
}