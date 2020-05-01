#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "server.h"

#include "chat/user.h"
#include "communication/requests.h"
#include "global.h"

#define MAX_USERS 20
#define USER_DATA_FILE "../data/user_data"
#define CHAT_DATA_FILE "../data/chat_data"

List users;
List chats;

void initServer()
{
	initList(&users, sizeof(User));
	initList(&chats, sizeof(Chat));
}

char* getDebugInfo(char* debugInfo)
{
	sprintf(debugInfo, "\n<DEBUG>\n");
	sprintf(debugInfo + strlen(debugInfo), "users: \n");
	Node* node = users.head;
	while(node != NULL)
	{
		User* user = (User*)node->element;
		sprintf(debugInfo + strlen(debugInfo), "%s\n", user->name);
		node = node->next;
	}
	sprintf(debugInfo + strlen(debugInfo), "\n");

	sprintf(debugInfo + strlen(debugInfo), "chats: \n");
	node = chats.head;
	while(node != NULL)
	{
		Chat* chat = (Chat*)node->element;
		sprintf(debugInfo + strlen(debugInfo), "chat between %s and %s\n", chat->user[0]->name, chat->user[1]->name);
		node = node->next;
	}
	sprintf(debugInfo + strlen(debugInfo), "\n");

	return debugInfo;
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
	if(!strcmp(name, "admin") && !strcmp(password, ADMIN_PASSWORD)) return VALID;

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

	addMessage(chat, sender, msg);
	return VALID;
}

void saveServerData()
{
	saveOnDisk(USER_DATA_FILE, &users);
	saveChats();

	printf("<DATA saved>\n\n");
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

	printf("<DATA cleared>\n\n");
}

void loadServerData()
{
	clearServerData();

	loadFromDisk(USER_DATA_FILE, &users);
	loadChats();

	printf("<DATA loaded>\n\n");
}

void saveChats()
{

}

void loadChats()
{

}