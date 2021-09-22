#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(){
    char entry[100];
    fgets(entry,100,stdin);
    int totalSpace = 0;
    for(int i = 0; i< strlen(entry);i++){
        if (entry[i] == ' ')
            totalSpace++;
    }
    char argument_list[totalSpace+1][100];
    int lastpos = 0;
    for(int i = 0; i < totalSpace+1;i++){
        int j =0;
        while(entry[lastpos + j] !=' '){
            if(entry[lastpos + j] == '\0' ||entry[lastpos + j] == '\n' )
                break;
            argument_list[i][j] = entry[lastpos + j];
            j++;
        }
        lastpos += j+1;
        argument_list[i][j] = '\0';
    }
    char* argl[totalSpace +2];
    for (int i = 0; i < totalSpace +1 ; i++) 
        argl[i] = argument_list[i];
    argl[totalSpace+1] = NULL;

    int id;
    
    if ((id = fork()) == 0){
        execvp(argl[0],argl);
    }else{
        wait(&id);
        printf("++++\n");
    }
    return 0;
}