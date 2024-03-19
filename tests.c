#include <stdio.h>
#include "arraylist.h"
#include "linkedlist.h"

void printIntArrayList(ArrayList *list) {
    for (int i = 0; i < list->length; ++i) {
        printf("%d ", *((int *)al_get(list, i)));
    }
    printf("\n");
}

void printIntLinkedList(LinkedList *list) {
    Node *current = list->nodes;
    while (current != NULL) {
        printf("%d ", (int)current->value);
        current = current->next;
    }
    printf("\n");
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

 
    int d = 5;
    al_prepend(intList, &d);
    printf("ArrayList after prepending 5: ");
    printIntArrayList(intList);


    int e = 15;
    al_insertAt(intList, 2, &e);
    printf("ArrayList after inserting 15 at index 2: ");
    printIntArrayList(intList);


    al_removeAt(intList, 3);
    printf("ArrayList after removing at index 3: ");
    printIntArrayList(intList);


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
    printIntLinkedList(intLinkedList);


    int d = 5;
    ll_prepend(intLinkedList, &d);
    printf("LinkedList after prepending 5: ");
    printIntLinkedList(intLinkedList);


    int e = 15;
    ll_insertAt(intLinkedList, 2, &e);
    printf("LinkedList after inserting 15 at index 2: ");
    printIntLinkedList(intLinkedList);


    ll_removeAt(intLinkedList, 3);
    printf("LinkedList after removing at index 3: ");
    printIntLinkedList(intLinkedList);


    ll_free(intLinkedList);
}

int main() {
    testArrayList();
    testLinkedList();

    return 0;
}
