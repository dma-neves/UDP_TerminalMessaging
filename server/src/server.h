#ifndef SERVER
#define SERVER

#include "util/list.h"
#include "chat/chat.h"

#define ADMIN_PASSWORD "admin"

void  initServer();
char* getDebugInfo(char* debugInfo);
Node* getUserNode(char* name);
Chat* getChat(char* name_0, char* name_1);
int   login(char* name, char* password);
void  regist(char* name, char* password);
void  getUserNames(char* userNames);
int   removeUser(char* name);
int   getMessages(char* name_0, char* name_1, char* messages);
int   sendMessage(char* sender, char* receiver, char* msg);
void  saveServerData();
void  clearServerData();
void  loadServerData();


void clearDiskData();

#endif