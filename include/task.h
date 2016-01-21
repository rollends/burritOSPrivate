#ifndef TASK_H
#define TASK_H

#include "types.h"

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
        /// The task id used to identify the task and index into the task block
        U8 id;
        
        /// The generation number, set at task creation
        U8 generation;
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

    /// The task priority
    U8 priority;
} TaskDescriptor;


#define TaskTableLength 5
#define TaskTableFlagLength (TaskTableLength + 31)/32

typedef struct 
{
    TaskDescriptor descriptors[ TaskTableLength ];
    U32 stackMemory[1024 * TaskTableLength];
	U32 tableFlag[ TaskTableFlagLength ];
} Tasks;


S32 taskInit(Tasks* tasks);

S32 taskAlloc(Tasks* tasks, U8 priority, U32 entry, U16 pid);

S32 taskFree(Tasks* table, U16 tid);

TaskDescriptor* taskGetDescriptor(Tasks* tasks, U16 tid);


#endif
