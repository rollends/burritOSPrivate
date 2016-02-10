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

    /// Performance timer state struct
    TimerState          perfState;
    
    /// The system running flag
    U8                  systemRunning;

    /// Terminal input queue
    QueueU8             terminalInput;
    
    /// Backing data for the terminal input buffer
    U8                  terminalInputData[TERM_BUFFER];

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
