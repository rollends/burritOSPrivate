#include "common/common.h"
#include "kernel/kernel.h"

#include "user/display/DigitalClockDisplay.h"
#include "user/display/IdlePerformanceDisplay.h"
#include "user/display/PerformanceDisplay.h"
#include "user/display/PromptDisplay.h"
#include "user/display/SensorDisplay.h"
#include "user/display/TacoDisplay.h"
#include "user/services/services.h"
#include "user/trainservers/trainservices.h"

static TaskID displayTasks[5];
static U8 index[2];
static S8 slots[2];

void updateSlot(U8 slot)
{
    MessageEnvelope env;

    if (slots[slot] >= 0)
    {
        env.message.MessageU8.body = index[slot] + 1;
        sysSend(displayTasks[slots[slot]].value, &env, &env);
    }
}

void clearSlot(U8 slot)
{
    U8 i;
    U8 offset = index[slot];

    MessageEnvelope env;
    env.message.MessageU8.body = 0;

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
    TaskID stdio = nsWhoIs(Terminal);

    sysCreate(2, &IdlePerformanceDisplay);
    sysCreate(4, &DigitalClockDisplay);
    displayTasks[0].value = sysCreate(4, &SensorDisplay);
    displayTasks[1].value = sysCreate(4, &PerformanceDisplay);
    displayTasks[4].value = sysCreate(20, &TacoDisplay);

    index[0] = 3;
    index[1] = 26;

    slots[0] = 0;
    slots[1] = -1;

    printf("\033[s\033[3;1H---------------------------------------------------------------------------------------------\033[u");
    printf("\033[s\033[26;1H---------------------------------------------------------------------------------------------\033[u");
    printf("\033[s\033[49;1H---------------------------------------------------------------------------------------------\033[u");

    MessageEnvelope env;
    TaskID sender;
    sysReceive(&sender.value, &env);

    updateSlot(0);
    for(;;)
    {
        char buffer[256];
        String ibuffer = buffer;
        
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
                printf("%c", *ibuffer);
                ++ibuffer;
            }
        }
        
        *ibuffer = '\0';
        printf("\r\n");

        if( buffer[0] == 'q' ) 
        {
            printf("Finished playing with trains... :)\r\n");
            sysReply(sender.value, &env);
            break; 
        }
        else if( buffer[0] == 'l' && (buffer[1] == 'a' || buffer[1] == 'b') )
        {
            U8 train = (buffer[1] == 'a' ? 68 : 64);
            trainLaunch(train);
        }
        else if (buffer[0] == 's' && buffer[1] == 'u')
        {
            const char* str = buffer;
            str += 2;
            strskipws(&str);
            U8 slot = stratoui(&str);
            strskipws(&str);
            U8 display = stratoui(&str);

            if (display > 4)
            {
                printf("\033[2;19H\033[2KInvalid display specified....");
            }

            if (slot > 1)
            {
                printf("\033[2;19H\033[2KInvalid slot specified....");
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
                printf("\033[2;19H\033[2K");
        }
    }
}
