#include "kernel/taskTable.h"
#include "kernel/print.h"

S32 taskTableInit(TaskTable* table)
{
    queueU8Init(&(table->allocationQueue), table->allocationTable, TASK_COUNT);

    U8 i;
    for(i = 0; i < TASK_COUNT; i++)
    {
        table->descriptors[i].tid.fields.id = i;
        table->descriptors[i].tid.fields.generation = 0;
        table->descriptors[i].pid.value = 0;
        table->descriptors[i].state = eZombie;
        queueU8Push(&(table->allocationQueue), i);
    }

    stackAllocatorInit(&table->stackAllocator, (U32*)(0x01000000));
    return 0;
}

S32 taskTableAlloc(TaskTable* table,
                   const U8 priority,
                   const U32 entryAddr,
                   const U32 exitAddr,
                   const U32 size,
                   const TaskID pid)
{
    if (table->allocationQueue.count == 0)
    {
        return -1;
    }

    U8 index;
    queueU8Pop(&(table->allocationQueue), &index);

    TaskDescriptor* desc = &(table->descriptors[index]);
    U32* stack = stackAllocatorAlloc(&table->stackAllocator, size);
    *(stack) = entryAddr;
    *(stack-14) = exitAddr;
    *(stack-15) = 0x10;
    
    desc->stack = stack - 15;
    desc->state = eReady;
    desc->pid = pid;
    desc->priority = priority;
    desc->performance = 0;

    queueU16Init(&desc->sendQueue,
                 table->sendQueueTable + index*SEND_QUEUE_LENGTH,
                 SEND_QUEUE_LENGTH);

    return desc->tid.value;
}

S32 taskTableFree(TaskTable* table, const TaskID tid)
{
    U32 index = tid.fields.id;

    TaskDescriptor* desc = &(table->descriptors[index]);

    if (desc->state != eZombie)
    {
        return -1;
    }

    queueU8Push(&(table->allocationQueue), index);

    desc->tid.fields.generation++;
    desc->pid.value = 0;
    desc->state = eZombie;

    stackAllocatorFree(&table->stackAllocator, desc->stack + 15);

    return 0;
}

S32 taskTablePerfClear(TaskTable* table)
{
    U32 i = 0;
    for(i = 0; i < TASK_COUNT; i++)
    {
        table->descriptors[i].performance = 0;
    }

    return 0;
}

U32 taskTablePerf(TaskTable* table, const TaskID tid, const U32 runtime)
{
    U32 index = tid.fields.id;
    TaskDescriptor* desc = &(table->descriptors[index]);

    U32 result = desc->performance;
    result *= 1000;
    result /= runtime;

    return result;
}
