#include "kernel/kernel.h"

#include "user/IdleTask.h"
#include "user/InitialTask.h"

void IdleTask()
{
    sysPerfReset();
    sysCreate(2, &InitialTask);
    while (sysRunning() != 0)
    {
        sysPass();
    }
}
