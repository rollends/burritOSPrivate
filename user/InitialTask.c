#include "kernel/message.h"
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

    for(i = 0; i < 4; i++)
    {
        MessageEnvelope env;
        U16 id;
        
        sysReceive(&id, &env);

        printString("FirstUserTask: received message from %x with data %x\r\n",
                    id,
                    env.message.NameserverRequest.name);
        env.message.NameserverRequest.name = 0x66;

        sysReply(id, &env);
    }

    printString("FirstUserTask: exiting\r\n");
    sysExit();
}
