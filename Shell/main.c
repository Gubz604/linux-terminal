#include <sys/wait.h>
#include <sys/types.h>
#include<stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {

    char user_input[1024];

    while (1) {
        printf("$ ");
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
            break;
        } 

        user_input[strcspn(user_input, "\n")] = '\0';

        if (strcmp(user_input, "q") == 0) {
            break;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child Process

            char *args[64];

            int i = 0;

            char *token = strtok(user_input, " \t\n");

            while (token != NULL) {
                args[i] = token;
                i++;

                token = strtok(NULL, " \t\n");
            }

            args[i] = NULL;

            execvp(args[0], args);
            exit(1);
        } else if (pid > 0) {
            // Parent Process
            int status;

            waitpid(pid, &status, 0);
        }
    } 

    return 0;
}