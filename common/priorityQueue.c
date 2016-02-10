#include "common/error.h"
#include "common/priorityQueue.h"
#include "common/utils.h"

S32 priorityQueueInit(PriorityQueue* queue)
{
    IS_NOT_NULL(queue);
    
    U8 i;
    for (i = 0; i < PRIORITY_COUNT; i++)
    {
        queue->head[i] = queue->tail[i] = 0;
    }

    queue->priorityBits = 0;

    return OK;
}

S32 priorityQueuePush(PriorityQueue* queue, const U8 priority, const U16 value)
{
    queue->data[queue->tail[priority] + priority * PRIORITY_LENGTH] = value;
    queue->tail[priority] = (queue->tail[priority] + 1) & (PRIORITY_LENGTH - 1);
    queue->priorityBits |= (1 << priority);

    return OK;
}

S32 priorityQueuePop(PriorityQueue* queue, U16* dest)
{
    if (queue->priorityBits == 0)
    {
        return -1;
    }

    U32 priority = __ctz(queue->priorityBits);

    *dest = queue->data[queue->head[priority] + priority * PRIORITY_LENGTH];
    queue->head[priority] = (queue->head[priority] + 1) & (PRIORITY_LENGTH - 1);
    queue->priorityBits ^=
        (queue->head[priority] == queue->tail[priority] ? 1 : 0) << (priority);

    return OK;
}

S32 priorityQueuePushPop(PriorityQueue* queue, const U8 priority, U16* value)
{
    U32 free = __ctz(queue->priorityBits);

    if (priority < free)
    {
        return 1;
    }

    queue->data[queue->tail[priority] + priority * PRIORITY_LENGTH] = *value;
    queue->tail[priority] = (queue->tail[priority] + 1) & (PRIORITY_LENGTH - 1);
    queue->priorityBits |= (1 << (priority));

    *value = queue->data[queue->head[free] + free * PRIORITY_LENGTH];
    queue->head[free] = (queue->head[free] + 1) & (PRIORITY_LENGTH - 1);
    queue->priorityBits ^=
        (queue->head[free] == queue->tail[free] ? 1 : 0) << (free);
        
    return OK;
}
