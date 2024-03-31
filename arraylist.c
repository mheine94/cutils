#include "arraylist.h"

ArrayList* al_new(size_t elementSize){
    ArrayList* newList = calloc(1, sizeof(ArrayList));
    newList->elementSize = elementSize;
    newList->length = 0;
    newList->capacity = 0;
    
    return newList;
}

void al_increaseCapacity(ArrayList* list){
    if(list->capacity == 0){
        int newCapacity = INITIAL_CAPACITY;
        void* newArray = malloc(sizeof(list->elementSize) * newCapacity);
        list->capacity = newCapacity;
        list->array = newArray;
    }else {
        int newCapacity = list->capacity * 2;
        void* newArray = malloc(sizeof(list->elementSize) * newCapacity);
        memcpy(newArray, list->array, list->length * list->elementSize);
        free(list->array);
        list->array = newArray;
        list->capacity = newCapacity;
    }
}

void al_add(ArrayList* list, void* value){
    if(list->length + 1 > list->capacity){
        al_increaseCapacity(list);
    }
    memcpy(list->array + list->length * list->elementSize, value, list->elementSize);
    list->length++;
}



void al_prepend(ArrayList* list, void* value){
    if(list->length + 1 > list->capacity){
        al_increaseCapacity(list);
    }
    memcpy(list->array + list->elementSize, list->array, list->elementSize * list->length);
    memcpy(list->array, value, list->elementSize);
    list->length++;
}

void al_insertAt(ArrayList* list, int index, void* value){
    if(list->length + 1 > list->capacity){
        al_increaseCapacity(list);
    }
    memcpy(list->array + list->elementSize * (index+1), list->array + list->elementSize * index, list->elementSize * (list->length - index));
    memcpy(list->array + list->elementSize * index, value, list->elementSize);
    
    list->length++;
}

void al_removeAt(ArrayList* list, int index){ 
    memcpy(list->array + list->elementSize * index, list->array + list->elementSize * (index+1),list->elementSize * (list->length - index)); 
    list->length--;
}

void* al_get(ArrayList* list, int index){
    return list->array + list->elementSize * index;
}

void al_free(ArrayList* list){
    if(list->capacity > 0){
        free(list->array);
    }
    free(list);
}
