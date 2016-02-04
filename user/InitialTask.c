#include "common/common.h"
#include "kernel/kernel.h"

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

    sysCreate(0, &Nameserver, STACK_SIZE_SMALL);
    sysCreate(1, &ClockServer, STACK_SIZE_SMALL);
    sysCreate(0, &PerformanceTask, STACK_SIZE_SMALL);

    TaskID clock;
    nsWhoIs( Clock, &clock );

    printString( "Creating all tasks at time %d\r\n", clockTime( clock ) );
    printString( "(TID)\t(Completed)\t(Delay Ticks)\t(Current Time)\r\n" );

    U8 i = 0;
    for(i = 0; i < 4; ++i)
    {
        sysCreate(3 + i, &DelayTestTask, STACK_SIZE_SMALL);
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