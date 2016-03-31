#include "common/common.h"
#include "kernel/kernel.h"

#include "user/display/PromptDisplay.h"
#include "user/services/services.h"
#include "user/trainservers/trainservers.h"

#include "user/trainservers/Locomotive.h"

#define SwitchCount 22

void InitialTask()
{
    setupUserServices();
    setupTrainServices();

    TaskID prompt = VAL_TO_ID(sysCreate(2, &PromptDisplay));

    TaskID clock   = nsWhoIs(Clock),
           train   = nsWhoIs(Train);

    trainStop(train);
    trainGo(train);
    clockLongDelayBy(clock, 20);

    {
        MessageEnvelope env;
        sysSend(prompt.value, &env, &env);
    }

    trainStop(train);
    clockLongDelayBy(clock, 3);
    sysShutdown();
}

