#include "worker.hpp"
#include "a4.hpp"

unsigned short port = 6013;

void printServerSettings(int localSocketFd) {
    char localHostName[256];
    gethostname(localHostName, 256);
    cout << "SERVER_ADDRESS " << localHostName << endl;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(localSocketFd, (struct sockaddr *)&sin, &len);
    cout << "SERVER_PORT " << ntohs(sin.sin_port) << endl;
}

int acceptConnection(int localSocketFd) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int newSocketFd = accept(localSocketFd, (struct sockaddr *) &clientAddress, &clientAddressSize);

    return newSocketFd;
}

void listenOnSocket(int localSocketFd) {
    struct sockaddr_in serverAddress;
    memset((struct sockaddr_in *)&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    bind(localSocketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    listen(localSocketFd, 5);
}

char *readDouble(char *buffer, double &d) {
    DataTypeConversion converter;
    for(unsigned int i = 0; i < 8; i++) {
        converter.charArray[i] = *buffer;
        buffer++;
    }

    d = converter.d;
    return buffer;
}

int handleRequest(int clientSocketFd, fd_set *master_set, Renderer &r) {
    int bufSize = 256;
    char *buffer = new char[bufSize];
    char *bufferStart = buffer;

    memset(buffer, 0, bufSize);

    int numDoublesToRead = 3;
    int totalBytes = numDoublesToRead * 8;
    int bytesLeftToRead = totalBytes;

    while (bytesLeftToRead > 0) {
        int bytesRead = recv(clientSocketFd, buffer, bytesLeftToRead, 0);

        if(bytesRead == 0) {
            cerr << "Connection closed"<<endl;
            return -1;
        } else if (bytesRead < 0) {
            cerr << "ERROR reading from socket"<<endl;
            return -1;
        }

        buffer += bytesRead;
        bytesLeftToRead -= bytesRead;
    }
    buffer = bufferStart;

    double ds[numDoublesToRead];
    for (int i = 0; i < numDoublesToRead; i++) {
        buffer = readDouble(buffer, ds[i]);
    }

    // cerr << "==============================================" << endl;
    // for (int i = 0; i < numDoublesToRead; i++) {
    //     cerr << ds[i] << endl;
    // }

    vector<double> results = r.render((int)ds[0], (int)ds[1]);

    return 0;
}

int wait(Renderer &r) {
    int localSocketFd = socket(AF_INET, SOCK_STREAM, 0);

    listenOnSocket(localSocketFd);
    printServerSettings(localSocketFd);

    int max_fd = localSocketFd;
    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(localSocketFd, &master_set);

    map<int, unsigned int> chunkInfo;

    while (true) {
        memcpy(&working_set, &master_set, sizeof(master_set));
        int selectResult = select(max_fd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i < max_fd + 1; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i != localSocketFd) {
                    int clientSocketFd = i;
                    int handleResult = handleRequest(clientSocketFd, &master_set, r);
                    if (handleResult < 0) {
                        FD_CLR(clientSocketFd, &master_set);
                        close(clientSocketFd);
                    }
                } else {
                    int newSocketFd = acceptConnection(localSocketFd);
                    max_fd = newSocketFd;
                    FD_SET(newSocketFd, &master_set);
                }
            }
        }
    }

    close(localSocketFd);

    return 0;
}