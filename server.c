#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "udp.h"

int main(){
    int udpSocket, nBytes;
    char buffer[1024];

    /*Create UDP socket*/
    udpSocket = initialise_listener(7891);
    if(udpSocket < 0) {
        perror("ERROR in Initialisation");
        return -1;
    }

    /*Initialize size variable to be used later on*/
    addr_size = sizeof serverStorage;

    while(1){
        nBytes = recvfrom(udpSocket,buffer,1024,0,NULL, NULL);

        printf("%d %s << \n",nBytes, buffer);
    }

    return 0;
}
