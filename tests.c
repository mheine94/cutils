#include <stdio.h>
#include "arraylist.h"
#include "linkedlist.h"
#include "stream.h"
#include "assert.h"

void printIntArrayList(ArrayList *list) {
    printf("[");
    for (int i = 0; i < list->length; ++i) {
        printf(" %d ", *((int *)al_get(list, i)));
    }
    printf("]");
}

void printIntLinkedList(LinkedList *list) {
    printf("[");
    Node *current = list->nodes;
    while (current != NULL) {
        printf(" %d ", (int)current->value);
        current = current->next;
    }
    printf("]");
}

void testArrayList() {
    printf("ArrayList Test:\n");

    ArrayList *intList = al_new(sizeof(int));


    int a = 10, b = 20, c = 30;
    al_add(intList, &a);
    al_add(intList, &b);
    al_add(intList, &c);

    // Print elements
    printf("Elements in the ArrayList: ");
    printIntArrayList(intList);
    printf("\n");
 
    int d = 5;
    al_prepend(intList, &d);
    printf("ArrayList after prepending 5: ");
    printIntArrayList(intList);
    printf("\n");

    int e = 15;
    al_insertAt(intList, 2, &e);
    printf("ArrayList after inserting 15 at index 2: ");
    printIntArrayList(intList);
    printf("\n");


    al_removeAt(intList, 3);
    printf("ArrayList after removing at index 3: ");
    printIntArrayList(intList);
    printf("\n");

    assert(intList->length == 4);
    assert(*(int *)al_get(intList, 0) == d);
    assert(*(int *)al_get(intList, 1) == a);
    assert(*(int *)al_get(intList, 2) == e);
    assert(*(int *)al_get(intList, 3) == c);

    al_free(intList);
}

void testLinkedList() {
    printf("\nLinkedList Test:\n");

    LinkedList *intLinkedList = ll_new(sizeof(int));


    int a = 10, b = 20, c = 30;
    ll_add(intLinkedList, &a);
    ll_add(intLinkedList, &b);
    ll_add(intLinkedList, &c);

    printf("Elements in the LinkedList: ");
    assert(intLinkedList->length == 3);
    printIntLinkedList(intLinkedList);
    printf("\n");


    int d = 5;
    ll_prepend(intLinkedList, &d);
    printf("LinkedList after prepending 5: ");
    assert(intLinkedList->length == 4);
    printIntLinkedList(intLinkedList);
    printf("\n");


    int e = 15;
    ll_insertAt(intLinkedList, 2, &e);
    printf("LinkedList after inserting 15 at index 2: ");
    printIntLinkedList(intLinkedList);
    assert(intLinkedList->length == 5);
    printf("\n");


    ll_removeAt(intLinkedList, 3);
    printf("LinkedList after removing at index 3: ");
    assert(intLinkedList->length == 4);
    printIntLinkedList(intLinkedList);
    printf("\n");

    assert(intLinkedList->length == 4);
    assert(*(int *)ll_get(intLinkedList, 0) == d);
    assert(*(int *)ll_get(intLinkedList, 1) == a);
    assert(*(int *)ll_get(intLinkedList, 2) == e);
    assert(*(int *)ll_get(intLinkedList, 3) == c);

    ll_free(intLinkedList);
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

void printTestArrayList(ArrayList *list) {
    for (int i = 0; i < list->length; i++) {
        printf("%d ", ((struct test *)al_get(list, i))->a);
    }
}

void printArrayOfIntArrayList(ArrayList *list) {
    printf("[");
    for (int i = 0; i < list->length; i++) {
        printIntArrayList(*(ArrayList**)al_get(list, i));
    }
    printf("]");
}

Stream* flattenArrayList(void* value){
    Stream* strm = al_strm_of(*(ArrayList**) value);
    return strm;
}
void testStreamDifferentCollector(){
    printf("\nStream collect to linkedlist Test:\n");
    int n1 = 1;
    int n2 = 2;
    int n3 = 3;

    ArrayList* al = al_new(sizeof(ArrayList*));
    al_add(al, &n1);
    al_add(al, &n2);
    al_add(al, &n3);

    printf("stream source\n");
    printIntArrayList(al);
    printf("\n");
    Stream* st = al_strm_of(al);

    printf("operations:\n");
    printf(" - none\n");

    Collector c = {
        .allocate = (Allocator) ll_new,
        .accumulate = (Accumulator) ll_add
    };

    LinkedList* collected = (LinkedList*) strm_collect(st, c);

    printf("\n");
    assert(collected->length == 3);
    assert(*(int *)ll_get(collected, 0) == 1);
    assert(*(int *)ll_get(collected, 1) == 2);
    assert(*(int *)ll_get(collected, 2) == 3);
}

void testStreamFilter(){
    printf("\nStream filter Test:\n");
    int n1 = 1;
    int n2 = 2;
    int n3 = 3;

    ArrayList* al = al_new(sizeof(ArrayList*));
    al_add(al, &n1);
    al_add(al, &n2);
    al_add(al, &n3);

    printf("stream source\n");
    printIntArrayList(al);
    printf("\n");
    Stream* st = al_strm_of(al);

    printf("operations:\n");
    printf(" - filter even\n");
    strm_filter(st, even);

    Collector c = {
        .allocate = (Allocator) al_new,
        .accumulate = (Accumulator) al_add
    };

    ArrayList* collected = (ArrayList*) strm_collect(st, c);
    printf("collected result\n");
    printIntArrayList(collected);

    printf("\n");
    assert(collected->length == 1);
    assert(*(int *)al_get(collected, 0) == 2);
  
}
void testStreamMapTo(){
    printf("\nStream MapTo Test:\n");
    int n1 = 1;
    int n2 = 2;
    int n3 = 3;

    ArrayList* al = al_new(sizeof(ArrayList*));
    al_add(al, &n1);
    al_add(al, &n2);
    al_add(al, &n3);

    printf("stream source\n");
    printIntArrayList(al);
    printf("\n");
    Stream* st = al_strm_of(al);

    printf("operations:\n");
    printf(" - map to double\n");
    strm_map_to(st, toDouble, sizeof(double));

    Collector c = {
        .allocate = (Allocator) al_new,
        .accumulate = (Accumulator) al_add
    };

    ArrayList* collected = (ArrayList*) strm_collect(st, c);
    printf("collected result\n");
    printDoubleArrayList(collected);

    printf("\n");
    assert(collected->length == 3);
    assert(*(double *)al_get(collected, 0) == 1.0);
    assert(*(double *)al_get(collected, 1) == 2.0);
    assert(*(double *)al_get(collected, 2) == 3.0);
  
}

void testStreamFlatmap(){
    printf("\nStream Flatmap Test:\n");
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
    printArrayOfIntArrayList(al);
    printf("\n");
    Stream* st = al_strm_of(al);
    //strm_filter(&st,even);
    //strm_map(st, printInt);
    //strm_filter(&st,unEven);
    //strm_map_to(&st, toTest, sizeof(struct test));
    //strm_map(&st, addOne);
    //strm_map_to(&st, toDouble, sizeof(double));
    printf("operations:\n");
    printf(" - flatmap\n");
    strm_flat_map(st, flattenArrayList,  sizeof(int));
    printf(" - filter even\n");
    strm_filter(st,even);
    printf(" - addone\n");
    strm_map(st, addOne);

    //strm_map(st, doubleIt);
    //strm_filter(&st,even);

    Collector c = {
        .allocate = (Allocator) al_new,
        .accumulate = (Accumulator) al_add
    };

    ArrayList* collected = (ArrayList*) strm_collect(st, c);
    printf("collected result\n");
    printIntArrayList(collected);
    printf("\n");
    assert(collected->length == 3);
    assert(*(int *)al_get(collected, 0) == 3);
    assert(*(int *)al_get(collected, 1) == 5);
    assert(*(int *)al_get(collected, 2) == 7);
}

int main() {
    testArrayList();
    testLinkedList();
    testStreamDifferentCollector();
    testStreamFilter();
    testStreamMapTo();
    testStreamFlatmap();
    return 0;
}
