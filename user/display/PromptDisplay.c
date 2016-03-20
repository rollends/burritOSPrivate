#include "common/common.h"
#include "kernel/kernel.h"

#include "user/display/DigitalClockDisplay.h"
#include "user/display/IdlePerformanceDisplay.h"
#include "user/display/PerformanceDisplay.h"
#include "user/display/PromptDisplay.h"
#include "user/display/SensorDisplay.h"
#include "user/display/TacoDisplay.h"
#include "user/services/services.h"

static TaskID displayTasks[5];
static S8 slot1;
static S8 slot2;

void DisplayNotifier(void)
{
    TaskID id = nsWhoIs(Clock);
    MessageEnvelope env;

    for (;;)
    {
        clockDelayBy(id, 25);

        if (slot1 >= 0)
        {
            env.message.MessageU8.body = 4;
            sysSend(displayTasks[slot1].value, &env, &env);
        }

        if (slot2 >= 0)
        {
            env.message.MessageU8.body = 27;
            sysSend(displayTasks[slot2].value, &env, &env);
        }
    }
}

void clearSlot(U8 slot)
{
    U8 i;
    U8 offset = 3;

    if (slot == 2)
    {
        offset = 26;
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

    sysCreate(2, &DisplayNotifier);

    slot1 = 0;
    slot2 = -1;

    printf("\033[s\033[3;1H---------------------------------------------------------------------------------------------\033[u");
    printf("\033[s\033[26;1H---------------------------------------------------------------------------------------------\033[u");
    printf("\033[s\033[49;1H---------------------------------------------------------------------------------------------\033[u");

    MessageEnvelope env;
    TaskID sender;
    sysReceive(&sender.value, &env);

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
        else if (buffer[0] == 's' && buffer[1] == 'u')
        {
            const char* str = buffer;
            str += 2;
            strskipws(&str);
            U8 slot = stratoui(&str);
            strskipws(&str);
            U8 target = stratoui(&str);

            if (target > 4)
            {
                printf("\033[2;19H\033[2KInvalid target specified....");
            }

            if (slot == 1)
            {
                if (slot2 == target)
                {
                    slot2 = -1;
                    clearSlot(2);
                }
                slot1 = target;
            }
            else if (slot == 2)
            {
                if (slot1 == target)
                {
                    slot1 = -1;
                    clearSlot(1);
                }
                slot2 = target;
            }
            else
            {
                printf("\033[2;19H\033[2KInvalid UI Slot specified...");
            }

            clearSlot(slot);
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
