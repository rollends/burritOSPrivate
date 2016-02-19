#include "user/services/services.h"

void DigitalClock(void)
{
    TaskID  clock   = nsWhoIs(Clock),
            output  = nsWhoIs(TerminalOutput);
    U16     minutes = 0;
    U16     seconds = 0;
    U16     millis  = 0;
    for(;;)
    {
        clockDelayBy(clock, 10);
        
        millis += 100;
        seconds += (millis / 1000);
        minutes += (seconds / 60);

        millis = millis % 1000;
        seconds = seconds % 60;
        
        printf( output,
                "\033[s\033[H%2d:%2d.%1d\033[u", 
                minutes,
                seconds,
                millis / 100 );
    }
}
