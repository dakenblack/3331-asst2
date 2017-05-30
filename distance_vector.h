#ifndef DISTANCE_VECTOR_H
#define DISTANCE_VECTOR_H

typedef void* DV;

DV DV_create();
/**
 * @returns 0 if no update, 1 if node was present but updated and 2 if node did not exist
 */
int DV_update(DV d, char from, unsigned short cost);
//@returns 
// 0: no update
// 1: updated already present node
// 2: new node added
// -1: nothing could be done
int DV_update2(DV d, char from, char to, unsigned short cost);
int DV_size(DV d);
void DV_get(DV d, int id, char* from, unsigned short *cost);
void DV_destroy(DV d);
#endif
