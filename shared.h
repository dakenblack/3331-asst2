/**
 * definition of message and serialization functions
 * Author: Jabez Wilson (z5027406)
 *
 */

#ifndef SHARED_H
#pragma pack(0)
#define SHARED_H

#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define STRING_SIZE 24

#define SIZE_INT sizeof(int)
#define SIZE_UINT sizeof(unsigned int)
#define SIZE_USHORT sizeof(unsigned short)
#define SIZE_ULONG sizeof(unsigned long)

//#######################################
// Custom Type Definition
//#######################################

//#######################################
// Basic Type Serialisation
//#######################################
char* serialize_int(char* buf,int a) {
    char* p = (char*)&a;
    for(int i=0;i<SIZE_INT;i++,p++,buf++)
        *buf = *p;
    return buf;
}

char* serialize_uint(char* buf,unsigned int a) {
    char* p = (char*)&a;
    for(int i=0;i<SIZE_UINT;i++,p++,buf++)
        *buf = *p;
    return buf;
}
char* serialize_ushort(char* buf,unsigned short a) {
    char* p = (char*)&a;
    for(int i=0;i<SIZE_USHORT;i++,p++,buf++)
        *buf = *p;
    return buf;
}
char* serialize_ulong(char* buf,unsigned long a) {
    char* p = (char*)&a;
    for(int i=0;i<SIZE_ULONG;i++,p++,buf++)
        *buf = *p;
    return buf;
}

char* serialize_string(char* buf, char* a, int size) {
    char* p = a;
    for(int i=0;i<size;i++,p++,buf++)
        *buf = *p;
    return buf;
}

char* deserialize_int(char* buf,int* a) {
    char* p = (char*)a;
    for(int i=0;i<SIZE_INT;i++,p++,buf++)
        *p = *buf;
    return buf;
}

char* deserialize_uint(char* buf,unsigned int* a) {
    char* p = (char*)a;
    for(int i=0;i<SIZE_UINT;i++,p++,buf++)
        *p = *buf;
    return buf;
}
char* deserialize_ushort(char* buf,unsigned short* a) {
    char* p = (char*)a;
    for(int i=0;i<SIZE_USHORT;i++,p++,buf++)
        *p = *buf;
    return buf;
}
char* deserialize_ulong(char* buf,unsigned long* a) {
    char* p = (char*)a;
    for(int i=0;i<SIZE_ULONG;i++,p++,buf++)
        *p = *buf;
    return buf;
}

char* deserialize_string(char* buf, char* a, int size) {
    char* p = a;
    for(int i=0;i<size;i++,p++,buf++)
        *p = *buf;
    return buf;
}

//#######################################
// Custom Type Serialisation
//#######################################


#endif
