#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child Process
        execl("/usr/bin/sleep", "sleep", "100", NULL);

        perror("execl");
        exit(1);
    } else if (pid > 0) {
        // Parent Process
        int status;

        sleep(2);

        kill(pid, SIGKILL);

        waitpid(pid, &status, 0);

        if (WIFSIGNALED(status)) {
            printf("Child terminated by signal %d\n", WTERMSIG(status));
        }
        printf("Child Reaped\n");
        exit(0);
    }

    return 0;
}