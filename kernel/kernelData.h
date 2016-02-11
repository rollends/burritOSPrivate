#ifndef KERNEL_DATA_H
#define KERNEL_DATA_H

#include "common/common.h"

#include "kernel/event.h"
#include "kernel/taskDescriptor.h"
#include "kernel/taskTable.h"

/**
 * Core kernel struct containing the task table, queues, etc
 */
typedef struct
{
    /// Active task descriptor
    TaskDescriptor*     activeTask;

    /// Task priority queue
    PriorityQueue       queue;

    /// The task descriptor table
    TaskTable           tasks;

    /// Event table data
    TaskID              eventTable[EVENT_TABLE_SIZE];

#ifdef KERNEL_PERF
    /// Performance timer state struct
    TimerState          perfState;
#endif
    
    /// The system running flag
    U8                  systemRunning;

    /// The CTS flag for UART_1
    U8                  cts;
} KernelData;

/**
 * Initializes a kernel data struct
 *
 * @return  0 on success, else an error code
 */
S32 kernelDataInit();

/**
 * Shared static kernel struct
 */
extern KernelData kernel;

#endif
