#include "print.h"
#include "sysCall.h"
#include "userTasks.h"

void TestTask()
{
    printString("Tid: %b\tPid: %b\r\n", sysTid(), sysPid());
    sysPass();
    printString("Tid: %b\tPid: %b\r\n", sysTid(), sysPid());
    sysExit();
}