#ifndef RUSH_UTILS_H
#define RUSH_UTILS_H

// necessary headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

// essential constants
#define MAX_TOKENS 255
#define MAX_PATHS 50
#define BASE_SHELL_PATH "/bin"
#define ERR_MSG "An error has occurred\n"

// Global variable declarations
extern int pathCount;
extern char *paths[MAX_PATHS];
extern int tokenCount;
extern char *tokens[MAX_TOKENS][MAX_TOKENS];

// Function prototypes
void initialize_shell();
void prompt();
void flush_error();
void free_tokens();
void parse_input(char **input);
int execute_built_in(int tokenNumber);
int redirect_output(int cmd);
void execute_cmd(int cmd);

#endif  // RUSH_UTILS_H
