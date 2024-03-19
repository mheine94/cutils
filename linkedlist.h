#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

typedef struct {
    void* next;
    char value;
} Node;

typedef struct {
    int length;
    int valueSize;
    Node* nodes;
} LinkedList;

LinkedList* ll_new(int valueSize);
void ll_add(LinkedList* list, void* value);
void ll_prepend(LinkedList* list, void* value);
void ll_insertAt(LinkedList* list, int index, void* value);
void ll_removeAt(LinkedList* list, int index);
void ll_remove(LinkedList* list, void* value);
void* ll_get(LinkedList* list, int index);
void ll_free(LinkedList* list);

#endif 
