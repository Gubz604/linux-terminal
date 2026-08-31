#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed\n");
    } else if (pid == 0) {
        // Child Process
        pid_t child_pid = getpid();

        printf("Child process PID %d\n", child_pid);
        exit(0);
    } else if (pid > 0) {
        // Parent Process
        int status;

        pid_t reaped_child = waitpid(pid, &status, 0);
        printf("Child reaped PID: %d\n", reaped_child);
        exit(0);
    }
    
    return 0;
}