#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    while (1)
    {
        printf("Warmup Exercise 1\n");

        pid_t pid = fork();

        if (pid < 0)
        {
            // fork failed
            perror("Fork failed!");
            exit(1);
        }
        else if (pid == 0)
        {
            // child process
            printf("I am child\n");
            exit(0);
        }
        else
        {
            // parent process
            wait(NULL);
            printf("I am parent\n");
            exit(0);
        }
    }
}