#include "kernel/kernelData.h"

KernelData kernel;

S32 kernelDataInit()
{
    kernel.systemRunning = 1;

    priorityQueueInit(&(kernel.queue));
    taskTableInit(&(kernel.tasks));
    queueU8Init(&kernel.terminalInput, kernel.terminalInputData, TERM_BUFFER);

    U32 i;
    for (i = 0; i < EVENT_TABLE_SIZE; i++)
    {
        kernel.eventTable[i].value = 0;
    }

    kernel.cts = 1;

    return 0;
}
