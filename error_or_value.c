#include <stdlib.h>
#include <stdio.h>

typedef struct eov {
    int isError;
    int msgLen;
    char* errorMsg;
    void* value;
} ErrorOrValue;

char* copyMsg(char* errorMsg, int msgLen){
    char* copy = malloc(msgLen);
    if(copy == NULL){
        printf("failed to allocate memory");
        exit(1);
    }
    for(int i = 0; i < msgLen; i++){
        copy[i] = errorMsg[i];
    }
    return copy;
}

int msgLen(char* string){
    int len = 0;
    while(string[len] != '\0'){
        len++;
    }
    return len;
}

void* copyData(void* memory, size_t length){
    char* source = memory;
    char* copy = malloc(length);
    if(copy == NULL){
        printf("failed to allocate memory");
        exit(1);
    }
    for(int i = 0; i < length; i++){
        copy[i] = source[i];
    }
    return copy;
}

ErrorOrValue eov_error_of(char* errorMsg, int msgLen){
    ErrorOrValue error = {
            .isError = 1,
            .msgLen = msgLen,
            .errorMsg = copyMsg(errorMsg, msgLen),
    };
    return error;
}

ErrorOrValue eov_value_of(void* value){
    ErrorOrValue newValue = {
            .isError = 0,
            .value = value
    };
    return newValue;
}

ErrorOrValue eov_copy_value_of(void* value, size_t elementSize){
    ErrorOrValue newValue = {
            .isError = 0,
            .value = copyData(value, elementSize)
    };
    return newValue;
}


void eov_print(ErrorOrValue eov){
    if(eov.isError == 1){
        printf("Error: '%s'\n", eov.errorMsg);
    }else{
        printf("Value: '%s'\n", (char*)eov.value);
    }
}

void eov_free(ErrorOrValue eov){
    if(eov.isError == 1){
        free(eov.errorMsg);
    }else{
        free(eov.value);
    }
}

int main() {

    char msg[] = "something went wrong";
    ErrorOrValue eov_error = eov_error_of(msg, msgLen(msg));

    char* value = malloc(sizeof(char) * 10);
    for(int i = 0; i< 10; i++){
        value[i] = '0'+i;
    }
    ErrorOrValue eov_ref_value = eov_value_of(value);

    char stringValue[] = "12 asdf 321";
    ErrorOrValue eov_copy_value = eov_copy_value_of(&stringValue, sizeof(stringValue));

    eov_print(eov_error);
    eov_print(eov_ref_value);
    eov_print(eov_copy_value);

    eov_free(eov_error);
    eov_free(eov_ref_value);
    eov_free(eov_copy_value);
}