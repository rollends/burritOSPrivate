#include "kernel/taskTable.h"
#include "kernel/print.h"

S32 taskTableInit(TaskTable* table)
{
    queueU8Init(&(table->allocationQueue), table->allocationTable, TASK_COUNT);

    U8 i;
    for(i = 0; i < TASK_COUNT; ++i)
    {
        table->descriptors[i].tid.fields.id = i + 1;
        table->descriptors[i].tid.fields.generation = 0;
        table->descriptors[i].pid.value = 0;
        table->descriptors[i].state = eZombie;
        queueU8Push(&(table->allocationQueue), i);
    }

    return 0;
}

S32 taskTableAlloc(TaskTable* table, U8 priority, U32 entry, const TaskID pid)
{
    if (table->allocationQueue.count == 0)
    {
        return -1;
    }

    U8 index;
    queueU8Pop(&(table->allocationQueue), &index);

    TaskDescriptor* desc = &(table->descriptors[index]);

    U32* tablebase = 0x01000000;
    U32* stack = (U32*)(tablebase + 1024*(index+1) - 1);
    *(stack) = entry;
    *(stack-12) = 0x10;
    
    desc->stack = stack - 12;
    desc->state = eReady;
    desc->pid = pid;
    desc->priority = priority;

    return desc->tid.value;
}

S32 taskTableFree(TaskTable* table, const TaskID tid)
{
    U32 index = tid.fields.id - 1;

    TaskDescriptor* desc = &(table->descriptors[index]);

    if (desc->state != eZombie)
    {
        return -1;
    }

    queueU8Push(&(table->allocationQueue), index);

    desc->tid.fields.generation++;
    desc->pid.value = 0;
    desc->state = eZombie;

    return 0;
}

TaskDescriptor* taskGetDescriptor(TaskTable* table, const TaskID tid)
{
    return &(table->descriptors[tid.fields.id - 1]);
}
