#include "common/queue.h"
#include "user/messageTypes.h"
#include "user/servers/TrainDriver.h"
#include "user/services/services.h"

void TrainSwitchCourier(void)
{
    MessageEnvelope env;
    env.type = MESSAGE_COURIER;

    TaskID  parent  = { sysPid() },
            train   = nsWhoIs(Train),
            clock   = nsWhoIs(Clock);
    for(;;)
    {
        env.type = MESSAGE_COURIER;
        sysSend(parent.value, &env, &env);
        sysSend(train.value, &env, &env);
        clockDelayBy(clock, 16);
    }
}

void TrainSwitchServer(void)
{
    nsRegister(TrainSwitches);
    
    TaskID  courier = { sysCreate(2, &TrainSwitchCourier) };
    
    TaskID rcvId;
    MessageEnvelope rcvEnv;

    
    U16 qMsgBacking[64];
    QueueU16 qMsg;

    queueU16Init(&qMsg, qMsgBacking, 64);

    U8 isCourierWaiting = 0;
    U8 turningOffSolenoid = 0;
    U16 command;
    
    for(;;)
    {
        sysReceive(&rcvId.value, &rcvEnv);
        switch(rcvEnv.type)
        {
        case MESSAGE_COURIER:
        {
            if(OK == queueU16Pop(&qMsg, &command))
            {
                rcvEnv.type = command >> 8;
                rcvEnv.message.MessageU8.body = command & 0xFF;
                sysReply(rcvId.value, &rcvEnv);

                turningOffSolenoid = 0;
                isCourierWaiting = 0;
            }
            else if(turningOffSolenoid)
            {
                isCourierWaiting = 1;
                turningOffSolenoid = 0;
            }
            else
            {
                rcvEnv.type = MESSAGE_TRAIN_SOLENOID_OFF;
                sysReply(rcvId.value, &rcvEnv);
                isCourierWaiting = 0;
                turningOffSolenoid = 1;
            }
            break;
        }

        case MESSAGE_TRAIN_SWITCH_STRAIGHT:
        case MESSAGE_TRAIN_SWITCH_CURVED:
        {
            if(isCourierWaiting)
            {
                sysReply(courier.value, &rcvEnv);
                isCourierWaiting = 0;
            }
            else
            {
                assertOk(queueU16Push(&qMsg, 
                    (rcvEnv.type << 8) | (rcvEnv.message.MessageU8.body)));
            }
            sysReply(rcvId.value, &rcvEnv);
            break;
        }

        default:
            break;
        }
    }
}
