#ifndef SERVER_MEDIATOR_H
#define SERVER_MEDIATOR_H

void initServerMediator(int port);

void sendResponse(char* message);

int readRequest();

char* getNextSpecification();

#endif