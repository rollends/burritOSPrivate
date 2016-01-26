#include "common/priorityQueue.h"
#include "common/types.h"
#include "kernel/print.h"
#include "kernel/sysCall.h"
#include "kernel/taskTable.h"
#include "kernel/uart.h"

#include "user/InitialTask.h"
#include "user/TestTask.h"

extern U32* enterTask(U32*);

int kernelMain(U32 pc)
{
    uartSpeed(UART_PORT_2, UART_SPEED_HI);
    uartConfig(UART_PORT_2, 0, 0, 0);                                                      

    printString("%c[2J\r", 27);

    PriorityQueue 	queue;
    TaskTable 	tasks;
	TaskID 	activeTaskID;
    U8 queueData[PQUEUE_MEM_SIZE(64, 3)];
	priorityQueueInit(&queue, queueData, 64, 3);

	taskTableInit(&tasks);
	activeTaskID.value = taskTableAlloc(&tasks, 1, (U32)(&InitialTask) + pc, VAL_TO_ID(0));
    
    do
    {
        TaskDescriptor* desc = taskGetDescriptor(&tasks, activeTaskID);
		
		// Context Switch into User Space Task
		desc->stack = enterTask(desc->stack);

		// User task made system call.
        U32 sysCall = desc->stack[2];
        U32 arg1 = desc->stack[3];
        U32 arg2 = desc->stack[4];
        U32* returnVal = &desc->stack[2];

        switch (sysCall)
        {
            case SYS_CALL_CREATE_ID:
                *returnVal = taskTableAlloc(&tasks, arg1, arg2 + pc, activeTaskID);
                priorityQueuePush(&queue, arg1, *returnVal);
                break;
           
            case SYS_CALL_PID_ID:
                *returnVal = desc->pid.value;
                break;

            case SYS_CALL_TID_ID:
                *returnVal = desc->tid.value;
                break;

            case SYS_CALL_PRINT_ID:
                printString("%x\r\n", arg1);
                break;

            default:
                break;
        }

        if (sysCall != SYS_CALL_EXIT_ID)
        {
			// User task didn't call exit, reEnqueue
            priorityQueuePush(&queue, desc->priority, activeTaskID.value);
        }
		else
		{
			// User task called sysExit. Free up its PCB.
			taskTableFree(&tasks, activeTaskID);
		}

    } while (priorityQueuePop( &queue, &activeTaskID.value ) >= 0);

    return 0;
}
