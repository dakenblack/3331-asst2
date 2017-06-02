#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include <pthread.h>
#include <semaphore.h>

#include "udp.h"
#include "shared.h"
#include "distance_vector.h"

//remove this before submitting
/*#define DEBUG*/

struct neighbor {
    char id;
    int port;
    int cost;
    struct sockaddr_in addr;
    int heartbeat_count;
};

static struct neighbor *known_neighbors = NULL;
static unsigned int num_known = 0;

static pthread_mutex_t socket_mutex;
static int _socket;
static char _id;
static DV _dv = NULL;
static BL _bl = NULL;

static void print_ascending();

/**
 * signal handler for SIGINT
 * basically the destructor of the application
 */
void intHandler(int a) {
    if(known_neighbors != NULL) {
        printf(" freeing neighbors\n");
        free(known_neighbors);
    }
    printf(" all cleaned up..\n");

    /*printf(" stopping thread...\n");*/
    /*pthread_mutex_lock(&socket_mutex);*/

    printf(" cleaning up distance vector table\n");
    if(_dv != NULL)
        DV_destroy(_dv);

    if(_bl != NULL)
        BL_destroy(_bl);

    printf(" Quiting now...\n");
    /*usleep(1000000);*/
    exit(0);
}

//gets pointer to neighbor element, NULL if no neighbor
static struct neighbor* NBR_get(char id) {
    for(int i=0; i<num_known; i++) {
        if(known_neighbors[i].id == id) 
            return &known_neighbors[i];
    }
    return NULL;
}

//listen for new messages
void* listener_thread(void * ignore) {
    int retval;
    char buf[1024];
    static int lastUpdated = 0;
    while(1) {
        int updatedFlag = 0;
        pthread_mutex_lock(&socket_mutex);
        retval = isSocketReady(_socket,500000);
        if(retval > 0) {
            //socket is ready
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

            //for each packet in the message
            int sizeRemaining = retval;
            while(sizeRemaining >= sizeof(struct packet)) {
                p = deserialize_packet(p,&temp);
                sizeRemaining -= sizeof(struct packet);

                #ifdef DEBUG
                printf("%c -> %c : %d\n",temp.from,temp.to,temp.cost);
                #endif

                //if the message is from this user skip
                if(temp.to == _id)
                    continue;

                if(temp.cost < 0) {
                    DV_remove(_dv,temp.to);
                    /*to get the new values;*/
                    /*DV_remove(_dv,temp.from);*/
                    /*struct neighbor* ptr = NBR_get(temp.from);*/
                    /*assert(ptr != NULL);*/
                    /*DV_update(_dv,ptr->id,ptr->id,ptr->cost);*/
                    #ifdef DEBUG
                    /*printf("Node %c has died\n",temp.to);*/
                    #endif
                    //system was updated
                    updatedFlag = 0;
                    continue;
                }
                
                struct neighbor* ptr = NBR_get(temp.from);
                if(ptr == NULL) {
                #ifdef DEBUG
                printf("from %c, not in known neighbors\n",temp.from);
                #endif
                } else {
                    ptr->heartbeat_count = -1;
                }


                if(DV_update2(_dv,temp.from,temp.to,temp.cost) > 0) {
                    updatedFlag = 1;
                } else {
                }
            }


        } else if(retval < 0) {
            perror("in listener: ");
            exit(1);
        } else {
            //socket is not ready
        }

        pthread_mutex_unlock(&socket_mutex);
        for(int i=0; i<num_known; i++) {
            known_neighbors[i].heartbeat_count ++;
            if(known_neighbors[i].heartbeat_count > 60) {
                #ifdef DEBUG
                printf("Node %c gone offline\n",known_neighbors[i].id);
                #endif
                BL_add(_bl,known_neighbors[i].id);
                DV_remove(_dv, known_neighbors[i].id);
                known_neighbors[i].heartbeat_count = -10000;
            }
        }

        if(!updatedFlag) {
            lastUpdated ++;
            /*printf("<hit> %d\n",lastUpdated);*/
            if(lastUpdated > 50) {
                print_ascending();
                /*usleep(8000000);*/
                lastUpdated = 0;
            }
        } else {
            lastUpdated = 0;
        }
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
        for(int i=0;i<num_known;i++) {
            toSend.from = _id;
            int j = DV_size(_dv)-1;
            for(;j>=0;j--) {
                unsigned short temp_ushort;
                DV_get(_dv,j,&(toSend.to),NULL,&temp_ushort);
                assert(temp_ushort >= 0);
                toSend.cost = (int)temp_ushort;
                serialize_packet(buf,toSend);
                sendto(_socket,buf,sizeof(struct packet),MSG_NOSIGNAL,(struct sockaddr*)&(known_neighbors[i].addr),(socklen_t)sizeof(known_neighbors[i].addr));
            }

            char* blcked = BL_items(_bl,&j);
            if(j != 0) {
                j--;
                for(;j>=0;j--) {
                    toSend.cost = -1;
                    toSend.to = blcked[j];
                    serialize_packet(buf,toSend);
                    sendto(_socket,buf,sizeof(struct packet),MSG_NOSIGNAL,(struct sockaddr*)&(known_neighbors[i].addr),(socklen_t)sizeof(known_neighbors[i].addr));
                }
                free(blcked);
            }
        }
        pthread_mutex_unlock(&socket_mutex);
        usleep(5000000);
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

    //initialize the DV
    _dv = DV_create();
    //initializing the blacklist
    _bl = BL_create();

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
        known_neighbors[i].heartbeat_count = 0;

        //add to the DV
        DV_update(_dv,n_id,n_id,(unsigned short)n_weight);
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

//used for sorting
struct temp_node {
    char to;
    char next;
    unsigned short cost;
};

static void print_ascending() {
    int size = DV_size(_dv);
    struct temp_node* sorted = malloc(size*sizeof(struct temp_node));
    for(int i=0;i<size;i++) {
        char to;
        unsigned short cost;
        char next;
        DV_get(_dv,i,&to,&next,&cost);


        //sorted insert
        int j;
        for(j=i; j>=1; j--) {
            if(sorted[j-1].to < to)
                break;
            sorted[j] = sorted[j-1];
        }
        sorted[j].to = to;
        sorted[j].cost = cost;
        sorted[j].next = next;
    }
    for(int i=0;i<size;i++) {
        printf("shortest path to node %c: the next hop is %c and the cost is %u\n",sorted[i].to, sorted[i].next ,sorted[i].cost);
    }
    free(sorted);
}
