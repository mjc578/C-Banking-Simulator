#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <signal.h>
#include <sys/time.h>
#define PORT 12144 

//globals for the acceptor thread
extern struct sockaddr_in address; 
extern int server_fd;
extern int addressLength;

extern pthread_mutex_t mutex;
extern pthread_mutex_t deposit_mutex;
extern pthread_mutex_t withdraw_mutex;
extern pthread_mutex_t sessionMutex;

typedef struct accountInfo{
    char * accountName;
    double currentBalance;
    int inSession; // 1 if in session, 0 if not in session
    struct accountInfo *next;
} accountInfo;

int handleMessage(char* clientMessage, int sock);

extern accountInfo *head;

