#include "common/random.h"

#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/sysCall.h"
#include "kernel/timer.h"

#include "user/MessageTimingTask.h"
#include "user/Nameserver.h"
#include "user/RPSServer.h"
#include "user/TestTask.h"

void InitialTask()
{
    U32 i;
    U8 seed = 11;

    sysCreate(0, &Nameserver);

    {
        U16 timingId = sysCreate(2, &MessageTimingTask);
        TimerState state;

        timerStart(&state);
        MessageEnvelope env;
        env.type = MESSAGE_RANDOM_BYTE;
        for(i = 0; i < 100; ++i)
        {
            sysSend( timingId, &env, &env );
        }
        printString("Timed basic message at %x\r\n", timerEnd( &state ) );
        env.type = 0;
        sysSend(timingId, &env, &env);
    }

    sysCreate(2, &RPSServer);
    
    nsRegister(God);

    for (i = 0; i < 2; i++)
    {
        U16 id = sysCreate(1, &TestTask);
        
        MessageEnvelope envelope;
        envelope.type = MESSAGE_RANDOM_BYTE;
        envelope.message.MessageU8.body = nextRandU8(&seed);
        sysSend(id, &envelope, &envelope);
    }

    printString("FirstUserTask: exiting\r\n");
    sysExit();
}
