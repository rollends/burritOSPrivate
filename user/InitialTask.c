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

static void TimingTask();

void InitialTask()
{
    while (1)
    {
        U32 val = timerValue();
        if ((val & 8191) == 8191)
        {
            printString("%x\r\n", val);
        }
    }
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
