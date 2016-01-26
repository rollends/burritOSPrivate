#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "common/types.h"
#include "common/queue.h"

#define PQUEUE_MEM_SIZE(length, count) \
				((sizeof(QueueU16) + QUEUE_MEM_SIZE(length, U16))*count)

/**
 * Defines a priority queue structure, consisting of a collection of QueueU16 
 * instances and a priority count.
 */
typedef struct
{
	/// Array of `count` QueueU16 instances, one for each priority
	QueueU16* queues;

	/// The number of priorities in the oqueue
	U8 count;
} PriorityQueue;

/**
 * Initializes a priority queue
 *
 * @param 	pqueue 	The priority queue to initialize
 * @param 	data 	The memory to use for the queue structures and their data
 * @param 	length	The length of each priority queue
 * @param 	count 	The number of priorities
 *
 * @return 	0 on success, else an error code
 */
S32 priorityQueueInit(PriorityQueue* pqueue,
					  U8* data,
					  const U8 length,
					  const U8 count);

/**
 * Appends a value onto the queue at a given priority
 *
 * @param 	pqueue 		The priority queue to append to
 * @param 	priority 	The priority to add the value at
 * @param 	value 		The value to add
 *
 * @return 	0 on success, else an error code
 */
S32 priorityQueuePush(PriorityQueue* pqueue, U8 priority, const U16 value);

/**
 * Removes the first element in the queue
 *
 * @param 	pqueue 		The priority queue to remove from
 * @param 	value 		The location to write the removed value
 *
 * @return 	0 on success, else an error code
 */
S32 priorityQueuePop(PriorityQueue* pqueue, U16* dest);

#endif