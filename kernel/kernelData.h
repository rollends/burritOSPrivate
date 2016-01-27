#ifndef KERNEL_DATA_H
#define KERNEL_DATA_H

#define PRIORITY_COUNT   3

#include "common/priorityQueue.h"
#include "kernel/taskDescriptor.h"
#include "kernel/taskTable.h"

/**
 * Core kernel struct containing the task table, queues, etc
 */
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

/**
 * Initializes a kernel data struct
 *
 * @param   data    The kernel data pointer
 * @param   pc      The pc (base address) where the kernel is loaded
 *
 * @return  0 on success, else an error code
 */
S32 kernelDataInit(KernelData* data, const U32 pc);

#endif
