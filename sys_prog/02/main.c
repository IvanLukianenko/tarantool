#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "parser.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include <fcntl.h>



int main(){
    /*main variables*/
    struct cmd *cmds; 
    int k;
    int pipe1[2], pipe2[2];
    int oRedirectFlag;
    char *oFileName;
    /*additional variables*/
    char buf[1024];

    
    while(1){
        printf("\nLukianenko@tarantoolShell> ");
        char *str = get_string(&k);

        if (strcmp(str, "exit") == 0){
            printf("exitting ...\n");
            return 0;
        }
        cmds = parserr(str);
        //print_args(cmds[0]);
        //break;
        if (strcmp(cmds[0].name, "cd") == 0){
            if (cmds[0].argv[1] == NULL) {
                fprintf(stderr, "> ожидается аргумент для \"cd\"\n");
            } else {
                if (chdir(cmds[0].argv[1]) != 0) {
                    perror("lsh");
                }
        }
        }
        for(int i = 0; i < k + 1; i++){

            oRedirectFlag = 0;

            for(int q = 0; q < cmds[i].argc - 1; q++){
                if (strcmp(cmds[i].argv[q], ">>") == 0){
                    oRedirectFlag++;
                    oRedirectFlag++;
                    oFileName = strdup(cmds[i].argv[q+1]);
                    cmds[i].argv[q] = NULL;
                    
                }
                else if(strcmp(cmds[i].argv[q], ">") == 0){
                    oRedirectFlag++;
                    oFileName = strdup(cmds[i].argv[q+1]);
                    cmds[i].argv[q] = NULL;
                }
            }
            if((k > 0) && (i != k)){
                pipe(pipe2);
            }
            if(fork() == 0){ 
                /*<Определить что делать с перенаправлением в файл>*/
                if(oRedirectFlag == 2 && i == k){
                    int ofd = open(oFileName, O_WRONLY | O_APPEND);
                    if(ofd < 0){
                        /*обработать*/
                    }
                    close(STDOUT_FILENO);
                    dup(ofd);
                    close(ofd);
                }
                if(oRedirectFlag == 1 && i == k){
                    int ofd = open(oFileName, O_WRONLY | O_CREAT, 00700);
                    if(ofd < 0){
                        /*обработать*/
                    }
                    close(STDOUT_FILENO);
                    dup(ofd);
                    close(ofd);
                }

                if (k > 0){
                    if (i == 0){// самый первый процесс
                        close(STDOUT_FILENO);
                        dup(pipe2[STDOUT_FILENO]);
                        close(pipe2[STDOUT_FILENO]);
                        close(pipe2[STDIN_FILENO]);
                    }
                    else if(i < k){ // промежуточный процесс
                        close(STDIN_FILENO);
                        dup(pipe1[STDIN_FILENO]);
                        close(pipe1[STDIN_FILENO]);
                        close(pipe1[STDOUT_FILENO]);
                        close(STDOUT_FILENO);
                        dup(pipe2[STDOUT_FILENO]);
                        close(pipe2[STDOUT_FILENO]);
                        close(pipe2[STDIN_FILENO]);
                    }
                    else{       // финальный процесс
                        close(STDIN_FILENO);
                        dup(pipe1[STDIN_FILENO]);
                        close(pipe1[STDIN_FILENO]);
                        close(pipe1[STDOUT_FILENO]);
                    }
                }
                execvp(cmds[i].name, cmds[i].argv);
                //break;
                
            }
            else{
                if(i > 0){
                    close(pipe1[0]);
                    close(pipe1[1]);
                }
                pipe1[0] = pipe2[0];
                pipe1[1] = pipe2[1];
                wait(NULL);
                if(oRedirectFlag > 0){
                    free(oFileName);
                }  
                //break;
            }     
            
            
        }  
    
    }
    return 0;
}