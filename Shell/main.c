#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void handle_exit(pid_t *background_pids, int background_count) {
    for (int i = 0; i < background_count; i++) {
        kill(background_pids[i], SIGTERM);
    }

    for (int i = 0; i < background_count; i++) {
        waitpid(background_pids[i], NULL, 0);
    }
}

int get_input(char *input, size_t input_size, pid_t *background_pids, int background_count) {
    int valid = 0;

    if (fgets(input, input_size, stdin) == NULL) {
        valid = 1;
    }

    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "exit") == 0) {
        handle_exit(background_pids, background_count);
        valid = 1;
    }

    return valid;
}

void tokenize_input(char **args, char *input, int *background) {
    int i = 0;

    char *token = strtok(input, " \t\n");

    while (token != NULL) {
        args[i] = token;
        i++;

        token = strtok(NULL, " \t\n");
    }

    args[i] = NULL;
    
    if (strcmp(args[i - 1], "&") == 0) {
        *background = 1;
        args[i - 1] = NULL;
    }
}

int change_directory(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (chdir(args[1]) == -1) {
            perror("chdir");
        }

        return 1;
    }

    return 0;
}

void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        write(
            STDOUT_FILENO, 
            "\nShell: Background process finished\n", 
            sizeof("\nShell: Background process finished\n") - 1
        );
    }
}

int main() {
    char cwd[1024];
    char user_input[1024];
    char *args[64];
    pid_t background_pids[64];
    int background_count = 0;

    signal(SIGCHLD, handle_sigchld); // signal handler for reaping background child processes

    while (1) {
        int background = 0;

        // Prints the Current Working Directory followed by $ for signalling user input
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s $ ", cwd);
        } else {
            perror("getcwd");
        }

        // Gets user input
        if (get_input(user_input, sizeof(user_input), background_pids, background_count)) {
            break;
        }

        // Check for if no command is entered and skips the rest of the loop if no command
        if (user_input[0] == '\0') {
            continue;
        }
        
        // Tokenizes input into args
        tokenize_input(args, user_input, &background);

        // Changes directory with command 'cd'
        if (change_directory(args)) {
            continue;
        }

        // ------------------------- Fork -------------------------
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child Process

            execvp(args[0], args);
            exit(1);
        } else if (pid > 0) {
            // Parent Process
            int status;

            if (background) {
                background_pids[background_count] = pid;
                background_count++;
            }

            if (!background) {
                waitpid(pid, &status, 0);
            }

            if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
                printf("\nCommand \'%s\' not found\n", args[0]);
            }
        }
    } 

    return 0;
}