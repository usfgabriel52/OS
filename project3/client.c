#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>
#include <pthread.h>

#define BUFFERSIZE 100
#define MAX_MSG_LEN 1024
#define NUMBER_OF_MSG 5


struct message_buffer {
	pthread_mutex_t mutex;
	pthread_cond_t full, empty;
	int front, tail;
	char messages[NUMBER_OF_MSG][MAX_MSG_LEN];
};

struct shm_data {
	struct message_buffer s2c;
	struct message_buffer c2s;
};

void * sendMessages();

struct shm_data *shared;
int sendArrayCount;

int main(int argc, char *argv[]){
    sendArrayCount = 0;
    char* key = argv[1];
    printf("Getting MemID\n");
    int memId;
    memId = shmget(atoi(key),sizeof(struct shm_data),0666);
    if (memId == 0){
        return -1;
    }
    printf("Attaching MemId\n");
    shared = (struct shm_data*) shmat(memId,NULL,0);
    printf("%d\n",shared->c2s.front);
    printf("%d\n",shared->c2s.tail);
    pthread_t sendThreadId;
    
        int err = pthread_create(&sendThreadId,NULL,&sendMessages,NULL);
    
    pthread_exit(NULL);
}

void * sendMessages(){
    while(1){
    
        sendArrayCount++;
        if (sendArrayCount == 5){
            sendArrayCount = 0;
        }

        pthread_mutex_lock(&shared->c2s.mutex);
        while((shared->c2s.tail - shared->c2s.front) == NUMBER_OF_MSG)
            pthread_cond_wait(&shared->c2s.empty,&shared->c2s.mutex);
        printf("Enter Message: ");
        fgets(shared->c2s.messages[shared->c2s.tail%5],MAX_MSG_LEN,stdin);
        shared->c2s.tail++;
        pthread_cond_signal(&shared->c2s.empty);
        pthread_mutex_unlock(&shared->c2s.mutex);
        

    }
    return 0;
}