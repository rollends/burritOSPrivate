#include "common/queue.h"

S32 queueU8Init(QueueU8* queue, U8* data, const U8 length)
{
    if (data == 0)
    {
        return -1;
    }

    queue->head = queue->tail = queue->count = 0;
    queue->data = data;
    queue->length = length;

    return 0;
}

S32 queueU8Push(QueueU8* queue, U8 value)
{
    if (queue->count == queue->length)
        return -1;
    
    queue->data[queue->tail] = value;
    queue->tail = (queue->tail + 1) % queue->length;
    queue->count++;

    return 0;
}

S32 queueU8Pop(QueueU8* queue, U8* result)
{
    if (queue->count == 0)
        return -1;

    queue->count--;
    *result = queue->data[queue->head];
    queue->head = (queue->head + 1) % queue->length;

    return 0;
}

S32 queueU16Init(QueueU16* queue, U16* data, const U8 length)
{
    if (data == 0)
    {
        return -1;
    }

    queue->head = queue->tail = queue->count = 0;
    queue->data = data;
    queue->length = length;

    return 0;
}

S32 queueU16Push(QueueU16* queue, U16 value)
{
    if (queue->count == queue->length)
        return -1;
    
    queue->data[queue->tail] = value;
    queue->tail = (queue->tail + 1) % queue->length;
    queue->count++;

    return 0;
}

S32 queueU16Pop(QueueU16* queue, U16* result)
{
    if (queue->count == 0)
        return -1;

    queue->count--;
    *result = queue->data[queue->head];
    queue->head = (queue->head + 1) % queue->length;

    return 0;
}
