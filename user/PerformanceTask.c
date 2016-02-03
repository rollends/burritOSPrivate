#include "kernel/print.h"
#include "kernel/systemCall.h"
#include "kernel/taskDescriptor.h"

#include "user/ClockServer.h"
#include "user/Nameserver.h"
#include "user/PerformanceTask.h"

void PerformanceTask()
{
    TaskID clock;
    nsWhoIs( Clock, &clock );

    clockDelayBy(clock, 250);
    U32 percentage = sysPerformance(0);
    printString("Idle task running time: %d.%d%%\r\n",
                percentage / 10,
                percentage % 10);

    sysShutdown();
    sysExit();
}
