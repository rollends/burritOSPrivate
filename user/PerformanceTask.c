#include "kernel/kernel.h"

#include "user/PerformanceTask.h"
#include "user/services/services.h"

void PerformanceTask()
{
#ifdef KERNEL_PERF
    TaskID  clock   = nsWhoIs(Clock);

#ifdef KERNEL_PERF_VERBOSE
    TaskID tid = { sysTid() };

    for(;;)
    {
        sysPerfReset();
        clockDelayBy(clock, 200);

        printf("\r\nPerformance summary: %d\r\n", tid.value);
    
        U32 i;
        printf("\tTaskId\t\tTsk %%\tKrnl %%\tTtl %%\tTsk us\t\tKrnl us\t\tTtl us\r\n");

        for (i = 0; i <= 12; i++)
        {
            if (i == 0)
            {
                printf("\x1b[7m");
            }

            U32 up = sysPerfQueryP(i, ePerfTask);
            U32 kp = sysPerfQueryP(i, ePerfKernel);
            U32 tp = sysPerfQueryP(i, ePerfBoth);

            U32 uu = sysPerfQueryT(i, ePerfTask);
            U32 ku = sysPerfQueryT(i, ePerfKernel);
            U32 tu = sysPerfQueryT(i, ePerfBoth);
     
            ConstString str = (ConstString)sysName(i);

            printf("\t%14s\t%2d.%2d\t%2d.%2d\t%2d.%2d\t%9n\t%9n\t%9n\r\n", str,
                                                               up / 100, up % 100,
                                                               kp / 100, kp % 100,
                                                               tp / 100, tp % 100,
                                                               uu,
                                                               ku,
                                                               tu);
            if (i == 0)
            {
                printf("\x1b[0m");
            }
        }

        U32 total = sysPerfQueryT(0, ePerfTotal);
        U32 totalms = total / 1000;
        printf("\r\n\tTotal run time: %nus ~ %d.%3ds\r\n", total,
                                                           totalms / 1000,
                                                           totalms % 1000);
    }
#else
    for(;;)
    {
        sysPerfReset();
        clockLongDelayBy(clock, 2);
        
        U32 totalIdlePercent = sysPerfQueryP(0, ePerfBoth);
        printf( "\033[s\033[2;1HIdling for %2d%%\033[u",
                totalIdlePercent / 100);
    }
#endif
#endif
}
