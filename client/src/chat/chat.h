#ifndef CHAT_H
#define CHAT_H

#include "user.h"
#include "../util/list.h"
#include "../global.h"

typedef struct _Message
{
    char sender[MAX_SMALL_STR_SIZE];
    char msg[MAX_STR_SIZE];
} Message;

typedef struct _Chat
{
    User* user[2];
    List messages;
} Chat;

void initChat(Chat* chat, User* user[2]);
void clearChat(Chat* chat);
void addMessage(Chat* chat, char* sender, char msg[MAX_STR_SIZE]);
int isChat(Chat* chat, User* user_0, User* user_1);

#endif