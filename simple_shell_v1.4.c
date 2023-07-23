#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100

// Structure to hold the alias information
typedef struct Alias {
    char *name;
    char *value;
    struct Alias *next;
} Alias;

Alias *alias_list = NULL; // Global linked list to store aliases

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

Alias* find_alias(const char *name) {
    // Function to find an alias by name in the linked list
    Alias *current = alias_list;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void add_alias(const char *name, const char *value) {
    // Function to add or update an alias in the linked list
    Alias *existing_alias = find_alias(name);

    if (existing_alias) {
        // If the alias already exists, update its value
        free(existing_alias->value);
        existing_alias->value = strdup(value);
    } else {
        // If the alias doesn't exist, create a new one
        Alias *new_alias = (Alias*)malloc(sizeof(Alias));
        new_alias->name = strdup(name);
        new_alias->value = strdup(value);
        new_alias->next = alias_list;
        alias_list = new_alias;
    }
}

void print_alias(const char *name) {
    // Function to print an alias by name
    Alias *alias = find_alias(name);

    if (alias) {
        printf("%s='%s'\n", alias->name, alias->value);
    }
}

void print_all_aliases() {
    // Function to print all aliases
    Alias *current = alias_list;

    while (current != NULL) {
        printf("%s='%s'\n", current->name, current->value);
        current = current->next;
    }
}

void free_aliases() {
    // Function to free the memory used by the aliases
    Alias *current = alias_list;

    while (current != NULL) {
        Alias *temp = current;
        current = current->next;
        free(temp->name);
        free(temp->value);
        free(temp);
    }
}

int parse_command(char *line) {
    char *token;
    char *saveptr;
    int exit_status = 0;

    // Split the line into separate commands based on &&
    token = strtok_r(line, "&&", &saveptr);

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
                free_aliases();
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
            } else if (strcmp(command, "alias") == 0) {
                // Handle the "alias" built-in command
                // Parse the arguments for alias
                char *name = args[0];

                if (!name) {
                    // If no argument is provided, print all aliases
                    print_all_aliases();
                } else if (args[1] && strcmp(args[1], "=") == 0) {
                    // If the arguments have the form "name='value'", define/update an alias
                    char *value = args[2];
                    add_alias(name, value);
                } else {
                    // If the arguments are alias names, print the corresponding aliases
                    int i = 0;
                    while (args[i]) {
                        print_alias(args[i]);
                        i++;
                    }
                }
            } else {
                // Handle regular commands
                int exit_status = execute_command(command, args);
                printf("Child process exited with status %d\n", exit_status);
            }
        }

        // Move to the next command separated by &&
        token = strtok_r(NULL, "&&", &saveptr);

        // Check the exit status of the previous command
        if (exit_status != 0 && token != NULL && strcmp(token, "||") != 0) {
            // If the previous command failed (exit_status != 0)
            // and the next command is not ||, skip executing the next command
            token = strtok_r(NULL, "&&", &saveptr);
        } else if (exit_status == 0 && token != NULL && strcmp(token, "||") == 0) {
            // If the previous command succeeded (exit_status == 0)
            // and the next command is ||, skip executing the next command
            token = strtok_r(NULL, "&&", &saveptr);
        }
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

