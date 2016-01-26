#include "kernel/print.h"
#include "kernel/sysCall.h"
#include "user/userTasks.h"

void TestTask()
{
    printString("Tid: %b\tPid: %b\r\n", sysTid(), sysPid());
    sysPass();
    printString("Tid: %b\tPid: %b\r\n", sysTid(), sysPid());
    sysExit();
}
