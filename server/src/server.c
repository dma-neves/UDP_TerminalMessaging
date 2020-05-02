#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "server.h"

#include "chat/user.h"
#include "communication/requests.h"
#include "global.h"

#define MAX_USERS 20
#define REMOVE_ALL_DATA_FILES "rm ../data/*.ldata"
#define USER_DATA_FILE "../data/user_data.ldata"
#define CHAT_USERS_DATA_FILE "../data/chat_users_data.ldata"
#define CHAT_MESSAGES_DATA_FILE "../data/chat_messages	_data_%d.ldata"

typedef struct _ChatUsers
{ 
	char name_a[MAX_SMALL_STR_SIZE];
	char name_b[MAX_SMALL_STR_SIZE];
} ChatUsers;

List users;
List chats;

void saveChats();
void loadChats();

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
	for(; node != NULL; node = node->next)
	{
		User* user = (User*)node->element;
		sprintf(debugInfo + strlen(debugInfo), "%s\n", user->name);
	}
	sprintf(debugInfo + strlen(debugInfo), "\n");

	sprintf(debugInfo + strlen(debugInfo), "chats: \n");
	node = chats.head;
	for(; node != NULL; node = node->next)
	{
		Chat* chat = (Chat*)node->element;
		sprintf(debugInfo + strlen(debugInfo), "chat between %s and %s\n", chat->user[0]->name, chat->user[1]->name);
		
	}
	sprintf(debugInfo + strlen(debugInfo), "\n");

	return debugInfo;
}

Node* getUserNode(char* name)
{
	Node* node = users.head;
	for(; node != NULL; node = node->next)
	{
		User* user = (User*)node->element;
		if(strcmp(user->name, name) == 0) return node;
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
	for(; node != NULL; node = node->next)
	{
		Chat* chat = (Chat*)node->element;

		if(isChat(chat, user_0, user_1)) return chat;
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
	for(; node != NULL; node = node->next)
	{
		User* user = (User*)node->element;
		sprintf(userNames + strlen(userNames), "%s", user->name);
		sprintf(userNames + strlen(userNames), "\n");
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
	for(; node != NULL; node = node->next)
	{	
		Message* msg = (Message*)node->element;
		sprintf(messages + strlen(messages), "(%s) %s\n", msg->sender, msg->msg);
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

void clearDiskData()
{
	system(REMOVE_ALL_DATA_FILES);
}

void saveServerData()
{
	clearDiskData();
	saveOnDisk(USER_DATA_FILE, &users);
	saveChats();

	printf("<DATA saved>\n\n");
}

void clearServerData()
{
	clearList(&users);

	Node* node = chats.head;
	for(; node != NULL; node = node->next)
	{
		Chat* chat = (Chat*)node->element;
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
	List chatUsers;
	initList(&chatUsers, sizeof(ChatUsers));


	int chat_num = 0;
	Node* node = chats.head;
	for(; node != NULL; node = node->next)
	{
		Chat* chat = (Chat*)node->element;

		ChatUsers* ncu = malloc(sizeof(ChatUsers));
		strcpy(ncu->name_a, chat->user[0]->name);
		strcpy(ncu->name_b, chat->user[1]->name);
		add(&chatUsers, ncu);

		char chatMessagesDataFile[MAX_SMALL_STR_SIZE];
		sprintf(chatMessagesDataFile, CHAT_MESSAGES_DATA_FILE, chat_num++);

		saveOnDisk(chatMessagesDataFile, &chat->messages);
	}

	saveOnDisk(CHAT_USERS_DATA_FILE, &chatUsers);
	clearList(&chatUsers);
}

void loadChats()
{
	List chatUsers;
	initList(&chatUsers, sizeof(ChatUsers));
	loadFromDisk(CHAT_USERS_DATA_FILE, &chatUsers);

	int chat_num = 0;
	Node* node = chatUsers.head;
	for(; node != NULL; node = node->next)
	{
		Chat* nchat = malloc(sizeof(Chat));

		ChatUsers* chatUsers = (ChatUsers*)node->element;
		nchat->user[0] = getUserNode(chatUsers->name_a)->element;
		nchat->user[1] = getUserNode(chatUsers->name_b)->element;

		char chatMessagesDataFile[MAX_SMALL_STR_SIZE];
		sprintf(chatMessagesDataFile, CHAT_MESSAGES_DATA_FILE, chat_num++);
		loadFromDisk(chatMessagesDataFile, &nchat->messages);

		add(&chats, nchat);
	}

	clearList(&chatUsers);
}