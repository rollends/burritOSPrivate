#include "queue.h"
#include "print.h"

void _queueInit( U16Queue* q )
{
	q->iHead = q->iTail = q->length = 0;
        U32 i = 0;
        for(; i < PriorityQueueMaximumLength; ++i)
            q->queue[i] = PriorityQueueMaximumLength - i;
}

int _queueEnqueue( U16Queue* q, U16 value )
{
        //printString("\r\nQueue Enqueue: %x\r\n", value);
	if( q->length == PriorityQueueMaximumLength )
		return -1;
	
	q->queue[q->iTail] = value;
	q->iTail = (q->iTail + 1);// % PriorityQueueMaximumLength;
	q->length++;
	return q->iTail;
}

int _queueDequeue( U16Queue* q, U16* result )
{
	if( q->length == 0 )
		return -1;

	q->length--;
	*result = q->queue[q->iHead];
	q->iHead = (q->iHead + 1);// % PriorityQueueMaximumLength;
        //printString("\r\nQueue Dequeue: %x\r\n", *result);
	return q->iHead;
}

void queueInit( Queue* pq )
{
	U32 i = 0;
	for(; i < PriorityCount; ++i)
		_queueInit( pq->queues + i );
}


int queueEnqueue( Queue* pq, U8 priority, U16 value )
{
	return _queueEnqueue( pq->queues + priority, value );
}

int queueDequeue( Queue* pq, U16* value )
{
	U8 priority = 0;
	U16Queue *queue = pq->queues;
	for(; priority < PriorityCount; priority++ )
	{
		if( queue->length )
			return _queueDequeue( queue, value );
		
		++queue;
	}
	return -1;
}
