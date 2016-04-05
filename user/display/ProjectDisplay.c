#include "common/common.h"
#include "kernel/kernel.h"

#include "user/display/ProjectDisplay.h"
#include "user/services/services.h"
#include "user/trainservers/trainservices.h"
#include "user/messageTypes.h"

#define PROJECT_SENSOR_COUNT    10

static void displaySpeed(U8 speed, U8 index)
{
    printf("\033[s\033[%d;20HSpeed: %2d\033[u\n",
        index + PROJECT_SENSOR_COUNT + 4, speed);
}

static void displaySwitch(U8 index, U8 direction)
{
    if (direction == 0)
    {
        printf("\033[s\033[%d;20H\033[2KSwitch Straight\033[u\n",
            index + PROJECT_SENSOR_COUNT + 5);
    }
    else
    {
        printf("\033[s\033[%d;20H\033[2KSwitch Curved\033[u\n",
            index + PROJECT_SENSOR_COUNT + 5);   
    }
}

static U8 sensorNodes[PROJECT_SENSOR_COUNT];

static void ProjectDisplayPoll(void)
{
    TaskID sensors = nsWhoIs(TrainSensors);
    TaskID attrib = nsWhoIs(NodeAttribution);
    TaskID parent = VAL_TO_ID(sysPid());

    U16 i = 0;
    MessageEnvelope env;
    MessageEnvelope att;

    for(;;)
    {
        U32 sensorValues[5];
        trainReadAllSensors(sensors, sensorValues);
        for(i = 1; i <= 5; ++i)
        {
            // Read all sensors
            U8 c = 0;
            U16 group = (U16)sensorValues[i-1];
            for(c = 0; c < 16; ++c)
            {
                if((1 << (15-c)) & group)
                {
                    att.message.MessageU8.body = (i-1)*16 + c;
                    sysSend(attrib.value, &att, &att);
                    U8 ownerValue = att.message.MessageU8.body;
                    
                    if (ownerValue == 70)
                    {
                        U8 j = 0;
                        for(j = 0; j < PROJECT_SENSOR_COUNT; j++)
                        {
                            if(sensorNodes[j] == (i-1)*16 + c)
                            {
                                env.message.MessageU8.body = j;
                                sysSend(parent.value, &env, &env);
                            }
                        }
                    }
                }
            }
        }
    }
}

void ProjectDisplay()
{
    MessageEnvelope env;
    TaskID sender;
    TaskID trainId;

    U8 index = 0;
    U8 speed = 0;
    U8 sensorsHit = PROJECT_SENSOR_COUNT;
    U32 randSeed = 0x1e4f8;
    
    memset(sensorNodes, 0xFF, sizeof(U8)*PROJECT_SENSOR_COUNT);

    TaskID poller = VAL_TO_ID(sysCreate(sysPriority(sysTid()) - 1, &ProjectDisplayPoll));
    for(;;)
    {
        sysReceive(&sender.value, &env);

        if (sender.value == poller.value && sensorsHit > 0)
        {
            U8 ind = env.message.MessageU8.body;
            sensorNodes[ind] = 0xFF;
            printf("\033[s\033[%d;20H| OK  |\033[u",
                    ind + index + 3);
            sensorsHit--;

            if (sensorsHit == 0)
            {
                printf("\033[s\033[%d;40H\033[1mYou won!!\033[m\033[u", index+10);
            }
        }
        else if (env.type == MESSAGE_NOTIFY)
        {
            index = env.message.MessageU8.body;
            if (index != 0)
            {
                printf("\033[s\033[%d;80H\033[1mCONNECT THE SENSORS\033[m\033[u", index);

                U8 i;
                for (i = 0; i < PROJECT_SENSOR_COUNT; i++)
                {
                    do
                    {
                        sensorNodes[i] = nextRandU32(&randSeed) % 80;
                    } while (sensorNodes[i] == 0x00 || sensorNodes[i] == 0x01 || sensorNodes[i] == 0x04 ||
                             sensorNodes[i] == 0x05 || sensorNodes[i] == 0x06 || sensorNodes[i] == 0x07 ||
                             sensorNodes[i] == 0x08 || sensorNodes[i] == 0x09 || sensorNodes[i] == 0x0A ||
                             sensorNodes[i] == 0x0B || sensorNodes[i] == 0x0C || sensorNodes[i] == 0x0D ||
                             sensorNodes[i] == 0x0E || sensorNodes[i] == 0x0F || sensorNodes[i] == 0x16 ||
                             sensorNodes[i] == 0x17 || sensorNodes[i] == 0x18 || sensorNodes[i] == 0x19 ||
                             sensorNodes[i] == 0x1A || sensorNodes[i] == 0x1B || sensorNodes[i] == 0x22 ||
                             sensorNodes[i] == 0x23 || sensorNodes[i] == 0x26 || sensorNodes[i] == 0x27);
 
                    printf("\033[s\033[%d;20H| \033[1m%c%2d\033[m |\033[u",
                           i + index + 3,
                           sensorNodes[i] / 16 + 'A',
                           sensorNodes[i] % 16 + 1);
                }
                sensorsHit = PROJECT_SENSOR_COUNT;

                displaySpeed(speed, index);
                assertOk(trainWhoIs(70, &trainId));
            }
        }
        else if (env.type == MESSAGE_RANDOM_BYTE)
        {
            U8 command = env.message.MessageU8.body;
            MessageEnvelope env;
            env.type = MESSAGE_TRAIN_SET_SPEED;

            if (command == 11)
            {
                if (speed > 5)
                {
                    speed--;
                }
                else if (speed == 5)
                {
                    speed = 0;
                }
                else if (speed == 0)
                {
                    env.type = MESSAGE_TRAIN_REVERSE;
                }
            }
            else if (command == 10)
            {
                if (speed == 0)
                {
                    speed = 5;
                }
                else if (speed < 13)
                {
                    speed++;
                }
            }

            if (command == 12 || command == 13)
            {
                displaySwitch(index, command == 12 ? 0 : 1);
                env.type = (command == 12 ? MESSAGE_TRAIN_SWITCH_STRAIGHT : MESSAGE_TRAIN_SWITCH_CURVED); 
                sysSend(trainId.value, &env, &env);
            }
            else
            {
                env.message.MessageU8.body = speed;
                sysSend(trainId.value, &env, &env);
                displaySpeed(speed, index);
            }
        }

        sysReply(sender.value, &env);
    }
}

