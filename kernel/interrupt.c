#include "common/common.h"
#include "hardware/hardware.h"

#include "kernel/event.h"
#include "kernel/kernelData.h"

void interruptHandler()
{
    TaskDescriptor* desc = kernel.activeTask;
    desc->performance[ePerfTask] += timerSample(TIMER_4, &kernel.perfState);
    
    U32 status1 = interruptStatus(INT_1);

    if (status1 & 0x10)
    {
        TaskID tid = kernel.eventTable[EVENT_10MS_TICK];

        if (tid.value != 0)
        {
            TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);

            desc->state = eReady;
            priorityQueuePush(&kernel.queue,
                              desc->priority,
                              desc->tid.value);

            kernel.eventTable[EVENT_10MS_TICK] = VAL_TO_ID(0);
        }

        timerClear(TIMER_1);
    }

    if (status1 & 0x20)
    {
        TaskID tid = kernel.eventTable[EVENT_100MS_TICK];

        if (tid.value != 0)
        {
            TaskDescriptor* desc = taskGetDescriptor(&kernel.tasks, tid);

            desc->state = eReady;
            priorityQueuePush(&kernel.queue,
                              desc->priority,
                              desc->tid.value);

            kernel.eventTable[EVENT_100MS_TICK] = VAL_TO_ID(0);
        }

        timerClear(TIMER_2);
    }
}
