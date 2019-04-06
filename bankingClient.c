#include "bankingClientHeader.h"

int clientHasSession = 0;
char* accountInSession;

volatile int run = 1;

void * commandFunction(void * voidptr){
    //gets user input and send it to server
    int i;
    int *sockptr = voidptr;
    int sock = *sockptr;
    while(run){
        char input[1024];
        printf("Input command: ");
        fgets(input, 1024, stdin);

        char* copy = malloc (strlen(input));
        strcpy(copy, input);
        int status = processInput(copy, input);

        if(status){
            send(sock, input, strlen(input) + 1, 0);
        }
        sleep(2);
    }
}

void * responseFunction(void* voidptr){
     //reads message from client 
    int *sockptr = voidptr;
    int sock = *sockptr;
    int readSock;
    while(run){
        char buffer[1024] = {0};
        readSock = read(sock , buffer, 1024);
        if(strcmp(buffer, "Error: Account does not exist.\n") == 0 || strcmp(buffer, "Error: Service session active for this account. Cannot start another session.\n") == 0){
            clientHasSession = 0;
        }
        if(readSock == 0){
            run = 0;
            printf("\nDisconnected from server.\n");
            break;
        }
        printf("Message recieved: %s\n", buffer);
    } 
}

int get_ip(char * hostname , char* ip) 
{  struct hostent *he;     
   struct in_addr **addr_list;     
   int i;     
   if ( (he = gethostbyname( hostname ) ) == NULL)     
   { herror("gethostbyname");         
     return 1;}     
   addr_list = (struct in_addr **) he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++)
    {   strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;}
    return 1;
}

int main(int argc, char **argv){

	if(argc != 3){
		printf("Invalid number of arguments.\n");
		exit(0);
	}

    struct addrinfo pHostInfo;
	struct addrinfo* rpl = NULL;
    struct sockaddr_in server_addr; 
    int sock = 0; 
    int *sockptr;

	char *hostname = argv[1];     
    char ip[100];     
    get_ip(hostname , ip); 

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("Error creating socket.\n"); 
		exit(0);
    }
   
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(atoi(argv[2])); 

    if(inet_pton(AF_INET, ip, &server_addr.sin_addr)<=0)  
    { 
        printf("\nError: Address not supported \n"); 
        return -1; 
    } 

    while(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        sleep(3);
    } 

    printf("Connection to server successful\n");

    pthread_t commandThread, responseThread;
    sockptr = (int*)malloc(sizeof(int));
    *sockptr = sock;
    pthread_create(&commandThread, NULL, commandFunction, (void *)sockptr);
    pthread_create(&responseThread, NULL, responseFunction, (void*)sockptr);

    while(run){}

    close(sock);

    pthread_cancel(&commandThread);
    pthread_cancel(&responseThread); 
	
    return 0; 
}



