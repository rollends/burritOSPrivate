#include "common/priorityQueue.h"
#include "common/utils.h"

S32 priorityQueueInit(PriorityQueue* queue)
{
	U8 i = 0;
	for(i = 0; i < 32; ++i)
	{
    	queue->head[i] = queue->tail[i] = 0;
	}
	queue->isNonEmpty = 0;
    return 0;
}

S32 priorityQueuePush(PriorityQueue* queue, const U8 priority, const U16 value)
{
    queue->data[queue->tail[priority] + priority * TASK_COUNT] = value;
    queue->tail[priority] = (queue->tail[priority] + 1) & (TASK_COUNT-1);
	queue->isNonEmpty |= 1 << priority;
    return 0;
}

S32 priorityQueuePop(PriorityQueue* queue, U16* dest)
{
	if( !queue->isNonEmpty )
		return -1;

	U8 priority = fastLog2( queue->isNonEmpty );
	*dest = queue->data[queue->head[priority]];
	queue->head[priority] = (queue->head[priority] + 1) & (TASK_COUNT - 1);
	queue->isNonEmpty ^= (queue->head[priority] == queue->tail[priority] ? 1 : 0) << priority;
    
	return 0;
}

S32 priorityQueuePushPop(PriorityQueue* queue, const U8 priority, U16* value)
{
    S8 freePriority = fastLog2( queue->isNonEmpty );

	if (priority < freePriority || !queue->isNonEmpty)
        return 1;

    queue->data[queue->tail[priority] + priority * TASK_COUNT] = *value;
    queue->tail[priority] = (queue->tail[priority] + 1) & (TASK_COUNT-1);
	queue->isNonEmpty |= 1 << priority;

    *value = queue->data[queue->head[freePriority] + freePriority * TASK_COUNT];
    queue->head[freePriority] = (queue->head[freePriority] + 1) & (TASK_COUNT-1);
	queue->isNonEmpty ^= 
		(queue->head[freePriority] == queue->tail[freePriority] ? 1 : 0) << freePriority;

    return 0;
}
