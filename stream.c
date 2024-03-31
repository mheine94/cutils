#include "stream.h"

typedef void* (*Operation)(void* value);
typedef Stream* (*Flatten)(void* value);
typedef int (*Predicate)(void* value);

typedef StreamElement (*Transform)(StreamElement se, Operation);
typedef StreamElement (*Filter)(StreamElement se, Predicate);
typedef StreamElement(*Transformer)(StreamElement se);

StreamElement al_strm_getNext(Stream* st, StreamElement se){
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

Stream* al_strm_of(ArrayList* list){
    Stream* new = malloc(sizeof(Stream));
    new->source = list;
    new->getNext = (StreamElement (*)(void*, StreamElement se)) al_strm_getNext;
    new->operationCount = 0;
    new->streamOps = al_new(sizeof(StreamOperation));
    return new;
}

StreamElement applyUntil(Stream* st, StreamElement se, int index);

StreamElement pullNext(Stream* st, StreamElement se, StreamOperation o){
        StreamElement nextElement =  st->getNext(se.origin, se);
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
        StreamElement first = newStream->getNext(newStream, startElem);

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

