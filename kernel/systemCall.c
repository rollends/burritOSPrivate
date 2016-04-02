#include "common/common.h"
#include "hardware/hardware.h"

#include "kernel/assert.h"
#include "kernel/event.h"
#include "kernel/kernelData.h"
#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/systemCall.h"

U32 systemCallHandler(U32 id, U32 arg0, U32 arg1, U32 arg2)
{
    TaskDescriptor* desc = kernel.activeTask;

    #ifdef KERNEL_PERF
        assertOk(timerSample(TIMER_4, &kernel.perfState));
        desc->performance[ePerfTask] += kernel.perfState.delta;
    #endif
        
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

        case SYS_CALL_KILL_ID:
        {
            kernel.systemRunning = 2;
            break;
        }

        case SYS_CALL_CREATE_ID:
        {
            U16 result = taskTableAlloc(&kernel.tasks,
                                        arg0,
                                        arg1,
                                        (U32)(&__taskExit),
                                        desc->tid,
                                        (ConstString)(arg2));
            assertOk(priorityQueuePush(&kernel.queue, arg0, result));
            return result;
        }

        case SYS_CALL_NAME_ID:
        {
            return (U32)(taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0))->name);
        }

        case SYS_CALL_PID_ID:
        {
            return desc->pid.value;
        }

        case SYS_CALL_TID_ID:
        {
            return desc->tid.value;
        }

        case SYS_CALL_PRIORITY_ID:
        {
            TaskDescriptor* task =
                taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0));
            return task->priority;
        }

        case SYS_CALL_STATE_ID:
        {
            TaskDescriptor* task =
                taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0));
            return task->state;
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

                assertOk(priorityQueuePush(&kernel.queue,
                                           receiver->priority,
                                           receiver->tid.value));
                return result;
            }
            else
            {
                assertOk(queueU16Push(&receiver->sendQueue, desc->tid.value));
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

                assert(sender->state == eReceiveBlocked);
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

            assert(replyTo->state == eReplyBlocked);
            replyTo->state = eReady;
            assertOk(priorityQueuePush(&kernel.queue,
                                       replyTo->priority,
                                       replyTo->tid.value));
            return result;
        }

#ifdef KERNEL_PERF
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

        case SYS_CALL_PERF_QUERYPC_ID:
        {
            TaskDescriptor* task =
                taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0));

            if (task->state == eZombie)
                return 0;

            return *(task->stack + 15);
        }

        case SYS_CALL_PERF_QUERYLR_ID:
        {
            TaskDescriptor* task =
                taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0));

            if (task->state == eZombie)
                return 0;

            return *(task->stack + 1);
        }

        case SYS_CALL_PERF_QUERYSP_ID:
        {
            TaskDescriptor* task =
                taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0));

            if (task->state == eZombie)
                return 0;

            return (U32)(task->stack);
        }

        case SYS_CALL_PERF_QUERYSB_ID:
        {
            TaskDescriptor* task =
                taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0));

            if (task->state == eZombie)
                return 0;

            return (U32)(task->stackBase);
        }

        case SYS_CALL_PERF_QUERYSI_ID:
        {
            TaskDescriptor* task =
                taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0));

            if (task->state == eZombie)
                return -1;

            return task->stackBlock;
        }

        case SYS_CALL_PERF_COUNT_ID:
        {
            return taskTableCount(&kernel.tasks);
        }

        case SYS_CALL_PERF_SEND_ID:
        {
            TaskDescriptor* task =
                taskGetDescriptor(&kernel.tasks, VAL_TO_ID(arg0));
            
            if (task->state == eZombie)
                return 0;

            return task->sendQueue.count;
        }
#endif
        
        case SYS_CALL_WRITE_ID:
        {
            switch (arg0)
            {
                case EVENT_TERMINAL_WRITE:
                    assertOk(uartInterruptTX(UART_2, 1));
                    break;

                case EVENT_TRAIN_WRITE:
                    assertOk(uartInterruptTX(UART_1, 1));
                    break;

                default:
                    return -1;
            }
        }
        case SYS_CALL_AWAIT_ID:
        case SYS_CALL_READ_ID:
        {
            kernel.eventTable[arg0] = desc->tid;
            desc->state = eEventBlocked;
            break;
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
