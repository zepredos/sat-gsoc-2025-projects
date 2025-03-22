# Mini Shell

## Overview
Mini Shell is a lightweight command-line shell implemented in C++ that executes basic commands (e.g., `ls`, `cd`, `mkdir`). It demonstrates system call usage, process management, and clean code structure in a Unix environment.

## Features
- Tokenises user input into commands.
- Executes built-in commands (like `cd`).
- Launches external commands using `fork()` and `execvp()`.
- Waits for child processes to complete.

## Build Instructions
Compile with:
```sh
g++ -std=c++17 minishell.cpp -o minishell
```
## Usage
1. Run the shell:
```sh
./minishell
```
2. Type commands at the prompt (for example, `ls`, `cd /path/to/dir`, `mkdir new_folder`).

3. The shell will execute built-in commands or launch external programs accordingly.

4. Type `exit` to quit the shell.
