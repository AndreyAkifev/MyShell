#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "structures.h"

void InitString(String* s) {
    s -> len = 0;
    s -> cap = 16;
    s -> data = (char*)malloc(s -> cap * sizeof(char));
    (s -> data)[0] = '\0';
}

void InitVector(Vector* arr) {
    arr -> len = 0;
    arr -> cap = 16;
    arr -> elem = (char**)malloc(arr -> cap * sizeof(char*));
}

void InitCommandsList(CommandsList* list) {
    list -> in_fd = NULL;
    list -> out_fd = NULL;
    list -> background = false;
    list -> append = false;
    list -> pipe_len = 0;
    list -> pipe_cap = 4;
    list -> pipe = (char***)malloc(list -> pipe_cap * sizeof(char**));
    (list -> pipe)[0] = NULL;
}

void FreeString(String* s) {
    free(s -> data);
}

void FreeVector(Vector* vec) {
    for (int i = 0; i < vec -> len; i++) {
        free((vec -> elem)[i]);
    }
    free(vec -> elem);
}

static void ExtraFreeFunction(char** arr) {
    char** temp1 = arr;
        while (*temp1 != NULL) {
            free(*temp1);
            temp1++;
        }
        free(arr);
}

void FreeCommandsList(CommandsList* list) {
    if (list -> in_fd != NULL) {
        ExtraFreeFunction(list -> in_fd);
    }
    if (list -> out_fd != NULL) {
        ExtraFreeFunction(list -> out_fd);
    }
    for (int i = 0; i < list -> pipe_len; i++) {
        ExtraFreeFunction((list -> pipe)[i]);
    }
    free(list -> pipe);
}

void ClearInfo(CommandsList* list, Vector* vec) {
    FreeCommandsList(list);
    FreeVector(vec);
}

char* GetCString(String* s) {
    char* temp = s -> data;
    InitString(s);
    return temp;
}

char** GetCommand(Vector* vec) {
    char** temp = vec -> elem;
    temp[vec -> len] = NULL;
    InitVector(vec);
    return temp;
}

void AppendSymbolToString(String* s, char c) {
    if (s -> len + 1 >= s -> cap) {
        s -> cap *= 2;
        s -> data = (char*)realloc(s -> data, 
                                   s -> cap * sizeof(char));
    }
    (s -> data)[s -> len] = c;
    s -> len += 1;
    (s -> data)[s -> len] = '\0';
}

void AppendStringToVector(Vector* arr, char* s) {
    if (arr -> len + 1 >= arr -> cap) {
        arr -> cap *= 2;
        arr -> elem = (char**)realloc(arr -> elem, 
                                      arr -> cap * sizeof(char*));
    }
    (arr -> elem)[arr -> len] = s;
    arr -> len += 1;
}

void AppendCommandToPipe(CommandsList* list, 
                         char** command) {
    if (list -> pipe_len + 1 >= list -> pipe_cap) {
        list -> pipe_cap *= 2;
        list -> pipe = (char***)realloc(list -> pipe, 
                                        list -> pipe_cap * sizeof(char**));
    }
    (list -> pipe)[list -> pipe_len] = command;
    list -> pipe_len += 1;
    (list -> pipe)[list -> pipe_len] = NULL;
}