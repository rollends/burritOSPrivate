#include "hardware/hardware.h"

#include "kernel/assert.h"
#include "kernel/kernelData.h"

KernelData kernel;

S32 kernelDataInit()
{
    kernel.systemRunning = 1;

    assertOk(priorityQueueInit(&(kernel.queue)));
    assertOk(taskTableInit(&(kernel.tasks)));

    U32 i;
    for (i = 0; i < EVENT_TABLE_SIZE; i++)
    {
        kernel.eventTable[i].value = 0;
    }

    kernel.cts = uartCTS(UART_1);

    return OK;
}
