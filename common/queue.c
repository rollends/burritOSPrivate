#include "common/error.h"
#include "common/queue.h"

S32 queueU8Init(QueueU8* queue, U8* data, const U32 length)
{
    IS_NOT_NULL(queue);
    IS_NOT_NULL(data);
    IS_POW_2(length);
    IS_NOT_ZERO(length);

    queue->head = queue->tail = queue->count = 0;
    queue->data = data;
    queue->length = length;

    return OK;
}

S32 queueU8Push(QueueU8* queue, const U8 value)
{
    if (queue->count == queue->length)
    {
        return -1;
    }
    
    queue->data[queue->tail] = value;
    queue->tail = (queue->tail + 1) & (queue->length-1);
    queue->count++;

    return OK;
}

S32 queueU8Pop(QueueU8* queue, U8* value)
{
    if (queue->count == 0)
    {
        return -1;
    }

    queue->count--;
    *value = queue->data[queue->head];
    queue->head = (queue->head + 1) & (queue->length-1);

    return OK;
}

S32 queueU16Init(QueueU16* queue, U16* data, const U32 length)
{
    IS_NOT_NULL(queue);
    IS_NOT_NULL(data);
    IS_POW_2(length);
    IS_NOT_ZERO(length);

    queue->head = queue->tail = queue->count = 0;
    queue->data = data;
    queue->length = length;

    return OK;
}

S32 queueU16Push(QueueU16* queue, const U16 value)
{
    if (queue->count == queue->length)
    {
        return -1;
    }
    
    queue->data[queue->tail] = value;
    queue->tail = (queue->tail + 1) & (queue->length-1);
    queue->count++;

    return OK;
}

S32 queueU16Pop(QueueU16* queue, U16* value)
{
    if (queue->count == 0)
    {
        return -1;
    }

    queue->count--;
    *value = queue->data[queue->head];
    queue->head = (queue->head + 1) & (queue->length-1);

    return OK;
}
