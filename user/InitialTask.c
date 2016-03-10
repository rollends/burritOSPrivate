#include "common/common.h"
#include "kernel/kernel.h"

#include "user/services/services.h"
#include "user/trains/trains.h"

#include "user/DigitalClock.h"
#include "user/PerformanceTask.h"
#include "user/SensorDisplay.h"
#include "user/Locomotive.h"

#define SwitchCount 22

void InitialTask()
{
    setupUserServices();
    setupTrainServices();

    sysCreate(2, &PerformanceTask);
    sysCreate(4, &DigitalClock);
    sysCreate(4, &SensorDisplay);

    TaskID  clock   = nsWhoIs(Clock),
            train   = nsWhoIs(Train),
            stdio   = nsWhoIs(Terminal);

    trainStop(train);
    trainGo(train);
    clockLongDelayBy(clock, 20);

    {
        MessageEnvelope env;
        env.message.MessageU8.body = 68;
        sysSend(sysCreate(6, &Locomotive), &env, &env);
    }

    for(;;)
    {
        char buffer[256];
        String ibuffer = buffer;
        
        printf("\033[40;1H\033[2K> ");

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
            break; 
        }
        else
        {
            if( dispatchSystemCommand(buffer) != OK )
            {
                // Well Shit?
                printf("\033[41;1H\033[2KThe command you sent could not be completed! Try again later...");
            }
            else
                printf("\033[41;1H\033[2K");
        }
    }
    trainStop(train);
    
    printf("Finished playing with trains... :)\r\n");
    clockLongDelayBy(clock, 3);
    sysShutdown();
}
