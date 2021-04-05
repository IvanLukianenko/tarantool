#include "string.h"
#include "stdlib.h"

struct cmd{
    const char *name;
    const char **argv;
    int argc;
};

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


struct cmd parser(char *command){
    struct cmd cmd;
    const char sep[] = " ";
    char *istr;
    int i = 0;
    int k, j;
    istr = strtok(command, sep);
    cmd.name = istr; 
    cmd.argv = (char **)malloc(sizeof(char*)*10);   
    cmd.argv[i] = cmd.name;
    i++;
    while(istr != NULL){
        istr = strtok(NULL, sep);
        //cmd.argv[i] = malloc(sizeof(char) * strlen(istr));
        cmd.argv[i] = istr;
        i++;
    }
    cmd.argc = i - 1;

    for (k = 0; k < cmd.argc; k++){
        if (cmd.argv[k][0] == '"'){
            for (j = k+1; j < cmd.argc; j++){
                
                if(cmd.argv[j][strlen(cmd.argv[j]) - 1] == '"') {
                    cmd.argv[k] = concat(cmd.argv[k], " ");
                    cmd.argv[k] = concat(cmd.argv[k], cmd.argv[j]);
                    //<сдвинуть все строки на j-k, начиная с k, уменьшить argc на j-k>
                    //<доработать>
                    for(int n = k + 1; n < cmd.argc - j + k; n++){
                        strcpy(cmd.argv[n], cmd.argv[n+j-k]);
                    }
                    cmd.argc = cmd.argc - j + k ;
                    break;
                }
                else{
                    cmd.argv[k] = concat(cmd.argv[k], " ");
                    cmd.argv[k] = concat(cmd.argv[k], cmd.argv[j]);
                }
            }
        }
    }
    cmd.argv[cmd.argc] = NULL;
    return cmd;
};


struct cmd *parserr(char* command){
    struct cmd *cmd;
    // <количество команд равно количество | + 1>
    int k = 0;
    const char sep[] = "|";
    for(int i = 0; i < strlen(command); i++){
        if (command[i] == '|'){
            k++;
        }
    }
    char **istr = (char**)malloc(sizeof(char*)*(k+1));
    cmd = (struct cmd *)malloc(sizeof(struct cmd) * (k + 1));
    istr[0] = strtok(command, sep);
    int i = 0;
    while(i < k+1){
        i++;
        istr[i] = strtok(NULL, sep);
    }
    for (int j = 0; j < k+1; j++){
        cmd[j] = parser(istr[j]);
    }
    return cmd;
};

char *get_string(int *k) {
    *k = 0;
    int len = 0; // изначально строка пуста
    int capacity = 1; // ёмкость контейнера динамической строки (1, так как точно будет '\0')
    char *s = (char*) malloc(sizeof(char)); // динамическая пустая строка

    char c = getchar(); // символ для чтения данных

    // читаем символы, пока не получим символ переноса строки (\n)
    while (c != '\n') {
        if (c == '|'){
            (*k)++;
        }
        s[(len)++] = c; // заносим в строку новый символ

        // если реальный размер больше размера контейнера, то увеличим его размер
        if (len >= capacity) {
            capacity *= 2; // увеличиваем ёмкость строки в два раза
            s = (char*) realloc(s, capacity * sizeof(char)); // создаём новую строку с увеличенной ёмкостью  
        }

        c = getchar(); // считываем следующий символ          
    }

    s[len] = '\0'; // завершаем строку символом конца строки

    return s; // возвращаем указатель на считанную строку
}


char add_hello_world(char *str){
    strcat(str, "hello world\n");
    return *str;
};

void print_args(struct cmd cmd){
    printf("name: %s\n",cmd.name);
    printf("args num: %d\n", cmd.argc);
    for(int i=0; i < cmd.argc - 1; i++){
        printf("args: %s\n",cmd.argv[i]);
    }
}