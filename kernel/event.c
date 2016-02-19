#include "common/common.h"
#include "hardware/hardware.h"

#include "kernel/assert.h"
#include "kernel/event.h"
#include "kernel/kernelData.h"

void eventHandler()
{
    #ifdef KERNEL_PERF
        TaskDescriptor* desc = kernel.activeTask;
        assertOk(timerSample(TIMER_4, &kernel.perfState));
        desc->performance[ePerfTask] += kernel.perfState.delta;
    #endif
        
    U32 status1, status2;
    assertOk(interruptStatus(INT_1, &status1));
    assertOk(interruptStatus(INT_2, &status2));
    
    if (status2 & 0x00100000)
    {
        U32 status;
        assertOk(uartInterruptStatus(UART_1, &status));
        if (status & 0x1)
        {
            assertOk(uartCTS(UART_1, &kernel.cts));
            if (kernel.cts == 0)
            {
                assertOk(uartInterruptTX(UART_1, 0));
            }
            else
            {
                assertOk(uartInterruptTX(UART_1, 1));
            }
        }

        if (status & 0x4)
        {
            TaskID tid = kernel.eventTable[EVENT_TRAIN_WRITE];

            if (tid.value != 0 && kernel.cts != 0)
            {
                TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);
                desc->state = eReady;
                assertOk(priorityQueuePush(&kernel.queue,
                                           desc->priority,
                                           desc->tid.value));

                U8 byte = (U8)(TASK_ARG_1(desc));
                assertOk(uartWriteByte(UART_1, byte));
                kernel.eventTable[EVENT_TRAIN_WRITE] = VAL_TO_ID(0);
                kernel.cts = 0;
            }

            assertOk(uartInterruptTX(UART_1, 0));
        }

        if (status & 0x2)
        {
            TaskID tid = kernel.eventTable[EVENT_TRAIN_READ];

            U8 byte;
            assertOk(uartReadByte(UART_1, &byte));

            if (tid.value != 0)
            {
                TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);

                TASK_RETURN(desc) = byte;
                desc->state = eReady;
                assertOk(priorityQueuePush(&kernel.queue,
                                           desc->priority,
                                           desc->tid.value));

                kernel.eventTable[EVENT_TRAIN_READ] = VAL_TO_ID(0);
            }
        }
    }

    if (status1 & 0x10)
    {
        TaskID tid = kernel.eventTable[EVENT_10MS_TICK];

        if (tid.value != 0)
        {
            TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);

            desc->state = eReady;
            assertOk(priorityQueuePush(&kernel.queue,
                                       desc->priority,
                                       desc->tid.value));

            kernel.eventTable[EVENT_10MS_TICK] = VAL_TO_ID(0);
        }

        timerClear(TIMER_1);
    }

    if (status1 & 0x20)
    {
        TaskID tid = kernel.eventTable[EVENT_150MS_TICK];

        if (tid.value != 0)
        {
            TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);

            desc->state = eReady;
            assertOk(priorityQueuePush(&kernel.queue,
                                       desc->priority,
                                       desc->tid.value));

            kernel.eventTable[EVENT_150MS_TICK] = VAL_TO_ID(0);
        }

        timerClear(TIMER_2);
    }

    if (status1 & 0x02000000)
    {
        TaskID tid = kernel.eventTable[EVENT_TERMINAL_READ];
        U8 byte;
        assertOk(uartReadByte(UART_2, &byte));

        if (tid.value != 0)
        {
            TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);
            desc->state = eReady;
            assertOk(priorityQueuePush(&kernel.queue,
                                       desc->priority,
                                       desc->tid.value));
            TASK_RETURN(desc) = byte;

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
            assertOk(priorityQueuePush(&kernel.queue,
                                       desc->priority,
                                       desc->tid.value));

            U8 byte = (U8)(TASK_ARG_1(desc));
            assertOk(uartWriteByte(UART_2, byte));
            
            kernel.eventTable[EVENT_TERMINAL_WRITE] = VAL_TO_ID(0);
        }

        assertOk(uartInterruptTX(UART_2, 0));
    }
}
