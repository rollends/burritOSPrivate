#ifndef LIST_H
#define LIST_H

#include "common/types.h"

typedef struct ListU32Node
{
    /// Data inside node
    U32                     data;

    /// Next node in list
    struct ListU32Node*     next;
} ListU32Node;

/**
 * Linked List Data Structure
 */
typedef struct ListU32
{
    /// Free Node List
    ListU32Node*            freeList;

    /// Head of List
    ListU32Node*            head;
} ListU32;

/**
 * Initializes a List
 *
 * @param   list    The list to initialize
 * @param   data    The backing data pointer of `length` nodes
 * @param   length  The length of buffer
 *
 * @return  0 on success, else an error code
 */
S32 listU32Init(ListU32* list, ListU32Node* data, const U32 length);

/**
 * Push-back a value in a list
 *
 * @param   list    The list
 * @param   value   The value to enqueue
 *
 * @return  0 on success, else an error code
 */
S32 listU32PushBack(ListU32* list, const U32 value);

/**
 * Push-front a value in a list
 *
 * @param   list    The list
 * @param   value   The value to enqueue
 *
 * @return  0 on success, else an error code
 */
S32 listU32PushFront(ListU32* list, const U32 value);

/**
 * Dequeues a value from the List
 *
 * @param   list    The list
 * @param   value   The destination to load the value
 *
 * @return  0 on success, else an error code
 */
S32 listU32PopFront(ListU32* list, U32* value);

#endif

