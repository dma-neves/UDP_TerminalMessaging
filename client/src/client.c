#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#include "chat/user.h"
#include "chat/chat.h"
#include "communication/client_mediator.h"
#include "communication/requests.h"
#include "global.h"

typedef enum _State {LOGGEDOUT, LOGGEDIN, TERMINATE} State;

User usr;
bool admin = false;
State state;

void scan(char* str)
{
	fgets(str, MAX_STR_SIZE, stdin);
	if(str[0] != '\n') str[strlen(str)-1] = (char)0;
}

void initClient()
{
	admin = false;
}

void login()
{
	char name[MAX_STR_SIZE];
	char pass[MAX_STR_SIZE];

	printf("\nuser name: ");
	fflush(stdout);
	scan(name);

	printf("user password: ");
	scan(pass);

	if(med_login(name, pass) != INVALID_SPECIFICATIONS)
	{
		printf("\nSuccessful login\n\n");
		strcpy(usr.name, name);
		strcpy(usr.pass, pass);
		state = LOGGEDIN;
		admin = strcmp(name, "admin") == 0;
	}
	else
	{
		printf("\nInvalid login\n\n");
	}
	
}

void regist()
{
	char name[MAX_STR_SIZE];
	char pass[MAX_STR_SIZE];

	printf("\nuser name: ");
	fflush(stdout);
	scan(name);

	printf("user password: ");
	scan(pass);

	if( med_regist(name, pass) == INVALID_SPECIFICATIONS )
	{
		printf("\nInvalid registration\n\n");
	}
	else
	{
		printf("\nSuccessful registration\n\n");
	}
}

void removeUser()
{
	if( med_removeUser(usr.name) == INVALID_SPECIFICATIONS )
	{
		printf("\nInvalid removal\n\n");
	}
	else
	{
		printf("\nSuccessful removal\n\n");
		state = LOGGEDOUT;
	}
}

void viewMessages()
{
	char name[MAX_STR_SIZE];
	char messages[MAX_STR_SIZE];

	printf("\nuser name: ");
	fflush(stdout);
	scan(name);

	if( med_viewMessages(usr.name, name, messages) == INVALID_SPECIFICATIONS )
	{
		printf("\nInvalid user\n\n");
	}
	else
	{
		printf("\n%s\n", messages);
	}
}

void sendMessage()
{
	char receiver[MAX_STR_SIZE];
	char message[MAX_STR_SIZE];

	printf("\nreceiver name: ");
	fflush(stdout);
	scan(receiver);

	printf("message: ");
	fflush(stdout);
	scan(message);

	if( med_sendMessage(usr.name, receiver, message) == INVALID_SPECIFICATIONS )
	{
		printf("\nInvalid user\n\n");
	}
	else
	{
		printf("\nMessage sent\n\n");
	}
}

void viewDebug()
{
	char debug_message[MAX_STR_SIZE];

	if( med_viewDebug(usr.pass, debug_message) == INVALID_SPECIFICATIONS )
	{
		printf("\nInvalid user\n\n");
	}
	else
	{
		printf("\n%s", debug_message);
	}
}

void dataOp(char* op)
{
	if(med_dataOp(usr.pass, op) == VALID)
	{
		printf("\nSuccessful data operation\n\n");
	}
	else
	{
		printf("\nInvalid data operatio\n\n");
	}
}

void run(char* serverHostName, int serverPort, int clientPort)
{
	initClientMediator(serverHostName, serverPort, clientPort);

	char cmd[MAX_STR_SIZE];

	while(state != TERMINATE)
	{
		if(state != LOGGEDOUT) printf("%s", usr.name);
		printf("> ");
		fflush(stdout);

		scan(cmd);
		if(cmd[0] == '\n') continue;

		if(state == LOGGEDOUT)
		{
			if(!strcmp(cmd, "help"))
			{
				printf("<login>\n");
				printf("<register>\n");
				printf("<exit>\n");
			}
			else if(!strcmp(cmd, "login"))
			{
				login();
			}
			else if(!strcmp(cmd, "register"))
			{
				regist();
			}
			else if(!strcmp(cmd, "exit"))
			{
				state = TERMINATE;
			}
			else
			{
				printf("Error: invalid command <%s> use <help> to view available commands\n", cmd);
			}
		}
		else if(state == LOGGEDIN)
		{
			if(admin)
			{
				if(!strcmp(cmd, "help"))
				{
					printf("<cleardata> - clear all data from the server\n");
					printf("<savedata> - save data on disk\n");
					printf("<loaddata> - load data from disk\n");
					printf("<debug> - get debug information\n");
					printf("<logout>\n");
					printf("\n");
				}
				else if(!strcmp(cmd, "logout"))
				{
					printf("Logged out\n\n");
					state = LOGGEDOUT;
				}
				else if(!strcmp(cmd, "debug"))
				{
					viewDebug();
				}
				else if(!strcmp(cmd, "cleardata") || !strcmp(cmd, "loaddata") || !strcmp(cmd, "savedata"))
				{
					dataOp(cmd);
				}
				else
				{
					printf("Error: invalid command <%s> use <help> to view available commands\n", cmd);
				}
			}
			else
			{
				if(!strcmp(cmd, "help"))
				{
					printf("<users> - view existing users\n");
					printf("<remove> - remove current user\n");
					printf("<send> - send message\n");
					printf("<view> - view messages\n");
					printf("<logout>\n");
					printf("\n");
				}
				else if(!strcmp(cmd, "logout"))
				{
					printf("Logged out\n\n");
					state = LOGGEDOUT;
				}
				else if(!strcmp(cmd, "users"))
				{
					char users[MAX_STR_SIZE];
					med_users(users);
					printf("%s\n", users);
				}
				else if(!strcmp(cmd, "remove"))
				{
					removeUser();
				}
				else if(!strcmp(cmd, "view"))
				{
					viewMessages();
				}
				else if(!strcmp(cmd, "send"))
				{
					sendMessage();
				}
				else
				{
					printf("Error: invalid command <%s> use <help> to view available commands\n", cmd);
				}	
			}
		}
	}
}

int main( int argc, char *argv[])
{

	int serverPort, clientPort;
	
	if( argc == 4)
	{
		char* serverHostName = argv[1];
		serverPort = atoi(argv[2]);
		clientPort = atoi(argv [3]);

		run(serverHostName, serverPort, clientPort);
	}
	else
	{
		printf("Usage: %s serverHostName serverPort clientPort\n", argv[0]);
		return 1;
	}
		
	return 0;
}