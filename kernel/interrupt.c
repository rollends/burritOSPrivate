#include "common/common.h"
#include "hardware/hardware.h"

#include "kernel/event.h"
#include "kernel/kernelData.h"
#include "kernel/print.h"

void interruptHandler()
{
    TaskDescriptor* desc = kernel.activeTask;
    desc->performance[ePerfTask] += timerSample(TIMER_4, &kernel.perfState);
    
    U32 status1 = interruptStatus(INT_1);

    if (status1 & 0x10)
    {
        TaskID tid = kernel.eventTable[EVENT_10MS_TICK];

        if (tid.value != 0)
        {
            TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);

            desc->state = eReady;
            priorityQueuePush(&kernel.queue,
                              desc->priority,
                              desc->tid.value);

            kernel.eventTable[EVENT_10MS_TICK] = VAL_TO_ID(0);
        }

        timerClear(TIMER_1);
    }

    if (status1 & 0x20)
    {
        TaskID tid = kernel.eventTable[EVENT_100MS_TICK];

        if (tid.value != 0)
        {
            TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);

            desc->state = eReady;
            priorityQueuePush(&kernel.queue,
                              desc->priority,
                              desc->tid.value);

            kernel.eventTable[EVENT_100MS_TICK] = VAL_TO_ID(0);
        }

        timerClear(TIMER_2);
    }

    if (status1 & 0x02000000)
    {
        TaskID tid = kernel.eventTable[EVENT_TERMINAL_READ];
        U8 byte;
        uartReadByte(UART_2, &byte);
        queueU8Push(&kernel.terminalInput, byte);

        if (tid.value != 0)
        {
            TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);

            queueU8Pop(&kernel.terminalInput, &byte);
            TASK_RETURN(desc) = byte;

            desc->state = eReady;
            priorityQueuePush(&kernel.queue,
                              desc->priority,
                              desc->tid.value);

            kernel.eventTable[EVENT_TERMINAL_READ] = VAL_TO_ID(0);
        }
    }

    if (status1 & 0x04000000)
    {
        TaskID tid = kernel.eventTable[EVENT_TERMINAL_WRITE];

        if (tid.value != 0)
        {
            TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);
            desc->state = eReady;

            U8 byte = (U8)(TASK_ARG_0(desc));
            uartWriteByte(UART_2, byte);
            
            kernel.eventTable[EVENT_TERMINAL_WRITE] = VAL_TO_ID(0);
        }

        interruptClear(INT_1, 0x04000000);
    }
}
