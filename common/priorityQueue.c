#include "common/priorityQueue.h"

S32 priorityQueueInit(PriorityQueue* pqueue, U8* data, const U8 length, const U8 count)
{
    if (data == 0)
    {
        return -1;
    }
    
    pqueue->count = count;

    // Set the memory for the queue structures
    pqueue->queues = (QueueU16*)(data);
    data += (sizeof(QueueU16) * count);

    // Initialize the queues
    U32 i;
    for(i = 0; i < count; i++)
    {
        queueU16Init(pqueue->queues + i, (U16*)data, length);
        data += sizeof(U16)*length;
    }

    return 0;
}

S32 priorityQueuePush(PriorityQueue* pqueue, U8 priority, U16 value)
{
    return queueU16Push(pqueue->queues + priority, value);
}

S32 priorityQueuePop(PriorityQueue* pqueue, U16* dest)
{
    U8 priority;
    QueueU16* queue = pqueue->queues;

    for (priority = 0; priority < pqueue->count; priority++)
    {
        if (queue->count)
        {
            return queueU16Pop(queue, dest);
        }
        
        queue++;
    }

    return -1;
}
