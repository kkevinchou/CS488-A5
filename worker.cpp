#include "worker.hpp"
#include "a4.hpp"

extern unsigned short port;

int Worker::handleRequest(queue<double> &inData) {
    double d;
    bool printProgress = true;
    int status = readDouble(coordSocketFd, d);

    if (status == 0) {
        inData.push(d);
    }

    if (inData.size() >= 4) {
        int column = (int)inData.front();
        inData.pop();
        int width = (int)inData.front();
        inData.pop();
        int height = (int)inData.front();
        inData.pop();
        int numWorkers = (int)inData.front();
        inData.pop();

        int totalColumns = width / numWorkers;
        int LeftOverColumns = width % numWorkers;
        if (column <= LeftOverColumns - 1) {
            totalColumns++;
        }

        int numPixels = totalColumns * height;
        int finishedPixels = 0;
        int percentage = 0;

        for (int i = column; i < width; i += numWorkers) {
            for (int j = 0; j < height; j++) {
                vector<double> colour = r->render(i, j);

                status = sendDouble(coordSocketFd, (double)i);
                if (status < 0) {
                    return status;
                }
                status = sendDouble(coordSocketFd, (double)j);
                if (status < 0) {
                    return status;
                }

                for (unsigned int k = 0; k < colour.size(); k++) {
                    int status = sendDouble(coordSocketFd, colour[k]);
                    if (status < 0) {
                        return status;
                    }
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

        return 1;
    }

    return status;
}

void Worker::wait() {
    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(localSocketFd, &master_set);
    FD_SET(coordSocketFd, &master_set);

    queue<double> inData;

    bool computationDone = false;

    while (!computationDone) {
        memcpy(&working_set, &master_set, sizeof(master_set));
        int selectResult = select(coordSocketFd + 1, &working_set, NULL, NULL, NULL);

        if (FD_ISSET(coordSocketFd, &working_set)) {
            int handleResult = handleRequest(inData);
            if (handleResult == 1) {
                computationDone = true;
            }
            if (handleResult < 0) {
                FD_CLR(coordSocketFd, &master_set);
                close(coordSocketFd);
            }
        }
    }

    close(coordSocketFd);
}

void Worker::accept() {
    if (firstRun) {
        localSocketFd = socket(AF_INET, SOCK_STREAM, 0);
        listenOnSocket(localSocketFd, port);
        firstRun = false;
    }

    printServerSettings(localSocketFd);
    coordSocketFd = acceptConnection(localSocketFd);
}

void Worker::setRenderer(Renderer *r) {
    this->r = r;
}

void Worker::setDimensions(int width, int height) {
    this->width = width;
    this->height = height;
}

Worker::Worker() {
    firstRun = true;
}