#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "udp_comm.h"
#include "../global.h"

int sd;
struct sockaddr_in addr;
char rcv_message[MAX_STR_SIZE];
char* specs;

void initServerMediator(int port)
{
    sd = UDP_Open(port);
}

void sendResponse(char* message)
{
    UDP_Write(sd, &addr, message, MAX_STR_SIZE);
}

char* getNextSpecification() { return strtok(NULL, "-"); }

int readRequest()
{
    bzero(rcv_message, MAX_STR_SIZE);
    
    int rc = UDP_Read(sd, &addr, rcv_message, MAX_STR_SIZE);
    if(rc > 0)
    {
        specs = strtok(rcv_message, "-");
        return atoi(specs);   
    }
}