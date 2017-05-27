#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <pthread.h>
#include <semaphore.h>

#include "udp.h"

//remove this before submitting
#define DEBUG

struct neighbor {
    char id;
    int port;
    int cost;
    struct sockaddr_in addr;
}


static struct neighbor *known_neighbors = NULL;
static unsigned int num_known = 0;

static pthread_mutex_t socket_mutex;
static int _socket;
static char _id;

/**
 * signal handler for SIGINT
 */
void intHandler(int) {
    if(known_neighbors != NULL) {
        printf("free neighbors");
        free(known_neighbors);
    }
    printf("all cleaned up..\n");
    exit(0);
}

//listen for new messages
void* listener_thread(void *) {
    while(1) {
        pthread_mutex_lock(&socket_mutex);
        retVal = isSocketReady(port,500000);
        if(retVal > 0) {

            retVal = read(_socket,buf,1024);
            if(retVal == 0) {
                printf("[listener] number of bytes read was 0??\n");
                exit(1);
            } else if (retval < 0) {
                perror("[listener] in read:");
                exit(1);
            }

            char *p = buf;
            struct packet temp;
            while(retval >= sizeof(struct packet)) {
                p = deserialize_packet(p,&temp);
                retval -= sizeof(struct packet);
                printf("from %c, msg: %i\n",temp.from, temp.byte);
            }

        } else if(retVal < 0) {
            perror("in listener: ");
            exit(1);
        } else {
            //socket is not ready
        }

        pthread_mutex_unlock(&port_mutex);
        //give a chance for other threads to play
        usleep(100000);
    }
    printf("end of listener??\n");
    exit(1);
}

//send messages
void* sender_thread(void *) {
    while(1) {
        pthread_mutex_lock(&socket_mutex);

        pthread_mutex_unlock(&socket_mutex);
        usleep(100000);
    }

    printf("end of sender??\n");
    exit(1);
}

int main(int argc, char* argv[]) {
    int port, fd;
    if(argc <= 3) {
        printf("Incorrect Usage: ./Dvr <id> <port> <config file> [-p]\n");
        return -1;
    }

    if(argc > 4) {
        //poisoned reverse is active
    }
    signal(SIGINT, intHandler);

    while(1);

    _socket = initialize_listener(atoi(argv[2]));
    _id = argv[1][0];
}
