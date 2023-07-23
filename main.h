#ifndef MAIN_H
#define MAIN_H

// Maximum length for a command in the shell
#define MAX_COMMAND_LENGTH 100

// Global variable to store the exit status of the last command
extern int last_exit_status;

// Function to display the shell prompt
void display_prompt();

// Function to execute a command and its arguments
int execute_command(const char *command, char **args);

// Function to replace variables in the input line
char* replace_variables(const char *input);

// Function to parse and execute commands in the input line
int parse_command(char *line);

// Add any additional function declarations for aliases and environment variables here

#endif /* MAIN_H */

