#include "common/types.h"
#include "hardware/hardware.h"
#include "kernel/kernelData.h"
#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/sysCall.h"

#include "user/IdleTask.h"
#include "user/InitialTask.h"

static KernelData kernel;

U32* kernelBoostrap(U32 pc)
{
    uartSpeed(UART_2, UART_SPEED_HI);
    uartConfig(UART_2, 0, 0, 0);
    timerInit(TIMER_3); 
    timerSetValue(TIMER_3, 508000);
    timerClear(TIMER_3);
    interruptEnable(INT_2, 0x80000);

    printString("%c[2J\r", 27);

    kernelDataInit(&kernel, pc);

    U16 taskID = taskTableAlloc(&kernel.tasks,
                                31,
                                (U32)(&IdleTask) + pc,
                                VAL_TO_ID(0));

    kernel.activeTask = taskGetDescriptor(&kernel.tasks, VAL_TO_ID(taskID));
    return kernel.activeTask->stack;
}

void kernelCleanup()
{
    interruptClear(INT_1, 0xFFFFFFFF);
    interruptClear(INT_2, 0xFFFFFFFF);
}

void kernelInterrupt()
{
    TaskID tid = kernel.eventTable[ EVENT_TIMER_TICK ];
    
    if (tid.value != 0)
    {
        TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);

        desc->state = eReady;
        priorityQueuePush(&kernel.queue,
                          desc->priority,
                          desc->tid.value);

        kernel.eventTable[1] = VAL_TO_ID(0);
    }

    timerClear(TIMER_3);
}

U32 kernelSystemCall(U32 id, U32 arg0, U32 arg1, U32 arg2)
{
    TaskDescriptor* desc = kernel.activeTask;

    switch (id)
    {
        case SYS_CALL_CREATE_ID:
        {
            U16 result = taskTableAlloc(&kernel.tasks, arg0, arg1 + kernel.baseAddress, desc->tid);
            priorityQueuePush(&kernel.queue, arg0, result);
            return result;
        }

        case SYS_CALL_SEND_ID:
        {
            TaskDescriptor* receiver =
                taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0));

            if (receiver->state == eSendBlocked)
            {
                S32 result = messageCopy((MessageEnvelope*)TASK_ARG_1(receiver),
                                         (MessageEnvelope const *)arg1);

                *((U16*)TASK_ARG_0(receiver)) = desc->tid.value;
                TASK_RETURN(receiver) = result;

                desc->state = eReplyBlocked;
                receiver->state = eReady;

                priorityQueuePush(&kernel.queue,
                                  receiver->priority,
                                  receiver->tid.value);
                return result;
            }
            else
            {
                queueU16Push(&receiver->sendQueue, desc->tid.value);
                desc->state = eReceiveBlocked;
            }

            return 0;
        }

        case SYS_CALL_RECEIVE_ID:
        {
            U16 senderId;

            if (queueU16Pop(&desc->sendQueue, &senderId) == 0)
            {
                TaskDescriptor* sender =
                    taskGetDescriptor(&kernel.tasks, VAL_TO_ID(senderId));
                
                *((U16*)arg0) = senderId;
                messageCopy((MessageEnvelope*)arg1,
                            (MessageEnvelope const *)TASK_ARG_1(sender));

                sender->state = eReplyBlocked;
                desc->state = eReady;
            }
            else
            {   
                desc->state = eSendBlocked;
            }
            
            return 0;
        }

        case SYS_CALL_REPLY_ID:
        {
            TaskDescriptor* replyTo =
                taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0));
            
            S32 result = messageCopy((MessageEnvelope*)TASK_ARG_2(replyTo),
                                     (MessageEnvelope const *)arg1);
            TASK_RETURN(replyTo) = result;

            replyTo->state = eReady;
            priorityQueuePush(&kernel.queue,
                              replyTo->priority,
                              replyTo->tid.value);
            return result;
        }

        case SYS_CALL_AWAIT_ID:
        {
            kernel.eventTable[arg0] = desc->tid;
            desc->state = eEventBlocked;
            break;
        }

        case SYS_CALL_PID_ID:
        {
            return desc->pid.value;
        }

        case SYS_CALL_TID_ID:
        {
            return desc->tid.value;
        }

        case SYS_CALL_RUNNING_ID:
        {
            return kernel.systemRunning;
        }

        case SYS_CALL_SHUTDOWN_ID:
        {
            kernel.systemRunning = 0;
            break;
        }

        case SYS_CALL_EXIT_ID:
        {
            desc->state = eZombie;
            break;
        }

        default:
            break;
    }

    return 0;
}

U32* kernelSchedule(U32* sp)
{
    TaskDescriptor* desc = kernel.activeTask;
    TaskID tid = desc->tid;

    if (desc->state == eReady)
    {
        if (priorityQueuePushPop(&kernel.queue,
                                 desc->priority,
                                 &(tid.value)) == 1)
        {
            return sp;
        }
    }
    else
    {
        if (desc->state == eZombie)
        {
            taskTableFree(&kernel.tasks, tid);
        }

        if (priorityQueuePop(&kernel.queue, &(tid.value)) != 0)
        {
            return 0;
        }
    }

    desc->stack = sp;
    kernel.activeTask = taskGetDescriptor(&kernel.tasks, tid);
    return kernel.activeTask->stack;
}
