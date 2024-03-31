#ifndef STREAM_H
#define STREAM_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "arraylist.h"

typedef struct strm Stream;

typedef struct {
    int pos;
    void* current;
    size_t elementSize;
    Stream* origin;
    int transformPos;
} StreamElement;

typedef struct strm {
    ArrayList* source;
    int operationCount;
    int isFlat;
    StreamElement nextOriginElement;
    ArrayList* streamOps;
    StreamElement (*getNext)(void* strm,  StreamElement se);
} Stream;


typedef void* (*Allocator)(size_t elementSize);
typedef int (*Accumulator)(void* collection, void* value);
typedef struct clct{
    Allocator allocate;
    Accumulator accumulate;
} Collector;


typedef void* (*Operation)(void* value);
typedef Stream* (*Flatten)(void* value);
typedef int (*Predicate)(void* value);

typedef enum t_so {
    FILTER,
    TRANSFORM,
    TRANSFORM_NEW_T,
    FLATMAP
} StreamOperationType;


typedef struct se{
    size_t pos;
    StreamOperationType type;
    Operation op;
    Predicate pred; 
    Flatten flat;
    size_t newElementSize;
} StreamOperation ;



Stream* strm_map(Stream* st, Operation op);
Stream* strm_map_to(Stream* st, Operation op, size_t newElementSize);
Stream* strm_flat_map(Stream* st, Flatten op, size_t newElementSize);
void* strm_collect(Stream* st, Collector collector);
Stream* strm_filter(Stream* st, Predicate pred);
Stream* al_strm_of(ArrayList* list);

#endif
