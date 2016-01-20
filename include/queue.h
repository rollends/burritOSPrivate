#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

#define PriorityCount 3
#define PriorityQueueMaximumLength 64

typedef struct U16Queue
{
	U8  iHead; 
        U8  iTail;
        U8  length;
	U16 queue[PriorityQueueMaximumLength];
} U16Queue;

typedef struct Queue
{
	U16Queue queues[PriorityCount];
} Queue;

void queueInit(Queue* pq);
S32 queueEnqueue(Queue* pq, U8 priority, U16 value);
S32 queueDequeue(Queue* pq, U16* dest);

#endif
