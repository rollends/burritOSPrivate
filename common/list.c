#include "common/error.h"
#include "common/list.h"

S32 listU32Init(ListU32* list, ListU32Node* data, const U32 length)
{
    U32 i = 0;
    for(i = 0; i < (length - 1); i++)
    {
        data[i].next = data + i + 1;
    }
    data[i].next = 0;
    list->freeList = data;
    list->head = 0;
    return 0;
}

S32 listU32PushBack(ListU32* list, const U32 value)
{
    if(!list->freeList) 
        return -1;
    
    ListU32Node* node = list->freeList;
    list->freeList = node->next;

    node->data = value;
    node->next = 0;

    if(list->head == 0)
    {
        list->head = node;
        return 0;
    }
    
    ListU32Node* tail = list->head;
    while(tail->next != 0)
    {
        tail = tail->next;
    }
    tail->next = node;
    return 0;
}

S32 listU32PushFront(ListU32* list, const U32 value)
{
    if(!list->freeList)
        return -1;

    ListU32Node* node = list->freeList;
    list->freeList = node->next;

    node->data = value;
    node->next = list->head;
    
    list->head = node;
    return 0;
}

S32 listU32PopFront(ListU32* list, U32* value)
{
    if(!list->head)
        return -1;

    ListU32Node* node = list->head;
    list->head = node->next;

    *value = node->data;

    node->next = 0;
    if(list->freeList)
        list->freeList->next = node;
    else
        list->freeList = node;
    return 0;
}

