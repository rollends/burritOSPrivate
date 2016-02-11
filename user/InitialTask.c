#include "common/common.h"
#include "hardware/hardware.h"
#include "kernel/kernel.h"
#include "kernel/print.h"

#include "user/messageTypes.h"
#include "user/Nameserver.h"
#include "user/ClockServer.h"
#include "user/PerformanceTask.h"

void MessageTimingTask()
{
    MessageEnvelope envelope;

    U16 id;
    for(;;)
    {
        sysReceive( &id, &envelope );
        sysReply( id, &envelope );
        if( envelope.type == 0 )
            break;
    }
}

void PerfTestTask()
{
    U32 i = 0;
    {
        U16 timingId = sysCreate(0, &MessageTimingTask);
        TimerState state;

        MessageEnvelope env;
        env.type = MESSAGE_RANDOM_BYTE;

        timerEnable(TIMER_3, 1, 0, 1);
        timerStart(TIMER_3, &state);
        for(i = 0; i < 10000; ++i)
        {
            sysSend( timingId, &env, &env );
        }
        printBlocking("%x\r\n", timerGetValue(TIMER_3));
        printBlocking("Timed receive-first message at %x\r\n", timerSample(TIMER_3, &state ) );
        env.type = 0;
        sysSend(timingId, &env, &env);
    }
}

void InitialTask()
{
    sysCreate(0, &Nameserver);
    sysCreate(1, &ClockServer);
    sysCreate(0, &PerformanceTask);

    sysWrite(EVENT_TRAIN_WRITE, 0x60);
    sysCreate(0, &PerfTestTask);

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
