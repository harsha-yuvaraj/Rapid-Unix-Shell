#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PATHS 50
#define BASE_SHELL_PATH "/bin"
int pathCount = 0;
char *paths[MAX_PATHS]; 

#define ERR_MSG "An error has occurred\n"

#define MAX_TOKENS 255
int tokenCount = 0;
char *tokens[MAX_TOKENS][MAX_TOKENS];

void initialize_shell(){
    paths[0] = malloc(sizeof(char) * (strlen(BASE_SHELL_PATH) + 1));
    strcpy(paths[0], BASE_SHELL_PATH);
    pathCount = 1;
}

void prompt() {
    printf("rush> ");
    fflush(stdout);
}

void flush_error(){
    write(STDERR_FILENO, ERR_MSG, strlen(ERR_MSG)); 
    fflush(stdout);
}

void free_tokens(){
    for(int j=0; j<=tokenCount; j++)
        for(int i=0; tokens[j][i] != NULL; i++)
            free(tokens[j][i]);

    tokenCount = 0; 
}

void print_parsed_input(){
    for(int j=0; j<=tokenCount; j++){
        for(int i=0; tokens[j][i] != NULL; i++)
           printf("Comand %d: %s\n", j, tokens[j][i]);
        
        printf("--------\n");
           
    }
}

void parse_input(char** input){
    char* token, *pos;
    int index = 0;

    pos = strchr(*input, '\t');
    while(pos != NULL){
       *pos = ' ';
        pos = strchr(pos+1, '\t');
    }
    // endline \n removal
    pos = strchr(*input, '\n');
    *pos = ' ';
    
    while((token = strsep(input, " ")) != NULL){
        if(*token == '\0' || strcmp(token, " ") == 0)
           continue;
        
        if(strcmp(token, "&") == 0){
            tokens[tokenCount][index] = NULL;
            tokenCount++;
            index=0;
            continue;
        }
        
        tokens[tokenCount][index] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(tokens[tokenCount][index], token);
        index++;
    }

    tokens[tokenCount][index] = NULL;
}


int execute_built_in(int tokenNumber){
    if(strcmp(tokens[tokenNumber][0], "path") == 0){
        for(int i = 0; i < pathCount; i++)
            free(paths[i]);

        pathCount = 0;

        for(int i = 0; tokens[tokenNumber][i+1] != NULL; i++){
            paths[i] = malloc(sizeof(char) * (strlen(tokens[tokenNumber][i+1]) + 1));
            strcpy(paths[i], tokens[tokenNumber][i+1]);
            pathCount++;
        }   
    }
    else if(strcmp(tokens[tokenNumber][0], "cd") == 0){

        if(tokens[tokenNumber][1] == NULL || tokens[tokenNumber][2] != NULL)
            flush_error();
    
        else if(chdir(tokens[tokenNumber][1]) != 0)
            flush_error();
    }
    else if(strcmp(tokens[tokenNumber][0], "exit") == 0){
        if(tokens[tokenNumber][1] != NULL)
            flush_error();
        else
            exit(0);
    }
    else
        return 0;

    return 1;
}

void execute_cmd(int cmd){
    char* path;

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
            free(path); 
    }

    if(path != NULL && execv(path, tokens[cmd]) == -1)
        flush_error();

    exit(0);
}

int main(int argc, char *argv[]) {

    if(argc > 1){
       flush_error();
       exit(1);
    }

    initialize_shell();

    while(1){
       char* input = NULL; 
       size_t no_buffer = 0;
       pid_t pids[tokenCount+1];

       prompt();
       
       if (getline(&input, &no_buffer, stdin) == -1){
           flush_error();
           free(input);
           continue;
       }
       
       parse_input(&input);
       
       for (int cmd = 0; cmd  <= tokenCount; cmd++){
           if(tokens[cmd][0] == NULL){
               pids[cmd] = -1;
               continue;
           }
              
           else if(execute_built_in(cmd))
               continue;

            else if(pathCount == 0){
                flush_error();
                pids[cmd] = -1; 
                continue;
            }

           else{
               pids[cmd] = fork();

               if(pids[cmd] == -1)
                   flush_error();

               if(pids[cmd] == 0)
                   execute_cmd(cmd);
           }
       }

       for(int i = 0; i <= tokenCount; i++)
           if(pids[i] != -1)
               wait(NULL);   

       free_tokens();

       if(input != NULL)
           free(input);
    }
}