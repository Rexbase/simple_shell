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

int parse_command(char *line, char **command, char **args, int *exit_status) {
    int arg_index = 0;
    char *token;
    char *saveptr;

    // Extract the command (first token)
    token = strtok_r(line, " \t\n", &saveptr);
    if (!token) {
        // Empty command
        return 0;
    }

    // Check if the command is "exit"
    if (strcmp(token, "exit") == 0) {
        // Check if an argument for the exit status is provided
        token = strtok_r(NULL, " \t\n", &saveptr);
        if (token) {
            *exit_status = atoi(token);
        }
        return 0; // Indicate that the command is built-in "exit"
    }

    // Check if the command is "setenv"
    if (strcmp(token, "setenv") == 0) {
        // Parse the VARIABLE and VALUE arguments for setenv
        char *var = strtok_r(NULL, " \t\n", &saveptr);
        char *val = strtok_r(NULL, " \t\n", &saveptr);
        if (!var || !val) {
            fprintf(stderr, "Usage: setenv VARIABLE VALUE\n");
            return 0; // Indicate that the command is built-in "setenv"
        }
        setenv(var, val, 1);
        return 0; // Indicate that the command is built-in "setenv"
    }

    // Check if the command is "unsetenv"
    if (strcmp(token, "unsetenv") == 0) {
        // Parse the VARIABLE argument for unsetenv
        char *var = strtok_r(NULL, " \t\n", &saveptr);
        if (!var) {
            fprintf(stderr, "Usage: unsetenv VARIABLE\n");
            return 0; // Indicate that the command is built-in "unsetenv"
        }
        unsetenv(var);
        return 0; // Indicate that the command is built-in "unsetenv"
    }

    // Check if the command is "cd"
    if (strcmp(token, "cd") == 0) {
        // Parse the DIRECTORY argument for cd
        char *dir = strtok_r(NULL, " \t\n", &saveptr);
        if (!dir) {
            // If no argument is provided, change to the home directory
            dir = getenv("HOME");
            if (!dir) {
                fprintf(stderr, "cd: No $HOME variable set\n");
                return 0; // Indicate that the command is built-in "cd"
            }
        }

        // Handle "cd -" to change to the previous working directory
        if (strcmp(dir, "-") == 0) {
            dir = getenv("OLDPWD");
            if (!dir) {
                fprintf(stderr, "cd: No previous working directory\n");
                return 0; // Indicate that the command is built-in "cd"
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
            return 0; // Indicate that the command is built-in "cd"
        }

        // Update the environment variable PWD
        char new_dir[MAX_COMMAND_LENGTH];
        if (getcwd(new_dir, sizeof(new_dir)) == NULL) {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }
        setenv("PWD", new_dir, 1);

        return 0; // Indicate that the command is built-in "cd"
    }

    *command = token;

    // Extract the arguments (remaining tokens)
    while ((token = strtok_r(NULL, " \t\n", &saveptr))) {
        args[arg_index++] = token;
    }
    args[arg_index] = NULL; // NULL-terminate the argument list

    return 1;
}

int main() {
    char line[MAX_COMMAND_LENGTH];
    char *command;
    char *args[MAX_COMMAND_LENGTH];
    int exit_status = 0;
    char *path = getenv("PATH");
    char *dir, *saveptr;

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

        // Parse the command and arguments
        if (parse_command(line, &command, args, &exit_status) == 0) {
            // Check if the command is "exit"
            printf("Exiting the shell with status %d.\n", exit_status);
            break;
        }

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
                int exit_status = execute_command(full_path, args);
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

