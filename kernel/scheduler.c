#include "common/common.h"
#include "hardware/hardware.h"

#include "kernel/assert.h"
#include "kernel/kernelData.h"
#include "kernel/taskDescriptor.h"

U32* scheduler(U32* sp)
{
    if (kernel.systemRunning == 2)
        return 0;

    TaskDescriptor* desc = kernel.activeTask;
    TaskID tid = desc->tid;
    desc->stack = sp;

    if (desc->state == eReady)
    {
        S32 result = priorityQueuePushPop(&kernel.queue,
                                          desc->priority,
                                          &(tid.value));
        assertOk(result);
        if (result == 1)
        {
            goto ExitScheduler;
        }
    }
    else
    {
        if (desc->state == eZombie)
        {
            taskTableFree(&kernel.tasks, tid);
        }

        S32 empty = priorityQueueEmpty(&kernel.queue);
        assertOk(empty);
        if (empty == 1)
        {
            assert(!kernel.systemRunning);
            return 0;
        }
        
        assertOk(priorityQueuePop(&kernel.queue, &(tid.value)));
    }

    kernel.activeTask = taskGetDescriptor(&kernel.tasks, tid);
    sp = kernel.activeTask->stack;

ExitScheduler:
    #ifdef KERNEL_PERF
        assertOk(timerSample(TIMER_4, &kernel.perfState));
        desc->performance[ePerfKernel] += kernel.perfState.delta;
    #endif

    return sp;
}
