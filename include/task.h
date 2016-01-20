#ifndef TASK_H
#define TASK_H

#include "types.h"

#define TASK_COUNT 64
#define TASK_BIT_COUNT (TASK_COUNT + 31)/32

/**
 * Defines a  TaskID type that includes both the task id and a
 * generation value for identifying whether a task id has expired
 */
typedef union
{
    /// The combined id that includes both generation and task id
    U16 value;
    
    struct
    {
        /// The generation number, set at task creation
        U8 generation;

        /// The task id used to identify the task and index into the task block
        U8 id;
    } fields;
} TaskID;

/**
 * Defines an enumeration of possible task states
 */
typedef enum
{
    eZombie,
    eReady,
    eActive,
} TaskState;

/**
 * Defines a task data structure that contains the necessary information
 * for the kernel to manage the task. 
 */
typedef struct
{
    /// A pointer to the last element in the task's stack
    U32* stack;

    /// The current state of the task
    TaskState state;

    /// The task id
    TaskID tid;

    /// The parent's task id
    TaskID pid;
} TaskDescriptor;

/**
 * Defines a table of task descriptors that are allocated to active
 * tasks.
 */
typedef struct
{
    /// The table of task descriptors 
    TaskDescriptor table[TASK_COUNT];

    /// The table used to determine if a task data entry is free
    U32 tableFlag[TASK_BIT_COUNT];
} TaskTable;

/**
 * Initializes a TaskTable instance
 *
 * @param   table   The TaskTable to initialize
 *
 * @return  0 if the table is initialized successfully, else an error code
 */
S32 taskTableInit(TaskTable* table);

/**
 * Allocates a task descriptor in a table
 *
 * @param   table   The TaskTable in which to allocate a descriptor
 * @param   desc    A pointer to the location to store the descriptor to
 *
 * @return  0 if the descriptor is allocated successfully, else an error code
 */
S32 taskTableAlloc(TaskTable* table, TaskDescriptor** desc);

/**
 * Frees a task descriptor
 *
 * @param   table   The TaskTable that holdes the descriptor
 * @param   desc    A point to the location to store the descriptor to
 *
 * @return  0 if the descriptor is freed succesfully, else an error code
 */
S32 taskTableFree(TaskTable* table, TaskDescriptor* desc);

#endif
