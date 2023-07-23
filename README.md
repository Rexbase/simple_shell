# Simple Shell

A simple UNIX command-line interpreter (shell) written in C.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Built-in Commands](#built-in-commands)
- [Files and Directories](#files-and-directories)
- [Authors](#authors)

## Introduction

The Simple Shell is a basic UNIX command-line interpreter that allows users to execute commands and interact with the system. It provides a prompt where users can type commands, and it will execute them accordingly.

This project implements a basic shell with features such as command execution, environment variable handling, built-in commands, handling of logical operators (`&&` and `||`), variable replacement for `$?` and `$$`, support for comments (`#`), command separator (`;`), and more.

## Features

- Displays a prompt and waits for user input.
- Executes simple commands and programs.
- Supports arguments for commands.
- Handles the PATH environment variable to locate executable programs.
- Supports built-in commands like `exit`, `cd`, `alias`, `setenv`, `unsetenv`, and `env`.
- Handles environment variables and replacement for `$?` and `$$`.
- Supports logical operators `&&` and `||`.
- Handles comments (`#`) and ignores anything after `#` on a line.
- Can read and execute commands from a file as a command-line argument.
- Implements a custom `getline` function for reading commands.
- Uses buffer to minimize `read` system calls.
- Ensures no memory leaks using the Betty style and checks.
- Limits the number of functions per file to 5.
- Uses system calls only when necessary.

## Getting Started

To compile the Simple Shell, use the following command:

```bash
gcc -o simple_shell simple_shell.c
```

## Usage

To run the shell in interactive mode (with a prompt), use:

```bash
./simple_shell
```

To run the shell in batch mode (read commands from a file), use:

```bash
./simple_shell filename
```

## Built-in Commands

The Simple Shell supports the following built-in commands:

- `exit`: Exits the shell. Optionally, you can provide an integer status as an argument.
- `cd [DIRECTORY]`: Changes the current directory. If no argument is provided, it changes to the home directory. It also supports `cd -` to change to the previous working directory.
- `alias [name[='value'] ...]`: Prints, defines, or updates aliases for commands.
- `setenv VARIABLE VALUE`: Initializes or modifies an environment variable.
- `unsetenv VARIABLE`: Removes an environment variable.
- `env`: Prints the current environment variables.

## Files and Directories

- `simple_shell.c`: The main C source code file.
- `README.md`: This readme file providing information about the project.
- Other files: Any additional files, libraries, or scripts used in the project.

## Authors

- [Etimbuk Essien](https://github.com/Rexbase)
