#include <string.h>
#include <stdlib.h>

#include "chat.h"

void initChat(Chat* chat, User* user[2])
{
    chat->user[0] = user[0];
    chat->user[1] = user[1];
    initList( &(chat->messages) );
}

void addMessage(Chat* chat, char* sender, char msg[MAX_STR_SIZE])
{
    Message* newMessage = malloc(sizeof(Message));
    
    strcpy(newMessage->msg, msg);
    strcpy(newMessage->sender, sender);

    add(&chat->messages, (void*)newMessage);
}

int isChat(Chat* chat, User* user_0, User* user_1)
{
    return (chat->user[0] == user_0 && chat->user[1] == user_1) || (chat->user[0] == user_1 && chat->user[1] == user_0);
}