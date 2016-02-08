#include "kernel/kernel.h"

#include "user/IdleTask.h"
#include "user/InitialTask.h"

void IdleTask()
{
    sysPerfReset();
    sysCreate(2, &InitialTask, STACK_SIZE_SMALL);

    while (sysRunning() != 0)
    {
        sysPass();
    }
}
