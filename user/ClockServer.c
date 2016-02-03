
#include "common/types.h"

#include "kernel/message.h"
#include "kernel/sysCall.h"
#include "kernel/taskDescriptor.h"

#include "user/messageTypes.h"
#include "user/Nameserver.h"

typedef struct DelayedTask
{
    struct DelayedTask*   	next;
    TaskID          		tid;
    S32             		delay;
} DelayedTask;

typedef struct
{
	DelayedTask     buffer[16];
    DelayedTask*    freeList;
    DelayedTask*    queue;
} DelayQueue;

void delayQueuePush( DelayQueue* dq, TaskID id, U32 delay );
void delayQueueInit( DelayQueue* dq );

void ClockServer()
{
    DelayQueue dqueue;
    delayQueueInit( &dqueue );

	nsRegister( Clock );

    MessageEnvelope response;
    response.type = MESSAGE_CLOCKSERVER_WAKE;

    for(;;)
    {
        MessageEnvelope envelope;
        TaskID id;

        sysReceive( &id.value, &envelope );

        switch( envelope.type )
        {
        case MESSAGE_CLOCKSERVER_DELAY_BY:
            delayQueuePush( &dqueue, id, envelope.message.MessageU32.body );
            break;

        case MESSAGE_CLOCKSERVER_NOTIFY_TICK:
        {
            if( !dqueue.queue )
            {
                break;
            }

            dqueue.queue->delay--;
            while( dqueue.queue && dqueue.queue->delay <= 0 )
            {
                sysReply( dqueue.queue->tid.value, &response );
                DelayedTask* free = dqueue.queue;
                dqueue.queue = free->next;
                free->next = dqueue.freeList;
                dqueue.freeList = free;
            }
            break;
        }

        default:
            break;
        }
    }
}

void delayQueuePush( DelayQueue* dq, TaskID id, U32 delay )
{
    DelayedTask* node = dq->freeList;
    dq->freeList = dq->freeList->next;

    node->tid = id;
    node->next = 0;

    if( !dq->queue )
    {
        dq->queue = node;
        node->delay = delay;
        return;
    }

    DelayedTask* previous = 0;
    DelayedTask* current = dq->queue;
    while( current && delay > current->delay )
    {
        delay -= current->delay;
        previous = current;
        current = current->next;
    }

    node->delay = delay;

    if( current )
    {
        if( previous )
            previous->next = node;
        else
            dq->queue = node;

        node->next = current;
        current->delay -= delay;
    }
    else
    {
        previous->next = node;
    }
}

void delayQueueInit( DelayQueue* dq )
{
    U8 i = 0;
    for(; i < 15; i++)
    {
        dq->buffer[i].next = dq->buffer + i + 1;
    }
    dq->buffer[15].next = 0;
    dq->queue = 0;
    dq->freeList = dq->buffer;
}
