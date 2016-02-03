#include "kernel/sysCall.h"
#include "user/IdleTask.h"
#include "user/InitialTask.h"

void IdleTask()
{
    sysCreate(1, &InitialTask);

    while (sysRunning() != 0)
    {
        sysPass();
    }

    sysExit();
}