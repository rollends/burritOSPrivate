#include "common/error.h"
#include "common/priorityQueue.h"
#include "common/utils.h"

RETURN priorityQueueInit(PriorityQueue* queue)
{
    IS_NOT_NULL(queue);
    IS_IN_RANGE(queue);
    
    U8 i;
    for (i = 0; i < PRIORITY_COUNT; i++)
    {
        queue->head[i] = queue->tail[i] = 0;
    }

    queue->priorityBits = 0;

    IS_OK();
}

RETURN priorityQueuePush(PriorityQueue* queue,
                         const U8 priority,
                         const U16 value)
{
    IS_NOT_NULL(queue);
    IS_IN_RANGE(queue);

#ifdef BOUNDS_CHECK
    if (priority >= PRIORITY_COUNT ||
            ((queue->priorityBits & (1 << priority)) &&
            queue->tail[priority] == queue->head[priority]))
    {
        return ERROR_FULL;
    }
#endif

    queue->data[queue->tail[priority] + priority * PRIORITY_LENGTH] = value;
    queue->tail[priority] = (queue->tail[priority] + 1) & (PRIORITY_LENGTH - 1);
    queue->priorityBits |= (1 << priority);

    IS_OK();
}

RETURN priorityQueuePop(PriorityQueue* queue, U16* dest)
{
    IS_NOT_NULL(queue);
    IS_IN_RANGE(queue);
    IS_NOT_NULL(dest);
    IS_IN_RANGE(dest);

#ifdef BOUNDS_CHECK
    if (queue->priorityBits == 0)
    {
        return ERROR_EMPTY;
    }
#endif

    U32 priority = __ctz(queue->priorityBits);

    *dest = queue->data[queue->head[priority] + priority * PRIORITY_LENGTH];
    queue->head[priority] = (queue->head[priority] + 1) & (PRIORITY_LENGTH - 1);
    queue->priorityBits ^=
        (queue->head[priority] == queue->tail[priority] ? 1 : 0) << (priority);

    IS_OK();
}

S32 priorityQueuePushPop(PriorityQueue* queue, const U8 priority, U16* value)
{
    IS_NOT_NULL(queue);
    IS_IN_RANGE(queue);
    IS_NOT_NULL(value);
    IS_IN_RANGE(value);

    U32 free = __ctz(queue->priorityBits);

    if (priority < free)
    {
        return 1;
    }

#ifdef BOUNDS_CHECK
    if (priority >= PRIORITY_COUNT ||
            ((queue->priorityBits & (1 << priority)) &&
            queue->tail[priority] == queue->head[priority]))
    {
        return ERROR_FULL;
    }
#endif

    queue->data[queue->tail[priority] + priority * PRIORITY_LENGTH] = *value;
    queue->tail[priority] = (queue->tail[priority] + 1) & (PRIORITY_LENGTH - 1);
    queue->priorityBits |= (1 << (priority));

#ifdef BOUNDS_CHECK
    if (queue->priorityBits == 0)
    {
        return ERROR_EMPTY;
    }
#endif

    *value = queue->data[queue->head[free] + free * PRIORITY_LENGTH];
    queue->head[free] = (queue->head[free] + 1) & (PRIORITY_LENGTH - 1);
    queue->priorityBits ^=
        (queue->head[free] == queue->tail[free] ? 1 : 0) << (free);
        
    return OK;
}
