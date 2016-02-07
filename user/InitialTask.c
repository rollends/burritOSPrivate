#include "common/common.h"
#include "kernel/kernel.h"
#include "kernel/print.h"

void InitialTask()
{
    while (1)
    {
        U8 byte = sysRead();
        printString("\t\t%b\r\n", byte);
    }
}
