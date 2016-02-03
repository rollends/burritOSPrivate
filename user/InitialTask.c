#include "common/memory.h"
#include "common/random.h"

#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/sysCall.h"
#include "kernel/timer.h"

#include "user/messageTypes.h"

#include "user/MessageTimingTask.h"
#include "user/Nameserver.h"
#include "user/RPSServer.h"
#include "user/TestTask.h"

extern void ClockServer();

static void TimingTask();

void InitialTask()
{
    U32 i;
    U8 seed = 11;

#ifdef TIME_MESSAGE_PASSING
    TimingTask();
#else 
    sysCreate(0, &Nameserver);
    sysCreate(0, &RPSServer);
	sysCreate(1, &ClockServer);

    nsRegister(God);

    // Create RPS Players
    for (i = 0; i < 6; i++)
    {
        U16 id = sysCreate(1, &TestTask);
        
        MessageEnvelope envelope;
        envelope.type = MESSAGE_RANDOM_BYTE;
        envelope.message.MessageU8.body = nextRandU8(&seed);
        sysSend(id, &envelope, &envelope);
    }
#endif
    printString("FirstUserTask: exiting\r\n");
    sysExit();
}

static void TimingTask()
{
	U8 i = 0;
    {
        U16 timingId = sysCreate(0, &MessageTimingTask);
        TimerState state;

        MessageEnvelope env;
        env.type = MESSAGE_RANDOM_BYTE;

        timerStart(&state);
        for(i = 0; i < 100; ++i)
        {
            sysSend( timingId, &env, &env );
        }
        printString("Timed receive-first message at %x\r\n", timerEnd( &state ) );
        env.type = 0;
        sysSend(timingId, &env, &env);
    }

    {
        U16 timingId = sysCreate(2, &MessageTimingTask);
        TimerState state;

        MessageEnvelope env;
        env.type = MESSAGE_RANDOM_BYTE;

        timerStart(&state);
        for(i = 0; i < 100; ++i)
        {
            sysSend( timingId, &env, &env );
        }
        printString("Timed send-first message at %x\r\n", timerEnd( &state ) );
        env.type = 0;
        sysSend(timingId, &env, &env);
    }

    {
        U16 timingId = sysCreate(0, &MessageTimingTask64);
        TimerState state;

        
        MessageEnvelope env;
        U32 buffer[16];
        env.type = MESSAGE_TYPE_64_BYTE;

        timerStart(&state);
        for(i = 0; i < 100; ++i)
        {
            env.message.MessageArbitrary.body = buffer;
            sysSend( timingId, &env, &env );
            __memcpy(buffer, env.message.MessageArbitrary.body, 16);
        }
        printString("Timed receive-first message at %x\r\n", timerEnd( &state ) );
        env.type = 0;
        sysSend(timingId, &env, &env);
    }

    {
        U16 timingId = sysCreate(2, &MessageTimingTask64);
        TimerState state;

        
        MessageEnvelope env;
        U32 buffer[16];
        env.type = MESSAGE_TYPE_64_BYTE;

        timerStart(&state);
        for(i = 0; i < 100; ++i)
        {
            env.message.MessageArbitrary.body = buffer;
            sysSend( timingId, &env, &env );
            __memcpy(buffer, env.message.MessageArbitrary.body, 16);
        }
        printString("Timed send-first message at %x\r\n", timerEnd( &state ) );
        env.type = 0;
        sysSend(timingId, &env, &env);
    }
	sysExit();
}
