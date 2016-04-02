#include "kernel/kernel.h"

#include "user/display/StackDisplay.h"
#include "user/services/services.h"
#include "user/messageTypes.h"

void StackDisplayPoll()
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

void StackDisplay()
{
#ifdef KERNEL_PERF
    TaskID id = VAL_TO_ID(sysCreate(sysPriority(sysTid()), &StackDisplayPoll));

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
                U32 i;
                for (i = 0; i < 18; i++)
                {
                    U32 sp = sysPerfQuerySP(i + roll);
                    U32 sb = sysPerfQuerySB(i + roll);
                    U32 usage = sb - sp;

                    printf("\033[s\033[%d;2H%18s\t%2d\t%x\t%x\t%b\t%d       \033[u",
                             index + 3 + i,
                             (ConstString)sysName(i + roll),
                             i + roll,
                             sp,
                             sb,
                             sysPerfQuerySI(i + roll),
                             usage);
                }
            }
        }
        else
        {
            if (env.type == MESSAGE_NOTIFY)
            {
                index = env.message.MessageU8.body;
                roll = 0;
                if (index != 0)
                {
                    printf("\033[s\033[%d;80H\033[1mSTACK STATE\033[m\033[u", index);
                    printf("\033[s\033[%d;2H\033[7mName\t\t\tTaskID\tSP\t\tSPBase\t\tBlock\tUsage\033[m\033[u", index + 2);
                }
            }
            else if (env.type == MESSAGE_RANDOM_BYTE)
            {
                U8 command = env.message.MessageU8.body;
                if (command == 10)
                {
                    if (roll > 0)
                        roll--;
                }
                else if (command == 11)
                {
                    if (roll < 46)
                        roll++;
                }
            }
        }

        sysReply(sender.value, &env);
    }
#endif
}

