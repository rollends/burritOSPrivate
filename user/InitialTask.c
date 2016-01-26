#include "kernel/print.h"
#include "kernel/sysCall.h"

#include "user/TestTask.h"

void InitialTask()
{
    U32 priorities[4];
    priorities[0] = 2;
    priorities[1] = 2;
    priorities[2] = 0;
    priorities[3] = 0;
    U32 i;

    for (i = 0; i < 4; i++)
    {
        printString("Created: %b\r\n", sysCreate(priorities[i], &TestTask));
    }

    printString("FirstUserTask: exiting\r\n");
    sysExit();
}
