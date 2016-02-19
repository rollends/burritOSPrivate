#include "common/queue.h"
#include "user/messageTypes.h"
#include "user/servers/TrainDriver.h"
#include "user/services/services.h"

static void TrainSensorCourier(void)
{
    MessageEnvelope env;

    TaskID  parent  = { sysPid() },
            train   = nsWhoIs(Train);
    for(;;)
    {
        env.type = MESSAGE_TRAIN_GET_SENSOR_ABCDE;
        env.message.MessageU32.body = 0;
        sysSend(train.value, &env, &env);
       
        env.type = MESSAGE_COURIER;
        sysSend(parent.value, &env, &env);
    }
}

void TrainSensorServer(void)
{
    nsRegister(TrainSensors);

    sysCreate(2, &TrainSensorCourier); 
    TaskID          rcvId;
    MessageEnvelope rcvEnv;
    U16             sensorBuffer[5];
    U8              i;

    for(i = 0; i < 5; i++)
    {
        sensorBuffer[i] = 0;
    }

    for(;;)
    {
        sysReceive(&rcvId.value, &rcvEnv);

        switch(rcvEnv.type)
        {
        case MESSAGE_COURIER:
        {
            for(i = 0; i < 5; i++)
                sensorBuffer[i] = rcvEnv.message.MessageArbitrary.body[i];
            break;
        }

        case MESSAGE_TRAIN_GET_SENSOR:
        {
            rcvEnv.message.MessageU16.body = sensorBuffer[rcvEnv.message.MessageU8.body - 1];
            break;
        }
        }
        sysReply(rcvId.value, &rcvEnv);
    }
}
