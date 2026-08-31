#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child Process
        execl("/usr/bin/ls", "ls", "-l", NULL);

        perror("execl");
        exit(1);
    } else if (pid > 0) {
        // Parent Process
        int status;

        waitpid(pid, &status, 0);
        exit(0);
    }

    return 0;
}