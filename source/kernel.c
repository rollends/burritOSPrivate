#include "print.h"
#include "queue.h"
#include "sysCall.h"
#include "task.h"
#include "types.h"
#include "uart.h"
#include "userTasks.h"

extern U32* enterTask(U32*);

void InitialTask()
{
    U32 priorities[4];
    priorities[0] = 2;
    priorities[1] = 2;
    priorities[2] = 0;
    priorities[3] = 0;
    U32 i;

    for (i = 0; i < 4; i++)
    {
        printString("Created: %b\r\n", sysCreate(priorities[i], &TestTask));
    }

    printString("FirstUserTask: exiting\r\n");
    sysExit();
}

int kernelMain(U32 pc)
{
    uartSpeed(UART_PORT_2, UART_SPEED_HI);
    uartConfig(UART_PORT_2, 0, 0, 0);                                                      

    printString("%c[2J\r", 27);

    Queue queue;
    queueInit(&queue);

    U16 currentId = 1;
    Tasks tasks;
    tasks.lastTid = 0;

    taskInit(&tasks, 1, (U32)(&InitialTask) + pc, 0);

    while(1)
    {
        TaskDescriptor* desc = tasks.descriptors + (currentId - 1);
        desc->stack = enterTask(desc->stack);

        U32 sysCall = desc->stack[2];
        U32 arg1 = desc->stack[3];
        U32 arg2 = desc->stack[4];
        U32* returnVal = &desc->stack[2];

        switch (sysCall)
        {
            case SYS_CALL_CREATE_ID:
                *returnVal = taskInit(&tasks, arg1, arg2 + pc, currentId);
                {
                    int i = queueEnqueue(&queue, arg1, *returnVal);
                    //printString("Enqueue: %x, %b, %x\r\n", i, *returnVal, arg1);
                }
                break;
           
            case SYS_CALL_PID_ID:
                *returnVal = desc->pid.fields.id;
                continue;

            case SYS_CALL_TID_ID:
                *returnVal = desc->tid.fields.id;
                continue;

            case SYS_CALL_PRINT_ID:
                printString("%x\r\n", arg1);
                continue;

            default:
                break;
        }

        if (sysCall != SYS_CALL_EXIT_ID)
        {
            int i = queueEnqueue(&queue, desc->priority, currentId);
            //printString("Enqueue: %x, %b, %x\r\n", i, currentId, desc->priority);
        }

        int result = queueDequeue(&queue, &currentId);
        //printString("Dequeue: %x, %b\r\n", result, currentId);
    }

    return 0;
}
