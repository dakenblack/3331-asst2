#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024

/**
 * @returns the sockaddr_in struct object associated with the port (at localhose)
 */
struct sockaddr_in getAddr(int port) {
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    return serverAddr;
}

/**
 * @returns socket to listen to if no error returns -1 if there was an error
 */
int initialise_listener(int listenport) {
    int udpSocket;
    struct sockaddr_in serverAddr;

    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
    serverAddr = getAddr(listenport);

    if(bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        return -1;
    }

    return udpSocket;
}

/**
 * Usage of the functions
 * nBytes = recvfrom(udpSocket,buffer,1024,0,NULL, NULL);
 * sendto(clientSocket,buffer,nBytes,MSG_NOSIGNAL,(struct sockaddr*)&addr,(socklen_t)sizeof(addr));
 *
 */

/**
 * @returns -1: error, 0: timeout, 1: fd is ready
 */
int isSocketReady(int fd,int usec) {
    struct timeval tv;
    fd_set f_set;

    FD_ZERO(&f_set);
    FD_SET(fd, &f_set);
    tv.tv_sec = 0;
    tv.tv_usec = usec;
    return select(fd+1, &f_set, NULL, NULL, &tv);
}

