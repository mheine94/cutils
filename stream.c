#include "arraylist.h"
#include "linkedlist.h"
#include <stdio.h>

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
    printf("strm_getNext - st source len is %d and se pos is %d\n", st->source->length, se.pos);
    if(se.pos < st->source->length){    
        StreamElement next = {
            .pos = se.pos+1,
            .curernt = al_get(st->source, se.pos)
        };
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

StreamElement apply(Stream* st, StreamElement se, StreamOperation o){
    printf("apply\n");
    printf("current is %d\n", *((int*)(se.curernt)));
    if(o.type == TRANSFORM) {
        
        void* result = o.op(se.curernt);
        // what about the old value?
        se.curernt = result;
        return se;
    } else if(o.type == TRANSFORM_NEW_T){
        void* result = o.op(se.curernt);
        // what about the old value?
        se.curernt = result;
        se.elementSize = o.newElementSize;
        return se;
    } else if(o.type == FILTER){
        if(o.pred(se.curernt) == 1) {
            printf("kept by filter\n");
            return se;
        }else{
            printf("filtered out\n");
            return strm_getNext(st, se);
        }
        return se;
    }
}

Stream* strm_map(Stream* st, Operation op){
    StreamOperation so = {
        .type = TRANSFORM,
        .op = op,
        .newElementSize = 0
    };
    al_add(st->streamOps, &so);
    st->operationCount++;
    return st;
}
Stream* strm_map_to(Stream* st, Operation op, size_t newElementSize){
    StreamOperation so = {
        .type = TRANSFORM_NEW_T,
        .op = op,
        .newElementSize = newElementSize
    };
    al_add(st->streamOps, &so);
    st->operationCount++;
    return st;
}


Stream* strm_filter(Stream* st, Predicate pred){
    //al_add(st.transForms, filter);
    StreamOperation so = {
        .type = FILTER,
        .pred = pred
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
    return st->getNext((void*)st, se);
}

int even(void* val){
    printf("even\n");
    int* i = val;
    return *i % 2 == 0;
}

void* doubleIt(void* val){
    printf("doubleIt\n");
    int* i = val;
    *i = *i+*i;
    return i;
}

void* toDouble(void* val){
    printf("toDouble\n");
    int* i = val;
    double* new = malloc(sizeof(double));
    *new = (double) *i;
    printf("double val %f\n", *new);
    return new;
}

void* strm_collect(Stream* st, Collector collector){
    
    void* collection = 0;

    StreamElement se = strm_start(st);
    int stop = 0;
    do {
        printf("getting next\n");
        se = st->getNext(st, se);
        if(se.pos == -1){
            printf("next was end\n");
            break;
        }
        printf("el pos was %d\n", se.pos);
        printf("el value was %d\n", *((int*) se.curernt));

        for(int i = 0; i < st->operationCount; i++){

            printf("get operand i=%d\n", i);
            StreamOperation so = *((StreamOperation*) al_get(st->streamOps, i));
            se = apply(st, se, so);
        }
        if(collection == 0){
             collection = collector.allocate(se.elementSize);
        }
        stop = collector.accumulate(collection, se.curernt);
    } while (stop == 0);

    return collection;
}
struct test {
    int a;
};

void* toTest(void* val){
    printf("toTest\n");
    int* i = val;
    struct test* new = malloc(sizeof(struct test));
    new->a = 5;
    return new;
}


void* testToInt(void* val){
    printf("toTest\n");
    struct test* value = val;
    int* new = malloc(sizeof(int));
    *new = value->a;
    return new;
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
    al_add(al,&n3);
    al_add(al,&n4);
    al_add(al,&n5);
    al_add(al,&n6);
    Stream st = strm_of(al);
    strm_filter(&st,even); 
    strm_map(&st, doubleIt);
    strm_map_to(&st, toTest, sizeof(struct test));
    strm_map_to(&st, testToInt, sizeof(int));
    strm_map(&st, doubleIt);
    strm_map_to(&st, toDouble, sizeof(double));

    Collector c = {
        .allocate = (Allocator) al_new,
        .accumulate = (Accumulator) al_add
    };
    ArrayList* collected = (ArrayList*) strm_collect(&st, c);
    printf("collected result\n");
    printf("[ ");
    printDoubleArrayList(collected);
    printf(" ]");
}
