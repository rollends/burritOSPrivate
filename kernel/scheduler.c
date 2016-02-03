#include "common/types.h"
#include "common/priorityQueue.h"
#include "hardware/hardware.h"
#include "kernel/kernelData.h"
#include "kernel/taskDescriptor.h"

U32* scheduler(U32* sp)
{
    TaskDescriptor* desc = kernel.activeTask;
    TaskID tid = desc->tid;

    if (desc->state == eReady)
    {
        if (priorityQueuePushPop(&kernel.queue,
                                 desc->priority,
                                 &(tid.value)) == 1)
        {
            return sp;
        }
    }
    else
    {
        if (desc->state == eZombie)
        {
            taskTableFree(&kernel.tasks, tid);
        }

        if (priorityQueuePop(&kernel.queue, &(tid.value)) != 0)
        {
            return 0;
        }
    }

    desc->stack = sp;
    kernel.activeTask = taskGetDescriptor(&kernel.tasks, tid);
    return kernel.activeTask->stack;
}
