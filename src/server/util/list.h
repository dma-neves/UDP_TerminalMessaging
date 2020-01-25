#ifndef LIST_H
#define LIST_H

typedef struct _Node Node;

struct _Node
{
    void* element;
    Node* next;
    Node* prev;
};

typedef struct _List
{
    Node* head;
    Node* tail;
} List;

void initList(List* list);

void add(List* list, void* element);

Node* find(List* list, void* element);

void* rem(List* list, Node* node);

#endif