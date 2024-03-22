#include "arraylist.h"
#include "linkedlist.h"
#include <stdio.h>

typedef struct {
    int pos;
    void* curernt;
} StreamElement;
typedef void* (*Operation)(void* value);

typedef struct ss {
    ArrayList* source;
    int operationCount ;
    Operation transForms[50];
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
        .operationCount = 0
    };
    return s;
}

StreamElement apply(StreamElement se, Operation o){
    printf("apply\n");
    printf("current is %d\n", *((int*)(se.curernt)));
    void* result = o(se.curernt);

    printf("after double is %d\n", *((int*)(result)));
    // what about the old value?
    se.curernt = result;
    return se;
}

Stream strm_map(Stream* st, Operation op){
    st->transForms[st->operationCount++] = op;
}

Stream strm_filter(Stream st, StreamElement(*filter)(StreamElement se)){
    //al_add(st.transForms, filter);
}

typedef struct {
    void* (*allocate)(size_t elementSize);
    int (*accumulate)(void* collection, void* value);
} Collector;



StreamElement strm_start(Stream* st){
    StreamElement se = {
        -1,
        0        
    };
    return st->getNext((void*)st, se);
}

void* doubleIt(void* val){
    printf("doubleIt\n");
    int* i = val;
    *i = *i+*i;
    return i;
}

void* strm_collect(Stream* st, Collector collector){
    
    void* collection = collector.allocate(st->source->elementSize);

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
            Operation op =(Operation) st->transForms[i];
            se = apply(se, op);
        }

        stop = collector.accumulate(collection, se.curernt);
    } while (stop == 0);

    return collection;
}


int al_addWrapper(void* list, void* value){
    printf("al_addWrapper\n");
    al_add((ArrayList*) list, value);
    return 0;
}

void* al_newWrapper(size_t elementSize){
    return al_new(elementSize);
}
void printIntArrayList(ArrayList *list) {
    for (int i = 0; i < list->length; ++i) {
        printf("%d ", *((int *)al_get(list, i)));
    }
    printf("\n");
}


void main(){
    ArrayList* al = al_new(sizeof(int));
    int a = 2;
    int b = 3;
    int cl = 5;
    al_add(al,&a);
    al_add(al,&b);
    al_add(al,&cl);
    Stream st = strm_of(al);
    strm_map(&st, doubleIt);
    
    Collector c = {
        .allocate = al_newWrapper,
        .accumulate = al_addWrapper
    };
    ArrayList* collected = (ArrayList*) strm_collect(&st, c);
    printf("collected result\n");
    printIntArrayList(collected);
}
