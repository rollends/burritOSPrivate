#include "common/random.h"

#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/sysCall.h"

#include "user/Nameserver.h"
#include "user/RPSServer.h"
#include "user/TestTask.h"

void InitialTask()
{
    U32 i;
    U8 seed = 11;

    sysCreate(0, &Nameserver);
    sysCreate(2, &RPSServer);
    
    nsRegister(God);

    for (i = 0; i < 2; i++)
    {
        U16 id = sysCreate(1, &TestTask);
        
        MessageEnvelope envelope;
        envelope.type = MESSAGE_RANDOM_BYTE;
        envelope.message.RandomSeed.seed = nextRandU8(&seed);
        sysSend(id, &envelope, &envelope);
    }

    printString("FirstUserTask: exiting\r\n");
    sysExit();
}
