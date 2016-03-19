#include "kernel/kernel.h"

#include "user/display/IdlePerformanceDisplay.h"
#include "user/services/services.h"

void IdlePerformanceDisplay()
{
    TaskID clock = nsWhoIs(Clock);

    for(;;)
    {
        clockLongDelayBy(clock, 2);
        
    #ifdef KERNEL_PERF
        U32 totalIdlePercent = sysPerfQueryP(0, ePerfBoth);
    #else
        U32 totalIdlePercent = 0;
    #endif

        printf( "\033[s\033[1;1H\033[36m[%2d.%2d%%\033[m\033[u",
                totalIdlePercent / 100,
                totalIdlePercent % 100);
    }
}
