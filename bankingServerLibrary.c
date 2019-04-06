#include "bankingServerHeader.h"

//return 0 if bad input
int handleMessage(char* clientMessage, int sock){

    printf("Message recieved: %s\n", clientMessage); 
    accountInfo* ptr;
    char *message;
    double amount;
    char * tok = strtok(clientMessage, " ");
    char * tok2;
    if (strcmp(tok, "create") == 0){
        tok = strtok(NULL, " "); //gets account name
        tok[strcspn(tok, "\n")] = 0;
        if(head != NULL){
            ptr = head;
            while(ptr != NULL){
                if (strcmp(ptr->accountName, tok) == 0){
                    message = "Error: Account already exists.\n";
                    send(sock , message , strlen(message) , 0 );
                    return;
                }
                ptr = ptr->next;
            }
        }
        //creates node for account
        accountInfo* temp = (accountInfo *) malloc (sizeof(accountInfo));
        temp -> accountName = malloc (256);
        strcpy(temp->accountName, tok);
        temp->currentBalance = 0;
        temp->inSession = 0;
        pthread_mutex_lock(&mutex);

        //add new account to list, mutexed
        temp -> next = head;
        head = temp;

        pthread_mutex_unlock(&mutex);
     
        message =  "account created\n";
        send(sock , message , strlen(message) , 0);      
    }
    else if (strcmp(tok, "serve") == 0){
        tok = strtok(NULL, " "); //gets account name
        tok[strcspn(tok, "\n")] = 0;
        if (head == NULL){
            message =  "Error: Account does not exist.\n";
            send(sock , message , strlen(message) , 0 );
        }
        else{
            ptr = head;
            //goes through whole linked list to see if any account is in session
            while(ptr != NULL){
                if (strcmp(ptr->accountName,tok) == 0 && ptr->inSession == 1){
                    message = "Error: Service session active for this account. Cannot start another session.\n";
                    send(sock , message , strlen(message) , 0 );
                    return;
                }
                ptr = ptr->next;
            }
            ptr = head; 
            //goes through list to find account name
            while(ptr != NULL){
                if (strcmp(ptr->accountName, tok) == 0){
                    
                    pthread_mutex_lock(&sessionMutex);

                    ptr->inSession = 1;

                    pthread_mutex_unlock(&sessionMutex);

                    message = "Account in session.\n";
                    send(sock , message , strlen(message) , 0 );
                    return;
                }
                ptr = ptr->next;
            }
            if (ptr == NULL){
                message = "Error: Account does not exist.\n";
                send(sock , message , strlen(message) , 0 );
            }
        }
    }
    else if (strcmp(tok, "deposit") == 0){
        tok = strtok(NULL, "\n"); //gets amount of money
        amount = atof(tok);
        tok2 = strtok(NULL,"\n"); //gets account name 
        if (head == NULL){
            message =  "Error: No accounts to deposit to. Create account first.\n";
            send(sock , message , strlen(message) , 0 );
        }
        else{
            ptr = head;
            while (ptr != NULL){
                //finds the account in session
                if (strcmp(ptr->accountName, tok2) == 0){

                    pthread_mutex_lock(&deposit_mutex);

                    ptr->currentBalance += amount;

                    pthread_mutex_unlock(&deposit_mutex);;

                    message = "Money deposited successfully.\n";
                    send(sock , message , strlen(message) , 0 );
                    return;
                }
                ptr = ptr->next;
            }
            if (ptr == NULL){
                message = "Error: No account in session. \n";
                send(sock , message , strlen(message) , 0 );
            }

        }
    }
    else if (strcmp(tok, "withdraw") == 0){
        tok = strtok(NULL, "\n");
        amount = atof(tok);
        tok2 = strtok(NULL,"\n"); //gets account name
         if (head == NULL){
            message =  "Error: No accounts to withdraw from. Create account first.\n";
            send(sock , message , strlen(message) , 0 );
            return;
        }
        else{
            ptr = head;
            while (ptr != NULL){
                //finds the account in session
                if (strcmp(ptr->accountName, tok2) == 0){
                    if (ptr->currentBalance >= amount){

                        pthread_mutex_lock(&withdraw_mutex);

                        ptr->currentBalance -= amount;

                        pthread_mutex_unlock(&withdraw_mutex);

                        message = "Money withdrawn successfully.\n";
                        send(sock , message , strlen(message) , 0 );
                        return;
                    }
                    else{
                        message = "Error: Not enough money in your account.\n";
                        send(sock , message , strlen(message) , 0 );
                        return;
                    }
                }
                ptr = ptr->next;
            }
            // if no account is in session, you cannot deposit
             if (ptr == NULL){
                message = "2. Error: No account in session\n";
                send(sock , message , strlen(message) , 0 );
            }
        }
    }
    else if (strcmp(tok, "query\n" )== 0){
        tok2 = strtok(NULL,"\n"); //gets account name
        if (head == NULL){
            message =  "Error: No accounts available. Create account first.\n";
            send(sock , message , strlen(message) , 0 );
        }
        else{
            ptr = head;
            while (ptr != NULL){
                //finds the account in session
                if (strcmp(ptr->accountName, tok2) == 0){
                    float x = ptr->currentBalance;
                    char buf[350];
                    gcvt(x, 6, buf);
                    send(sock , (void *)buf , 350 , 0 );  
                    return;
                }
                ptr = ptr->next;
                //if no account in session
            }
            if (ptr == NULL){
                message = "Error: No account in session\n";
                send(sock , message , strlen(message) , 0);
            }
        }
    }
    else if (strcmp(tok, "end\n") == 0){
        tok2 = strtok(NULL,"\n"); //gets account name
        if (head == NULL){
            message =  "Error: No account in session. Create account first.\n";
            send(sock , message , strlen(message) , 0 );
        }
        else{
            ptr = head;
            while (ptr != NULL){
                //finds the account in session
                if (strcmp(ptr->accountName, tok2) == 0){

                    pthread_mutex_lock(&sessionMutex);

                    ptr->inSession = 0; 

                    pthread_mutex_unlock(&sessionMutex);

                    message = "Current session has ended.\n";
                    send(sock , message , strlen(message) , 0 );  
                    return;
                }
                ptr = ptr->next;
            }
            if (ptr == NULL){
                message = "Error: No account in session\n";
                send(sock , message , strlen(message) , 0 );
            }
        }
    }
    else if (strcmp(tok, "quit\n") == 0){ //has to disconnet client from server. IDK how to do that yet...
        if(head != NULL){
            tok2 = (strtok(NULL, "\n"));
            ptr = head;
            while(ptr != NULL){
                if(strcmp(tok2, ptr -> accountName) == 0){

                    pthread_mutex_lock(&sessionMutex);

                    ptr -> inSession = 0;

                    pthread_mutex_unlock(&sessionMutex);
                    break;
                }
                ptr = ptr -> next;
            }
        }
        close(sock);
    }
    else{
        message = "Invalid command\n";
        send(sock , message , strlen(message) , 0 );
    }
}