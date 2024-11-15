# Smallsh: A Simple Shell in C

Smallsh is a basic shell implementation written in C that mimics some of the key features of well-known shells like Bash. It provides functionality for running commands, managing built-in commands, handling input/output redirection, and managing foreground and background processes.

## Features

- **Command Prompt**: Displays a `:` prompt for each command line.
- **Blank Lines and Comments**: Ignores blank lines and lines starting with `#` (comments).
- **Variable Expansion**: Expands `$$` to the process ID of the shell.
- **Built-in Commands**: Supports three built-in commands: `exit`, `cd`, and `status`.
- **Execution of External Commands**: Uses `fork()`, `exec()`, and `waitpid()` to execute external commands.
- **Input/Output Redirection**: Supports redirection of input and output using `dup2()`.
- **Foreground and Background Processes**: Supports executing commands in both foreground and background processes.
- **Signal Handling**: Implements custom handlers for `SIGINT` and `SIGTSTP` signals.

## Learning Outcomes

Upon completion, you will be able to:

- Describe and utilize the Unix process API.
- Write programs using the Unix process API.
- Explain signals and handle them in programs.
- Implement I/O redirection in shell programs.

## Program Functionality

### 1. Command Prompt
- Displays a `:` prompt for each new command.
- Supports command syntax:
  ```bash
  command [arg1 arg2 ...] [< input_file] [> output_file] [&]
##Usage
Clone the repo
git clone https://github.com/yourusername/smallsh.git
cd smallsh
Compile the program
gcc -o smallsh smallsh.c
Run the shell 
./smallsh
##Technologies used
Mobaxterm
C
Visual studiocode
Note: This is a school project hosted by Oregon state university
