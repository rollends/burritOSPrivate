#include "kernel/kernelData.h"

KernelData kernel;

S32 kernelDataInit(const U32 pc)
{
    kernel.baseAddress = pc;
    kernel.systemRunning = 1;

    priorityQueueInit(&(kernel.queue));
    taskTableInit(&(kernel.tasks));

    U32 i;
    for (i = 0; i < EVENT_TABLE_SIZE; i++)
    {
        kernel.eventTable[i].value = 0;
    }

    return 0;
}
