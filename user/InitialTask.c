#include "common/random.h"

#include "hardware/hardware.h"

#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/systemCall.h"

#include "user/messageTypes.h"

#include "user/ClockServer.h"
#include "user/MessageTimingTask.h"
#include "user/Nameserver.h"
#include "user/PerformanceTask.h"
#include "user/TestTask.h"
#include "user/DelayTestTask.h"

static void TimingTask();

void InitialTask()
{
    U16 DelayTimes[4],      DelayCount[4];
    DelayTimes[0] = 10;     DelayCount[0] = 20;
    DelayTimes[1] = 23;     DelayCount[1] = 9;
    DelayTimes[2] = 33;     DelayCount[2] = 6;
    DelayTimes[3] = 71;     DelayCount[3] = 3;

    sysCreate( 0, &Nameserver );
    sysCreate( 1, &ClockServer );
    sysCreate(0, &PerformanceTask);

    TaskID clock;
    nsWhoIs( Clock, &clock );

    printString( "Creating all tasks at time %d\r\n", clockTime( clock ) );
    printString( "(TID)        (Completed)        (Delay Ticks)    (Current Time)\r\n" );

    U8 i = 0;
    for(i = 0; i < 4; ++i)
    {
        sysCreate( 3 + i, &DelayTestTask );
    }

    for(i = 0; i < 4; ++i)
    {
        MessageEnvelope env;
        TaskID id;

        sysReceive( &id.value, &env );
        env.message.MessageU32.body = ( DelayTimes[i] << (8*2) ) | DelayCount[i];
        sysReply( id.value, &env );
    }

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

        timerStart(TIMER_3, &state);
        for(i = 0; i < 100; ++i)
        {
            sysSend( timingId, &env, &env );
        }
        printString("Timed receive-first message at %x\r\n", timerSample(TIMER_3, &state));
        env.type = 0;
        sysSend(timingId, &env, &env);
    }

    {
        U16 timingId = sysCreate(2, &MessageTimingTask);
        TimerState state;

        MessageEnvelope env;
        env.type = MESSAGE_RANDOM_BYTE;

        timerStart(TIMER_3, &state);
        for(i = 0; i < 100; ++i)
        {
            sysSend( timingId, &env, &env );
        }
        printString("Timed send-first message at %x\r\n", timerSample(TIMER_3, &state ) );
        env.type = 0;
        sysSend(timingId, &env, &env);
    }

    {
        U16 timingId = sysCreate(0, &MessageTimingTask64);
        TimerState state;


        MessageEnvelope env;
        U32 buffer[16];
        env.type = MESSAGE_TYPE_64_BYTE;

        timerStart(TIMER_3, &state);
        for(i = 0; i < 100; ++i)
        {
            env.message.MessageArbitrary.body = buffer;
            sysSend( timingId, &env, &env );
            __memcpy(buffer, env.message.MessageArbitrary.body, 16);
        }
        printString("Timed receive-first message at %x\r\n", timerSample(TIMER_3, &state ) );
        env.type = 0;
        sysSend(timingId, &env, &env);
    }

    {
        U16 timingId = sysCreate(2, &MessageTimingTask64);
        TimerState state;


        MessageEnvelope env;
        U32 buffer[16];
        env.type = MESSAGE_TYPE_64_BYTE;

        timerStart(TIMER_3, &state);
        for(i = 0; i < 100; ++i)
        {
            env.message.MessageArbitrary.body = buffer;
            sysSend( timingId, &env, &env );
            __memcpy(buffer, env.message.MessageArbitrary.body, 16);
        }
        printString("Timed send-first message at %x\r\n", timerSample(TIMER_3, &state ) );
        env.type = 0;
        sysSend(timingId, &env, &env);
    }
    sysExit();
}
