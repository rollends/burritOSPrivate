#include "common/priorityQueue.h"

S32 priorityQueueInit(PriorityQueue* queue)
{
    queue->head[0] = queue->head[1] = queue->head[2] = 0;
    queue->tail[0] = queue->tail[1] = queue->tail[2] = 0;

    return 0;
}

S32 priorityQueuePush(PriorityQueue* queue, const U8 priority, const U16 value)
{
    queue->data[queue->tail[priority] + priority * TASK_COUNT] = value;
    queue->tail[priority] = (queue->tail[priority] + 1) & (TASK_COUNT-1);

    return 0;
}

S32 priorityQueuePop(PriorityQueue* queue, U16* dest)
{
    if (queue->head[0] != queue->tail[0])
    {
        *dest = queue->data[queue->head[0]];
        queue->head[0] = (queue->head[0] + 1) & (TASK_COUNT-1);

        return 0;
    }

    if (queue->head[1] != queue->tail[1])
    {
        *dest = queue->data[queue->head[1] + TASK_COUNT];
        queue->head[1] = (queue->head[1] + 1) & (TASK_COUNT-1);

        return 0;
    }

    if (queue->head[2] != queue->tail[2])
    {
        *dest = queue->data[queue->head[2] + TASK_COUNT*2];
        queue->head[2] = (queue->head[2] + 1) & (TASK_COUNT-1);

        return 0;
    }

    return -1;
}

S32 priorityQueuePushPop(PriorityQueue* queue, const U8 priority, U16* value)
{
    S8 freePriority = 3;

    if (queue->head[0] != queue->tail[0])
    {
        freePriority = 0;
    }
    else if (queue->head[1] != queue->tail[1])
    {
        freePriority = 1;
    }
    else if (queue->head[2] != queue->tail[2])
    {
        freePriority = 2;
    }

    if (priority < freePriority)
    {
        return 1;
    }

    queue->data[queue->tail[priority] + priority * TASK_COUNT] = *value;
    queue->tail[priority] = (queue->tail[priority] + 1) & (TASK_COUNT-1);

    *value = queue->data[queue->head[freePriority] + freePriority * TASK_COUNT];
    queue->head[freePriority] = (queue->head[freePriority] + 1) & (TASK_COUNT-1);

    return 0;
}
