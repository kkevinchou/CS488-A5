#include "dist.hpp"

#include <string>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h> //getenv
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void printServerSettings(int localSocketFd) {
    char localHostName[256];
    gethostname(localHostName, 256);
    cout << "SERVER_ADDRESS: " << localHostName << endl;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(localSocketFd, (struct sockaddr *)&sin, &len);
    cout << "SERVER_PORT: " << ntohs(sin.sin_port) << endl;
}

int acceptConnection(int localSocketFd) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int newSocketFd = accept(localSocketFd, (struct sockaddr *) &clientAddress, &clientAddressSize);

    return newSocketFd;
}

void listenOnSocket(int localSocketFd, unsigned short port) {
    struct sockaddr_in serverAddress;
    memset((struct sockaddr_in *)&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    bind(localSocketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    listen(localSocketFd, 5);
}

int setupSocketAndReturnDescriptor(const char * serverAddressString, int serverPort)
{
    int socketFileDescriptor;

    struct sockaddr_in serverAddressStruct;
    struct hostent *server;

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor < 0) {
        return -1;
    }

    server = gethostbyname(serverAddressString);

    if (server == NULL) {
       return -1;
    }

    memset((char *) &serverAddressStruct, 0, sizeof(serverAddressStruct));

    serverAddressStruct.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serverAddressStruct.sin_addr.s_addr,
         server->h_length);
    serverAddressStruct.sin_port = htons(serverPort);

    if (connect(socketFileDescriptor,(struct sockaddr *) &serverAddressStruct,sizeof(serverAddressStruct)) < 0) {
        return -1;
    }

    return socketFileDescriptor;
}

int sendDouble(int workerFd, double d) {
    DataTypeConversion converter;
    converter.d = d;

    char *bytes = converter.charArray;
    unsigned int bytesLeftToSend = 8;

    while (bytesLeftToSend > 0) {
        int bytesSent = send(workerFd, bytes, bytesLeftToSend, 0);

        if (bytesSent == 0) {
            cerr << "SECOND COMPLETE OR WORKER TERMINATED?" << endl;
            return -1;
        } else if (bytesSent < 0) {
            cerr << "SEND ERROR" << endl;
            return -1;
        }

        bytesLeftToSend -= bytesSent;
        bytes += bytesSent;
    }
    return 0;
}

int sendDoubleMutexed(int workerFd, double d, pthread_mutex_t *m) {
    DataTypeConversion converter;
    converter.d = d;

    char *bytes = converter.charArray;
    unsigned int bytesLeftToSend = 8;

    while (bytesLeftToSend > 0) {
        pthread_mutex_lock(m);
        int bytesSent = send(workerFd, bytes, bytesLeftToSend, 0);
        pthread_mutex_unlock(m);

        if (bytesSent == 0) {
            cerr << "SECOND COMPLETE OR WORKER TERMINATED?" << endl;
            return -1;
        } else if (bytesSent < 0) {
            cerr << "SEND ERROR" << endl;
            return -1;
        }

        bytesLeftToSend -= bytesSent;
        bytes += bytesSent;
    }
    return 0;
}

double readDoubleFromBuffer(char *buffer) {
    DataTypeConversion converter;
    for(unsigned int i = 0; i < 8; i++) {
        converter.charArray[i] = *buffer;
        buffer++;
    }

    return converter.d;
}

int readDouble(int connectionFd, double &d) {
    int bufSize = 8;
    char *buffer = new char[bufSize];
    char *bufferStart = buffer;

    memset(buffer, 0, bufSize);

    int bytesLeftToRead = 8;

    while (bytesLeftToRead > 0) {
        int bytesRead = recv(connectionFd, buffer, bytesLeftToRead, 0);

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
    d = readDoubleFromBuffer(buffer);

    return 0;
}