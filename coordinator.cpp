#include "coordinator.hpp"

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

void sendDouble(int workerFd, double d) {
    DataTypeConversion converter;
    converter.d = d;

    char *bytes = converter.charArray;
    unsigned int bytesLeftToSend = 8;

    while (bytesLeftToSend > 0) {
        int bytesSent = send(workerFd, bytes, bytesLeftToSend, 0);

        if(bytesSent == 0) {
            cerr << "SECOND COMPLETE OR WORKER TERMINATED?" << endl;
            break;
        } else if (bytesSent < 0) {
            cerr << "SEND ERROR" << endl;
        }

        bytesLeftToSend -= bytesSent;
        bytes += bytesSent;
    }
}