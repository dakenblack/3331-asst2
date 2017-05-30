#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include <pthread.h>
#include <semaphore.h>

#include "udp.h"
#include "shared.h"

//remove this before submitting
#define DEBUG

struct neighbor {
    char id;
    int port;
    int cost;
    struct sockaddr_in addr;
};

static struct neighbor *known_neighbors = NULL;
static unsigned int num_known = 0;

static pthread_mutex_t socket_mutex;
static int _socket;
static char _id;

/**
 * signal handler for SIGINT
 */
void intHandler(int a) {
    if(known_neighbors != NULL) {
        printf(" freeing neighbors\n");
        free(known_neighbors);
    }
    printf(" all cleaned up..\n");

    printf(" stopping thread...\n");
    pthread_mutex_lock(&socket_mutex);
    printf(" Quiting now...\n");
    /*usleep(1000000);*/
    exit(0);
}

//listen for new messages
void* listener_thread(void * ignore) {
    int retval;
    char buf[1024];
    while(1) {
        pthread_mutex_lock(&socket_mutex);
        retval = isSocketReady(_socket,500000);
        if(retval > 0) {

            retval = read(_socket,buf,1024);
            if(retval == 0) {
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

        } else if(retval < 0) {
            perror("in listener: ");
            exit(1);
        } else {
            //socket is not ready
        }

        pthread_mutex_unlock(&socket_mutex);
        //give a chance for other threads to play
        usleep(100000);
    }
    printf("end of listener??\n");
    exit(1);
}

//send messages
void* sender_thread(void * ignore) {
    char buf[1024];
    while(1) {
        pthread_mutex_lock(&socket_mutex);

        struct packet toSend;
        toSend.from = _id;
        for(int i=0;i<num_known;i++) {
            toSend.byte = rand()%256;
            serialize_packet(buf,toSend);
            sendto(_socket,buf,sizeof(struct packet),MSG_NOSIGNAL,(struct sockaddr*)&(known_neighbors[i].addr),(socklen_t)sizeof(known_neighbors[i].addr));
        }
        pthread_mutex_unlock(&socket_mutex);
        usleep(100000);
    }

    printf("end of sender??\n");
    exit(1);
}

int main(int argc, char* argv[]) {
    if(argc <= 3) {
        printf("Incorrect Usage: ./Dvr <id> <port> <config file> [-p]\n");
        return -1;
    }

    if(argc > 4) {
        //poisoned reverse is active
    }

    signal(SIGINT, intHandler);

    srand((int)argv[1][0]*atoi(argv[2]));

    //initializing the mutexes
    pthread_mutex_init(&socket_mutex,NULL);

    //getting known neighbors
    char buf[64];
    FILE* fd = fopen(argv[3],"r");
    fgets(buf,64,fd);
    int n = atoi(buf);
    known_neighbors = malloc(n*sizeof(struct neighbor));
    num_known = n;

    for(int i=0; i<n;i++) {
        char n_id;
        int n_weight, n_port;
        fscanf(fd,"%c",&n_id);
        fscanf(fd,"%d",&n_weight);
        fscanf(fd,"%d",&n_port);
        while(fgetc(fd) != '\n');
        
        known_neighbors[i].id = n_id;
        known_neighbors[i].port = n_port;
        known_neighbors[i].cost = n_weight;
        known_neighbors[i].addr = getAddr(n_port);
    }

    fclose(fd);

    //initializing the socket
    _socket = initialise_listener(atoi(argv[2]));
    _id = argv[1][0];

    //starting the threads
    pthread_t pth;
    pthread_create(&pth,NULL,listener_thread,NULL);
    sender_thread(NULL);

    printf("end of main????????\n");
    return 1;
}
