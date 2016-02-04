#ifndef KERNEL_DATA_H
#define KERNEL_DATA_H

#include "common/priorityQueue.h"
#include "hardware/timer.h"
#include "kernel/event.h"
#include "kernel/taskDescriptor.h"
#include "kernel/taskTable.h"

/**
 * Core kernel struct containing the task table, queues, etc
 */
typedef struct
{
    /// Active task descriptor
    TaskDescriptor*  activeTask;

    /// Task priority queue
    PriorityQueue   queue;

    /// The task descriptor table
    TaskTable   tasks;

    /// Event Table
    TaskID eventTable[EVENT_TABLE_SIZE];

    /// Performance timer state struct
    TimerState perfState;

    /// The kernel base address
    U32 baseAddress;

    /// The system running flag
    U8  systemRunning;
} KernelData;

/**
 * Initializes a kernel data struct
 *
 * @param   data    The kernel data pointer
 * @param   pc      The pc (base address) where the kernel is loaded
 *
 * @return  0 on success, else an error code
 */
S32 kernelDataInit(const U32 pc);

/**
 * Shared static kernel struct
 */
extern KernelData kernel;

#endif
