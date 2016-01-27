#ifndef KERNEL_DATA_H
#define KERNEL_DATA_H

#define PRIORITY_COUNT   3

#include "common/priorityQueue.h"
#include "kernel/taskDescriptor.h"
#include "kernel/taskTable.h"

typedef struct
{
    /// Backing data for the task queue
    U8 queueData[PQUEUE_MEM_SIZE(TASK_COUNT, PRIORITY_COUNT)];

    /// Task priority queue
    PriorityQueue   queue;

    /// The task descriptor table
    TaskTable   tasks;

    /// The kernel base address
    U32 baseAddress;

    /// Active task descriptor
    TaskDescriptor*  activeTask;
} KernelData;

S32 kernelDataInit(KernelData* data, const U32 pc);

#endif
