#include "user/messageTypes.h"
#include "user/servers/ClockNotifier.h"
#include "user/services/nameService.h"

typedef struct DelayedTask
{
    struct DelayedTask*       next;
    TaskID                  tid;
    S32                     delay;
} DelayedTask;

typedef struct
{
    DelayedTask     buffer[32];
    DelayedTask*    freeList;
    DelayedTask*    queue;
} DelayQueue;

void delayQueuePush( DelayQueue* dq, TaskID id, U32 delay );
void delayQueueInit( DelayQueue* dq );

void ClockServer()
{
    nsRegister( Clock );
    
    U32 absoluteTime = 0;

    DelayQueue          dqueue,
                        dqueueLoRes;
    MessageEnvelope     response, 
                        respondTime;
 
    delayQueueInit( &dqueue );
    delayQueueInit( &dqueueLoRes );
    
    response.type = MESSAGE_CLOCKSERVER_WAKE;
    respondTime.type = MESSAGE_CLOCKSERVER_WAKE;

    sysCreate(0, &ClockNotifier);
    sysCreate(1, &ClockLoResNotifier);

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
        
        case MESSAGE_CLOCKSERVER_LONG_DELAY_BY:
            delayQueuePush( &dqueueLoRes, id, envelope.message.MessageU32.body );
            break;

        case MESSAGE_CLOCKSERVER_GET_TIME:
        {
            respondTime.message.MessageU32.body = absoluteTime;
            sysReply( id.value, &respondTime );
            break;
        }

        case MESSAGE_CLOCKSERVER_NOTIFY_TICK:
        {
            ++absoluteTime;
            sysReply( id.value, &envelope );

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

        case MESSAGE_CLOCKSERVER_NOTIFY_LORES:
        {
            sysReply( id.value, &envelope );

            if( !dqueueLoRes.queue )
            {
                break;
            }

            dqueueLoRes.queue->delay--;
            while( dqueueLoRes.queue && dqueueLoRes.queue->delay <= 0 )
            {
                sysReply( dqueueLoRes.queue->tid.value, &response );
                DelayedTask* free = dqueueLoRes.queue;
                dqueueLoRes.queue = free->next;
                free->next = dqueueLoRes.freeList;
                dqueueLoRes.freeList = free;
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

    assert( node != 0 );

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
    for(; i < 31; i++)
    {
        dq->buffer[i].next = dq->buffer + i + 1;
    }
    dq->buffer[31].next = 0;
    dq->queue = 0;
    dq->freeList = dq->buffer;
}
