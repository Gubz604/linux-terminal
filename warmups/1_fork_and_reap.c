#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed\n");
    } else if (pid == 0) {
        printf("In child process\n");
        exit(0);
    } else if (pid > 0) {
        int status;

        waitpid(pid, &status, 0);
        printf("In Parent Process -- Child process %d\n", pid);
    }
    
    return 0;
}