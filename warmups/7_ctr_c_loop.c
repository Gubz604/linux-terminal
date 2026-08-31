#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int sig) {
    const char message[] = "\nI will print forever\n";
    write(STDOUT_FILENO, message, sizeof(message) - 1);
}


int main() {
    signal(SIGINT, handle_sigint);

    while(1) {
        printf("Program is running...\n");
        sleep(1);
    }
}