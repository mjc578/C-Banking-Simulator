#include "bankingServerHeader.h"
#define INTERVAL 500

pthread_mutex_t mutex;
pthread_mutex_t deposit_mutex;
pthread_mutex_t withdraw_mutex;
pthread_mutex_t sessionMutex;

//globals for the acceptor thread
struct sockaddr_in address; 
int server_fd;
int addressLength;

accountInfo *head = NULL;

//TODO could make run global and all threads could stop when run == 0
volatile int run = 1;

void ctrlcHandler(int var){
    run = 0;
}

void printAccounts(void);

void printAccounts(void){
    pthread_mutex_lock(&mutex);
    pthread_mutex_lock(&deposit_mutex);
    pthread_mutex_lock(&withdraw_mutex);
    pthread_mutex_lock(&sessionMutex);
    printf("\n___________________________________________\n\n");
    printf("BANK ACCOUNTS:\n");
    if (head != NULL){
        accountInfo * ptr = head;
        printf("\n");
        while(ptr != NULL){
            printf("%s  %lf  ", ptr->accountName, ptr->currentBalance);
            if(ptr->inSession == 1){
                printf("IN SERVICE\n");
            }
            else{
                printf("\n");
            }
            ptr = ptr->next;
                }
        }
    printf("___________________________________________\n\n\n");
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&deposit_mutex);
    pthread_mutex_unlock(&withdraw_mutex);
    pthread_mutex_unlock(&sessionMutex);

}
void listAccounts(){

    struct itimerval timer;  
    if (signal(SIGALRM, (void (*)(int)) printAccounts) == SIG_ERR) {
        perror("Error catching SIGALRM");
    }
    timer.it_value.tv_sec =     15;
    timer.it_value.tv_usec =    0;   
    timer.it_interval = timer.it_value;
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("Error calling setitimer");
    }

    while (1) 
        pause();
}

void* handleClient(void* passedTheSock){

    int sock = *(int*) passedTheSock;

    int read_size;
    char client_message[2000];

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0){
        //What you want to do with the client's message
        handleMessage(client_message, sock);
        
    }

    printf("Client disconnected\n");
    fflush(stdout);

    if(read_size == -1)
    {
        //error whilereading the client pls
    }

    //Free the socket pointer
    free(passedTheSock);
    close(sock);
}

void* clientAcceptor(void* nothing){

    int csock;

    while((csock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addressLength)) && run){ 
        
        printf("Client connection accepted\n");
        //call client handler thread
        
        pthread_t clientHandler;
        int* passTheSock = malloc(1);
        *passTheSock = csock;

        if(pthread_create(&clientHandler, NULL, handleClient, (void*) passTheSock) < 0){

            //thread create error here please

        }
    } 
}

int main(int argc, char *argv[]) {

    if(argc != 2){
        printf("Invalid number of arguments.\n");
        exit(0);
        //for stupid commit
    }
 
    struct sockaddr_in address; 
    int sockOpt = 1; 
    addressLength = sizeof(address); 
    char buffer[1024] = {0}; 
    
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&deposit_mutex, NULL);
    pthread_mutex_init(&withdraw_mutex, NULL);
    pthread_mutex_init(&sessionMutex, NULL);
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 12144
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &sockOpt, sizeof(sockOpt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( atoi(argv[1]) ); 
       
    // Forcefully attaching socket to the port 12144 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }

    //set handler for crtl + C
    signal(SIGINT, ctrlcHandler);

    //spawn client acceptor
    pthread_t acceptor;
    pthread_create(&acceptor, NULL, clientAcceptor, NULL);

    pthread_t listAccountsThread;
    pthread_create(&listAccountsThread, NULL, listAccounts, NULL);

    //program will stay active unless ctrl + C is hit server side
    while(run){

    }
    //ctrl + c must have been pressed to get here so kill threads and clean up stuff...

    pthread_join(&acceptor, NULL);
    pthread_join(&listAccountsThread, NULL);



    return 0; 
} 