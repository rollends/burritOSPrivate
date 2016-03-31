#include "kernel/kernel.h"

#include "user/display/PerformanceDisplay.h"
#include "user/services/services.h"

void PerformanceDisplayPoll()
{
    TaskID clock = nsWhoIs(Clock);
    TaskID parent = VAL_TO_ID(sysPid());
    MessageEnvelope env;

    for(;;)
    {
        sysPerfReset();
        clockDelayBy(clock, 250);
        sysSend(parent.value, &env, &env);
    }
}

void PerformanceDisplay()
{
#ifdef KERNEL_PERF
    TaskID id = VAL_TO_ID(sysCreate(sysPriority()-1, &PerformanceDisplayPoll));

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
                for (i = 0; i < 13; i++)
                {
                    U32 up = sysPerfQueryP(i + roll, ePerfTask);
                    U32 kp = sysPerfQueryP(i + roll, ePerfKernel);
                    U32 tp = sysPerfQueryP(i + roll, ePerfBoth);

                    U32 uu = sysPerfQueryT(i + roll, ePerfTask);
                    U32 ku = sysPerfQueryT(i + roll, ePerfKernel);
                    U32 tu = sysPerfQueryT(i + roll, ePerfBoth);

                    U32 pos = sysPerfQueryPC(i + roll);
             
                    ConstString str = (ConstString)sysName(i + roll);

                    printf("\033[s\033[%d;2H%18s\t%2d.%2d\t%2d.%2d\t%2d.%2d\t%9n\t%9n\t%9n\t%x\033[u",
                                                                       index + 3 + i,
                                                                       str,
                                                                       up / 100, up % 100,
                                                                       kp / 100, kp % 100,
                                                                       tp / 100, tp % 100,
                                                                       uu,
                                                                       ku,
                                                                       tu,
                                                                       pos);
                }

                U32 total = sysPerfQueryT(0, ePerfTotal);
                U32 totalms = total / 1000;
                printf("\033[s\033[%d;2H\033[2K\tTotal run time: %nus ~ %d.%3ds\033[u",
                                                                   index + 17,
                                                                   total,
                                                                   totalms / 1000,
                                                                   totalms % 1000);
                printf("\033[s\033[%d;2H\033[2K\tTotal task count: %d\033[u",
                                                                   index + 18,
                                                                   taskCount);
                roll += 13;
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
                printf("\033[s\033[%d;80H\033[1mPERFORMANCE\033[m\033[u", index);
                printf("\033[s\033[%d;2H\033[7mTaskId\t\t\tTsk %%\tKrnl %%\tTtl %%\tTsk us\t\tKrnl us\t\tTtl us\t\tPC\033[m\033[u", index + 2);

            }
        }

        sysReply(sender.value, &env);
    }
#endif
}

