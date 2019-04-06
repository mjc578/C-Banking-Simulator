#include "bankingClientHeader.h"

int processInput(char* copy, char* input){

    char * tok = strtok(copy, " ");
    if(strcmp(tok, "create") == 0){
        //get name
        tok = strtok(NULL, " ");
        //checks if this client already is in session
        if(clientHasSession){
            printf("In session. Cannot create new account at this time.\n");
            return 0;
        }
        if(strlen(tok) > 255){
            printf("Username inputted is too long. Must be less than 256 characters.\n");
            return 0;
        }
    }
    else if(strcmp(tok, "serve") == 0){
        //check if client is in a session already
        if(clientHasSession){
            printf("In session. Cannot start another session.\n");
            return 0;
        }
        //otherwise, set the serve name as this current name and flip their session bool
        else{
            tok = strtok(NULL, " ");
            clientHasSession = 1;
            accountInSession = tok;
        }
    }
    else if(strcmp(tok, "deposit") == 0){
        //check if client is in session, if not error
        if(!clientHasSession){
            printf("Not in session. Start session to deposit.\n");
            return 0;
        }
        else{
            strcat(input, accountInSession);
        }
    }
    else if(strcmp(tok, "withdraw") == 0){
        //check if client is in session, if not error
        if(!clientHasSession){
            printf("Not in session. Start session to withdraw.\n");
            return 0;
        }
        else{
            strcat(input, accountInSession);
        }
    }
    else if(strcmp(tok, "query\n") == 0){
        //check if in session
        if(!clientHasSession){
            printf("Not in session. Start session to query.\n");
            return 0;
        }
        //add the account in session name to end of command to send to thing
        else{
            char* space = strcat(input," ");
            strcat(space, accountInSession);
        }
    }
    else if(strcmp(tok, "end\n") == 0){
        //check if in session
        if(!clientHasSession){
            printf("Not in session.\n");
            return 0;
        }
        //turn client in session off
        else{
            char* space = strcat(input," ");
            strcat(space, accountInSession);
            clientHasSession = 0;
        }
    }
    else if(strcmp(tok, "quit\n") == 0){
        //server handles this, just close the client's socket
        char* space = strcat(input, " ");
        strcat(space, accountInSession);

    }
    else{
        printf("Invalid input.\n");
        return 0;
    }
    //no errors!
    return 1;
}


//kill acceptor thread for server when shutting down