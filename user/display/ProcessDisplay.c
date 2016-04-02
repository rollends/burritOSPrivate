#include "kernel/kernel.h"

#include "user/display/ProcessDisplay.h"
#include "user/services/services.h"

void ProcessDisplayPoll()
{
    TaskID clock = nsWhoIs(Clock);
    TaskID parent = VAL_TO_ID(sysPid());
    MessageEnvelope env;

    for(;;)
    {
        clockLongDelayBy(clock, 1);
        sysSend(parent.value, &env, &env);
    }
}

static inline const char* ProcessStateString(U32 state)
{
    switch (state)
    {
        case eZombie:
            return "Z ";
        case eReady:
            return "R ";
        case eActive:
            return "A ";
        case eSendBlocked:
            return "SB";
        case eReceiveBlocked:
            return "RB";
        case eReplyBlocked:
            return "MB";
        case eEventBlocked:
            return "EB";
        default:
            return "NA";
    }
}

void ProcessDisplay()
{
#ifdef KERNEL_PERF
    TaskID id = VAL_TO_ID(sysCreate(sysPriority(sysTid()), &ProcessDisplayPoll));

    U8 index = 0;
    U8 roll = 0;

    MessageEnvelope env;
    TaskID sender;

    for(;;)
    {
        sysReceive(&sender.value, &env);
        if (sender.value == id.value)
        {
            if (index != 0)
            {
                U32 taskCount = sysPerfCount();
                U32 i;
                for (i = 0; i < 18; i++)
                {
                    printf("\033[s\033[%d;2H%18s\t%2d\t%2s\t%2d\t%x\t%x\t%x\033[u",
                             index + 3 + i,
                             (ConstString)sysName(i + roll),
                             i + roll,
                             ProcessStateString(sysState(i + roll)),
                             sysPerfSendCount(i + roll),
                             sysPerfQueryPC(i + roll),
                             sysPerfQueryLR(i + roll),
                             sysPerfQuerySP(i + roll));
                }
                //roll += 13;
                if (roll >= taskCount)
                {
                    roll = 0;
                }

            }
        }
        else
        {
            index = env.message.MessageU8.body;
            if (index != 0)
            {
                printf("\033[s\033[%d;80H\033[1mPROCESS STATE\033[m\033[u", index);
                printf("\033[s\033[%d;2H\033[7mName\t\t\tTaskID\tState\tQueue\t\tPC\t\tLR\t\tSP\033[m\033[u", index + 2);

            }
        }

        sysReply(sender.value, &env);
    }
#endif
}

