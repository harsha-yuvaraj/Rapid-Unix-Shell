# Rapid Unix Shell

## Project Overview
The **Rapid Unix Shell** is a custom Unix-based shell developed in C, designed to execute standard Unix commands like `ls`, `grep`, `mkdir`, and `awk`. It supports parallel command execution, output redirection, and robust error handling. The shell also provides built-in commands like `cd`, `path`, and `exit` for convenient navigation and process control.

## Features
- **Command Execution**: Supports Unix commands such as `cat`, `grep`, `touch`, `mkdir`, and more.
- **Parallel Execution**: Use `&` to run multiple commands in parallel.
- **Input/Output Redirection**: Redirect output to a file using the `>` operator.
- **Built-in Commands**:
  - `cd`: Change directories.
  - `path`: Manage executable search paths.
  - `exit`: Exit the shell.
- **Robust Error Handling**: Includes mechanisms for handling incorrect commands and system call failures.
- **Flexible Input Parsing**: Programmed to handle multiple spaces or tabs between commands and arguments, empty commands between parallel operators (&), and mixed use of spaces and tabs for formatting.

## Getting Started

### Prerequisites
- A Unix-based environment (Linux or macOS).
- GCC compiler (version 11.4.0 or later).

### Compilation
To compile the project, use the provided `Makefile`. Simply run:
```bash
make
```

### Running the Shell
Once compiled, start the shell by running:
```bash
./rush
```
The shell will prompt with rush>, ready to accept commands. You can execute standard Unix commands, run multiple commands in parallel, or redirect output to files.

### Examples
Here are some examples of how to use the shell:

**Single Command**: Run a simple command like ls to list directory contents:
```
rush> ls -l
```
**Output Redirection**: Redirect the output of a command to a file using the `>` operator:
```
rush> ls -l > output.txt
```
**Parallel Commands**: Run multiple commands in parallel by using the `&` operator:
```
rush> mkdir newdir & grep "text" file.txt & ls -l > output.txt
```

## Error Handling
The shell includes robust error handling for situations such as invalid commands, incorrect use of redirection, failed command execution, and more. For example: 

**Invalid Directory in cd**: If an invalid directory is provided to the cd command, an error will be displayed.
```
rush> cd nonexistentdir
An error has occurred
```

