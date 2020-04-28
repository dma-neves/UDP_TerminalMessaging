#ifndef USER_H
#define USER_H

#include "../global.h"

typedef struct _User
{
    char name[MAX_STR_SIZE];
    char pass[MAX_STR_SIZE];
} User;

#endif