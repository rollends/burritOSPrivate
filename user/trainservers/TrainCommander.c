#include "common/common.h"
#include "user/messageTypes.h"
#include "user/services/nameService.h"
#include "user/trainservers/TrainCommander.h"

S32 pushTrainCommand(String string)
{
    MessageEnvelope env;
    env.type = 0;
    env.message.MessageArbitrary.body = (U32*)string;
    sysSend(nsWhoIs(TrainCommander).value, &env, &env);
    if(env.type == MESSAGE_FAILURE)
        return -1;
    return 0;
}

S32 pollTrainCommand(U8 trainId, MessageEnvelope* env)
{
    env->type = 1;
    env->message.MessageU8.body = trainId;
    sysSend(nsWhoIs(TrainCommander).value, env, env);
    return OK;
}

static U8 parseTrainCommand(String string, U8* type, U32* info)
{
    ConstString cstring = (ConstString)string;

    if( cstring[0] == 't' && cstring[1] == 'r' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 train = stratoui(&cstring);
        strskipws(&cstring);
        U8 speed = stratoui(&cstring);
        
        *type = MESSAGE_TRAIN_SET_SPEED;
        *info = speed;
        return train;
    }
    else if( cstring[0] == 'r' && cstring[1] == 'v' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 train = stratoui(&cstring);
     
        *type = MESSAGE_TRAIN_REVERSE;
        return train;
    }
    else if( cstring[0] == 's' && cstring[1] == 'a' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 train = stratoui(&cstring);
        strskipws(&cstring);
        U16 group = *(cstring++) - 'a';
        U16 id = stratoui(&cstring);
        *type = MESSAGE_TRAIN_STOP;
        *info = group * 16 + (id-1);
        return train;
    }
    else if( cstring[0] == 'd' && cstring[1] == 'a' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 train = stratoui(&cstring);
        strskipws(&cstring);
        U8 speed = stratoui(&cstring);
        
        *type = MESSAGE_TRAIN_DUMP_ACCEL;
        *info = speed;
        return train;
    }
    else if (cstring[0] == 'd' && cstring[1] == 'v' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 train = stratoui(&cstring);
     
        *type = MESSAGE_TRAIN_DUMP_VEL;
        return train; 
    }

    return 0xFF;
}

void TrainCommanderServer(void)
{
    nsRegister(TrainCommander);

    U16 trainWaiting[80];
    U16 i = 0;
    for(i = 0; i < 80; i++)
        trainWaiting[i] = 0xFFFF;

    for(;;)
    {
        TaskID from;
        MessageEnvelope env;
        sysReceive(&from.value, &env);
        switch(env.type)
        {
        case 0:
        {
            // Command being sent to us!
            U8 trainId = parseTrainCommand((String)env.message.MessageArbitrary.body, &env.type, &env.message.MessageU32.body);
            if(trainId < 80)
            {
                if(trainWaiting[trainId] != 0xFFFF)
                {
                    sysReply(trainWaiting[trainId], &env);
                    trainWaiting[trainId] = 0xFFFF;
                }
                else
                    env.type = MESSAGE_FAILURE;  
            }
            else 
                env.type = MESSAGE_FAILURE;  
            sysReply(from.value, &env);
            break;
        }

        case 1:
        {
            // Train operator asking for command (block until one comes!)
            trainWaiting[env.message.MessageU8.body] = from.value;
            break;
        }
        }
    }
}

