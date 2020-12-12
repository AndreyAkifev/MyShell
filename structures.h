#pragma once

#include <stdbool.h>

typedef struct String {
    char* data;
    int cap;
    int len;
} String;

typedef struct Vector {
    char** elem;
    int cap;
    int len;
} Vector;

typedef struct CommandsList {
    char** in_fd;
    char** out_fd;
    bool background;
    bool append;
    char*** pipe;
    int pipe_cap;
    int pipe_len;
} CommandsList;

typedef struct Pair {

} Pair;

void InitString(String* s);

void InitVector(Vector* arr);

void InitCommandsList(CommandsList* list);

void FreeString(String* s);

void FreeVector(Vector* vec);

void FreeCommandsList(CommandsList* list);

void ClearInfo(CommandsList* list, Vector* vec);

char* GetCString(String* s);

char** GetCommand(Vector* vec);

void AppendSymbolToString(String* s, char c);

void AppendStringToVector(Vector* arr, char* s);

void AppendCommandToPipe(CommandsList* list, 
                         char** command);