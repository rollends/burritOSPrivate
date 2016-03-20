#ifndef TASK_TABLE_H
#define TASK_TABLE_H

#include "common/common.h"

#include "kernel/config.h"
#include "kernel/memoryAllocator.h"
#include "kernel/taskDescriptor.h"

/**
 * Table for storing task state, i.e. task descriptors and the allocation queue
 */
typedef struct
{
    /// The task descriptor array
    TaskDescriptor descriptors[TASK_COUNT];

    /// The queue of availabe descriptors
    QueueU8 allocationQueue;

    /// Backing data for the descriptor queue
    U8 allocationTable[TASK_COUNT];

    /// Backing data for the send queue
    U16 sendQueueTable[TASK_COUNT*SEND_QUEUE_LENGTH];
    
    /// Stack and block allocation table
    MemoryAllocator memoryAllocator;
} TaskTable;

/**
 * Initializes the task table
 *
 * @param   table   The table to initialize
 *
 * @return  0 on success, else an error code
 */
S32 taskTableInit(TaskTable* table);

/**
 * Gets a free task descriptor from a table
 *
 * @param   table       The table to get the descriptor
 * @param   priority    The priority to assign to the descriptor
 * @param   entryAddr   The entry point (pc) of the task
 * @param   exitAddr    The exit point (lr) of the task
 * @param   pid         The parent task id
 * @param   name        The nice task name
 *
 * @return  TaskID on success, else an error code
 */
S32 taskTableAlloc(TaskTable* table,
                   const U8 priority,
                   const U32 entryAddr,
                   const U32 exitAddr,
                   const TaskID pid,
                   ConstString name);

/**
 * Frees a task descriptor
 *
 * @param   table       The table in which the descriptor is stored
 * @param   tid         The task id
 *
 * @return  0 on success, else an error code
 */
S32 taskTableFree(TaskTable* table, const TaskID tid);

/**
 * Clears performance data for all tasks in the table
 *
 * @param   table       The table to clear data in
 *
 * @return  0 on success, else an error code
 */
S32 taskTablePerfClear(TaskTable* table);

/**
 * Returns the run time percentage to 2 decimal place for a given task
 *
 * @param   table       The table to get the performance from
 * @param   tid         The task id
 * @param   perf        The performance mode to measure
 * @param   runtime     The total system runtime
 *
 * @return  performance percentage in 10ths of a percent
 */
U32 taskTablePerfP(TaskTable* table,
                   const TaskID tid,
                   const TaskPerf perf,
                   const U32 runtime);

/**
 * Returns the absolute run time for a given task
 *
 * @param   table       The table to get the performance from
 * @param   tid         The task id
 * @param   perf        The performance mode to measure
 * @param   runtime     The total system runtime
 *
 * @return  performance percentage in 10ths of a percent
 */
U32 taskTablePerfT(TaskTable* table,
                   const TaskID tid,
                   const TaskPerf perf,
                   const U32 runtime);

/**
 * Gets a descriptor from a table by ID
 *
 * @param   table       The table to get the descriptor
 * @param   tid         The task id
 *
 * @return  A TaskDescriptor pointer
 */
static inline TaskDescriptor* taskGetDescriptor(TaskTable* table,
                                                const TaskID tid)
{
    return &(table->descriptors[tid.fields.id]);
}

/**
 * Returns the number of task descriptors currentl allocated
 *
 * @param   table       The table to check allocations in
 *
 * @return  The number of allocated tasks
 */
static inline U32 taskTableCount(TaskTable* table)
{
    return TASK_COUNT - table->allocationQueue.count;
}

#endif
