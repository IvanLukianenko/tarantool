#include "string.h"
#include "stdlib.h"
#include "ctype.h"

struct cmd{
    char *name;
    char **argv;
    int argc;
};

void remove_spaces(char *str) {
    char *strBuff = (char*) malloc(strlen(str) + 1), *p = strBuff;
    if(strBuff == NULL){
        perror("remove_spaces:malloc");
    }
    while (*str) {
        if (!isspace(*str)) {
            *p = *str;
            p++;
        }
        str++;
    }
    *p = '\0';
    free(strBuff);
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); 
    if(result == NULL){
        perror("concat:malloc");
    }
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}

void print_args(struct cmd cmd);

struct cmd parser(char *command){
    struct cmd cmd;
    const char sep[] = " ";
    char *istr;
    int i = 0;
    int k, j;
    istr = strtok(command, sep);
    cmd.name = istr; 
    cmd.argv = (char **)malloc(sizeof(char*)*(strlen(command)+2));   
    if(cmd.argv == NULL){
        perror("parser:malloc");
    }
    cmd.argv[i] = cmd.name;
    i++;
    while(istr != NULL){
        istr = strtok(NULL, sep);
        cmd.argv[i] = istr;
        i++;
    }
    cmd.argc = i - 1;

    for (k = 1; k < cmd.argc; k++){
        for(int q = 0; q < strlen(cmd.argv[k])-1; q++){
            if(cmd.argv[k][q] == '\\' && cmd.argv[k][q+1] == '\\'){
                cmd.argv[k][q+1] = ' '; 
            }
        }
    }
    for (k = 1; k < cmd.argc; k++){

        if (cmd.argv[k][0] == '"'){
            if(cmd.argv[k][strlen(cmd.argv[k]) - 1] == '"' && cmd.argv[k][strlen(cmd.argv[k])-2] != '\\'){
                for (int y = 0; y < strlen(cmd.argv[k])-1; y++){
                    cmd.argv[k][y] = cmd.argv[k][y+1];
                }
                cmd.argv[k][strlen(cmd.argv[k])-2] = '\0';
            }
            else{
                for (j = k+1; j < cmd.argc+1; j++){
                    if(cmd.argv[j][strlen(cmd.argv[j]) - 1] == '"' && cmd.argv[j][strlen(cmd.argv[j])-2] != '\\') {
                        cmd.argv[k] = concat(cmd.argv[k], cmd.argv[j]);
                        for(int n = k+1; n < cmd.argc - j + k; n++){
                            cmd.argv[n] = cmd.argv[n-k+j];
                        }
                        cmd.argc = cmd.argc - j + k ;
                        break;
                    }
                    else{
                        cmd.argv[k] = concat(cmd.argv[k], cmd.argv[j]);
                    }
                }
                for (int y = 0; y < strlen(cmd.argv[k])-2; y++){
                    cmd.argv[k][y] = cmd.argv[k][y+1];
                }
                cmd.argv[k][strlen(cmd.argv[k])-2] = '\0';
            }
        }

        else if (cmd.argv[k][0] == '\''){

            if(cmd.argv[k][strlen(cmd.argv[k]) - 1] == '\'' && cmd.argv[k][strlen(cmd.argv[k])-2] != '\\'){
                for (int y = 0; y < strlen(cmd.argv[k])-1; y++){
                    cmd.argv[k][y] = cmd.argv[k][y+1];
                }
                cmd.argv[k][strlen(cmd.argv[k])-2] = '\0';   
            }
            else{
                for (j = k+1; j < cmd.argc+1; j++){

                    if(cmd.argv[j][strlen(cmd.argv[j]) - 1] == '\'' && cmd.argv[j][strlen(cmd.argv[j])-2] != '\\') {
                        cmd.argv[k] = concat(cmd.argv[k], cmd.argv[j]);
                        for(int n = k+1; n < cmd.argc - j + k; n++){
                            cmd.argv[n] = cmd.argv[n-k+j];
                        }
                        cmd.argc = cmd.argc - j + k ;
                        break;
                    }
                    else{
                        cmd.argv[k] = concat(cmd.argv[k], cmd.argv[j]);
                    }
                }
                for (int y = 0; y < strlen(cmd.argv[k])-1; y++){
                    cmd.argv[k][y] = cmd.argv[k][y+1];
                }
                cmd.argv[k][strlen(cmd.argv[k])-2] = '\0';
            }
        }
        else if(cmd.argv[k][strlen(cmd.argv[k])-1] == '\\'){          
            cmd.argv[k][strlen(cmd.argv[k])-1] = '\0';
            cmd.argv[k] = concat(cmd.argv[k], cmd.argv[k+1]);
            for(int n = k+1; n < cmd.argc - 1; n++){
                cmd.argv[n] = cmd.argv[n+1];
            }
            cmd.argc = cmd.argc - 1;
            k--;
        }
        remove_spaces(cmd.argv[k]);
    }
    
    cmd.argv[cmd.argc] = NULL;
    return cmd;
};


struct cmd *parserr(char* command){
    struct cmd *cmd;
    int k = 0;
    int iprev = 0;
    const char sep[] = "|";
    int max = 0;
    for(int i = 0; i < strlen(command); i++){
        if (command[i] == '|'){
            if(i - iprev > max){
                max = i - iprev;
                iprev = i;
            }
            k++;
        }
    }
    char **istr = (char**)malloc(sizeof(char*) * (strlen(command) + 1) * (k + 1));
    cmd = (struct cmd *)malloc(sizeof(struct cmd) * (k + 1));
    if (cmd == NULL){
        perror("parserr:malloc");
    }
    if (istr == NULL){
        perror("parserr:malloc");
    }
    istr[0] = strtok(command, sep);
    int i = 0;
    while(i < k+1){
        i++;
        istr[i] = strtok(NULL, sep);
    }
    for (int j = 0; j < k+1; j++){
        cmd[j] = parser(istr[j]);
    }
    free(istr);
    return cmd;
};

char *get_string(int *k) {
    *k = 0;
    int len = 0; 
    int capacity = 1; 
    char *s = (char*) malloc(sizeof(char)); 
    if(s == NULL){
        perror("get_string: malloc");
    }
    int ignore_flag_bs = 0;
    int ignore_flag_q = 0;
    char c = getchar(); 

    while (c != '\n') {
        if (c == '|'){
            (*k)++;
        }
        if(c == '\\'){
            ignore_flag_bs = 1;
        }
        
        if(c == '"' && ignore_flag_q == 0){
            ignore_flag_q = 1;
        }
        else if(c == '"' && ignore_flag_q == 1){
            ignore_flag_q = 0;
        }

        s[(len)++] = c; 
        if (len >= capacity) {
            capacity *= 2; 
            s = (char*) realloc(s, capacity * sizeof(char));  
            if(s == NULL){
                printf("get_string:realloc");
            }
        }

        c = getchar();  

        if (c == '\n' && ignore_flag_bs == 1){
            len--;

            c = getchar();
            ignore_flag_bs = 0;
        }     
        else{
            if (c == '\n' && ignore_flag_q == 1){
                s[(len)++] = ' ';
                if (len >= capacity) {
                    capacity *= 2; 
                    s = (char*) realloc(s, capacity * sizeof(char));  
                }
                c = getchar();
            } 
        }
        ignore_flag_bs = 0;
    }
    s[len] = '\0'; 
    return s; 
}

/*Для удобства отладки*/

char add_hello_world(char *str){
    strcat(str, " ");
    strcat(str, "Hello");
    return *str;
};

void print_args(struct cmd cmd){
    printf("name: %s\n",cmd.name);
    printf("args num: %d\n", cmd.argc-1 );
    for(int i=0; i < cmd.argc; i++){
        printf("args: %s\n",cmd.argv[i]);
    }
}