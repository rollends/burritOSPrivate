#include "common/queue.h"
#include "user/messageTypes.h"
#include "user/servers/TrainDriver.h"
#include "user/services/services.h"

static void TrainSensorStartupDelay(void)
{
    MessageEnvelope env;
    clockLongDelayBy(nsWhoIs(Clock), 10);
    env.type = MESSAGE_NOTIFY;
    sysSend(sysPid(), &env, &env);
}

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

    assert(sysPriority() >= 1);
    sysCreate(sysPriority()-1, &TrainSensorCourier);
    TaskID          rcvId;
    MessageEnvelope rcvEnv;
    U32             sensorBuffer[5];
    U8              i;

    U32             *qBodyBuffer[16];
    U16             qBodyFreeBacking[16];
    QueueU16        qBodyFree;
    queueU16Init(&qBodyFree, qBodyFreeBacking, 16);

    for(i = 0; i < 16; i++)
        queueU16Push(&qBodyFree, i);

    U16             qBodyIndexBacking[16];
    U16             qTaskBacking[16];
    QueueU16        qTask;
    QueueU16        qBodyIndex;
    
    queueU16Init(&qTask, qTaskBacking, 16);
    queueU16Init(&qBodyIndex, qBodyIndexBacking, 16);

    for(i = 0; i < 5; i++)
    {
        sensorBuffer[i] = 0;
    }

    U8 startupStage = 1;
    sysCreate(sysPriority() - 1, &TrainSensorStartupDelay);
   
    for(;;)
    {
        sysReceive(&rcvId.value, &rcvEnv);

        switch(rcvEnv.type)
        {
        case MESSAGE_NOTIFY:
            sysReply(rcvId.value, &rcvEnv);
            startupStage = 0;
            break;

        case MESSAGE_COURIER:
        {
            sysReply(rcvId.value, &rcvEnv);
         
            if( startupStage ) 
            {
                continue;
            }

            U32 changed = 0;
            for(i = 0; i < 5; i++)
            {
                U32 val1 = sensorBuffer[i];
                U32 val2 = rcvEnv.message.MessageArbitrary.body[i];
                changed |= ((val1 ^ val2) & ~val1);
                sensorBuffer[i] = val2;
            }
            if( changed )
            {
                U16 task;
                while(queueU16Pop(&qTask, &task) >= OK)
                {
                    U16 index;
                    queueU16Pop(&qBodyIndex, &index);
                    U32* bufOut = qBodyBuffer[index];
                    
                    for(i = 0; i < 5; i++)
                        bufOut[i] = sensorBuffer[i];
                    
                    sysReply(task, &rcvEnv);

                    queueU16Push(&qBodyFree, index);
                }
            }
            break;
        }

        case MESSAGE_TRAIN_GET_SENSOR_ABCDE:
        {
            U16 index;
            queueU16Push(&qTask, rcvId.value);
            queueU16Pop(&qBodyFree, &index);
            qBodyBuffer[index] = rcvEnv.message.MessageArbitrary.body;
            queueU16Push(&qBodyIndex, index);
            break;
        }

        case MESSAGE_TRAIN_GET_SENSOR:
        {
            rcvEnv.message.MessageU16.body = sensorBuffer[rcvEnv.message.MessageU8.body - 1];
            sysReply(rcvId.value, &rcvEnv);
            break;
        }
        }
    }
}
