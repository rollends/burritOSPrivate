#ifndef TASK_TABLE_H
#define TASK_TABLE_H

#include "common/queue.h"
#include "common/types.h"
#include "kernel/taskDescriptor.h"

#define TASK_COUNT  64

typedef struct
{
    TaskDescriptor descriptors[TASK_COUNT];
    U32 stacks[TASK_COUNT];
    U8 allocationTable[TASK_COUNT];
    QueueU8 allocationQueue;
} TaskTable;

S32 taskTableInit(TaskTable* table);
S32 taskTableAlloc(TaskTable* table, U8 priority, U32 entry, const TaskID pid);
S32 taskTableFree(TaskTable* table, const TaskID tid);
TaskDescriptor* taskGetDescriptor(TaskTable* table, const TaskID tid);


#endif
