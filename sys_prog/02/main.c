#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "parser.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"


int main(){
    struct cmd *cmds; 
    int k;
    int to_next[2];
    char buf[1024];
    while(1){
        printf("> ");
        char *str = get_string(&k);
        cmds = parserr(str);
        pipe(to_next);
        if(fork() == 0){
            
            if(k != 0){
                close(to_next[0]);
                dup2(to_next[1], 1);
                close(to_next[1]);
                return execvp(cmds[0].name, cmds[0].argv);
            }
            else{
                return execvp(cmds[0].name, cmds[0].argv);
            }
            
        }
        wait(NULL);
        for(int i = 1; i < k + 1; i++){
            if(i != k){
                if(fork() == 0){
                    printf("Hell world");
                    dup2(to_next[0], 0);
                    close(to_next[0]);
                    read(to_next[1], buf, sizeof(buf));
                    printf("%s", buf);
                    dup2(to_next[1], 1);
                    //close(to_next[1]);
                    return execvp(cmds[i].name, cmds[i].argv);
                }
            }
            else{
                if(fork() == 0){
                    dup2(to_next[0], 0);
                    close(to_next[0]);
                    close(to_next[1]);
                    return execvp(cmds[i].name, cmds[i].argv);
                }
            }   
            close(to_next[1]);
            close(to_next[0]);
        }
        //close(to_next[1]);
        //close(to_next[0]);
        wait(NULL);    
    
    }
    return 0;
}