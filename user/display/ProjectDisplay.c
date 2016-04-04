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

void ProjectDisplay()
{
    MessageEnvelope env;
    TaskID sender;
    TaskID trainId;

    U8 index = 0;
    U8 speed = 0;
    U8 sensorNodes[PROJECT_SENSOR_COUNT];
    U32 randSeed = 0x1e4f8;

    for(;;)
    {
        sysReceive(&sender.value, &env);

        if (env.type == MESSAGE_NOTIFY)
        {
            index = env.message.MessageU8.body;
            if (index != 0)
            {
                printf("\033[s\033[%d;80H\033[1mPROJECT DISPLAY\033[m\033[u", index);

                U8 i;
                for (i = 0; i < PROJECT_SENSOR_COUNT; i++)
                {
                    sensorNodes[i] = nextRandU32(&randSeed) % 80;
                    printf("\033[s\033[%d;20H| %c%2d |\033[u",
                           i + index + 3,
                           sensorNodes[i] / 16 + 'A',
                           sensorNodes[i] % 16 + 1);
                }

                displaySpeed(speed, index);
                assertOk(trainWhoIs(69, &trainId));
            }
        }
        else if (env.type == MESSAGE_RANDOM_BYTE)
        {
            U8 command = env.message.MessageU8.body;
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

            MessageEnvelope env;
            env.type = MESSAGE_TRAIN_SET_SPEED;
            env.message.MessageU8.body = speed;
            sysSend(trainId.value, &env, &env);

            displaySpeed(speed, index);
        }

        sysReply(sender.value, &env);
    }
}

