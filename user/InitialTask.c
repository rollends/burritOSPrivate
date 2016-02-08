#include "common/common.h"
#include "kernel/kernel.h"
#include "kernel/print.h"

void InitialTask()
{
    sysWrite(PORT_TRAIN, 0x60);
    while (1)
    {
        sysRead(PORT_TERMINAL);

        sysWrite(PORT_TRAIN, 0x85);
        sysWrite(PORT_TRAIN, 0x00);
        
        U32 i;
        for (i = 0; i < 10; i++)
        {
            sysWrite(PORT_TERMINAL, sysRead(PORT_TRAIN) + 'a');
        }

        sysWrite(PORT_TERMINAL, '\r');
        sysWrite(PORT_TERMINAL, '\n');
    }
}
