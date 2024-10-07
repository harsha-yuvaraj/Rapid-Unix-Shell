/*
 * Description: 
 *     This program implements a custom Unix shell interface, allowing users 
 *     to execute standard Unix commands. It supports multiple features such as:
 *     
 *     1. Command execution using the execv() system call. Example commands include 
 *        `ls` for listing directory contents, `mkdir` for creating directories, 
 *        `grep` for searching text, and `cat` for displaying file contents.
 *     2. Support for running multiple commands in parallel by using the "/&" operator.
 *     3. Output redirection (e.g., using ">" for directing output to files).
 *     4. Robust error checking to handle incorrect commands and system call failures.
 *     5. Built-in commands including `cd` for changing directories, `path` to 
 *        manage executable search paths, and `exit` to terminate the shell.
 *     
 *     The program uses system calls like fork(), wait(), and execv() to manage process creation 
 *     and control, providing a simple but effective shell-like interface for Unix-based systems.
 */

#include "rush_utils.h"

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