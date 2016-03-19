#include "user/services/services.h"

#define SENSOR_LIST_COUNT 14

typedef struct
{
    U8 r;
    U8 c;
    char s;
} MapEntry;

static MapEntry sMapping[255];

static U32 decmod(U32 val, U32 mod)
{
    if(val == 0)
    {
        return mod - 1;
    }
    else
    {
        return val - 1;
    }
}

static void updateSensor(U8 val, U8 clear, U8 index)
{
    U8 r = sMapping[val].r + index;
    U8 c = sMapping[val].c + 18;
    char symbol = sMapping[val].s;

    if (clear == 1)
    {
        printf("\033[s\033[%d;%dH.\033[u", r, c);
    }
    else
    {
        printf("\033[s\033[%d;%dH\033[1m%c\033[m\033[u", r, c, symbol);
    }
}

static void updateSensorUi( U8* recentList, U8 recentHead, U8 index )
{
    U8 i;
    U8 lastVal = 0;
    for(i = 3; i < SENSOR_LIST_COUNT; ++i )
    {
        U8 val = recentList[recentHead];
        if( val == 0xFF )
        {
            return;
        }
       
        if (i == 3)
        {
            printf("\033[s\033[%d;4H\033[1m%c%2d\033[m\033[u", i + index + 1, 'A' + (val >> 4), 1 + (val & 0x0F));
            lastVal = val;
        }
        else if (i == 4)
        {
            printf("\033[s\033[%d;4H%c%2d\033[u", i + index + 1, 'A' + (val >> 4), 1 + (val & 0x0F));
            updateSensor(val, 1, index);
            updateSensor(lastVal, 0, index);
        }
        else
        {
            printf("\033[s\033[%d;4H%c%2d\033[u", i + index + 1, 'A' + (val >> 4), 1 + (val & 0x0F));
        }

        recentHead = decmod(recentHead, SENSOR_LIST_COUNT);
    }
}

static void initSensorUi(U8 index)
{
    U8 i;
    for(i = 3; i < SENSOR_LIST_COUNT + 2; ++i )
    {
        if (i == 3)
        {
            printf("\033[s\033[%d;2H|  v  |\033[u", i + index);
        }
        else if (i == SENSOR_LIST_COUNT + 1)
        {
            printf("\033[s\033[%d;2H|  ^  |\033[u", i + index);
        }
        else
        {
            printf("\033[s\033[%d;2H|     |\033[u", i + index);
        }
    }

    printf("\033[s\033[%d;18H______.______._____________________.___\033[u",          4 + index);
    printf("\033[s\033[%d;18H_____._/ /___.________.___.________.___\\\033[u",        5 + index);
    printf("\033[s\033[%d;18H  __._/ //           \\     /           \\\\.\033[u",    6 + index);
    printf("\033[s\033[%d;18H       //            .\\ | /.           .\\\\\033[u",    7 + index);
    printf("\033[s\033[%d;18H       |.             .\\|/.              |\033[u",      8 + index);
    printf("\033[s\033[%d;18H       |                |                |\033[u",       9 + index);
    printf("\033[s\033[%d;18H       |.             ./|\\.              |\033[u",      10 + index);
    printf("\033[s\033[%d;18H  _.__ \\\\            ./ | \\.           .//\033[u",    11 + index);
    printf("\033[s\033[%d;18H_.__._\\ \\\\___._______/.___.\\_______.___//.\033[u",   12 + index);
    printf("\033[s\033[%d;18H_.___._\\ \\___.____.___________.____.___/\033[u",       13 + index);
    printf("\033[s\033[%d;18H_.____._\\______._\\_____________/_.__________\033[u",   14 + index);
}

static void initMapping()
{
    sMapping[0x00].r = 4;
    sMapping[0x00].c = 6;
    sMapping[0x00].s = '>';

    sMapping[0x01].r = 4;
    sMapping[0x01].c = 6;
    sMapping[0x01].s = '<';

    sMapping[0x02].r = 8;
    sMapping[0x02].c = 8;
    sMapping[0x02].s = '^';

    sMapping[0x03].r = 8;
    sMapping[0x03].c = 8;
    sMapping[0x03].s = 'v';

    sMapping[0x04].r = 14;
    sMapping[0x04].c = 6;
    sMapping[0x04].s = '>';

    sMapping[0x05].r = 14;
    sMapping[0x05].c = 6;
    sMapping[0x05].s = '<';

    sMapping[0x06].r = 13;
    sMapping[0x06].c = 5;
    sMapping[0x06].s = '<';

    sMapping[0x07].r = 13;
    sMapping[0x07].c = 5;
    sMapping[0x07].s = '>';

    sMapping[0x08].r = 12;
    sMapping[0x08].c = 4;
    sMapping[0x08].s = '<';

    sMapping[0x09].r = 12;
    sMapping[0x09].c = 4;
    sMapping[0x09].s = '>';

    sMapping[0x0A].r = 11;
    sMapping[0x0A].c = 3;
    sMapping[0x0A].s = '>';

    sMapping[0x0B].r = 11;
    sMapping[0x0B].c = 3;
    sMapping[0x0B].s = '<';

    sMapping[0x0C].r = 5;
    sMapping[0x0C].c = 5;
    sMapping[0x0C].s = '>';

    sMapping[0x0D].r = 5;
    sMapping[0x0D].c = 5;
    sMapping[0x0D].s = '<';

    sMapping[0x0E].r = 6;
    sMapping[0x0E].c = 4;
    sMapping[0x0E].s = '<';

    sMapping[0x0F].r = 6;
    sMapping[0x0F].c = 4;
    sMapping[0x0F].s = '>';

    sMapping[0x10].r = 12;
    sMapping[0x10].c = 22;
    sMapping[0x10].s = '>';
    
    sMapping[0x11].r = 12;
    sMapping[0x11].c = 22;
    sMapping[0x11].s = '<';

    sMapping[0x12].r = 11;
    sMapping[0x12].c = 21;
    sMapping[0x12].s = '^';
    
    sMapping[0x13].r = 11;
    sMapping[0x13].c = 21;
    sMapping[0x13].s = 'v';

    sMapping[0x14].r = 5;
    sMapping[0x14].c = 22;
    sMapping[0x14].s = '>';
    
    sMapping[0x15].r = 5;
    sMapping[0x15].c = 22;
    sMapping[0x15].s = '<';

    sMapping[0x16].r = 12;
    sMapping[0x16].c = 1;
    sMapping[0x16].s = '>';
    
    sMapping[0x17].r = 12;
    sMapping[0x17].c = 1;
    sMapping[0x17].s = '<';

    sMapping[0x18].r = 14;
    sMapping[0x18].c = 1;
    sMapping[0x18].s = '>';

    sMapping[0x19].r = 14;
    sMapping[0x19].c = 1;
    sMapping[0x19].s = '<';

    sMapping[0x1A].r = 13;
    sMapping[0x1A].c = 1;
    sMapping[0x1A].s = '>';

    sMapping[0x1B].r = 13;
    sMapping[0x1B].c = 1;
    sMapping[0x1B].s = '<';

    sMapping[0x1C].r = 10;
    sMapping[0x1C].c = 26;
    sMapping[0x1C].s = '^';

    sMapping[0x1D].r = 10;
    sMapping[0x1D].c = 26;
    sMapping[0x1D].s = 'v';

    sMapping[0x1E].r = 10;
    sMapping[0x1E].c = 8;
    sMapping[0x1E].s = '^';

    sMapping[0x1F].r = 10;
    sMapping[0x1F].c = 8;
    sMapping[0x1F].s = 'v';

    sMapping[0x20].r = 10;
    sMapping[0x20].c = 22;
    sMapping[0x20].s = 'v';

    sMapping[0x21].r = 10;
    sMapping[0x21].c = 22;
    sMapping[0x21].s = '^';

    sMapping[0x22].r = 14;
    sMapping[0x22].c = 33;
    sMapping[0x22].s = '>';

    sMapping[0x23].r = 14;
    sMapping[0x23].c = 33;
    sMapping[0x23].s = '<';

    sMapping[0x24].r = 13;
    sMapping[0x24].c = 13;
    sMapping[0x24].s = '>';

    sMapping[0x25].r = 13;
    sMapping[0x25].c = 13;
    sMapping[0x25].s = '<';

    sMapping[0x26].r = 14;
    sMapping[0x26].c = 15;
    sMapping[0x26].s = '>';

    sMapping[0x27].r = 14;
    sMapping[0x27].c = 15;
    sMapping[0x27].s = '<';

    sMapping[0x28].r = 12;
    sMapping[0x28].c = 13;
    sMapping[0x28].s = '<';

    sMapping[0x29].r = 12;
    sMapping[0x29].c = 13;
    sMapping[0x29].s = '>';

    sMapping[0x2A].r = 5;
    sMapping[0x2A].c = 13;
    sMapping[0x2A].s = '>';

    sMapping[0x2B].r = 5;
    sMapping[0x2B].c = 13;
    sMapping[0x2B].s = '<';

    sMapping[0x2C].r = 4;
    sMapping[0x2C].c = 13;
    sMapping[0x2C].s = '>';

    sMapping[0x2D].r = 4;
    sMapping[0x2D].c = 13;
    sMapping[0x2D].s = '<';

    sMapping[0x2E].r = 13;
    sMapping[0x2E].c = 18;
    sMapping[0x2E].s = '>';

    sMapping[0x2F].r = 13;
    sMapping[0x2F].c = 18;
    sMapping[0x2F].s = '<';


    sMapping[0x30].r = 8;
    sMapping[0x30].c = 26;
    sMapping[0x30].s = 'v';

    sMapping[0x31].r = 8;
    sMapping[0x31].c = 26;
    sMapping[0x31].s = '^';

    sMapping[0x32].r = 5;
    sMapping[0x32].c = 26;
    sMapping[0x32].s = '>';

    sMapping[0x33].r = 5;
    sMapping[0x33].c = 26;
    sMapping[0x33].s = '<';

    sMapping[0x34].r = 7;
    sMapping[0x34].c = 39;
    sMapping[0x34].s = '^';

    sMapping[0x35].r = 7;
    sMapping[0x35].c = 39;
    sMapping[0x35].s = 'v';

    sMapping[0x36].r = 6;
    sMapping[0x36].c = 41;
    sMapping[0x36].s = 'v';

    sMapping[0x37].r = 6;
    sMapping[0x37].c = 41;
    sMapping[0x37].s = '^';

    sMapping[0x38].r = 12;
    sMapping[0x38].c = 41;
    sMapping[0x38].s = 'v';

    sMapping[0x39].r = 12;
    sMapping[0x39].c = 41;
    sMapping[0x39].s = '^';

    sMapping[0x3A].r = 13;
    sMapping[0x3A].c = 30;
    sMapping[0x3A].s = '<';

    sMapping[0x3B].r = 13;
    sMapping[0x3B].c = 30;
    sMapping[0x3B].s = '>';

    sMapping[0x3C].r = 12;
    sMapping[0x3C].c = 26;
    sMapping[0x3C].s = '<';

    sMapping[0x3D].r = 12;
    sMapping[0x3D].c = 26;
    sMapping[0x3D].s = '>';

    sMapping[0x3E].r = 11;
    sMapping[0x3E].c = 27;
    sMapping[0x3E].s = '^';

    sMapping[0x3F].r = 11;
    sMapping[0x3F].c = 27;
    sMapping[0x3F].s = 'v';


    sMapping[0x40].r = 8;
    sMapping[0x40].c = 22;
    sMapping[0x40].s = 'v';

    sMapping[0x41].r = 8;
    sMapping[0x41].c = 22;
    sMapping[0x41].s = '^';

    sMapping[0x42].r = 7;
    sMapping[0x42].c = 27;
    sMapping[0x42].s = 'v';

    sMapping[0x43].r = 7;
    sMapping[0x43].c = 27;
    sMapping[0x43].s = '^';

    sMapping[0x44].r = 5;
    sMapping[0x44].c = 35;
    sMapping[0x44].s = '>';

    sMapping[0x45].r = 5;
    sMapping[0x45].c = 35;
    sMapping[0x45].s = '<';

    sMapping[0x46].r = 4;
    sMapping[0x46].c = 35;
    sMapping[0x46].s = '>';

    sMapping[0x47].r = 4;
    sMapping[0x47].c = 35;
    sMapping[0x47].s = '<';

    sMapping[0x48].r = 11;
    sMapping[0x48].c = 39;
    sMapping[0x48].s = '^';

    sMapping[0x49].r = 11;
    sMapping[0x49].c = 39;
    sMapping[0x49].s = 'v';

    sMapping[0x4A].r = 13;
    sMapping[0x4A].c = 35;
    sMapping[0x4A].s = '>';

    sMapping[0x4B].r = 13;
    sMapping[0x4B].c = 35;
    sMapping[0x4B].s = '<';

    sMapping[0x4C].r = 12;
    sMapping[0x4C].c = 35;
    sMapping[0x4C].s = '<';

    sMapping[0x4D].r = 12;
    sMapping[0x4D].c = 35;
    sMapping[0x4D].s = '>';

    sMapping[0x4E].r = 7;
    sMapping[0x4E].c = 21;
    sMapping[0x4E].s = '^';

    sMapping[0x4F].r = 7;
    sMapping[0x4F].c = 21;
    sMapping[0x4F].s = 'v';
}

static U8 buffer[SENSOR_LIST_COUNT];
static U8 bufferHead;

void SensorDisplayPoll(void)
{
    TaskID sensors = nsWhoIs(TrainSensors);
    U16 i = 0;
    bufferHead = 0;

    for(i = 0; i < SENSOR_LIST_COUNT; i++) buffer[i] = 0xFF;

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
                    bufferHead = (bufferHead + 1) % SENSOR_LIST_COUNT;
                }
            }
        }
    }
}

void SensorDisplay(void)
{
    sysCreate(sysPriority()-1, &SensorDisplayPoll);

    U8 index = 0;
    initMapping();

    MessageEnvelope env;
    TaskID sender;

    for(;;)
    {
        sysReceive(&sender.value, &env);
        sysReply(sender.value, &env);

        if (env.message.MessageU8.body != index)
        {
            index = env.message.MessageU8.body;
            initSensorUi(index);
        }

        updateSensorUi(buffer, decmod(bufferHead, SENSOR_LIST_COUNT), index); 
    }
}
