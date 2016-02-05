#include "common/common.h"
#include "hardware/hardware.h"

#include "kernel/event.h"
#include "kernel/kernelData.h"
#include "kernel/print.h"

void interruptHandler()
{
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

    if (status1 & 0x06000000)
    {
        if (status1 & 0x02000000)
        {
            U8 byte;
            uartReadByte(UART_2, &byte);
            printString("%c\r\n", byte);
        }
        else
        {
            uartWriteByte(UART_2, 'a');
        }
    }
}
