#include "arraylist.h"
#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct st Stream;

typedef struct {
    int pos;
    void* current;
    size_t elementSize;
    Stream* origin;
    int transformPos;
} StreamElement;
typedef void* (*Operation)(void* value);
typedef Stream* (*Flatten)(void* value);
typedef int (*Predicate)(void* value);

typedef enum t_so {
    FILTER,
    TRANSFORM,
    TRANSFORM_NEW_T,
    FLATMAP
} StreamOperationType;

typedef StreamElement (*Transform)(StreamElement se, Operation);
typedef StreamElement (*Filter)(StreamElement se, Predicate);
typedef struct se{
    size_t pos;
    StreamOperationType type;
    Operation op;
    Predicate pred; 
    Flatten flat;
    size_t newElementSize;
} StreamOperation ;

typedef struct st {
    ArrayList* source;
    int operationCount;
    int isFlat;
    StreamElement nextOriginElement;
    ArrayList* streamOps;
    StreamElement (*getNext)(void* strm,  StreamElement se);
} Stream;

typedef StreamElement(*Transformer)(StreamElement se);
StreamElement strm_getNext(Stream* st, StreamElement se){
    if(se.pos + 1 < st->source->length){    
        StreamElement next = {
            .pos = se.pos+1,
            .current = malloc(st->source->elementSize),
            .elementSize = st->source->elementSize,
            .origin = st,
            .transformPos = se.transformPos
        };
        memcpy(next.current, al_get(st->source, se.pos + 1), st->source->elementSize);
        return next;
    }
     
    StreamElement end = {
        .pos = -1,
        .current = 0,
        .origin = st
    };
    return end;
}

Stream* strm_of(ArrayList* list){
    Stream* new = malloc(sizeof(Stream));
    new->source = list;
    new->getNext = (StreamElement (*)(void*, StreamElement se)) strm_getNext;
    new->operationCount = 0;
    new->streamOps = al_new(sizeof(StreamOperation));
    return new;
}

StreamElement applyUntil(Stream* st, StreamElement se, int index);

StreamElement pullNext(Stream* st, StreamElement se, StreamOperation o){
        StreamElement nextElement =  strm_getNext(se.origin, se);
        if(nextElement.pos == -1 && st->isFlat == 0){
            return nextElement;
        } else if(nextElement.pos == -1 && st->isFlat == 1){
            return nextElement;
        }
        nextElement = applyUntil(st, nextElement, o.pos);
        return nextElement;
}

StreamElement applyFilter(Stream* st, StreamElement se, StreamOperation o){
    if(o.pred(se.current) == 1) {
        return se;
    }else{
        StreamElement nextElement =  pullNext(st, se, o);
        if(nextElement.pos == -1){
            return nextElement;
        }
        return applyFilter(st, nextElement, o);
    }
}

StreamElement strm_start(Stream* st);
StreamElement apply(Stream* st, StreamElement se, StreamOperation o){
    if(se.pos == -1){
        return se;
    }
    if(o.type == TRANSFORM) {
        void* result = o.op(se.current);
        se.current = result;
        return se;
    } else if(o.type == TRANSFORM_NEW_T){
        void* result = o.op(se.current);
        se.current = result;
        se.elementSize = o.newElementSize;
        return se;
    } else if(o.type == FLATMAP){
        StreamElement nextElement =  pullNext(st, se, o);
        Stream* newStream = o.flat(se.current);
        free(newStream->streamOps);

        newStream->streamOps = st->streamOps;
        newStream->operationCount = st->operationCount;

        newStream->isFlat = 1;
        newStream->nextOriginElement = nextElement;

        StreamElement startElem = strm_start(newStream);
        StreamElement first = strm_getNext(newStream, startElem);

        if(first.pos == -1) {
            return nextElement;
        }

        first.transformPos = o.pos + 1;
        first.origin = newStream;
        return first;
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
        .pos = st->operationCount
    };
    al_add(st->streamOps, &so);
    st->operationCount++;
    return st;
}

Stream* strm_flat_map(Stream* st, Flatten op, size_t newElementSize){
    StreamOperation so = {
        .type = FLATMAP,
        .flat = op,
        .newElementSize = newElementSize,
        .pos = st->operationCount
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
        .pos = st->operationCount
    };
    al_add(st->streamOps, &so);
    st->operationCount++;
    return st;
}


Stream* strm_filter(Stream* st, Predicate pred){
    StreamOperation so = {
        .type = FILTER,
        .pred = pred,
        .pos = st->operationCount,
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
        .current = 0,
        .elementSize = st->source->elementSize,
        .origin = st,
        .transformPos = 0
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
    int from = 0;
    if(se.origin->isFlat == 1) {
        from = se.transformPos;
    }
    for(int i = from; i < st->operationCount && i < index; i++){
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

        if(se.origin->isFlat == 1 && se.pos == -1){
            se = se.origin->nextOriginElement;
        }else{
            se = se.origin->getNext(se.origin, se);
        }

        if(se.pos != -1) {
            se = applyUntil(se.origin, se, se.origin->streamOps->length);
        }

        if(se.pos != -1) {
            if (collection == 0) {
                collection = collector.allocate(se.elementSize);
            }

            stop = collector.accumulate(collection, se.current);
        }

    } while (stop != 1 && (se.pos != -1 || se.origin->isFlat));

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
    for (int i = 0; i < list->length; i++) {
        printf("%f ", *((double *)al_get(list, i)));
    }
}

void printIntArrayList(ArrayList *list) {
    for (int i = 0; i < list->length; i++) {
        printf("%d ", *((int *)al_get(list, i)));
    }
}

void printTestArrayList(ArrayList *list) {
    for (int i = 0; i < list->length; i++) {
        printf("%d ", ((struct test *)al_get(list, i))->a);
    }
}

void printArrayOfIntArrayList(ArrayList *list) {
    for (int i = 0; i < list->length; i++) {
        printf("[");
        printIntArrayList(*(ArrayList**)al_get(list, i));
        printf("]");
    }
}

Stream* flattenArrayList(void* value){
    Stream* strm = strm_of(*(ArrayList**) value);
    return strm;
}

int main(){
    int n1 = 1;
    int n2 = 2;
    int n3 = 3;
    int n4 = 4;
    int n5 = 5;
    int n6 = 6;

    ArrayList* nested_1 = al_new(sizeof(int));
    al_add(nested_1,&n1);
    al_add(nested_1,&n2);
    al_add(nested_1,&n3);

    ArrayList* nested_2 = al_new(sizeof(int));
    al_add(nested_2,&n4);
    al_add(nested_2,&n5);
    al_add(nested_2,&n6);

    ArrayList* al = al_new(sizeof(ArrayList*));
    al_add(al,&nested_1);
    al_add(al,&nested_2);

    printf("stream source\n");
    printf("[ ");
    printArrayOfIntArrayList(al);
    printf("]\n");
    Stream* st = strm_of(al);
    //strm_filter(&st,even);
    //strm_map(st, printInt);
    //strm_filter(&st,unEven);
    //strm_map_to(&st, toTest, sizeof(struct test));
    //strm_map(&st, addOne);
    //strm_map_to(&st, toDouble, sizeof(double));
    strm_flat_map(st, flattenArrayList,  sizeof(int));
    strm_map(st, printInt);
    strm_filter(st,even);
    strm_map(st, addOne);

    //strm_map(st, doubleIt);
    //strm_filter(&st,even);

    Collector c = {
        .allocate = (Allocator) al_new,
        .accumulate = (Accumulator) al_add
    };

    ArrayList* collected = (ArrayList*) strm_collect(st, c);
    printf("collected result\n");
    printf("[ ");
    printIntArrayList(collected);
    printf("]\n");
}
