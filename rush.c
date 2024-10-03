#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int main(int argc, char *argv[]) {

    if(argc > 1){
       flush_error();
       exit(1);
    }

    initialize_shell();

    while(1){
       char* input = NULL; 
       size_t no_buffer = 0;

       prompt();
       
       if (getline(&input, &no_buffer, stdin) == -1){
           flush_error();
           free(input);
           continue;
       }
       
       parse_input(&input);

       print_parsed_input();
       printf("tokenCount: %d\n", tokenCount);

       


       free_tokens();
       free(input);
    }


   
}