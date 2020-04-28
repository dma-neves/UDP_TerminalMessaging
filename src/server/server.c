#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "util/list.h"
#include "chat/chat.h"
#include "chat/user.h"
#include "communication/requests.h"
#include "communication/server_mediator.h"
#include "global.h"

#define MAX_USERS 20

char* itoa(char* str, int num) { sprintf(str, "%d", num); return str; }

char buf[MAX_STR_SIZE];
List users;
List chats;

void debug()
{
	printf("\n<DEBUG>\n");
	printf("users: \n");
	Node* node = users.head;
	while(node != NULL)
	{
		User* user = (User*)node->element;
		printf("%s\n", user->name);
		node = node->next;
	}
	printf("\n");

	printf("chats: \n");
	node = chats.head;
	while(node != NULL)
	{
		Chat* chat = (Chat*)node->element;
		printf("chat between %s and %s\n", chat->user[0]->name, chat->user[1]->name);
		node = node->next;
	}
	printf("\n");
}

Node* getUserNode(char* name)
{
	Node* node = users.head;
	while(node != NULL)
	{
		User* user = (User*)node->element;
		if(strcmp(user->name, name) == 0) return node;

		node = node->next;
	}

	return NULL;
}

Chat* getChat(char* name_0, char* name_1)
{
	if(strcmp(name_0, name_1) == 0) return NULL;

	Node* node = getUserNode(name_0);
	if(node == NULL) return NULL;
	User* user_0 = (User*) node->element;

	node = getUserNode(name_1);
	if(node == NULL) return NULL;
	User* user_1 = (User*) node->element;

	node = chats.head;
	while(node != NULL)
	{
		Chat* chat = (Chat*)node->element;

		if(isChat(chat, user_0, user_1)) return chat;

		node = node->next;
	}

	Chat* chat = malloc(sizeof(Chat));
	User* users[2] = {user_0, user_1};
	initChat(chat, users);
	add(&chats, chat);

	return chat;
}

int login(char* name, char* password)
{
	Node* node = getUserNode(name);
	if(node == NULL) return INVALID_SPECIFICATIONS;

	User* user = node->element;

	if(strcmp(user->pass, password) == 0) return VALID;

	return INVALID_SPECIFICATIONS;
}

void regist(char* name, char* password)
{
	User* user = malloc(sizeof(User));
	strcpy(user->name, name);
	strcpy(user->pass, password);
	add(&users, user);
}

void getUserNames(char* userNames)
{
	userNames[0] = '\0';
	Node* node = users.head;
	while(node != NULL)
	{
		User* user = (User*)node->element;
		sprintf(userNames + strlen(userNames), "%s", user->name);
		sprintf(userNames + strlen(userNames), "\n");
		node = node->next;
	}
}

int removeUser(char* name)
{
	Node* node = chats.head;
	while(node != NULL)
	{
		Chat* chat = (Chat*)node->element;
		Node* nextNode = node->next;

		if(strcmp(chat->user[0]->name, name) == 0 || strcmp(chat->user[1]->name, name) == 0)
		{
			free( rem(&chats, node) );
		}

		node = nextNode;
	}

	node = getUserNode(name);
	if(node == NULL) return INVALID_SPECIFICATIONS;
	free( rem(&users, node) );

	return VALID;
}

int getMessages(char* name_0, char* name_1, char* messages)
{
	Chat* chat = getChat(name_0, name_1);
	if(chat == NULL) return INVALID_SPECIFICATIONS;

	messages[0] = '\0';
	Node* node = chat->messages.head;
	int i = 0;
	while(node != NULL)
	{	
		Message* msg = (Message*)node->element;
		sprintf(messages + strlen(messages), "(%s) %s\n", msg->sender, msg->msg);
		node = node->next;
	}

	return VALID;
}

int sendMessage(char* sender, char* receiver, char* msg)
{
	Chat* chat = getChat(sender, receiver);
	if(chat == NULL) return INVALID_SPECIFICATIONS;

	printf("New message between %s & %s:\n(%s) %s\n\n", sender, receiver, sender, msg);
	addMessage(chat, sender, msg);

	return VALID;
}

void saveServerData()
{
	saveOnDisk("user_data", &users);
	saveOnDisk("chat_data", &chats);
}

void clearServerData()
{
	clearList(&users);

	Node* curNode = chats.head;
	while(curNode != NULL)
	{
		Chat* chat = (Chat*)curNode->element;
		clearChat(chat);
	}

	clearList(&chats);
}

void loadServerData()
{
	clearServerData();

	loadFromDisk("user_data", &users);
	loadFromDisk("chat_data", &chats);
}

void server(int port)
{
	initList(&users, sizeof(User));
	initList(&chats, sizeof(Chat));

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

			default:
				printf("Error: Invalid request\n");
				sendResponse(itoa(buf, INVALID_REQUEST));
				break;
		}

		debug();
	}

	clearList(&users);
	clearList(&chats);
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

	server(port);
	return 0;
}

