#ifndef SHELL_H
#define SHELL_H

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

// Function to handle the cd built-in command
void change_directory(char *dir);

// Function to print all aliases or specified aliases
void print_aliases(char *alias_name);

// Function to add an alias with its value
void add_alias(char *alias_name, char *alias_value);

// Function to execute a command with alias substitution
int execute_alias(char *command);

// Function to handle the exit built-in command
void exit_shell(int status);

// Function to print the current environment variables
void print_environment();

// Function to handle the setenv built-in command
void set_environment_variable(char *variable, char *value);

// Function to handle the unsetenv built-in command
void unset_environment_variable(char *variable);

// Add any additional function declarations for other built-in commands or features here

#endif /* SHELL_H */

