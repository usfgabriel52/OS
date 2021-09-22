#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int argi = 1;
    char* cmd1 = argv[argi];
    while(*argv[argi] != '|'){
        argi++;
    }
    char* params1[argi];
    int params1Size = argi;
    argi++;
    
    for(int i = 1; i < argi-1; i++){
        params1[i-1] = argv[i];
    }
    params1[params1Size-1] = NULL;
    
    char* cmd2 = argv[argi];
    char* params2[argc-argi+1];
    int params2Size = argc-argi+1;
    
    int i = 0;
    while(argi < argc){
        params2[i] = argv[argi];
        i++;
        argi++;
    }
    params2[params2Size-1] = NULL;

    int link[2];
    pipe(link);
    int id,id2;

    if((id = fork()) == 0){
        dup2 (link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]); 
        execvp(cmd1,params1); 
    }
    else if((id2 = fork()) == 0) {
            dup2(link[0],STDIN_FILENO);
            close(link[0]);
            close(link[1]);
            execvp(cmd2,params2);
    }
    close(link[0]);
    close(link[1]);
    wait(&id);
    wait(&id2);
    printf("++++\n");

    return 0;
};