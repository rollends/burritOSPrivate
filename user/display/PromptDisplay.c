#include "common/common.h"
#include "kernel/kernel.h"

#include "user/display/DigitalClockDisplay.h"
#include "user/display/HydroDisplay.h"
#include "user/display/IdlePerformanceDisplay.h"
#include "user/display/LogDisplay.h"
#include "user/display/OwnerDisplay.h"
#include "user/display/PerformanceDisplay.h"
#include "user/display/ProcessDisplay.h"
#include "user/display/ProjectDisplay.h"
#include "user/display/PromptDisplay.h"
#include "user/display/SensorDisplay.h"
#include "user/display/StackDisplay.h"
#include "user/display/SwitchDisplay.h"
#include "user/display/TacoDisplay.h"
#include "user/services/services.h"
#include "user/trainservers/trainservices.h"
#include "user/messageTypes.h"

#define DISPLAY_TASK_COUNT 10

static TaskID displayTasks[DISPLAY_TASK_COUNT];
static U8 index[2];
static S8 slots[2];

void updateSlot(U8 slot)
{
    MessageEnvelope env;

    if (slots[slot] >= 0)
    {
        env.message.MessageU8.body = index[slot] + 1;
        env.type = MESSAGE_NOTIFY;
        sysSend(displayTasks[slots[slot]].value, &env, &env);
    }
}

void commandSlot(U8 slot, U8 command)
{
    MessageEnvelope env;

    if (slots[slot] >= 0)
    {
        env.message.MessageU8.body = command;
        env.type = MESSAGE_RANDOM_BYTE;
        sysSend(displayTasks[slots[slot]].value, &env, &env);
    }   
}

void clearSlot(U8 slot)
{
    U8 i;
    U8 offset = index[slot];

    MessageEnvelope env;
    env.message.MessageU8.body = 0;
    env.type = MESSAGE_NOTIFY;

    if (slots[slot] >= 0)
    {
        sysSend(displayTasks[slots[slot]].value, &env, &env);
    }

    for (i = 1; i < 23; i++)
    {
        printf("\033[s\033[%d;1H\033[2K\033[u", i + offset);
    }
}

void PromptDisplay(void)
{
    sysCreate(2, &IdlePerformanceDisplay);
    sysCreate(4, &DigitalClockDisplay);
    
    displayTasks[0].value = sysCreate(10, &SensorDisplay);
    displayTasks[1].value = sysCreate(10, &SwitchDisplay);
    displayTasks[2].value = sysCreate(10, &OwnerDisplay);
    displayTasks[3].value = sysCreate(5,  &PerformanceDisplay);
    displayTasks[4].value = sysCreate(0,  &ProcessDisplay);
    displayTasks[5].value = sysCreate(10, &LogDisplay);
    displayTasks[6].value = sysCreate(10, &StackDisplay);
    displayTasks[7].value = sysCreate(10, &ProjectDisplay);
    displayTasks[8].value = sysCreate(20, &TacoDisplay);
    displayTasks[9].value = sysCreate(20, &HydroDisplay);
    
    TaskID stdio = nsWhoIs(Terminal);

    index[0] = 3;
    index[1] = 26;

    slots[0] = 4;
    slots[1] = 0;

    U8 focus = 0;

    MessageEnvelope env;
    TaskID sender;
    sysReceive(&sender.value, &env);

    printf("\033[s\033[3;1H---------------------------------------------------------------------------------------------\033[u");
    printf("\033[s\033[26;1H---------------------------------------------------------------------------------------------\033[u");
    printf("\033[s\033[49;1H---------------------------------------------------------------------------------------------\033[u");

    updateSlot(0);
    updateSlot(1);
    for(;;)
    {
        char buffer[256];
        String ibuffer = buffer;
        U8 sequence = 0;
        
        printf("\033[1;18H\033[K> ");

        // Fill our buffer til carriage return
        while( '\r' != (*ibuffer = getc(stdio)) )
        {
            if(*ibuffer == '\b')
            {   
                if( ibuffer == buffer ) continue;
                printf("\x1B[1D \x1B[1D");
                --ibuffer;
            }
            else
            {
                if (*ibuffer == 0x5b)
                {
                    sequence = 1;
                }
                else if (*ibuffer == 0x41 && sequence == 1)
                {
                    sequence = 10;
                    break;
                }
                else if (*ibuffer == 0x42 && sequence == 1)
                {
                    sequence = 11;
                    break;
                }
                else if (*ibuffer != 0x1b)
                {
                    printf("%c", *ibuffer);
                }

                ++ibuffer;
            }
        }

        if (sequence > 1)
        {
            commandSlot(focus, sequence);
            continue;
        }

        *ibuffer = '\0';
        printf("\r\n");

        if( buffer[0] == 'q' ) 
        {
            printf("Finished playing with trains... :)\r\n");
            sysReply(sender.value, &env);
            break; 
        }
        else if (buffer[0] == 'k')
        {
            sysKill();
        }
        else if (buffer[0] == 'l' && buffer[1] == 'g' )
        {
            const char* str = buffer;
            str += 2;
            strskipws(&str);
            U8 train = stratoui(&str);
            trainPlayerLaunch(train);
        }
        else if( buffer[0] == 'l' && buffer[1] == 'a' )
        {
            const char* str = buffer;
            str += 2;
            strskipws(&str);
            U8 train = stratoui(&str);
            trainLaunch(train);
        }
        else if (buffer[0] == 'f' && buffer[1] == 'u')
        {
            const char* str = buffer;
            str += 2;
            strskipws(&str);
            U8 slot = stratoui(&str);

            if (slot <= 1)
            {
                focus = slot;
            }
            else
            {
                printf("\033[2;19H\033[2KInvalid slot specified....");
            }
        }
        else if (buffer[0] == 's' && buffer[1] == 'u')
        {
            const char* str = buffer;
            str += 2;
            strskipws(&str);
            U8 slot = stratoui(&str);
            strskipws(&str);
            U8 display = stratoui(&str);

            if (display >= DISPLAY_TASK_COUNT)
            {
                printf("\033[2;19H\033[2KInvalid display specified....");
                continue;
            }

            if (slot > 1)
            {
                printf("\033[2;19H\033[2KInvalid slot specified....");
                continue;
            }

            clearSlot(slot);

            U8 other = (slot + 1) % 2;
            if (slots[other] == display)
            {
                clearSlot(other);
                slots[other] = -1;
            }

            slots[slot] = display;
            updateSlot(slot);

        }
        else
        {
            if( dispatchSystemCommand(buffer) != OK )
            {
                // Well Shit?
                printf("\033[2;19H\033[2KThe command you sent could not be completed! Try again later...");
            }
            else
            {
                printf("\033[2;19H\033[2K");
            }
        }
    }
}
