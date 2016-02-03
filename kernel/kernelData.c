#include "kernel/kernelData.h"

S32 kernelDataInit(KernelData* data, const U32 pc)
{
    data->baseAddress = pc;
    data->systemRunning = 1;

    priorityQueueInit(&(data->queue));
    taskTableInit(&(data->tasks));

	U8 i = 0;
	for(; i < EVENT_TABLE_SIZE; ++i)
	{
		data->eventTable[i].value = 0;
	}

    return 0;
}
