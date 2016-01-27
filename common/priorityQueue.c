#include "common/priorityQueue.h"

S32 priorityQueueInit(PriorityQueue* queue,
                      U8* data,
                      const U8 length,
                      const U8 count)
{
    if (data == 0)
    {
        return -1;
    }
    
    queue->count = count;

    // Set the memory for the queue structures
    queue->queues = (QueueU16*)(data);
    data += (sizeof(QueueU16) * count);

    // Initialize the queues
    U32 i;
    for(i = 0; i < count; i++)
    {
        queueU16Init(queue->queues + i, (U16*)data, length);
        data += sizeof(U16)*length;
    }

    return 0;
}

S32 priorityQueuePush(PriorityQueue* queue, const U8 priority, const U16 value)
{
    return queueU16Push(queue->queues + priority, value);
}

S32 priorityQueuePop(PriorityQueue* queue, U16* dest)
{
    U8 priority;
    QueueU16* pqueue = queue->queues;

    // Find the first empty queue
    for (priority = 0; priority < queue->count; priority++)
    {
        if (pqueue->count)
        {
            return queueU16Pop(pqueue, dest);
        }
        
        pqueue++;
    }

    return -1;
}
