#include "common/types.h"

#include "kernel/kernelData.h"
#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/sysCall.h"
#include "kernel/uart.h"

#include "user/InitialTask.h"

static KernelData kernel;

U32* kernelBoostrap(U32 pc)
{
    uartSpeed(UART_PORT_2, UART_SPEED_HI);
    uartConfig(UART_PORT_2, 0, 0, 0);
    printString("%c[2J\r", 27);

    kernelDataInit(&kernel, pc);
    U16 taskID = taskTableAlloc(&kernel.tasks, 1, (U32)(&InitialTask) + pc, VAL_TO_ID(0));
    
    kernel.activeTask = taskGetDescriptor(&kernel.tasks, VAL_TO_ID(taskID));
    return kernel.activeTask->stack;
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
       
        case SYS_CALL_PID_ID:
        {
            return desc->pid.value;
        }

        case SYS_CALL_TID_ID:
        {
            return desc->tid.value;
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
    desc->stack = sp;

    if (desc->state == eReady)
    {
        priorityQueuePush(&kernel.queue, kernel.activeTask->priority, tid.value);
    }
    else if (desc->state == eZombie)
    {
        taskTableFree(&kernel.tasks, tid);
    }

    if (priorityQueuePop(&kernel.queue, &(tid.value)) != 0)
    {
        return 0;
    }

    kernel.activeTask = taskGetDescriptor(&kernel.tasks, tid);
    return kernel.activeTask->stack;
}
