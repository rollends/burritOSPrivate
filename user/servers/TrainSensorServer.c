#include "common/queue.h"
#include "user/messageTypes.h"
#include "user/servers/TrainDriver.h"
#include "user/services/services.h"

static void TrainSensorCourier(void)
{
    MessageEnvelope env;
    env.type = MESSAGE_COURIER;

    TaskID  parent  = { sysPid() },
            train   = nsWhoIs(Train);
    for(;;)
    {
        env.type = MESSAGE_COURIER;
        sysSend(parent.value, &env, &env);
        sysSend(train.value, &env, &env);
    }
}

void TrainSensorServer(void)
{
    nsRegister(TrainSensors);

    TaskID courier = { sysCreate(2, &TrainSensorCourier) };
    U8 isCourierWaiting = 0;

    U16 qTaskBacking[16];
    U8 qRequestBacking[16];
    QueueU16 qTask;
    QueueU8 qRequest;

    queueU16Init(&qTask, qTaskBacking, 16);
    queueU8Init(&qRequest, qRequestBacking, 16);

    MessageEnvelope rcvEnv;
    TaskID rcvId;
    
    U16 activeTask = 0; 
    U8 activeRequest = 0;
    for(;;)
    {
        sysReceive(&rcvId.value, &rcvEnv);

        switch(rcvEnv.type)
        {
        case MESSAGE_COURIER:
        {
            if(activeTask)
            {
                // Forward result to requesting task.
                sysReply(activeTask, &rcvEnv);
                activeTask = 0;
            }

            if(OK == queueU16Pop(&qTask, &activeTask))
            {
                assertOk(queueU8Pop(&qRequest, &activeRequest));
                assert(courier.value == rcvId.value);

                rcvEnv.type = MESSAGE_TRAIN_GET_SENSOR + activeRequest;
                sysReply(courier.value, &rcvEnv);
            }
            else
            {
                isCourierWaiting = 1;
            }
            break;
        }

        case MESSAGE_TRAIN_GET_SENSOR:
        {
            // We assume the idle task doesn't send anything!
            assert(rcvId.value != 0);

            if(isCourierWaiting)
            {
                rcvEnv.type = MESSAGE_TRAIN_GET_SENSOR + rcvEnv.message.MessageU8.body;
                sysReply(courier.value, &rcvEnv);
                activeTask = rcvId.value;
                activeRequest = rcvEnv.message.MessageU8.body;
                isCourierWaiting = 0;
            }
            else
            {
                assertOk(queueU16Push(&qTask, rcvId.value));
                assertOk(queueU8Push(&qRequest, rcvEnv.message.MessageU8.body));
            }           
            break;
        }
        }
    }
}
