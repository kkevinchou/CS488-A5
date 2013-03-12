#include "worker.hpp"
#include "a4.hpp"

unsigned short port = 6013;

int handleRequest(int coordinatorFd, Renderer &r, queue<double> &inData, int width, int height) {
    double d;
    bool printProgress = true;
    int status = readDouble(coordinatorFd, d);

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
                vector<double> colour = r.render(i, j);

                status = sendDouble(coordinatorFd, (double)i);
                if (status < 0) {
                    return status;
                }
                status = sendDouble(coordinatorFd, (double)j);
                if (status < 0) {
                    return status;
                }

                for (unsigned int k = 0; k < colour.size(); k++) {
                    int status = sendDouble(coordinatorFd, colour[k]);
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

bool main_first_run = true;
int localSocketFd;

int wait(Renderer &r, int width, int height) {
    if (main_first_run) {
        main_first_run = false;
        localSocketFd = socket(AF_INET, SOCK_STREAM, 0);
        listenOnSocket(localSocketFd, port);
        printServerSettings(localSocketFd);
    }

    int max_fd = localSocketFd;
    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(localSocketFd, &master_set);

    queue<double> inData;

    bool computationDone = false;
    int coordSocketFd;

    while (!computationDone) {
        memcpy(&working_set, &master_set, sizeof(master_set));
        int selectResult = select(max_fd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i < max_fd + 1; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i != localSocketFd) {
                    int coordinatorFd = i;
                    int handleResult = handleRequest(coordinatorFd, r, inData, width, height);
                    if (handleResult == 1) {
                        computationDone = true;
                    }
                    if (handleResult < 0) {
                        FD_CLR(coordinatorFd, &master_set);
                        close(coordinatorFd);
                    }
                } else {
                    coordSocketFd = acceptConnection(localSocketFd);
                    max_fd = coordSocketFd;
                    FD_SET(coordSocketFd, &master_set);
                }
            }
        }
    }

    close(coordSocketFd);
    // close(localSocketFd);

    return 0;
}