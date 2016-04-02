#include "common/common.h"
#include "kernel/kernel.h"

#include "user/display/TacoDisplay.h"
#include "user/services/services.h"
#include "user/messageTypes.h"

void HydroDisplayPoll()
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

void HydroDisplay()
{
    MessageEnvelope env;
    TaskID sender;
    TaskID id = VAL_TO_ID(sysCreate(sysPriority(sysTid()), &HydroDisplayPoll));

    U32 seed1 = 0x45df435;
    U32 seed2 = 0x3246da;

    U8 index = 0;
    for(;;)
    {
        sysReceive(&sender.value, &env);
        sysReply(sender.value, &env);


        if (sender.value == id.value)
        {   
            if (index != 0)
            {
                U32 randVal = (nextRandU32(&seed1) % 100) + 5950;

                printf("\033[s\033[%d;36H   %2d.%2d Hertz (at 120 volts)\033[u", 9+index, randVal/100, randVal%100);

                randVal = (nextRandU32(&seed2) % 20);
                printf("\033[s\033[%d;36H   Error bound: 0.%2d\033[u", 10+index, randVal);
            }
        }
        else
        {
            if (env.type == MESSAGE_NOTIFY)
            {
                index = env.message.MessageU8.body;
                if (index != 0)
                {
                    printf("\033[s\033[%d;36H\033[1mWaterloo North Hydro Frequency\033[m\033[u", 8+index);
                }
            }
        }
    }
}


