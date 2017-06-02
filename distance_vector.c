#include "distance_vector.h"
#include <assert.h>
#include <stdlib.h>

/**
 * Everything is poorly named
 * some froms actually refer to the tos\
 */

struct node {
    char from; //should be to
    char next; //next jump
    unsigned short cost;
    struct node* link;
};

struct linkedList {
    struct node* start;
    struct node** end_ptr;
    unsigned short size;
};

/**
 * @pre id < ll->size and > 0
 */
static struct node* LL_get(struct linkedList* ll, int id) {
    struct node* ptr = ll->start;
    int i=0;
    while(ptr != NULL) {
        if(i>=id)
            break;
        ptr = ptr->link;
        i++;
    }
    assert(ptr != NULL);
    return ptr;
}

DV DV_create() {
    struct linkedList* ret = malloc(sizeof(struct linkedList));
    ret->start = NULL;
    ret->size = 0;
    ret->end_ptr = &(ret->start);
    return (DV)ret;
}

int DV_update(DV d, char from, char next, unsigned short cost) {
    struct linkedList* ll = (struct linkedList*)d;
    struct node* ptr = ll->start;

    while(ptr != NULL) {
        if(ptr->from == from)
            break;
        ptr = ptr->link;
    }

    int ret = 0;
    if(ptr == NULL) {
        //new node
        ret = 2;
        struct node* new_node = malloc(sizeof(struct node));
        new_node->from = from;
        new_node->cost = cost;
        new_node->next = next;
        *(ll->end_ptr) = new_node;
        ll->end_ptr = &(new_node->link);
        ll->size = ll->size + 1;
    } else {
        if(ptr->cost > cost) {
            ret = 1;
            ptr->cost = cost;
            ptr->next = next;
        } else {
            ret = 0;
        }
    }

    return ret;
}

int DV_size(DV d) {
    return ((struct linkedList*)d)->size;
}

void DV_get(DV d, int id, char* from ,char* next, unsigned short *cost) {
    struct linkedList* ll = (struct linkedList*)d;
    if( id >= ll->size || id < 0) {
        *from = '\0';
        *cost = 0;
        return ;
    }
    struct node* n = LL_get(ll,id);
    *from = n->from;
    *cost = n->cost;
    if(next != NULL)
        *next = n->next;
}

void DV_destroy(DV d) {
    struct linkedList* ll = (struct linkedList*)d;
    struct node* ptr = ll->start;

    while(ptr != NULL) {
        struct node* temp = ptr;
        ptr = ptr->link;
        free(temp);
    }
    free(ll);
}

int DV_update2(DV d, char from, char to, unsigned short cost) {
    struct linkedList* ll = (struct linkedList*)d;
    struct node* ptr = ll->start;

    while(ptr != NULL) {
        if(ptr->from == from)
            break;
        ptr = ptr->link;
    }

    if(ptr == NULL) {
        //from node could not be found
        //nothing can be done
        return -1;
    } else {
        return DV_update(d,to,from,cost + ptr->cost);
    }
}
