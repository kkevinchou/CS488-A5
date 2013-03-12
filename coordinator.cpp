#include "coordinator.hpp"

#include "a4.hpp"
#include "background.hpp"
#include "collider.hpp"
#include "worker.hpp"
#include "dist.hpp"
#include "renderer.hpp"

extern unsigned short port;

vector<string> getWorkerHosts() {
    vector<string> hosts;
    hosts.push_back("gl23");
    hosts.push_back("gl24");
    hosts.push_back("gl25");
    hosts.push_back("gl26");
    hosts.push_back("gl27");

    return hosts;
}

int readPixelData(int workerFd, Image &img) {
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

    // cerr << x << ", " << y << endl;

    return 0;
}

void waitForResults(int width, int height, fd_set &master_set, int max_fd, string filename) {
    Image img(width, height, 3);
    int numPixels = width * height;
    int finishedPixels = 0;

    bool printProgress = true;
    int percentage = 0;
    clock_t t = clock();

    fd_set working_set;

    while (finishedPixels < numPixels) {
        memcpy(&working_set, &master_set, sizeof(master_set));
        int selectResult = select(max_fd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i < max_fd + 1; i++) {
            if (FD_ISSET(i, &working_set)) {
                int workerFd = i;

                int readResult = readPixelData(workerFd, img);
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

    t = clock() - t;
    cerr << "Took " << ((float)t)/CLOCKS_PER_SEC << " seconds to complete" << endl;
    img.savePng(filename);
}

// void dispatchWork(int height, vector<int> &workerFds) {
//     unsigned int numWorkers = workerFds.size();
//     int nextRow = 0;
//     int rowsLeft = height;
//     int rowsPerWorker = max((int)(height / numWorkers), 1);

//     for (unsigned int i = 0; i < numWorkers; i++) {
//         int workerFd = workerFds[i];

//         sendDouble(workerFd, nextRow);

//         if (rowsPerWorker * 2 <= rowsLeft) {
//             sendDouble(workerFd, rowsPerWorker);
//             rowsLeft -= rowsPerWorker;
//             nextRow += rowsPerWorker;
//         } else {
//             sendDouble(workerFd, rowsLeft);
//             rowsLeft -= rowsLeft;
//             nextRow += rowsLeft;
//             break;
//         }
//     }
// }

void dispatchWork(int width, int height, vector<int> &workerFds) {
    unsigned int nextWorker = 0;
    int numWorkers = (int)workerFds.size();

    // for (int i = 0; i < width; i++) {
    //     for (int j = 0; j < height; j++) {
    //         sendDouble(workerFds[nextWorker], (double)i);
    //         sendDouble(workerFds[nextWorker], (double)j);
    //         nextWorker = (nextWorker + 1) % numWorkers;
    //     }
    // }

    for (unsigned int i = 0; i < numWorkers; i++) {
        sendDouble(workerFds[i], (double)i);
        sendDouble(workerFds[i], (double)width);
        sendDouble(workerFds[i], (double)height);
        sendDouble(workerFds[i], (double)numWorkers);
    }

    cerr << "DONE SENDING" << endl;
}

void go(int width, int height, string filename) {
    double i1 = 8;
    double i2 = 3;
    int i = i1 /i2;
    cerr << "INT TEST " << i << endl;
    vector<string> workerHosts = getWorkerHosts();

    int max_fd = 0;
    fd_set master_set;
    FD_ZERO(&master_set);

    vector<int> workerFds;
    for (unsigned int i = 0; i < workerHosts.size(); i++) {
        int workerFd = setupSocketAndReturnDescriptor(workerHosts[i].c_str(), port);
        if (workerFd < 0) {
            cerr << "Connection Failed for host: " << workerHosts[i] << endl;
            continue;
        }

        workerFds.push_back(workerFd);
        FD_SET(workerFd, &master_set);
        max_fd = max(max_fd, workerFd);
    }

    if (workerFds.size() == 0) {
        return;
    }

    dispatchWork(width, height, workerFds);
    waitForResults(width, height, master_set, max_fd, filename);
}