#ifndef CLIENT_MEDIATOR_H
#define CLIENT_MEDIATOR_H

void initClientMediator(char* serverHostName, int serverPort, int clientPort);
int med_login(char* name, char* pass);
int med_regist(char* name, char* pass);
void med_users(char* users);
int med_removeUser(char* name);
int med_viewMessages(char* name_0, char* name_1, char* messages);
int med_sendMessage(char* sender, char* receiver, char* msg);
int med_viewDebug(char* adminPass, char* debug_message);
int med_dataOp(char* adminPass, char* op);

#endif