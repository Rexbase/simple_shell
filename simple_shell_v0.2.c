#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100

void display_prompt() {
    printf("simple_shell> ");
}

int execute_command(const char *command) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execlp(command, command, NULL);

        // If the child reaches this point, execlp failed
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            return exit_status;
        }
    }

    return -1; // Should never reach this point
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char *path = getenv("PATH");
    char *dir, *saveptr;

    while (1) {
        display_prompt();

        // Read the command from the user
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            // Handling "end of file" condition (Ctrl+D)
            printf("\n");
            break;
        }

        // Remove the trailing newline character
        command[strcspn(command, "\n")] = '\0';

        // Split the PATH into individual directories
        dir = strtok_r(path, ":", &saveptr);

        int command_found = 0;
        while (dir != NULL) {
            // Construct the full path to the command
            char full_path[MAX_COMMAND_LENGTH];
            snprintf(full_path, MAX_COMMAND_LENGTH, "%s/%s", dir, command);

            // Check if the command exists at this path
            if (access(full_path, X_OK) == 0) {
                command_found = 1;
                int exit_status = execute_command(full_path);
                printf("Child process exited with status %d\n", exit_status);
                break;
            }

            // Move to the next directory in PATH
            dir = strtok_r(NULL, ":", &saveptr);
        }

        // If the command is not found in any directory, display an error message
        if (!command_found) {
            printf("Command not found: %s\n", command);
        }
    }

    return 0;
}

