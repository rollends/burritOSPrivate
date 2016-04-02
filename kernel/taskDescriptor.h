#ifndef TASK_DESCRIPTOR_H
#define TASK_DESCRIPTOR_H

#include "common/common.h"
#include "kernel/config.h"
#include "kernel/memoryAllocator.h"

#define VAL_TO_ID(val)      (TaskID)((U16)(val))
#define TASK_RETURN(desc)   desc->stack[2]
#define TASK_ARG_0(desc)    desc->stack[3]
#define TASK_ARG_1(desc)    desc->stack[4]
#define TASK_ARG_2(desc)    desc->stack[5]

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
    /// No longer running; waiting to be recycled
    eZombie,

    /// Ready to run, i.e. not blocked
    eReady,

    /// Currently running
    eActive,

    /// Blocked on receiving a messaage
    eSendBlocked,

    /// Blocked on sending a message
    eReceiveBlocked,

    /// Blocked in receiving a reply
    eReplyBlocked,

    /// Blocked on an interrupt event
    eEventBlocked,
} TaskState;

/**
 * Enumeration of potential performance values to query
 */
typedef enum
{
    /// Time spent in kernel mode, e.g. handling system calls or interrupts
    ePerfKernel,

    /// Time spent in user mode, i.e. the task code itself
    ePerfTask,

    /// Total time spent in both modes
    ePerfBoth,

    /// Total time spent since the last reset
    ePerfTotal,

    /// Length flag
    ePerfCount
} TaskPerf;

/**
 * Defines a task data structure that contains the necessary information
 * for the kernel to manage the task. 
 */
typedef struct
{
    /// A pointer to the last element in the task's stack
    U32* stack;

    /// The base stack pointer
    U32* stackBase;

    /// The stack block id
    U16  stackBlock;

    /// The current state of the task
    TaskState state;

    /// The task priority
    U32 priority;
    
    /// The task id
    TaskID tid;

    /// The parent's task id
    TaskID pid;

    /// The send queue for the task
    QueueU16 sendQueue;

    /// The cummulative performance since the last clear
    U32 performance[ePerfCount];

    /// Task string name
    ConstString name;
} TaskDescriptor;

#endif
