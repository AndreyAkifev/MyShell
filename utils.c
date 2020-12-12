#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "utils.h"
#include "structures.h"

bool IsStrEquals(const char* s1, const char* s2) {
    while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
        ++s1;
        ++s2;
    }
    return (*s1 == *s2);
}

bool IsEndOfInput(int c) {
    return (c == '\n' || c == EOF);
}

bool IsSep(char c) {
    return (c != '<' && c != '>' && c != '&' && c != '|');
}

void PrintInvitation() {
    printf("\033[0;32m> \033[0m");
    fflush(stdout);
}

static void PrintVector(char** vec) {
    char** temp = vec;
    while (*temp != NULL) {
        printf("%s ", *temp);
        temp++;
    }
}

void PrintInput(CommandsList* list) {
    if (list -> in_fd != NULL) {
        printf("input file: %s\n", (list -> in_fd)[0]);
    } else {
        printf("input file: stdin\n");
    }
    if (list -> out_fd != NULL) {
        printf("output file: %s\n", (list -> out_fd)[0]);
    } else {
        printf("input file: stdout\n");
    }
    printf("background: %d\n", list -> background);
    printf("append: %d\n", list -> append);
    printf("pipe: \n");
    for (int i = 0; i < list -> pipe_len; i++) {
        PrintVector((list -> pipe)[i]);
        printf("\n");
    }
    printf("-----------------\n");
}