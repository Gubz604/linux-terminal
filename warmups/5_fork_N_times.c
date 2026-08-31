#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    int children_created = 0;

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed\n");
            exit(1);
        } else if (pid == 0) {
            // Child Process
            printf("PID: %d\n", getpid());
        } else if (pid > 0) {
            // Parent Process
            children_created++;
        }
    }

    for (int i = 0; i < children_created; i++) {
        wait(NULL);
    }

    return 0;
}