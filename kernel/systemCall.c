#include "common/common.h"
#include "hardware/hardware.h"

#include "kernel/event.h"
#include "kernel/kernelData.h"
#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/systemCall.h"

U32 systemCallHandler(U32 id, U32 arg0, U32 arg1, U32 arg2)
{
    TaskDescriptor* desc = kernel.activeTask;
    desc->performance[ePerfTask] += timerSample(TIMER_4, &kernel.perfState);

    switch (id)
    {
        case SYS_CALL_EXIT_ID:
        {
            desc->state = eZombie;
            break;
        }

        case SYS_CALL_PASS_ID:
        {
            break;
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

        case SYS_CALL_CREATE_ID:
        {
            U16 result = taskTableAlloc(&kernel.tasks,
                                        arg0,
                                        arg1,
                                        (U32)(&__taskExit),
                                        desc->tid);
            priorityQueuePush(&kernel.queue, arg0, result);
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

        case SYS_CALL_PERF_RESET_ID:
        {
            timerStart(TIMER_4, &kernel.perfState);
            taskTablePerfClear(&kernel.tasks);

            break;
        }

        case SYS_CALL_PERF_QUERYP_ID:
        {
            if (arg1 == ePerfTotal)
            {
                return 10000;
            }

            return taskTablePerfP(&kernel.tasks,
                                  VAL_TO_ID(arg0),
                                  arg1,
                                  kernel.perfState.total);
        }

        case SYS_CALL_PERF_QUERYT_ID:
        {
            if (arg1 == ePerfTotal)
            {
                U32 total = kernel.perfState.total;
                total *= 1000;
                total /= 983;

                return total;
            }

            return taskTablePerfT(&kernel.tasks,
                                  VAL_TO_ID(arg0),
                                  arg1,
                                  kernel.perfState.total);
        }


        case SYS_CALL_AWAIT_ID:
        {
            kernel.eventTable[arg0] = desc->tid;
            desc->state = eEventBlocked;
            break;
        }

        case SYS_CALL_READ_ID:
        {
            U8 byte;
            switch (arg0)
            {
                case PORT_TERMINAL:
                    if (queueU8Pop(&kernel.terminalInput, &byte) != 0)
                    {
                        kernel.eventTable[EVENT_TERMINAL_READ] = desc->tid;
                        desc->state = eEventBlocked;
                    }
                    break;

                case PORT_TRAIN:
                    kernel.eventTable[EVENT_TRAIN_READ] = desc->tid;
                    desc->state = eEventBlocked;
                    break;

                default:
                    return -1;
            }

            return byte;
        }

        case SYS_CALL_WRITE_ID:
        {
            switch (arg0)
            {
                case PORT_TERMINAL:
                    uartInterruptTX(UART_2, 1);
                    kernel.eventTable[EVENT_TERMINAL_WRITE] = desc->tid;
                    break;

                case PORT_TRAIN:
                    uartInterruptTX(UART_1, 1);
                    kernel.eventTable[EVENT_TRAIN_WRITE] = desc->tid;
                    break;

                default:
                    return -1;
            }

            desc->state = eEventBlocked;

            return 0;
        }
        
        case SYS_CALL_ALLOC_ID:
        {
            return (U32)(memoryAllocatorAlloc(&kernel.tasks.memoryAllocator));
        }

        case SYS_CALL_FREE_ID:
        {
            return memoryAllocatorFree(&kernel.tasks.memoryAllocator,
                                       (U32*)arg0);
        }

        default:
            break;
    }

    return 0;
}
