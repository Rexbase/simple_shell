#include <shell.h>
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

void execute_command(const char *command) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execlp(command, command, NULL);

        // If the child reaches this point, execlp failed
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            printf("Child process exited with status %d\n", exit_status);
        }
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];

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

        // Execute the command
        execute_command(command);
    }

    return 0;
}

