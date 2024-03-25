#include "arraylist.h"
#include "linkedlist.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    int pos;
    void* curernt;
    size_t elementSize;
} StreamElement;
typedef void* (*Operation)(void* value);
typedef int (*Predicate)(void* value);

typedef enum t_so {
    FILTER,
    TRANSFORM,
    TRANSFORM_NEW_T
} StreamOperationType;

typedef StreamElement (*Transform)(StreamElement se, Operation);
typedef StreamElement (*Filter)(StreamElement se, Predicate);
typedef struct se{
    size_t pos;
    StreamOperationType type;
    Operation op;
    Predicate pred; 
    size_t newElementSize;
} StreamOperation ;

typedef struct ss {
    ArrayList* source;
    int operationCount;
    ArrayList* streamOps;
    StreamElement (*getNext)(void* strm,  StreamElement se);
} Stream;



typedef StreamElement(*Transformer)(StreamElement se);
StreamElement strm_getNext(Stream* st, StreamElement se){
    if(se.pos + 1 < st->source->length){    
        StreamElement next = {
            .pos = se.pos+1,
            .curernt = malloc(st->source->elementSize),
            .elementSize = st->source->elementSize
        };
        memcpy(next.curernt, al_get(st->source, se.pos + 1), st->source->elementSize);
        return next;
    }
     
    StreamElement end = {
        .pos = -1,
        .curernt = 0,
    };
    return end;
}

Stream strm_of(ArrayList* list){
    Stream s = {
        .source = list,
        .getNext = (StreamElement (*)(void*, StreamElement se)) strm_getNext,
        .operationCount = 0,
        .streamOps = al_new(sizeof(StreamOperation))
    };
    return s;
}

StreamElement applyUntil(Stream* st, StreamElement se, int index);
StreamElement applyFilter(Stream* st, StreamElement se, StreamOperation o){
    if(o.pred(se.curernt) == 1) {
        return se;
    }else{
        StreamElement nextElement =  strm_getNext(st, se);
        if(nextElement.pos == -1){
            return nextElement;
        }
        nextElement = applyUntil(st, nextElement, o.pos);
        return applyFilter(st, nextElement, o);
    }
}

StreamElement apply(Stream* st, StreamElement se, StreamOperation o){
    if(se.pos == -1){
        return se;
    }
    if(o.type == TRANSFORM) {
        void* result = o.op(se.curernt);
        se.curernt = result;
        return se;
    } else if(o.type == TRANSFORM_NEW_T){
        void* result = o.op(se.curernt);
        se.curernt = result;
        se.elementSize = o.newElementSize;
        return se;
    } else if(o.type == FILTER){
        return applyFilter(st, se, o); 
    }
    printf("unsupported\n");
    exit(1);
}

Stream* strm_map(Stream* st, Operation op){
    StreamOperation so = {
        .type = TRANSFORM,
        .op = op,
        .newElementSize = 0,
        .pos = st->streamOps->length
    };
    al_add(st->streamOps, &so);
    st->operationCount++;
    return st;
}
Stream* strm_map_to(Stream* st, Operation op, size_t newElementSize){
    StreamOperation so = {
        .type = TRANSFORM_NEW_T,
        .op = op,
        .newElementSize = newElementSize,
        .pos = st->streamOps->length
    };
    al_add(st->streamOps, &so);
    st->operationCount++;
    return st;
}


Stream* strm_filter(Stream* st, Predicate pred){
    StreamOperation so = {
        .type = FILTER,
        .pred = pred,
        .pos = st->streamOps->length,
        .newElementSize = 0
    };
    al_add(st->streamOps, &so);
    st->operationCount++;
    return st;
}

typedef void* (*Allocator)(size_t elementSize);
typedef int (*Accumulator)(void* collection, void* value);
typedef struct {
    Allocator allocate;
    Accumulator accumulate;
} Collector;



StreamElement strm_start(Stream* st){
    StreamElement se = {
        .pos = -1,
        .curernt = 0,
        .elementSize = st->source->elementSize
    };
    return se;
}

int even(void* val){
    //printf("even\n");
    int* i = val;
    return *i % 2 == 0;
}

int unEven(void* val){
    //printf("even\n");
    
    return 1 - even(val);
}

void* addOne(void* val){
    //printf("even\n");
     int* i = val;
    *i += 1;
    return i;
}

void* doubleIt(void* val){
    //printf("doubleIt\n");
    int* i = val;
    *i = *i+*i;
    return i;
}

void* toDouble(void* val){
    //printf("toDouble\n");
    int* i = val;
    double* new = malloc(sizeof(double));
    *new = (double) *i;
    //printf("double val %f\n", *new);
    return new;
}

StreamElement applyUntil(Stream* st, StreamElement se, int index) {
    for(int i = 0; i < st->operationCount && i < index; i++){
        //printf("apply operand i=%d\n", i);
        StreamOperation so = *((StreamOperation*) al_get(st->streamOps, i));
        se = apply(st, se, so);
    }
    return se; 
}
 
void* strm_collect(Stream* st, Collector collector){
    
    void* collection = 0;

    StreamElement se = strm_start(st);
    int stop = 0;
    do {
        se = st->getNext(st, se);
        if(se.pos == -1){
            break;
        }

        se = applyUntil(st, se, st->streamOps->length);
        if(collection == 0){
             collection = collector.allocate(se.elementSize);
        }

        if(se.pos != -1){
            stop = collector.accumulate(collection, se.curernt);
        }

    } while (stop != 1 && se.pos != -1);

    if(collection == 0){
        collection = collector.allocate(st->source->elementSize);
    }

    return collection;
}
struct test {
    int a;
};

void* toTest(void* val){
    //printf("toTest\n");
    int* i = val;
    struct test* new = malloc(sizeof(struct test));
    new->a = *i;
    free(val);
    return new;
}


void* testToInt(void* val){
    //printf("toTest\n");
    struct test* value = val;
    int* new = malloc(sizeof(int));
    *new = value->a;
    free(val);
    return new;
}

void* printInt(void* val){
    printf("%d\n", *((int *)val));
    return val;
}

void printDoubleArrayList(ArrayList *list) {
    for (int i = 0; i < list->length; ++i) {
        printf("%f ", *((double *)al_get(list, i)));
    }
}

void printIntArrayList(ArrayList *list) {
    for (int i = 0; i < list->length; ++i) {
        printf("%d ", *((int *)al_get(list, i)));
    }
}

void printTestArrayList(ArrayList *list) {
    for (int i = 0; i < list->length; ++i) {
        printf("%d ", ((struct test *)al_get(list, i))->a);
    }
}

void main(){
    // todo support flat map
    ArrayList* al = al_new(sizeof(int));
    int n1 = 1;
    int n2 = 2;
    int n3 = 3;
    int n4 = 4;
    int n5 = 5;
    int n6 = 6;
    al_add(al,&n1);
    al_add(al,&n2);
    al_add(al,&n3);
    al_add(al,&n4);
    al_add(al,&n5);
    al_add(al,&n6);

    printf("stream source\n");
    printf("[ ");
    printIntArrayList(al);
    printf("]\n");
    Stream st = strm_of(al);
    strm_filter(&st,even); 
    strm_map(&st, doubleIt);
    strm_map(&st, printInt);
    strm_filter(&st,unEven); 
    //strm_map_to(&st, toTest, sizeof(struct test));
    //strm_map(&st, addOne);
    //strm_map_to(&st, toDouble, sizeof(double));
    
    Collector c = {
        .allocate = (Allocator) al_new,
        .accumulate = (Accumulator) al_add
    };

    ArrayList* collected = (ArrayList*) strm_collect(&st, c);
    printf("collected result\n");
    printf("[ ");
    printDoubleArrayList(collected);
    printf("]\n");
}
