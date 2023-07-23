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

int execute_command(const char *command, char **args) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execvp(command, args);

        // If the child reaches this point, execvp failed
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

int parse_command(char *line) {
    char *token;
    char *saveptr;
    int exit_status = 0;

    // Split the line into separate commands based on semicolon
    token = strtok_r(line, ";", &saveptr);

    while (token != NULL) {
        char *command;
        char *args[MAX_COMMAND_LENGTH];
        int arg_index = 0;
        char *arg;

        // Parse each command and its arguments
        command = strtok(token, " \t\n");
        if (command) {
            // Extract the arguments (remaining tokens)
            while ((arg = strtok(NULL, " \t\n"))) {
                args[arg_index++] = arg;
            }
            args[arg_index] = NULL; // NULL-terminate the argument list

            // Check if the command is "exit"
            if (strcmp(command, "exit") == 0) {
                // Check if an argument for the exit status is provided
                if (args[0]) {
                    exit_status = atoi(args[0]);
                }
                printf("Exiting the shell with status %d.\n", exit_status);
                return exit_status;
            }

            // Check if the command is "cd"
            if (strcmp(command, "cd") == 0) {
                // Parse the DIRECTORY argument for cd
                char *dir = args[0];
                if (!dir) {
                    // If no argument is provided, change to the home directory
                    dir = getenv("HOME");
                    if (!dir) {
                        fprintf(stderr, "cd: No $HOME variable set\n");
                        return 1;
                    }
                }

                // Handle "cd -" to change to the previous working directory
                if (strcmp(dir, "-") == 0) {
                    dir = getenv("OLDPWD");
                    if (!dir) {
                        fprintf(stderr, "cd: No previous working directory\n");
                        return 1;
                    }
                }

                // Save the current working directory for "cd -"
                char current_dir[MAX_COMMAND_LENGTH];
                if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
                    perror("getcwd");
                    exit(EXIT_FAILURE);
                }
                setenv("OLDPWD", current_dir, 1);

                // Change the current working directory
                if (chdir(dir) == -1) {
                    perror("chdir");
                    return 1;
                }

                // Update the environment variable PWD
                char new_dir[MAX_COMMAND_LENGTH];
                if (getcwd(new_dir, sizeof(new_dir)) == NULL) {
                    perror("getcwd");
                    exit(EXIT_FAILURE);
                }
                setenv("PWD", new_dir, 1);
            } else {
                // Handle regular commands
                int exit_status = execute_command(command, args);
                printf("Child process exited with status %d\n", exit_status);
            }
        }

        // Move to the next command
        token = strtok_r(NULL, ";", &saveptr);
    }

    return 0;
}

int main() {
    char line[MAX_COMMAND_LENGTH];

    while (1) {
        display_prompt();

        // Read the command from the user
        if (fgets(line, MAX_COMMAND_LENGTH, stdin) == NULL) {
            // Handling "end of file" condition (Ctrl+D)
            printf("\n");
            break;
        }

        // Remove the trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Execute the commands
        int exit_status = parse_command(line);
        if (exit_status != 0) {
            // If the user entered "exit" command, break the loop
            break;
        }
    }

    return 0;
}

