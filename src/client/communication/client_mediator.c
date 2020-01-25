#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>	

#include "requests.h"
#include "udp_comm.h"
#include "../global.h"

#define MAX_REQUEST_ATTEMPTS 10

int sd;
struct sockaddr_in addrSnd, addrRcv;

void initClientMediator(char* serverHostName, int serverPort, int clientPort)
{
	sd = UDP_Open(clientPort);
	UDP_FillSockAddr(&addrSnd, serverHostName, serverPort);
}

int sendRequest(char* message)
{
	int response = INVALID_REQUEST;
	char response_str[MAX_STR_SIZE];

	for(int i = 0; i < MAX_REQUEST_ATTEMPTS && response == INVALID_REQUEST; i++)
	{
		int rc = UDP_Write(sd, &addrSnd, message, MAX_STR_SIZE);
		if(rc > 0)
		{
			bzero(response_str, MAX_STR_SIZE);

			rc = UDP_Read(sd, &addrRcv, response_str, MAX_STR_SIZE);
			if(rc > 0)
			{
				response = atoi(response_str);
				if(response == 0) response = VALID;
			}
		}
	}

	if(response == INVALID_REQUEST) printf("Error: Wasnt able to perform request\n");

	bzero(message, MAX_STR_SIZE);
	strcpy(message, response_str);
	return response;
}

int med_login(char* name, char* pass)
{
    char message[MAX_STR_SIZE];
	sprintf(message, "%d-%s-%s", LOGIN, name, pass);
	return sendRequest(message);
}

int med_regist(char* name, char* pass)
{
    char message[MAX_STR_SIZE];
	sprintf(message, "%d-%s-%s", REGISTER, name, pass);
	return sendRequest(message);
}

void med_users(char* users)
{
    char message[MAX_STR_SIZE];
	sprintf(message, "%d", USERS);
	if(sendRequest(message) == VALID) strcpy(users, message);
}

int med_removeUser(char* name)
{
    char message[MAX_STR_SIZE];
	sprintf(message, "%d-%s", REMOVE, name);
	return sendRequest(message);	
}

int med_viewMessages(char* name_0, char* name_1, char* messages)
{
    char message[MAX_STR_SIZE];
	sprintf(message, "%d-%s-%s", VIEW, name_0, name_1);

	int ret = sendRequest(message);
	if(ret == VALID) strcpy(messages, message);

	return ret;
}	

int med_sendMessage(char* sender, char* receiver, char* msg)
{
    char message[MAX_STR_SIZE];
	sprintf(message, "%d-%s-%s-%s", SEND, sender, receiver, msg);
	return sendRequest(message);
}