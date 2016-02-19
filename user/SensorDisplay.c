#include "user/services/services.h"

static U32 decmod(U32 val, U32 mod)
{
    if(val == 0)
        return mod - 1;
    else
        return val - 1;
}

static void updateSensorUi( U8* recentList, U8 recentHead )
{
	U8 i = 0;
	for(; i < 10; ++i )
	{
        U8 val = recentList[recentHead];
        if( val == 0xFF )
            return;
	
        printf("\033[s\033[%d;3H%c%2d\033[u", 9 + i, 'A' + (val >> 4), 1 + (val & 0x0F));
        recentHead = decmod(recentHead, 10);
	}
}

void SensorDisplay(void)
{
    TaskID  sensors = nsWhoIs(TrainSensors);
    U8      i       = 0;
    U8      buffer[10];
    U8      bufferHead = 0;

    printf("\033[s\033[6;1H*----------*\033[u");
    printf("\033[s\033[7;1H|  SENSOR  |\033[u");
	printf("\033[s\033[8;1H|~~~~~~~~~~|\033[u");
	for(i = 0; i <= 21; ++i )
	{
	    printf("\033[s\033[%d;1H|          |\033[u", 9 + i);
	}
	printf("\033[s\033[19;1H|----------|\033[u", 9 + i);
	printf("\033[s\033[31;1H*~~~~~~~~~~*\033[u");

    for(i = 0; i < 10; i++) buffer[i] = 0xFF;
    
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
                    U8 storedValue = ((i-1) << 4) | c;
                    buffer[bufferHead] = storedValue;
                    bufferHead = (bufferHead + 1) % 10;
                }
            }
        }

        updateSensorUi(buffer, decmod(bufferHead, 10)); 
    }

}
