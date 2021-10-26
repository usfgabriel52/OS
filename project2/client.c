#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

#define BUFFERSIZE 100


typedef struct MESSAGE{ 
   int recv_count; 
   int send_count; 
   char buf[4]; 
}Message; 

int main(int argc, char *argv[]){
    char* key = argv[1];
    printf("Getting MemID\n");
    int memId;
    memId = shmget(atoi(key),sizeof(Message),0666);
    if (memId == 0){
        return -1;
    }
    printf("Attaching MemId\n");
    Message *shared;
    shared = (Message *) shmat(memId,NULL,0);
    char input[100];
    printf("Enter Message: ");
    fgets(input,100,stdin);
    while(*input != '\n'){
        
        int i = 0 ;
        while(i+4 <= strlen(input)){
            while(shared->send_count > shared->recv_count);
                for(int j = 0; j<4;j++){
                    shared->buf[j] = input[i+j];
                }
            shared->send_count++;
            i+=4;
        }
        while(shared->send_count > shared->recv_count);
        int j = 0;
        while (i < strlen(input)){
            shared->buf[j] = input[i];
            i++;
            j++;
        }
        for(j;j<4;j++){
            shared->buf[j] = NULL;
        }
        shared->send_count++;
        
        printf("Message Sent\n");
        printf("Enter Message: ");
        fgets(input,100,stdin);
    }
    if(shmdt(shared) < 0){
        return -1;
    }
    return 0;
}