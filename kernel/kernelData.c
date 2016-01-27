#include "kernel/kernelData.h"

S32 kernelDataInit(KernelData* data, const U32 pc)
{
    data->baseAddress = pc;

    priorityQueueInit(&(data->queue),
                      data->queueData,
                      TASK_COUNT,
                      PRIORITY_COUNT);
    taskTableInit(&(data->tasks));

    return 0;
}
