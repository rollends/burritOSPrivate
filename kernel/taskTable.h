#ifndef TASK_TABLE_H
#define TASK_TABLE_H

#include "common/common.h"

#include "kernel/config.h"
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
 * @param   entry       The entry point (pc) of the task
 * @param   pid         The parent task id
 *
 * @return  0 on success, else an error code
 */
S32 taskTableAlloc(TaskTable* table,
                   const U8 priority,
                   const U32 entry,
                   const TaskID pid);

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

#endif
