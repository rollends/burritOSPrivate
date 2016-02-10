#include "common/common.h"
#include "kernel/kernel.h"

#include "user/services/services.h"
#include "user/DigitalClock.h"
#include "user/PerformanceTask.h"
#include "user/SensorDisplay.h"

#define SwitchCount 22

void InitialTask()
{
    U16 i = 0;
    setupUserServices();

    sysCreate(2, &PerformanceTask);
    sysCreate(4, &DigitalClock);
    sysCreate(4, &SensorDisplay);


    TaskID  clock   = nsWhoIs(Clock),
            train   = nsWhoIs(Train),
            sSwitch = nsWhoIs(TrainSwitches),
            stdout  = nsWhoIs(TerminalOutput), 
            stdin   = nsWhoIs(TerminalInput);

	U8 switchId2Row[256];
    for(i = 0; i < 256; ++i) { switchId2Row[i] = 255; }
    
    for(i = 1; i <= 18; i++)
		switchId2Row[i] = i - 1;
	switchId2Row[0x99] = 18;
	switchId2Row[0x9A] = 19;
	switchId2Row[0x9B] = 20;
	switchId2Row[0x9C] = 21; 

    trainStop(train);
    trainGo(train);

    for(i = 0; i < 256; i++)
    {
        U8 row = switchId2Row[i];

      if( row >= SwitchCount ) continue;
    
        trainSwitch(sSwitch, i, eCurved);
    }

    for(;;)
    {
        char buffer[256];
        String ibuffer = buffer;
        
        printf(stdout, "\033[20;1H\033[2K> ");

        // Fill our buffer til carriage return
        while( '\r' != (*ibuffer = getc(stdin)) )
        {
            if(*ibuffer == '\b')
            {   
                if( ibuffer == buffer ) continue;
                printf(stdout, "\x1B[1D \x1B[1D");
                --ibuffer;
            }
            else
            {
                putc(stdout, *ibuffer);
                ++ibuffer;
            }
        }
        
        *ibuffer = '\0';

        printf(stdout, "\r\n");

        if( buffer[0] == 'q' ) 
        {
            break; 
        }
        else
        {
            dispatchTrainCommand(buffer);
        }
    }
    trainStop(train);
    
    printf(stdout, "Finished playing with trains... :)\r\n");
    clockLongDelayBy(clock, 3);
    sysShutdown();
}
