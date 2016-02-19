#include "user/services/services.h"

static U32 decmod(U32 val, U32 mod)
{
    if(val == 0)
        return mod - 1;
    else
        return val - 1;
}

void SensorDisplay(void)
{
    TaskID  clock   = nsWhoIs(Clock),
            output  = nsWhoIs(TerminalOutput),
            sensors = nsWhoIs(TrainSensors);
    U8      i       = 0;
    U8      buffer[10];
    U8      bufferHead = 0;

    for(i = 0; i < 10; i++) buffer[i] = 0xFF;

    for(;;)
    {
        clockDelayBy(clock, 10);
        
        for(i = 1; i <= 5; ++i)
        {
            // Read all sensors
            U8 c = 0;
            U16 group = trainReadSensorGroup(sensors, i);
            for(c = 0; c < 16; ++c)
            {
                if((1 << (15-c)) & group)
                {
                    buffer[bufferHead] = ((i-1) << 4) | (c );
                    bufferHead = (bufferHead + 1) % 10;
                }
            }
        }
        U8 bi = bufferHead;
        for(i = 0; i < 10; i++)
        {
            U8 val = buffer[bi];
            
            if(val == 0xFF)
            { 
                break;
            }

            printf(output, "\033[s\033[%d;1H%c%2d\033[u", i + 3, 'A' + (val >> 4), 1 + (val & 0x0F));
            bi = decmod(bi, 10);        
        } 
    }

}
