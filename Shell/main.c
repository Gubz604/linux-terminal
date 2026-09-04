#include <sys/wait.h>
#include <sys/types.h>
#include<stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    char cwd[1024];
    char user_input[1024];

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s $ ", cwd);
        } else {
            perror("getcwd");
        }

        if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
            break;
        } 

        user_input[strcspn(user_input, "\n")] = '\0';

        if (strcmp(user_input, "q") == 0) {
            break;
        }

        if (user_input[0] == '\0') {
            continue;
        }

        char *args[64];

        int i = 0;

        char *token = strtok(user_input, " \t\n");

        while (token != NULL) {
            args[i] = token;
            i++;

            token = strtok(NULL, " \t\n");
        }

        args[i] = NULL;

        if (strcmp(args[0], "cd") == 0) {
            if (chdir(args[1]) == -1) {
                perror("chdir");
            }

            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child Process

            execvp(args[0], args);
            exit(1);
        } else if (pid > 0) {
            // Parent Process
            int status;

            waitpid(pid, &status, 0);

            if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
                printf("\nCommand \'%s\' not found\n", args[0]);
            }
        }
    } 

    return 0;
}