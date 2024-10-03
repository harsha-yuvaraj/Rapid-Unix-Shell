/*
 * Description: 
 *     This program implements a custom Unix shell interface, allowing users 
 *     to execute standard Unix commands. It supports multiple features such as:
 *     
 *     1. Command execution using the execv() system call. Example commands include 
 *        `ls` for listing directory contents, `mkdir` for creating directories, 
 *        `grep` for searching text, and `cat` for displaying file contents.
 *     2. Support for running multiple commands in parallel by using the "&" operator.
 *     3. Output redirection (e.g., using ">" for directing output to files).
 *     4. Robust error checking to handle incorrect commands and system call failures.
 *     5. Built-in commands including `cd` for changing directories, `path` to 
 *        manage executable search paths, and `exit` to terminate the shell.
 *     
 *     The program uses system calls like fork(), wait(), and execv() to manage process creation 
 *     and control, providing a simple but effective shell-like interface for Unix-based systems.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_TOKENS 255
#define MAX_PATHS 50
#define BASE_SHELL_PATH "/bin"
#define ERR_MSG "An error has occurred\n"

// Global variables for managing shell paths and command tokens
int pathCount = 0;
char *paths[MAX_PATHS]; 
int tokenCount = 0;
char *tokens[MAX_TOKENS][MAX_TOKENS];

// Initialize shell with default path "/bin"
void initialize_shell(){
    paths[0] = malloc(sizeof(char) * (strlen(BASE_SHELL_PATH) + 1));
    strcpy(paths[0], BASE_SHELL_PATH);
    pathCount = 1;
}

// Print the shell prompt
void prompt() {
    printf("rush> ");
    fflush(stdout);
}

// Print error message to stderr
void flush_error(){
    write(STDERR_FILENO, ERR_MSG, strlen(ERR_MSG)); 
    fflush(stdout);
}

// Free the memory used for storing command tokens
void free_tokens(){
    for(int j=0; j<=tokenCount; j++)
        for(int i=0; tokens[j][i] != NULL; i++)
            free(tokens[j][i]);

    tokenCount = 0; 
}

// Parse user input and tokenize commands, handling parallel commands separated by "&"
void parse_input(char** input){
    char* token, *pos;
    int index = 0;

    // Replace tabs with spaces
    pos = strchr(*input, '\t');
    while(pos != NULL){
       *pos = ' ';
        pos = strchr(pos+1, '\t');
    }

    // Endline \n removal
    pos = strchr(*input, '\n');
    *pos = ' ';
    
    // Split the input string into tokens (commands and arguments)
    while((token = strsep(input, " ")) != NULL){
        if(*token == '\0' || strcmp(token, " ") == 0)
            continue;
        
        // Handle "&" for parallel commands
        if(strcmp(token, "&") == 0){
            tokens[tokenCount][index] = NULL; // End of token list
            tokenCount++;
            index=0;
            continue;
        }
        
        tokens[tokenCount][index] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(tokens[tokenCount][index], token);
        index++;
    }

    // End of token list
    tokens[tokenCount][index] = NULL;
}

// Execute built-in commands like "path", "cd", and "exit"
int execute_built_in(int tokenNumber){
    if(strcmp(tokens[tokenNumber][0], "path") == 0){
        for(int i = 0; i < pathCount; i++)
            free(paths[i]);

        pathCount = 0;

        // Update shell path with the given arguments 
        for(int i = 0; tokens[tokenNumber][i+1] != NULL; i++){
            paths[i] = malloc(sizeof(char) * (strlen(tokens[tokenNumber][i+1]) + 1));
            strcpy(paths[i], tokens[tokenNumber][i+1]);
            pathCount++;
        }   
    }
    else if(strcmp(tokens[tokenNumber][0], "cd") == 0){
        // Change directory, handle errors
        if(tokens[tokenNumber][1] == NULL || tokens[tokenNumber][2] != NULL || chdir(tokens[tokenNumber][1]) != 0)
            flush_error();
    }
    else if(strcmp(tokens[tokenNumber][0], "exit") == 0){
        // Exit command, handle errors
        if(tokens[tokenNumber][1] != NULL)
            flush_error();
        else
            exit(0);
    }
    else
        return 0; // Not a built-in command

    return 1; // built-in command successful execution
}

// Handle output redirection (">" operator)
int redirect_output(int cmd){

    int redirect_found = -1, fd;

    // Search for ">" in the tokens
    for(int i = 0; tokens[cmd][i] != NULL; i++){
        if(strcmp(tokens[cmd][i], ">") == 0){
            redirect_found = i;
            break;
        }
    }
    
    if(redirect_found == -1)
        return -2; // No redirection found

    // Open the file for output redirection
    if(tokens[cmd][redirect_found+1] != NULL && tokens[cmd][redirect_found+2] == NULL && strcmp(tokens[cmd][redirect_found+1], ">") != 0){
        fd = open(tokens[cmd][redirect_found+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if(fd < 0 || dup2(fd, STDOUT_FILENO) < 0){
            flush_error();
            return -1; // Error in redirection
        }
        else{
            close(fd);
            return redirect_found; // Redirection successful
        }
    }

    return -1;  // Error in redirection
}

// Execute external commands by finding the executable path
void execute_cmd(int cmd){
    char* path;

    // Search through the available paths to find the executable
    for(int i = 0; i < pathCount; i++){
        int path_len = strlen(paths[i]) + strlen(tokens[cmd][0]) + 1;
        path = malloc(sizeof(char) * (path_len));
        memset(path, '\0', path_len);
        strcpy(path, paths[i]);
        strcat(path, "/");
        strcat(path,tokens[cmd][0]);

        if(access(path, X_OK) == 0)
            break;
        else
            free(path); // Free path if not executable
    }
    
    if(path != NULL){

        int redirect = redirect_output(cmd); // Handle output redirection, if any

        if(redirect >= 0)
            tokens[cmd][redirect] = NULL;
        
        // Execute the command
        if(redirect == -1 || execv(path, tokens[cmd]) == -1)
            flush_error();
    }
    else
        flush_error(); // No path set to search for command executables 
    
    exit(0); // exit child process
}

int main(int argc, char *argv[]) {

    if(argc > 1){ // Error if the shell is invoked with arguments
       flush_error();
       exit(1);
    }

    // Set up shell paths
    initialize_shell();

    // Main shell loop
    while(1){
        char* input = NULL; 
        size_t no_buffer = 0;
        pid_t pids[tokenCount+1];
 
        // Display the shell prompt
        prompt();
        
        // Read input
        if (getline(&input, &no_buffer, stdin) == -1){
            flush_error();
            free(input);
            continue;
        }
        
        // Parse the input into tokens
        parse_input(&input);
        
        // Loop through each command
        for (int cmd = 0; cmd  <= tokenCount; cmd++){
            if(tokens[cmd][0] == NULL){
                pids[cmd] = -1;
                continue;
            }

            // Execute built-in commands  
            else if(execute_built_in(cmd))
                continue;
 
             // No valid paths
            else if(pathCount == 0){ 
                 flush_error();
                 pids[cmd] = -1; 
                 continue;
            }
 
            else{
                // Fork child process
                pids[cmd] = fork();
 
                // Fork fails
                if(pids[cmd] == -1)
                    flush_error();
 
                // Child executes the command, if Fork successful
                if(pids[cmd] == 0)
                    execute_cmd(cmd);
            }
        }

        // Wait for all child processes to finish
        for(int i = 0; i <= tokenCount; i++)
            if(pids[i] != -1)
                wait(NULL);   

        // Free command tokens for next prompt  
        free_tokens();

        // Free input buffer, if not empty
        if(input != NULL)
            free(input);
    }

    return 0;
}