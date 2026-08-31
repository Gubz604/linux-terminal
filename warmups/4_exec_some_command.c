#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child Process
        char *args[] = {
            "sleep",
            "5",
            NULL
        };

        printf("Process will sleep 5 seconds\n");
        execvp(args[0], args);
        printf("Process finished sleep of 5 seconds\n");
        exit(1);
    } else if (pid > 0) {
        // Parent Process
        int status;

        waitpid(pid, &status, 0);
        exit(0);
    }

    return 0;
}