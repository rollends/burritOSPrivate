#include "user/display/DigitalClockDisplay.h"
#include "user/services/services.h"

U16 minutes;
U16 seconds;
U16 millis;

void DigitalClockDisplay(void)
{
    TaskID  clock   = nsWhoIs(Clock);

    minutes = 0;
    seconds = 0;
    millis  = 0;

    for(;;)
    {
        clockDelayBy(clock, 10);
        
        millis += 1;
        seconds += (millis / 10);
        minutes += (seconds / 60);

        millis = millis % 10;
        seconds = seconds % 60;
        
        printf("\033[s\033[1;8H\033[33m|%2d:%2d.%1d]\033[m\033[u", 
                minutes,
                seconds,
                millis);
    }
}
