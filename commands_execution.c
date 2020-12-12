#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "structures.h"
#include "utils.h"
#include "commands_execution.h"

static bool ChangeInStream(const char* file) {
    int rd = open(file, O_RDONLY);
    if (rd == -1) {
        fprintf(stderr, "\033[0;31mopen: %s \033[0m\n",
                strerror(errno));
        return false;
    }
    if (dup2(rd, 0) == -1) {
        fprintf(stderr, "\033[0;31mdup2: %s \033[0m\n",
                strerror(errno));
        return false;
    }
    close(rd);
    return true;
}

static bool ChangeOutStream(const char* file, bool append) {
    int append_flag = append ? O_APPEND : O_TRUNC;
    int wd = open(file, O_WRONLY | O_CREAT | append_flag, 0666);
    if (wd == -1) {
        fprintf(stderr, "\033[0;31mopen: %s \033[0m\n",
                strerror(errno));
        return false;
    }
    if (dup2(wd, 1) == -1) {
        fprintf(stderr, "\033[0;31mdup2: %s \033[0m\n",
                strerror(errno));
        return false;
    }
    close(wd);
    return true;
}

static bool RunOneCommand(CommandsList* list) {
    char* cmd = (list -> pipe)[0][0];
    if (IsStrEquals(cmd, "exit")) {
        if (!list -> background) {
            return true;
        }
    }
    if (IsStrEquals(cmd, "cd")) {
        if ((list -> pipe)[0][2] != NULL) {
            fprintf(stderr, "\033[0;31mcd: too many arguments \033[0m\n");
        } else if (chdir((list -> pipe)[0][1]) == -1) {
            fprintf(stderr, "\033[0;31mcd: %s \033[0m\n", strerror(errno));
        } else {
            char *buf = getcwd(NULL,0);
            printf("Current directory:\033[0;34m %s\033[0m\n", buf);
            free(buf);
        }
        return false;
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork: %s\n", strerror(errno));
        return false;
    }
    if (!pid) {
        if (list -> in_fd != NULL) {
            if (!ChangeInStream((list -> in_fd)[0])) {
                FreeCommandsList(list);
                _exit(1);
            }
        }
        if (list -> out_fd != NULL) {
            if (!ChangeOutStream((list -> out_fd)[0], list -> append)) {
                FreeCommandsList(list);
                _exit(1);
            }
        }
        execvp(cmd, (list -> pipe)[0]);
        fprintf(stderr, "\033[0;31mexecvp: %s: %s \n\033[0m",
                cmd, strerror(errno));
        _exit(1);
    }
    if (!list -> background) {
        waitpid(pid, NULL, 0);
    }
    return false;
}

static void Conveyor(CommandsList* list) {
    int fd[2];
    int fd_in = -1;
    for (int i = 0; i < list -> pipe_len; i++) {
        if (pipe(fd) == -1){
            close(fd[0]);
            close(fd[1]);
            close(fd_in);
            fprintf(stderr, "\033[0;31mpipe: %s\033[0m\n",
                    strerror(errno));
            break;
        }
        int child = fork();
        if (child < 0){
            close(fd[0]);
            close(fd[1]);
            close(fd_in);
            fprintf(stderr, "fork: %s\n", strerror(errno));
        }
        if (!child) {
            close(fd[0]);
            if (i == 0 && list -> in_fd != NULL) {
                if (!ChangeInStream((list -> in_fd)[0])) {
                    FreeCommandsList(list);
                    _exit(1);
                }
            }
            if (i == list -> pipe_len - 1 && 
                list -> out_fd != NULL) {
                if (!ChangeOutStream((list -> out_fd)[0], list -> append)) {
                    FreeCommandsList(list);
                    _exit(1);
                }
            }
            if (i != 0){
                dup2(fd_in, 0);
            }
            if (i != list -> pipe_len - 1) {
                dup2(fd[1], 1);
            }
            close(fd[1]);
            close(fd_in);
            execvp((list -> pipe)[i][0], (list -> pipe)[i]);
            fprintf(stderr, "\033[0;31mexecvp: %s: %s\033[0m\n",
                    (list -> pipe)[i][0], strerror(errno));
            _exit(1);
        }
        close(fd_in);
        fd_in = fd[0];
        close(fd[1]);
    }
    close(fd[0]);
    if (!list -> background) {
        while (wait(NULL) != -1);
    }
}

bool RunInput(CommandsList* list, int sticks_counter) {
    if (list -> pipe_len == 1 && sticks_counter == 0) {
        return RunOneCommand(list);
    } else {
        if (sticks_counter + 1 == list -> pipe_len) {
            Conveyor(list);
        } else {
            fprintf(stderr, 
                    "\033[0;31mnot enough commands ");
            fprintf(stderr, "for conveyor\033[0m\n");
        }
    }
    return false;
}
