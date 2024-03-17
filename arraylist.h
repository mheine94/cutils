#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 64

typedef struct {
    int length;
    int capacity;
    size_t elementSize;
    void* array;
} ArrayList;

ArrayList* al_new(size_t elementSize);
void al_add(ArrayList* list, void* value);
void al_prepend(ArrayList* list, void* value);
void al_insertAt(ArrayList* list, int index, void* value);
void al_removeAt(ArrayList* list, int index);
void* al_get(ArrayList* list, int index);
void al_free(ArrayList* list);

#endif
