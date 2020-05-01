#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"
#include "communication/requests.h"
#include "communication/server_mediator.h"
#include "global.h"

char buf[MAX_STR_SIZE];

char* itoa(char* str, int num) { sprintf(str, "%d", num); return str; }

void debug()
{
	char debugInfo[MAX_STR_SIZE];
	getDebugInfo(debugInfo);

	printf("%s", debugInfo);
}

void run(int port)
{
	initServer();
	initServerMediator(port);

	while(1)
	{
		int request = readRequest();

		switch(request)
		{
			case LOGIN:
			{
				printf("<LOGIN>\n");
				char name[MAX_STR_SIZE];
				char pass[MAX_STR_SIZE];
				strcpy(name, getNextSpecification());
				strcpy(pass, getNextSpecification());

				sendResponse(itoa(buf, login(name, pass) ));
				break;
			}

			case REGISTER:
			{
				printf("<REGISTER>\n");
				char name[MAX_STR_SIZE];
				char pass[MAX_STR_SIZE];
				strcpy(name, getNextSpecification());
				strcpy(pass, getNextSpecification());

				regist(name, pass);
				sendResponse(itoa(buf, VALID));
				break;
			}
			
			case USERS:
			{
				printf("<USERS>\n");
				char users[MAX_STR_SIZE];

				getUserNames(users);
				sendResponse(users);
				break;
			}

			case REMOVE:
			{
				printf("<REMOVE>\n");
				char name[MAX_STR_SIZE];
				strcpy(name, getNextSpecification());

				sendResponse(itoa( buf, removeUser(name) ));
				break;
			}

			case VIEW:
			{
				printf("<VIEW>\n");
				char name_0[MAX_STR_SIZE];
				char name_1[MAX_STR_SIZE];
				strcpy(name_0, getNextSpecification());
				strcpy(name_1, getNextSpecification());

				char messages[MAX_STR_SIZE];
				if(getMessages(name_0, name_1, messages) == INVALID_SPECIFICATIONS)
				{
					sendResponse(itoa(buf, INVALID_SPECIFICATIONS));
				}
				else
				{
					sendResponse(messages);
				}

				break;
			}

			case SEND:
			{
				printf("<SEND>\n");
				char sender[MAX_STR_SIZE];
				char receiver[MAX_STR_SIZE];
				char msg[MAX_STR_SIZE];
				strcpy(sender, getNextSpecification());
				strcpy(receiver, getNextSpecification());
				strcpy(msg, getNextSpecification());

				sendResponse(itoa( buf, sendMessage(sender, receiver, msg) ));
				break;
			}

			case DEBUG:
			{
				char adminPass[MAX_STR_SIZE];
				strcpy(adminPass, getNextSpecification());

				if(strcmp(adminPass, ADMIN_PASSWORD) != 0)
				{
					sendResponse(itoa(buf, INVALID_SPECIFICATIONS));
				}
				else
				{
					char debugInfo[MAX_STR_SIZE];
					sendResponse(getDebugInfo(debugInfo));
				}

				break;
			}

			case DATA_OP:
			{
				char adminPass[MAX_STR_SIZE];
				strcpy(adminPass, getNextSpecification());
				char op[MAX_STR_SIZE];
				strcpy(op, getNextSpecification());

				if(strcmp(adminPass, ADMIN_PASSWORD) != 0)
				{
					sendResponse(itoa(buf, INVALID_SPECIFICATIONS));
				}
				else
				{
					if(!strcmp(op, "cleardata")) clearServerData();
					else if(!strcmp(op, "loaddata")) loadServerData();
					else if(!strcmp(op, "savedata")) saveServerData();
					else sendResponse(itoa(buf, INVALID_SPECIFICATIONS));

					sendResponse(itoa(buf, VALID));
				}
				
				break;
			}

			default:
				printf("Error: Invalid request\n");
				sendResponse(itoa(buf, INVALID_REQUEST));
				break;
		}

		debug();
	}
}

int main( int argc, char *argv[])
{
	int port;
	
	if(argc == 2)
	{
		port = atoi(argv[1]);
	}
	else
	{
		printf("Usage: %s port\n", argv[0]);
		return 1;
	}

	run(port);
	return 0;
}