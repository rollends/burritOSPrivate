#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "common/types.h"
#include "common/queue.h"

#include "kernel/config.h"

/**
 * Defines a priority queue structure
 */
typedef struct
{
    /// The backing data for the priority queue
    U16 data[PRIORITY_LENGTH * PRIORITY_COUNT];

    /// The head indices for the queues
    U16 head[PRIORITY_COUNT];

    /// The tail indices for the queues
    U16 tail[PRIORITY_COUNT];

    /// The queue state bits
    U32 priorityBits;
} PriorityQueue;

/**
 * Initializes a priority queue
 *
 * @param   queue   The priority queue to initialize
 *
 * @return  0 on success, else an error code
 */
S32 priorityQueueInit(PriorityQueue* queue);

/**
 * Appends a value onto the queue at a given priority
 *
 * @param   queue       The priority queue to append to
 * @param   priority    The priority to add the value at
 * @param   value       The value to add
 *
 * @return  0 on success, else an error code
 */
S32 priorityQueuePush(PriorityQueue* queue, const U8 priority, const U16 value);

/**
 * Removes the first element in the queue
 *
 * @param   queue       The priority queue to remove from
 * @param   value       The location to write the removed value
 *
 * @return  0 on success, else an error code
 */
S32 priorityQueuePop(PriorityQueue* queue, U16* dest);

/**
 * Performs a combined push and pop operation that minimizes the amount of
 * queue access needed. If the pushed and popped value are the same, the
 * function returns out early.
 *
 * @param   queue       The priority queue to push and pop from
 * @param   priority    The priority level to push/pop to
 * @param   value       The location of the value to read and write
 *
 * @return  0 on success, else an error code
 */
S32 priorityQueuePushPop(PriorityQueue* queue, const U8 priority, U16* value);

#endif
