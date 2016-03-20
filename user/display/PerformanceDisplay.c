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
        clockDelayBy(clock, 200);
        sysSend(parent.value, &env, &env);
    }
}

void PerformanceDisplay()
{
#ifdef KERNEL_PERF
    TaskID id = VAL_TO_ID(sysCreate(sysPriority()-1, &PerformanceDisplayPoll));

    U8 index = 0;

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
                printf("\033[s\033[%d;2H\tTaskId\t\tTsk %%\tKrnl %%\tTtl %%\tTsk us\t\tKrnl us\t\tTtl us\033[u", index + 2);

                for (i = 0; i <= 12; i++)
                {
                    U32 up = sysPerfQueryP(i, ePerfTask);
                    U32 kp = sysPerfQueryP(i, ePerfKernel);
                    U32 tp = sysPerfQueryP(i, ePerfBoth);

                    U32 uu = sysPerfQueryT(i, ePerfTask);
                    U32 ku = sysPerfQueryT(i, ePerfKernel);
                    U32 tu = sysPerfQueryT(i, ePerfBoth);
             
                    ConstString str = (ConstString)sysName(i);

                    printf("\033[s\033[%d;2H\t%14s\t%2d.%2d\t%2d.%2d\t%2d.%2d\t%9n\t%9n\t%9n\033[u",
                                                                       index + 3 + i,
                                                                       str,
                                                                       up / 100, up % 100,
                                                                       kp / 100, kp % 100,
                                                                       tp / 100, tp % 100,
                                                                       uu,
                                                                       ku,
                                                                       tu);
                }

                U32 total = sysPerfQueryT(0, ePerfTotal);
                U32 totalms = total / 1000;
                printf("\033[s\033[%d;2H\033[2K\tTotal run time: %nus ~ %d.%3ds\033[u",
                                                                   index + 17,
                                                                   total,
                                                                   totalms / 1000,
                                                                   totalms % 1000);
            }
        }
        else
        {
            index = env.message.MessageU8.body;
        }

        sysReply(sender.value, &env);
    }
#endif
}

