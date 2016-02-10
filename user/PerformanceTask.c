#include "kernel/kernel.h"

#include "user/ClockServer.h"
#include "user/Nameserver.h"
#include "user/PerformanceTask.h"

void PerformanceTask()
{
    TaskID clock;
    nsWhoIs( Clock, &clock );

    clockDelayBy(clock, 250);

    printBlocking("\r\nPerformance summary:\r\n");
    
    U32 i;
    printBlocking("\tTaskId\tTsk %%\tKrnl %%\tTtl %%\tTsk us\t\tKrnl us\t\tTtl us\r\n");
    for (i = 0; i < 10; i++)
    {
        if (i == 0)
        {
            printBlocking("\x1b[7m");
        }

        U32 up = sysPerfQueryP(i, ePerfTask);
        U32 kp = sysPerfQueryP(i, ePerfKernel);
        U32 tp = sysPerfQueryP(i, ePerfBoth);

        U32 uu = sysPerfQueryT(i, ePerfTask);
        U32 ku = sysPerfQueryT(i, ePerfKernel);
        U32 tu = sysPerfQueryT(i, ePerfBoth);
 
        printBlocking("\t%b\t%2d.%2d\t%2d.%2d\t%2d.%2d\t%9n\t%9n\t%9n\r\n", i,
                                                           up / 100, up % 100,
                                                           kp / 100, kp % 100,
                                                           tp / 100, tp % 100,
                                                           uu,
                                                           ku,
                                                           tu);
        if (i == 0)
        {
            printBlocking("\x1b[0m");
        }
    }

    U32 total = sysPerfQueryT(0, ePerfTotal);
    U32 totalms = total / 1000;
    printBlocking("\r\n\tTotal run time: %nus ~ %d.%3ds\r\n", total,
                                                        totalms / 1000,
                                                        totalms % 1000);

    sysShutdown();
}
