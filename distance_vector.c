#include "distance_vector.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

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
    unsigned short size;
};

/**
 * @pre id < ll->size and > 0
 */
static struct node* LL_get(struct linkedList* ll, int id) {
    struct node* ptr = ll->start;
    int i=0;
    assert(id < ll->size);
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
    return (DV)ret;
}

int DV_update(DV d, char from, char next, unsigned short cost) {
    struct linkedList* ll = (struct linkedList*)d;
    struct node* ptr = ll->start;
    struct node* back = NULL;

    while(ptr != NULL) {
        if(ptr->from == from)
            break;
        back = ptr;
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
        new_node->link = NULL;

        if(back != NULL) {
            back->link = new_node;
        } else {
            ll->start = new_node;
        }

        ll->size = ll->size + 1;
    } else {
        if(ptr->cost > cost) {
            ret = 1;
            ptr->cost = cost;
            ptr->next = next;
        } else {
            //if the next node transmitted a new cost, update anyway
            if(ptr->next == next) {
                /*ptr->cost = cost;*/
            } else {
                ret = 0;
            }
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
        //the from attribute is poorly named
        if(ptr->from == from)
            break;
        ptr = ptr->link;
    }

    if(ptr == NULL) {
        //from node could not be found
        //nothing can be done
        return -1;
    } else {
        /*printf("%c<%u>%c + %c<%u>%c\n",ptr->next,ptr->cost,ptr->from,from,cost,to);*/
        return DV_update(d,to,ptr->next,cost + ptr->cost);
    }
}

void DV_remove(DV d, char id) {
    struct linkedList* ll = (struct linkedList*)d;
    struct node* ptr = ll->start;
    struct node* back = NULL;

    while(ptr != NULL) {
        if(ptr->next == id || ptr->from == id) {
            struct node* temp = ptr;
            if(back != NULL) {
                back->link = ptr->link;
            } else {
                ll->start = ptr->link;
            }

            ptr = ptr->link;

            free(temp);
            ll->size = ll->size - 1;
        } else {
            back = ptr;
            ptr = ptr->link;
        }
    }
}
