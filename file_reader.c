#ifndef _FILE_READER_H_
#define _FILE_READER_H_

#include <stdio.h>
#include "main.h"

int count_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    int line_count = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            line_count++;
        }
    }

    fclose(file);
    return line_count;
}

#endif /* _FILE_READER_H_ */

