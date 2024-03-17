#include <stdlib.h>

typedef struct {
    void* value;
    void* next;
} Node;

typedef struct {
    int length;
    Node* nodes;
} LinkedList;


LinkedList* ll_new() {
    LinkedList* ll = calloc(1, sizeof(LinkedList));
    return ll;
}

Node* ll_newNode(void* value) {
    Node* node = calloc(1, sizeof(Node));
    node->value = value;
    return node;
}

void ll_add(LinkedList* list, void* value){
    Node* newNode = ll_newNode(value);
    if(list->length == 0){
       list->nodes = newNode;
    }else if(list->length > 0){
        Node* node = 0;
        node = list->nodes;
        while(node->next != 0){
            node = node->next;
        }
        node->next = newNode;
    }
    list->length++;
}

void ll_prepend(LinkedList* list, void* value){
    list->length++;
    Node* head = ll_newNode(value);
    head->next = list->nodes;
    list->nodes = head; 
}
void ll_insertAt(LinkedList* list, int index, void* value){
    if(index == 0 && list->length > 0){
        Node* node = list->nodes;
        list->nodes = ll_newNode(value);
        list->nodes->next = node;
        list->length++;
        return;
    }

    int i = 0;
    Node* node = list->nodes;
    while(node != 0){
        if(i+1 == index && node->next != 0){
            Node* toMove = node->next;
            Node* newNode = ll_newNode(value);
            node->next = newNode;
            newNode->next = toMove;
            list->length++;
            return;
        }else if(node->next != 0){
            i++;
            node = node->next;
        } else{
            return;
        }
    }
}

void ll_removeAt(LinkedList* list, int index){
    if(index == 0 && list->length > 0){
        Node* toRemove = list->nodes;
        list->nodes = toRemove->next;
        list->length--;
        free(toRemove);
        return;
    }

    int i = 0;
    Node* node = list->nodes;
    while(node != 0){
        if(i+1 == index && node->next != 0){
            Node* toRemove = node->next;
            node->next = toRemove->next;
            list->length--;
            free(toRemove);
            return;
        }else if(node->next != 0){
            i++;
            node = node->next;
        } else{
            return;
        }
    }
}

void ll_remove(LinkedList* list, void* value){
    if(list->length > 0 && list->nodes->value == value){
        Node* toRemove = list->nodes;
        list->nodes = toRemove->next;
        list->length--;
        free(toRemove);
        return;
    }

    Node* node = list->nodes;
    while(node != 0){
        if(node->next != 0 && ((Node*)node->next)->value == value){
            Node* toRemove = node->next;
            node->next = toRemove->next;
            list->length--;
            free(toRemove);
            return;
        }else if(node->next != 0){
            node = node->next;
        } else{
            return;
        }
    }
}


void* ll_get(LinkedList* list, int index){
    if(index == 0){
        return list->nodes->value;
    }
    int i = 0;
    Node* node = list->nodes;
    while(node->next != 0){
        node = node->next;
        i++;
        if(i == index){
            return node->value;
        }
    }
    return 0;
}

void ll_free(LinkedList* list){
    if(list->length == 0){
        return;
    }
    Node* current = list->nodes;
    Node* temp = 0;
    while(current != 0){
        temp = current;
        current = current->next;
        free(temp); 
    }
}