#include "common/types.h"

#include "kernel/kernelData.h"
#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/sysCall.h"
#include "kernel/timer.h"
#include "kernel/uart.h"

#include "user/InitialTask.h"

static KernelData kernel;

U32* kernelBoostrap(U32 pc)
{
    uartSpeed(UART_PORT_2, UART_SPEED_HI);
    uartConfig(UART_PORT_2, 0, 0, 0);
    printString("%c[2J\r", 27);
    timerInit(); 

    kernelDataInit(&kernel, pc);
    U16 taskID = taskTableAlloc(&kernel.tasks, 1, (U32)(&InitialTask) + pc, VAL_TO_ID(0));
    
    kernel.activeTask = taskGetDescriptor(&kernel.tasks, VAL_TO_ID(taskID));
    printString("starting task %x\r\n", kernel.activeTask->stack);
    return kernel.activeTask->stack;
}

void kernelInterrupt()
{
    printString("kernelInterrupt occured\r\n");
    timerClear();
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
    printString("Scheduling for task %x\r\n", sp);
    TaskDescriptor* desc = kernel.activeTask;
    TaskID tid = desc->tid;

    int i;
    for ( i = 0; i < 16; i++)
    {
//        printString("\t%x\r\n", sp[i]);
    }

    if (desc->state == eReady)
    {
        if (priorityQueuePushPop(&kernel.queue,
                                 desc->priority,
                                 &(tid.value)) == 1)
        {
            printString("fast path sp = %x\r\n", sp);
            return sp;
        }
    }
    else
    {
        if (desc->state == eZombie)
        {
            printString("exiting task\r\n");
            taskTableFree(&kernel.tasks, tid);
        }

        if (priorityQueuePop(&kernel.queue, &(tid.value)) != 0)
        {
            printString("Exiting kernel\r\n");
            return 0;
        }
    }

    desc->stack = sp;
    kernel.activeTask = taskGetDescriptor(&kernel.tasks, tid);
    printString("slow path sp = %x\r\n");
    return kernel.activeTask->stack;
}
