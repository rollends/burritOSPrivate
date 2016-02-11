#include "common/common.h"
#include "kernel/kernel.h"
#include "kernel/print.h"

void InitialTask()
{
    sysWrite(EVENT_TRAIN_WRITE, 0x60);

    while (1)
    {
        sysRead(EVENT_TERMINAL_READ);

        sysWrite(EVENT_TRAIN_WRITE, 0x85);
        sysWrite(EVENT_TRAIN_WRITE, 0x00);
        
        U32 i;
        for (i = 0; i < 10; i++)
        {
            sysWrite(EVENT_TERMINAL_WRITE, sysRead(EVENT_TRAIN_READ) + 'a');
        }

        sysWrite(EVENT_TERMINAL_WRITE, '\r');
        sysWrite(EVENT_TERMINAL_WRITE, '\n');
    }
}
