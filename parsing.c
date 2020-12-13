#include <stdio.h>

#include "parsing.h"
#include "structures.h"
#include "utils.h"
#include "commands_execution.h"

static int ReadCommand(Vector* cmd, int* c) {
    while (IsSep(*c) && !IsEndOfInput(*c)) {
        String word;
        InitString(&word);
        bool word_existance = false;
        while (*c == ' ') {
            *c = getchar();
            if (IsEndOfInput(*c)) break;
        }
        while (*c != ' ' && !IsEndOfInput(*c)) {
            if (*c == '\'') {
                word_existance = true;
                while ((*c = getchar()) != '\'') {
                    if (*c == EOF) {
                        fprintf(stderr, "\033[0;31mwrong input\033[0m\n");
                        return false;
                    }
                    if (*c == '\n') {
                        printf("> ");
                    }
                    AppendSymbolToString(&word, *c);
                }
            }
            if (*c == '\"') {
                word_existance = true;
                while ((*c = getchar()) != '\"') {
                    if (*c == EOF) {
                        fprintf(stderr, "\033[0;31mwrong input\033[0m\n");
                        return false;
                    }
                    if (*c == '\n') {
                        printf("> ");
                    }
                    if (*c == '\\') {
                        char next = getchar();
                        if (next == EOF) {
                            fprintf(stderr, "\033[0;31mwrong input\033[0m\n");
                            return false;
                        }
                        if (next == '\n') {
                            continue;
                        } else if (next == '\"') {
                            AppendSymbolToString(&word, next);
                            continue;
                        } else {
                            AppendSymbolToString(&word, *c);
                            AppendSymbolToString(&word, next);
                            continue;
                        }
                    }
                    AppendSymbolToString(&word, *c);
                }
            }
            if (*c == '\\') {
                int next = getchar();
                if (next == EOF) {
                    fprintf(stderr, "\033[0;31mwrong input\033[0m\n");
                    return false;
                }
                if (next == '\n') {
                    *c = getchar();
                    continue;
                } else {
                    AppendSymbolToString(&word, *c);
                    AppendSymbolToString(&word, next);
                    *c = getchar();
                    continue;
                }
            }
            if (!IsSep(*c)) {
                break;
            }
            if (*c != '\'' && *c != '\"') {
                AppendSymbolToString(&word, *c);
            }
            *c = getchar();
        }
        if (word.len || word_existance) {
            AppendStringToVector(cmd, GetCString(&word));
        }    
        FreeString(&word);
        if (!IsEndOfInput(*c) && IsSep(*c)) {
            *c = getchar();
        }
    }
    return true;
}

bool ProcessLeftArrow(CommandsList* list, Vector* cmd, int* c) {
    if (cmd -> len != 0) {
        AppendCommandToPipe(list, GetCommand(cmd));
    }
    if (list -> pipe_len > 1) {
        fprintf(stderr, 
                "\033[0;31mredirecting of input is allowed ");
        fprintf(stderr, 
                "only in the first command of conveyor\033[0m\n");
        return false;
    }
    *c = getchar();
    if (IsEndOfInput(*c)) {
        fprintf(stderr,
                "\033[0;31minvalid file name\033[0m\n");
        return false;
    }
    if (!ReadCommand(cmd, c)) return false;
    if (cmd -> len != 1) {
        fprintf(stderr,
                "\033[0;31minvalid file name\033[0m\n");
        return false;
    } else if (list -> in_fd != NULL) {
        fprintf(stderr, 
                "\033[0;31mtoo many redirectings\033[0m\n");
        return false;
    } else {
        list -> in_fd = GetCommand(cmd);
    }
    if (*c == '>') {
        if (!ProcessRightArrow(list, cmd, c)) {
            return false;
        }
    } else if (*c == '<') {
        fprintf(stderr, 
                "\033[0;31mtoo many redirectings\033[0m\n");
        return false;
    }
    return true;
}

bool ProcessRightArrow(CommandsList* list, Vector* cmd, int* c) {
    if (cmd -> len != 0) {
        AppendCommandToPipe(list, GetCommand(cmd));
    }
    *c = getchar();
    if (IsEndOfInput(*c)) {
        fprintf(stderr,
                "\033[0;31minvalid file name\033[0m\n");
        return false;
    } else if (*c == '>') {
        list -> append = true;
        *c = getchar();
    }
    if (!ReadCommand(cmd, c)) return false;
    if (cmd -> len != 1) {
        fprintf(stderr,
                "\033[0;31minvalid file name\033[0m\n");
        return false;
    } else if (list -> out_fd != NULL) {
        fprintf(stderr, 
                "\033[0;31mtoo many redirectings\033[0m\n");
        return false;
    } else {
        list -> out_fd = GetCommand(cmd);
    }
    if (*c == '<') {
        if (!ProcessLeftArrow(list, cmd, c)) {
            return false;
        }
    } else if (*c == '>') {
        fprintf(stderr, 
                "\033[0;31mtoo many redirectings\033[0m\n");
        return false;
    }
    return true;
}

bool ParseInput(int* c) {
    bool is_end_of_input = false;
    do {
        CommandsList list;
        InitCommandsList(&list);
        bool is_list_inited = true;
        int sticks_counter = 0;
        while (!IsEndOfInput(*c = getchar())) {
            Vector cmd;
            InitVector(&cmd);
            bool last_sep_was_stick = false;
            if (!ReadCommand(&cmd, c)) {
                ClearInfo(&list, &cmd);
                while (!IsEndOfInput(getchar()));
                return false;
            }
            if (*c == '>') {
                if (!ProcessRightArrow(&list, &cmd, c)) {
                    ClearInfo(&list, &cmd);
                    while (!IsEndOfInput(getchar()));
                    return false;
                }
            }
            if (*c == '<') {
                if (!ProcessLeftArrow(&list, &cmd, c)) {
                    ClearInfo(&list, &cmd);
                    while (!IsEndOfInput(getchar()));
                    return false;
                }
            }
            if (*c == '|') {
                last_sep_was_stick = true;
                sticks_counter++;
                if (cmd.len == 0 && sticks_counter > list.pipe_len) {
                    fprintf(stderr, 
                            "\033[0;31msyntax error near unexpected token `|'\033[0m\n");
                    ClearInfo(&list, &cmd);
                    while (!IsEndOfInput(getchar()));
                    return false;
                } else if (list.out_fd != NULL) {
                    fprintf(stderr, 
                            "\033[0;31mredirecting of output is allowed ");
                    fprintf(stderr, 
                            "only in the last command of conveyor\033[0m\n");
                    ClearInfo(&list, &cmd);
                    while (!IsEndOfInput(getchar()));
                    return false;
                } else if ((list.in_fd == NULL || last_sep_was_stick) &&
                            cmd.len > 0) {
                    AppendCommandToPipe(&list, GetCommand(&cmd));
                }
            } else if (*c == '&') {
                if (sticks_counter + 1 != list.pipe_len) {
                    if (cmd.len > 0) {
                        AppendCommandToPipe(&list, GetCommand(&cmd));
                    } else {
                        fprintf(stderr, 
                                "\033[0;31msyntax error near unexpected token `&'\033[0m\n");
                        ClearInfo(&list, &cmd);
                        while (!IsEndOfInput(getchar()));
                        return false;
                    }
                } 
                list.background = true;
                is_end_of_input = RunInput(&list, sticks_counter);
                //PrintInput(&list);
                ClearInfo(&list, &cmd);
                is_list_inited = false;
                break;
            } else {
                if (cmd.len != 0) {
                    AppendCommandToPipe(&list, GetCommand(&cmd));
                }
            }
            FreeVector(&cmd);
            if (IsEndOfInput(*c)) {
                break;
            }
        }
        if (is_list_inited) {
            if (list.pipe_len != 0) {
                is_end_of_input = RunInput(&list, sticks_counter);
                //PrintInput(&list);
            }
            FreeCommandsList(&list);
        } 
    } while (!IsEndOfInput(*c));
    return is_end_of_input;
}
