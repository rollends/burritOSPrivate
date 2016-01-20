#include "task.h"

S32 taskInit(Tasks* tasks, U8 priority, U32 entry, U16 pid)
{
    U32 tid = tasks->lastTid + 1;

    TaskDescriptor* desc = tasks->descriptors + tasks->lastTid;
    desc->state = eReady;
    desc->tid.fields.generation = 0;
    desc->tid.fields.id = tid;
    desc->pid.fields.id = pid;
    desc->priority = priority;

    U32* stack = tasks->stackMemory+(1024*tid - 1);
    *(stack) = entry;
    *(stack-15) = 0x10;

    desc->stack = stack - 15;

    tasks->lastTid = tid;
    
    return desc->tid.value;
}
