#include "kernel/kernel.h"

#include "user/ClockServer.h"
#include "user/Nameserver.h"
#include "user/PerformanceTask.h"

void PerformanceTask()
{
    TaskID clock;
    nsWhoIs( Clock, &clock );

    sysPerfStart();
    clockDelayBy(clock, 250);
    U32 percentage = sysPerfQuery(0);

    printString("\r\nIdle task running time: %d.%d%%\r\n",
                percentage / 10,
                percentage % 10);
    printString("Full performance summary:\r\n");
    
    U32 i;
    for (i = 0; i < 10; i++)
    {
        U32 percentage = sysPerfQuery(i);
        printString("\t%b:\t%2d.%d%%\r\n", i, percentage / 10, percentage % 10);   
    }

    sysShutdown();
    sysExit();
}
