#include "kernel/assert.h"
#include "kernel/taskTable.h"
#include "kernel/print.h"

S32 taskTableInit(TaskTable* table)
{
    assertOk(queueU8Init(&(table->allocationQueue),
             table->allocationTable, TASK_COUNT));

    U8 i;
    for (i = 0; i < TASK_COUNT; i++)
    {
        table->descriptors[i].tid.fields.id = i;
        table->descriptors[i].tid.fields.generation = 0;
        table->descriptors[i].pid.value = 0;
        table->descriptors[i].state = eZombie;
        assertOk(queueU8Push(&(table->allocationQueue), i));
    }

    assertOk(memoryAllocatorInit(&table->memoryAllocator, (U32*)(0x01000000)));

    return 0;
}

S32 taskTableAlloc(TaskTable* table,
                   const U8 priority,
                   const U32 entryAddr,
                   const U32 exitAddr,
                   const TaskID pid,
                   ConstString name)
{
    if (table->allocationQueue.count == 0)
    {
        return -1;
    }

    U8 index;
    assertOk(queueU8Pop(&(table->allocationQueue), &index));

    TaskDescriptor* desc = &(table->descriptors[index]);
    U32* stack = memoryAllocatorAlloc(&table->memoryAllocator);
    assert(stack != 0);
    *(stack) = entryAddr;
    *(stack-14) = exitAddr;
    *(stack-15) = 0x10;
    
    desc->stack = stack - 15;
    desc->state = eReady;
    desc->pid = pid;
    desc->priority = priority;
    desc->performance[ePerfKernel] = 0;
    desc->performance[ePerfTask] = 0;
    desc->name = name;

    assertOk(queueU16Init(&desc->sendQueue,
                          table->sendQueueTable + index*SEND_QUEUE_LENGTH,
                          SEND_QUEUE_LENGTH));

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

    assertOk(memoryAllocatorFree(&table->memoryAllocator, desc->stack + 15));

    return 0;
}

S32 taskTablePerfClear(TaskTable* table)
{
    U32 i;
    for(i = 0; i < TASK_COUNT; i++)
    {
        table->descriptors[i].performance[ePerfKernel] = 0;
        table->descriptors[i].performance[ePerfTask] = 0;
    }

    return 0;
}

U32 taskTablePerfP(TaskTable* table, const TaskID tid, const TaskPerf perf, const U32 runtime)
{
    U32 index = tid.fields.id;
    U32 denom = runtime;
    denom >>= 3;
    TaskDescriptor* desc = &(table->descriptors[index]);

    U32 result;
    if (perf == ePerfBoth)
    {
        result = desc->performance[ePerfTask] + desc->performance[ePerfKernel];
    }
    else
    {
        result = desc->performance[perf];
    }

    result *= 1000;
    result /= denom;
    result *= 10;
    result >>= 3;

    return result;
}

U32 taskTablePerfT(TaskTable* table, const TaskID tid, const TaskPerf perf, const U32 runtime)
{
    U32 index = tid.fields.id;
    TaskDescriptor* desc = &(table->descriptors[index]);

    U32 result;
    if (perf == ePerfBoth)
    {
        result = desc->performance[ePerfTask] + desc->performance[ePerfKernel];
    }
    else
    {
        result = desc->performance[perf];
    }

    result *= 1000;
    result /= 983;

    return result;
}
