#ifndef MYMALLOC_H
#define MYMALLOC_H

#include <stdlib.h>
#include <stdio.h>

#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)

#define MYBLOCK_SIZE 4096
#define METADATA_SIZE sizeof(MetaData)

void* mymalloc(size_t, char*, int);
void myfree(void*, char*, int);
void printMemory();
void printMetaData();

#endif