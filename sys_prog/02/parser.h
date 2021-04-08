#include "string.h"
#include "stdlib.h"

struct cmd{
    const char *name;
    char **argv;
    int argc;
};

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}

void print_args(struct cmd cmd);

struct cmd parser(char *command){
    char *bufForChanges = (char *)malloc(sizeof(char)*1000);
    struct cmd cmd;
    const char sep[] = " ";
    char *istr;
    int i = 0;
    int k, j, j2;
    istr = strtok(command, sep);
    cmd.name = istr; 
    cmd.argv = (char **)malloc(sizeof(char*)*1000);   
    cmd.argv[i] = cmd.name;
    i++;
    while(istr != NULL){
        istr = strtok(NULL, sep);
        cmd.argv[i] = istr;
        i++;
    }
    char *buf = (char *)malloc(sizeof(char)*(strlen(command)+1));
    char *buf1 = (char *)malloc(sizeof(char)*(strlen(command)+1));
    int index;
    cmd.argc = i - 1;
    //print_args(cmd);
    //printf("До цикла \n");
    for (k = 1; k < cmd.argc; k++){
        if (cmd.argv[k][0] == '"'){
            //<это вроде хорошо работает>
            if(cmd.argv[k][strlen(cmd.argv[k]) - 1] == '"' && cmd.argv[k][strlen(cmd.argv[k])-2] != '\\'){
                for (int y = 0; y < strlen(cmd.argv[k])-1; y++){
                    cmd.argv[k][y] = cmd.argv[k][y+1];
                }
                cmd.argv[k][strlen(cmd.argv[k])-2] = '\0';
                //printf("нашел 2 и пошел дальше\n");
            }
            //<конец хорошего>
            //<Протестировать этот код>
            else{
                //buf[index] = ' ';
                //*(cmd.argv[k] + strlen(cmd.argv[k])) = ' ';


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
                //printf("нашел 1 и пошел дальше\n");
                
            }
            else{
                strcat(buf, cmd.argv[k]);
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
        /*else if(cmd.argv[k][strlen(cmd.argv[k])-1] == '\\'){          //<Присоединить и сместить все на одну>
            cmd.argv[k] = concat(cmd.argv[k], cmd.argv[k+1]);
            for(int n = k+1; n < cmd.argc - 1; n++){
                cmd.argv[n] = cmd.argv[n+1];
            }
            cmd.argc = cmd.argc - j + k ;
        }*/
        
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
    char **istr = (char**)malloc(sizeof(char*) * (max + 1) * (k + 1));
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
    int ignore_flag_bs = 0;
    int ignore_flag_q = 0;
    char c = getchar(); // символ для чтения данных

    // читаем символы, пока не получим символ переноса строки (\n)
    while (c != '\n') {
        if (c == '|'){
            (*k)++;
        }
        if(ignore_flag_bs ==1){
            ignore_flag_bs = 0;
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

        s[(len)++] = c; // заносим в строку новый символ

        // если реальный размер больше размера контейнера, то увеличим его размер
        if (len >= capacity) {
            capacity *= 2; // увеличиваем ёмкость строки в два раза
            s = (char*) realloc(s, capacity * sizeof(char)); // создаём новую строку с увеличенной ёмкостью  
        }

        c = getchar(); // считываем следующий символ  

        if (c == '\n' && ignore_flag_bs == 1){
            len--;

            c = getchar();
            ignore_flag_bs = 0;
        }     
        else{
            if (c == '\n' && ignore_flag_q == 1){
                s[(len)++] = ' ';
                if (len >= capacity) {
                    capacity *= 2; // увеличиваем ёмкость строки в два раза
                    s = (char*) realloc(s, capacity * sizeof(char)); // создаём новую строку с увеличенной ёмкостью  
                }

                c = getchar();
            } 
        }
    }

    s[len] = '\0'; // завершаем строку символом конца строки

    return s; // возвращаем указатель на считанную строку
}


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