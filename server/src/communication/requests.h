#ifndef REQUESTS_H
#define REQUESTS_H

#define VALID 0
#define INVALID_REQUEST -1
#define INVALID_SPECIFICATIONS -2

typedef enum
{
    LOGIN = 0,
    REGISTER,
    USERS,
    REMOVE,
    VIEW,
    SEND,
    DEBUG,
    DATA_OP
} Request;

#endif
