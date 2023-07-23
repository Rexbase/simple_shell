i#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 100
#define BUFFER_SIZE 1024

static char buffer[BUFFER_SIZE];
static int buffer_index = 0;
static int buffer_size = 0;

char* custom_getline() {
    char* line = NULL;
    int line_index = 0;
    int character;

    while (1) {
        // If the buffer is empty, read new data from stdin
        if (buffer_index >= buffer_size) {
            buffer_size = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            buffer_index = 0;

            if (buffer_size <= 0) {
                // End of file or error occurred
                break;
            }
        }

        // Copy characters from the buffer to the line until newline or end of buffer
        while (buffer_index < buffer_size) {
            character = buffer[buffer_index++];
            if (character == '\n' || character == EOF) {
                break;
            }

            line = realloc(line, line_index + 2); // Allocate space for the new character and null terminator
            if (!line) {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }

            line[line_index++] = character;
        }

        if (character == '\n' || character == EOF) {
            break;
        }
    }

    // Add null terminator to the line
    line = realloc(line, line_index + 1);
    if (!line) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    line[line_index] = '\0';

    return line;
}

void display_prompt() {
    printf("simple_shell> ");
}

int main() {
    char *command;

    while (1) {
        display_prompt();

        // Read the command from the user using the custom_getline function
        command = custom_getline();
        if (!command) {
            // Handling "end of file" condition (Ctrl+D)
            printf("\n");
            break;
        }

        // Display the command entered by the user
        printf("Command entered: %s\n", command);

        // Free the allocated memory for the command
        free(command);
    }

    return 0;
}
0.1
