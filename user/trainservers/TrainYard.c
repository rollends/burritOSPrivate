#include "kernel/kernel.h"
#include "user/messageTypes.h"
#include "user/services/services.h"
#include "user/trainservers/TrainYard.h"
#include "user/trainservers/PlayerLocomotive.h"
#include "user/trainservers/trainservices.h"

void TrainYardServer(void)
{
    U16 train2Id[80];
    U8 i = 0;
    for(i = 0; i < 80; ++i)
        train2Id[i] = 0xFFFF;

    nsRegister(TrainYard);
    for(;;)
    {
        TaskID from;
        MessageEnvelope env;
        sysReceive(&from.value, &env);

        switch(env.type)
        {
        case MESSAGE_NAMESERVER_REGISTER:
        {
            train2Id[env.message.MessageU8.body] = from.value;
            sysReply(from.value, &env);
            break;
        }

        case MESSAGE_NAMESERVER_WHOIS:
        {
            env.message.MessageU16.body = train2Id[env.message.MessageU8.body];
            sysReply(from.value, &env);
            break;
        }

        case MESSAGE_RPS:
        {
            sysSend(sysCreate(7, &PlayerLocomotive), &env, &env);
            sysReply(from.value, &env);
            break;
        }
        }
    }
}

void trainPlayerLaunch(U8 trainId)
{
    MessageEnvelope env;
    env.type = MESSAGE_RPS;
    env.message.MessageU8.body = trainId;
    sysSend(nsWhoIs(TrainYard).value, &env, &env);
}

void trainRegister(U8 trainId)
{
    MessageEnvelope env;
    env.type = MESSAGE_NAMESERVER_REGISTER;
    env.message.MessageU8.body = trainId;
    sysSend(nsWhoIs(TrainYard).value, &env, &env);
}

S32 trainWhoIs(U8 trainId, TaskID* rid)
{
    MessageEnvelope env;
    env.type = MESSAGE_NAMESERVER_WHOIS;
    env.message.MessageU8.body = trainId;
    sysSend(nsWhoIs(TrainYard).value, &env, &env);
    
    U16 val = env.message.MessageU16.body;
 
    if(val == 0xFFFF)
        return -1;

    rid->value = val;
    return 0;
}
