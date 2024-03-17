#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

typedef struct {
    void* value;
    void* next;
} Node;

typedef struct {
    int length;
    Node* nodes;
} LinkedList;

LinkedList* ll_new();
Node* ll_newNode(void* value);
void ll_add(LinkedList* list, void* value);
void ll_prepend(LinkedList* list, void* value);
void ll_insertAt(LinkedList* list, int index, void* value);
void ll_removeAt(LinkedList* list, int index);
void ll_remove(LinkedList* list, void* value);
void* ll_get(LinkedList* list, int index);
void ll_free(LinkedList* list);

#endif 
