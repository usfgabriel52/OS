#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h> // For FIONREAD
#include <stdbool.h>
#define BUFFERSIZE 100
#define MAX_MSG_LEN 1024
#define NUMBER_OF_MSG 5

struct key_hit {
    pthread_mutex_t mutex;
    pthread_cond_t noType, noReceive;
    int typing,receiving;
};

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
void * recieveMessages();
void * keyPressed();
// void set_mode(int want_key);
// int get_key();
int kbhit(void);

struct key_hit *kh;
struct shm_data *shared;
int sendArrayCount;

int main(int argc, char *argv[]){
    char* key = argv[1];
    int threadShareId;
    threadShareId = shmget(IPC_PRIVATE,sizeof(struct key_hit),IPC_CREAT | 0666);
    if(threadShareId < 0) return 0;
    
    kh = (struct key_hit*) shmat(threadShareId,NULL,0);

    pthread_cond_init(&kh->noType,NULL);
    pthread_cond_init(&kh->noReceive,NULL);
    pthread_mutex_init(&kh->mutex,NULL);
    kh->typing = 0;
    kh->receiving =0;
    sendArrayCount = 0;
    
    int memId;
    memId = shmget(atoi(key),sizeof(struct shm_data),0666);
    if (memId == 0){
        return -1;
    }
    shared = (struct shm_data*) shmat(memId,NULL,0);





    pthread_t sendThreadId,recieveThreadId,khThreadId;
    
    int err = pthread_create(&sendThreadId,NULL,&sendMessages,NULL);
    int err2 = pthread_create(&recieveThreadId,NULL,&recieveMessages,NULL);
    pthread_exit(NULL);
}

void * sendMessages(){
    char message[MAX_MSG_LEN];
    
    while(1){
        fflush(stdout);
        printf("client>");
        fflush(stdout);
        pthread_mutex_lock(&shared->c2s.mutex);
        while((shared->c2s.tail - shared->c2s.front) != 0)
            pthread_cond_wait(&shared->c2s.full,&shared->c2s.mutex);
        while(!kbhit());
        fflush(stdout);
        pthread_mutex_lock(&kh->mutex);
        while(kh->receiving == 1)
            pthread_cond_wait(&kh->noReceive,&kh->mutex);
        kh->typing = 1;
        if(fgets(shared->c2s.messages[shared->c2s.tail%NUMBER_OF_MSG],MAX_MSG_LEN,stdin) != NULL){
            shared->c2s.tail++;
        }  
        kh->typing = 0;
        pthread_cond_signal(&kh->noType);
        pthread_mutex_unlock(&kh->mutex);
        pthread_cond_signal(&shared->c2s.empty);
        pthread_mutex_unlock(&shared->c2s.mutex);
    }
    return 0;
}

void * recieveMessages(){
    while(1){
        pthread_mutex_lock(&shared->s2c.mutex);
        while(!(shared->s2c.front < shared->s2c.tail))
            pthread_cond_wait(&shared->s2c.empty,&shared->s2c.mutex); 
        pthread_mutex_lock(&kh->mutex);
        while(kh->typing == 1)
            pthread_cond_wait(&kh->noType,&kh->mutex);
        printf("\nMessage Recieved: %s",shared->s2c.messages[shared->s2c.front%NUMBER_OF_MSG]);
        fflush(stdout);
        printf("client>");
        fflush(stdout);
        shared->s2c.front++;
        pthread_cond_signal(&kh->noReceive);
        pthread_mutex_unlock(&kh->mutex);
        pthread_cond_signal(&shared->s2c.full);
        pthread_mutex_unlock(&shared->s2c.mutex);
        
       

    }

}


int kbhit(void) {
    static bool initflag = false;
    static const int STDIN = 0;

    if (!initflag) {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initflag = true;
    }

    int nbbytes;
    ioctl(STDIN, FIONREAD, &nbbytes);  // 0 is STDIN
    return nbbytes;
}
