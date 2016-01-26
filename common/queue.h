#ifndef QUEUE_H
#define QUEUE_H

#include "common/types.h"

#define QUEUE_MEM_SIZE(length, type) sizeof(type)*length 

typedef struct
{
    U8*    	data;
    U8      head; 
    U8      tail;
    U8      count;
    U8      length;
} QueueU8;

typedef struct
{
    U16*    data;
    U8      head; 
    U8      tail;
    U8      count;
    U8      length;
} QueueU16;

S32 queueU8Init(QueueU8* queue, U8* data, const U8 length);
S32 queueU8Push(QueueU8* queue, U8 value);
S32 queueU8Pop(QueueU8* queue, U8* result);

S32 queueU16Init(QueueU16* queue, U16* data, const U8 length);
S32 queueU16Push(QueueU16* queue, U16 value);
S32 queueU16Pop(QueueU16* queue, U16* result);

#endif
