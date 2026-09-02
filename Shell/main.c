#include <sys/wait.h>
#include <sys/types.h>
#include<stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child Process
        char user_input[1024];

        printf("$ ");
        fgets(user_input, sizeof(user_input), stdin);

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
        exit(0);
    }

    return 0;
}