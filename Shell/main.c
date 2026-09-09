#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t foreground_pid = -1;

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

int tokenize_input(char **args, char *input, int *background) {
    int i = 0;

    char *token = strtok(input, " \t\n");

    while (token != NULL) {
        args[i] = token;
        i++;

        token = strtok(NULL, " \t\n");
    }

    args[i] = NULL;
    
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        *background = 1;
        args[--i] = NULL;
    }

    return i;
}

void count_commands(char **args, int args_length,
                    char ***commands, int *command_count,
                    int *parallel)
{
    *command_count = 0;
    *parallel = 0;

    if (args_length == 0) {
        return;
    }

    commands[(*command_count)++] = &args[0];

    for (int i = 0; i < args_length; i++) {
        int serial_separator = strcmp(args[i], "&&") == 0;
        int parallel_separator = strcmp(args[i], "&&&") == 0;

        if (serial_separator || parallel_separator) {
            if (parallel_separator) {
                *parallel = 1;
            }

            args[i] = NULL;
            commands[(*command_count)++] = &args[i + 1];
        }
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

void reap_background_processes() {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        printf("Shell: Background process finished\n");
    }
}

void wait_for_child(pid_t pid) {
    pid_t result;

    do {
        result = waitpid(pid, NULL, 0);
    } while (result == -1 && errno == EINTR);

    if (result == -1) {
        perror("waitpid");
    }
}

void handle_sigint(int sig){
    if (foreground_pid > 0) {
        kill(-(pid_t)foreground_pid, SIGINT);
    }
}

int main() {
    char cwd[1024];
    char user_input[1024];
    char *args[64];
    pid_t background_pids[64];
    int background_count = 0;
    char **commands[64];

    signal(SIGINT, handle_sigint);

    while (1) {
        int background = 0;
        int command_count = 0;
        int parallel = 0;

        reap_background_processes();

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
        int token_count = tokenize_input(args, user_input, &background);

        if (token_count == 0) {
            continue;
        }

        count_commands(args, token_count, commands, &command_count, &parallel);

        // Changes directory with command 'cd'
        if (change_directory(args)) {
            continue;
        }

        // ------------------------- Fork -------------------------
        pid_t parallel_pids[64];
        int parallel_count = 0;

        for (int i = 0; i < command_count; i++) {
            pid_t pid = fork();

            if (pid < 0) {
                perror("Fork failed");
                break;
            } else if (pid == 0) {
                // Child Process

                setpgid(0, 0);

                execvp(commands[i][0], commands[i]);
                perror(commands[i][0]);
                exit(1);
            } else if (pid > 0) {
                // Parent Process
                if (parallel) {
                    parallel_pids[parallel_count++] = pid;
                } else if (background) {
                    background_pids[background_count++] = pid;
                } else {
                    foreground_pid = pid;
                    wait_for_child(pid);
                    foreground_pid = -1;
                }
            }
        }

        for (int i = 0; i < parallel_count; i++) {
            wait_for_child(parallel_pids[i]);
        }
    } 

    return 0;
}