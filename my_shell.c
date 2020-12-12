#include <stdio.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "structures.h"
#include "parsing.h"
#include "utils.h"
#include "commands_execution.h"

int main() {
    printf("\033[0;32m> Welcome to My Shell! \033[0m\n");
    printf("\033[0;32m> Enter your commands: \033[0m\n");
    bool is_exit = false;
    int c;
    do {
        PrintInvitation();
        is_exit = ParseInput(&c);
        while (waitpid(-1, NULL, WNOHANG) > 0);
    } while (!is_exit && c != EOF);
    printf("\033[0;32mBye! \033[0m\n");
    return 0;
}
