#include "kernel/kernel.h"
#include "user/IdleTask.h"
#include "user/InitialTask.h"

void IdleTask()
{
#ifdef KEREL_PERF
    sysPerfReset();
#endif
    sysCreate(2, &InitialTask);
    
    while (sysRunning() != 0)
    {
        sysPass();
    }
}
