#include "user/display/SensorDisplay.h"
#include "user/services/services.h"

#define SENSOR_LIST_COUNT 14

typedef struct
{
    U8 r;
    U8 c;
    char s;
} SensorMapEntry;

static SensorMapEntry sSensorMapping[255];
static U8 lastSensorMapping[80][2];

static U8 sensorColor(U8 owner)
{
    switch (owner)
    {
        case 58:
            return 36;
        case 64:
            return 31;
        case 68:
            return 33;
        case 69:
            return 35;
        case 70:
            return 36;
        default:
            return 37;
    }
}

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

static void updateSensor(U8 val, U8 clear, U8 color, U8 index)
{
    U8 r = sSensorMapping[val].r + index;
    U8 c = sSensorMapping[val].c + 24;
    char symbol = sSensorMapping[val].s;

    if (clear == 1)
    {
        printf("\033[s\033[%d;%dH.\033[u", r, c);
    }
    else
    {
        printf("\033[s\033[%d;%dH\033[%d;1m%c\033[m\033[u", r, c, color, symbol);
    }
}

static void updateSensorUi( U8* recentList, U8* ownerList, U8 recentHead, U8 index )
{
    U8 i;
    for(i = 3; i < SENSOR_LIST_COUNT; ++i )
    {
        U8 val = recentList[recentHead];
        U8 owner = ownerList[recentHead];

        if( val == 0xFF )
        {
            break;
        }
       
        if (i == 3)
        {
            printf("\033[s\033[%d;4H\033[1m%c%2d\033[m\033[u", i + index + 1, 'A' + (val >> 4), 1 + (val & 0x0F));
        }
        else
        {
            printf("\033[s\033[%d;4H%c%2d\033[u", i + index + 1, 'A' + (val >> 4), 1 + (val & 0x0F));
        }

        printf("\033[s\033[%d;10H\033[%dm%2d\033[m\033[u", i + index + 1, sensorColor(owner),
                owner);
        recentHead = decmod(recentHead, SENSOR_LIST_COUNT);
    }

    for (i = 50; i <= 75; i++)
    {
        if (lastSensorMapping[i][0] != 0xFF)
        {
            updateSensor(lastSensorMapping[i][0], 0, sensorColor(i), index);

            if (lastSensorMapping[i][1] != 0xFF &&
                lastSensorMapping[i][0] != lastSensorMapping[i][1])
            {
                updateSensor(lastSensorMapping[i][1], 1, sensorColor(i), index);
            }

            lastSensorMapping[i][1] = lastSensorMapping[i][0];
        }
    }
}

static void initSensorUi(U8 index)
{
    U8 i;
    for(i = 3; i < SENSOR_LIST_COUNT + 2; ++i )
    {
        if (i == 3)
        {
            printf("\033[s\033[%d;2H|     |    |\033[u", i + index);
        }
        else if (i == SENSOR_LIST_COUNT + 1)
        {
            printf("\033[s\033[%d;2H|     |    |\033[u", i + index);
        }
        else
        {
            printf("\033[s\033[%d;2H|     |    |\033[u", i + index);
        }
    }

    printf("\033[s\033[%d;80H\033[1mSENSORS\033[m\033[u", index);
    printf("\033[s\033[%d;24H______.______._____________________.___\033[u",          4 + index);
    printf("\033[s\033[%d;24H_____._/ /___.________.___.________.___\\\033[u",        5 + index);
    printf("\033[s\033[%d;24H  __._/ //           \\     /           \\\\.\033[u",    6 + index);
    printf("\033[s\033[%d;24H       //            .\\ | /.           .\\\\\033[u",    7 + index);
    printf("\033[s\033[%d;24H       |.             .\\|/.              |\033[u",      8 + index);
    printf("\033[s\033[%d;24H       |                |                |\033[u",       9 + index);
    printf("\033[s\033[%d;24H       |.             ./|\\.              |\033[u",      10 + index);
    printf("\033[s\033[%d;24H  _.__ \\\\            ./ | \\.           .//\033[u",    11 + index);
    printf("\033[s\033[%d;24H_.__._\\ \\\\___._______/.___.\\_______.___//.\033[u",   12 + index);
    printf("\033[s\033[%d;24H_.___._\\ \\___.____.___________.____.___/\033[u",       13 + index);
    printf("\033[s\033[%d;24H_.____._\\______._\\_____________/_.__________\033[u",   14 + index);
}

static void initSensorMapping()
{
    sSensorMapping[0x00].r = 4;
    sSensorMapping[0x00].c = 6;
    sSensorMapping[0x00].s = '>';

    sSensorMapping[0x01].r = 4;
    sSensorMapping[0x01].c = 6;
    sSensorMapping[0x01].s = '<';

    sSensorMapping[0x02].r = 8;
    sSensorMapping[0x02].c = 8;
    sSensorMapping[0x02].s = '^';

    sSensorMapping[0x03].r = 8;
    sSensorMapping[0x03].c = 8;
    sSensorMapping[0x03].s = 'v';

    sSensorMapping[0x04].r = 14;
    sSensorMapping[0x04].c = 6;
    sSensorMapping[0x04].s = '>';

    sSensorMapping[0x05].r = 14;
    sSensorMapping[0x05].c = 6;
    sSensorMapping[0x05].s = '<';

    sSensorMapping[0x06].r = 13;
    sSensorMapping[0x06].c = 5;
    sSensorMapping[0x06].s = '<';

    sSensorMapping[0x07].r = 13;
    sSensorMapping[0x07].c = 5;
    sSensorMapping[0x07].s = '>';

    sSensorMapping[0x08].r = 12;
    sSensorMapping[0x08].c = 4;
    sSensorMapping[0x08].s = '<';

    sSensorMapping[0x09].r = 12;
    sSensorMapping[0x09].c = 4;
    sSensorMapping[0x09].s = '>';

    sSensorMapping[0x0A].r = 11;
    sSensorMapping[0x0A].c = 3;
    sSensorMapping[0x0A].s = '>';

    sSensorMapping[0x0B].r = 11;
    sSensorMapping[0x0B].c = 3;
    sSensorMapping[0x0B].s = '<';

    sSensorMapping[0x0C].r = 5;
    sSensorMapping[0x0C].c = 5;
    sSensorMapping[0x0C].s = '>';

    sSensorMapping[0x0D].r = 5;
    sSensorMapping[0x0D].c = 5;
    sSensorMapping[0x0D].s = '<';

    sSensorMapping[0x0E].r = 6;
    sSensorMapping[0x0E].c = 4;
    sSensorMapping[0x0E].s = '<';

    sSensorMapping[0x0F].r = 6;
    sSensorMapping[0x0F].c = 4;
    sSensorMapping[0x0F].s = '>';

    sSensorMapping[0x10].r = 12;
    sSensorMapping[0x10].c = 22;
    sSensorMapping[0x10].s = '>';
    
    sSensorMapping[0x11].r = 12;
    sSensorMapping[0x11].c = 22;
    sSensorMapping[0x11].s = '<';

    sSensorMapping[0x12].r = 11;
    sSensorMapping[0x12].c = 21;
    sSensorMapping[0x12].s = '^';
    
    sSensorMapping[0x13].r = 11;
    sSensorMapping[0x13].c = 21;
    sSensorMapping[0x13].s = 'v';

    sSensorMapping[0x14].r = 5;
    sSensorMapping[0x14].c = 22;
    sSensorMapping[0x14].s = '>';
    
    sSensorMapping[0x15].r = 5;
    sSensorMapping[0x15].c = 22;
    sSensorMapping[0x15].s = '<';

    sSensorMapping[0x16].r = 12;
    sSensorMapping[0x16].c = 1;
    sSensorMapping[0x16].s = '>';
    
    sSensorMapping[0x17].r = 12;
    sSensorMapping[0x17].c = 1;
    sSensorMapping[0x17].s = '<';

    sSensorMapping[0x18].r = 14;
    sSensorMapping[0x18].c = 1;
    sSensorMapping[0x18].s = '>';

    sSensorMapping[0x19].r = 14;
    sSensorMapping[0x19].c = 1;
    sSensorMapping[0x19].s = '<';

    sSensorMapping[0x1A].r = 13;
    sSensorMapping[0x1A].c = 1;
    sSensorMapping[0x1A].s = '>';

    sSensorMapping[0x1B].r = 13;
    sSensorMapping[0x1B].c = 1;
    sSensorMapping[0x1B].s = '<';

    sSensorMapping[0x1C].r = 10;
    sSensorMapping[0x1C].c = 26;
    sSensorMapping[0x1C].s = '^';

    sSensorMapping[0x1D].r = 10;
    sSensorMapping[0x1D].c = 26;
    sSensorMapping[0x1D].s = 'v';

    sSensorMapping[0x1E].r = 10;
    sSensorMapping[0x1E].c = 8;
    sSensorMapping[0x1E].s = '^';

    sSensorMapping[0x1F].r = 10;
    sSensorMapping[0x1F].c = 8;
    sSensorMapping[0x1F].s = 'v';

    sSensorMapping[0x20].r = 10;
    sSensorMapping[0x20].c = 22;
    sSensorMapping[0x20].s = 'v';

    sSensorMapping[0x21].r = 10;
    sSensorMapping[0x21].c = 22;
    sSensorMapping[0x21].s = '^';

    sSensorMapping[0x22].r = 14;
    sSensorMapping[0x22].c = 33;
    sSensorMapping[0x22].s = '>';

    sSensorMapping[0x23].r = 14;
    sSensorMapping[0x23].c = 33;
    sSensorMapping[0x23].s = '<';

    sSensorMapping[0x24].r = 13;
    sSensorMapping[0x24].c = 13;
    sSensorMapping[0x24].s = '>';

    sSensorMapping[0x25].r = 13;
    sSensorMapping[0x25].c = 13;
    sSensorMapping[0x25].s = '<';

    sSensorMapping[0x26].r = 14;
    sSensorMapping[0x26].c = 15;
    sSensorMapping[0x26].s = '>';

    sSensorMapping[0x27].r = 14;
    sSensorMapping[0x27].c = 15;
    sSensorMapping[0x27].s = '<';

    sSensorMapping[0x28].r = 12;
    sSensorMapping[0x28].c = 13;
    sSensorMapping[0x28].s = '<';

    sSensorMapping[0x29].r = 12;
    sSensorMapping[0x29].c = 13;
    sSensorMapping[0x29].s = '>';

    sSensorMapping[0x2A].r = 5;
    sSensorMapping[0x2A].c = 13;
    sSensorMapping[0x2A].s = '>';

    sSensorMapping[0x2B].r = 5;
    sSensorMapping[0x2B].c = 13;
    sSensorMapping[0x2B].s = '<';

    sSensorMapping[0x2C].r = 4;
    sSensorMapping[0x2C].c = 13;
    sSensorMapping[0x2C].s = '>';

    sSensorMapping[0x2D].r = 4;
    sSensorMapping[0x2D].c = 13;
    sSensorMapping[0x2D].s = '<';

    sSensorMapping[0x2E].r = 13;
    sSensorMapping[0x2E].c = 18;
    sSensorMapping[0x2E].s = '>';

    sSensorMapping[0x2F].r = 13;
    sSensorMapping[0x2F].c = 18;
    sSensorMapping[0x2F].s = '<';

    sSensorMapping[0x30].r = 8;
    sSensorMapping[0x30].c = 26;
    sSensorMapping[0x30].s = 'v';

    sSensorMapping[0x31].r = 8;
    sSensorMapping[0x31].c = 26;
    sSensorMapping[0x31].s = '^';

    sSensorMapping[0x32].r = 5;
    sSensorMapping[0x32].c = 26;
    sSensorMapping[0x32].s = '>';

    sSensorMapping[0x33].r = 5;
    sSensorMapping[0x33].c = 26;
    sSensorMapping[0x33].s = '<';

    sSensorMapping[0x34].r = 7;
    sSensorMapping[0x34].c = 39;
    sSensorMapping[0x34].s = '^';

    sSensorMapping[0x35].r = 7;
    sSensorMapping[0x35].c = 39;
    sSensorMapping[0x35].s = 'v';

    sSensorMapping[0x36].r = 6;
    sSensorMapping[0x36].c = 41;
    sSensorMapping[0x36].s = 'v';

    sSensorMapping[0x37].r = 6;
    sSensorMapping[0x37].c = 41;
    sSensorMapping[0x37].s = '^';

    sSensorMapping[0x38].r = 12;
    sSensorMapping[0x38].c = 41;
    sSensorMapping[0x38].s = 'v';

    sSensorMapping[0x39].r = 12;
    sSensorMapping[0x39].c = 41;
    sSensorMapping[0x39].s = '^';

    sSensorMapping[0x3A].r = 13;
    sSensorMapping[0x3A].c = 30;
    sSensorMapping[0x3A].s = '<';

    sSensorMapping[0x3B].r = 13;
    sSensorMapping[0x3B].c = 30;
    sSensorMapping[0x3B].s = '>';

    sSensorMapping[0x3C].r = 12;
    sSensorMapping[0x3C].c = 26;
    sSensorMapping[0x3C].s = '<';

    sSensorMapping[0x3D].r = 12;
    sSensorMapping[0x3D].c = 26;
    sSensorMapping[0x3D].s = '>';

    sSensorMapping[0x3E].r = 11;
    sSensorMapping[0x3E].c = 27;
    sSensorMapping[0x3E].s = '^';

    sSensorMapping[0x3F].r = 11;
    sSensorMapping[0x3F].c = 27;
    sSensorMapping[0x3F].s = 'v';

    sSensorMapping[0x40].r = 8;
    sSensorMapping[0x40].c = 22;
    sSensorMapping[0x40].s = 'v';

    sSensorMapping[0x41].r = 8;
    sSensorMapping[0x41].c = 22;
    sSensorMapping[0x41].s = '^';

    sSensorMapping[0x42].r = 7;
    sSensorMapping[0x42].c = 27;
    sSensorMapping[0x42].s = 'v';

    sSensorMapping[0x43].r = 7;
    sSensorMapping[0x43].c = 27;
    sSensorMapping[0x43].s = '^';

    sSensorMapping[0x44].r = 5;
    sSensorMapping[0x44].c = 35;
    sSensorMapping[0x44].s = '>';

    sSensorMapping[0x45].r = 5;
    sSensorMapping[0x45].c = 35;
    sSensorMapping[0x45].s = '<';

    sSensorMapping[0x46].r = 4;
    sSensorMapping[0x46].c = 35;
    sSensorMapping[0x46].s = '>';

    sSensorMapping[0x47].r = 4;
    sSensorMapping[0x47].c = 35;
    sSensorMapping[0x47].s = '<';

    sSensorMapping[0x48].r = 11;
    sSensorMapping[0x48].c = 39;
    sSensorMapping[0x48].s = '^';

    sSensorMapping[0x49].r = 11;
    sSensorMapping[0x49].c = 39;
    sSensorMapping[0x49].s = 'v';

    sSensorMapping[0x4A].r = 13;
    sSensorMapping[0x4A].c = 35;
    sSensorMapping[0x4A].s = '>';

    sSensorMapping[0x4B].r = 13;
    sSensorMapping[0x4B].c = 35;
    sSensorMapping[0x4B].s = '<';

    sSensorMapping[0x4C].r = 12;
    sSensorMapping[0x4C].c = 35;
    sSensorMapping[0x4C].s = '<';

    sSensorMapping[0x4D].r = 12;
    sSensorMapping[0x4D].c = 35;
    sSensorMapping[0x4D].s = '>';

    sSensorMapping[0x4E].r = 7;
    sSensorMapping[0x4E].c = 21;
    sSensorMapping[0x4E].s = '^';

    sSensorMapping[0x4F].r = 7;
    sSensorMapping[0x4F].c = 21;
    sSensorMapping[0x4F].s = 'v';
}

static U8 buffer[SENSOR_LIST_COUNT];
static U8 owner[SENSOR_LIST_COUNT];
static U8 bufferHead;

void SensorDisplayPoll(void)
{
    TaskID sensors = nsWhoIs(TrainSensors);
    TaskID attrib = nsWhoIs(NodeAttribution);
    TaskID parent = VAL_TO_ID(sysPid());

    U16 i = 0;
    bufferHead = 0;
    MessageEnvelope env;
    MessageEnvelope att;

    for(i = 0; i < SENSOR_LIST_COUNT; i++)
    {
        buffer[i] = 0xFF;
        owner[i] = 0x00;
    }

    for (i = 0; i < 80; i++)
    {
        lastSensorMapping[i][0] = 0xFF;
        lastSensorMapping[i][1] = 0xFF;
    }

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

                    att.message.MessageU8.body = (i-1)*16 + c;
                    sysSend(attrib.value, &att, &att);
                    U8 ownerValue = att.message.MessageU8.body;
                    owner[bufferHead] = ownerValue; 
                    if (ownerValue != 0)
                    {
                        lastSensorMapping[owner[bufferHead]][0] = storedValue;
                    }

                    bufferHead = (bufferHead + 1) % SENSOR_LIST_COUNT;
                }
            }
        }

        sysSend(parent.value, &env, &env);
    }
}

void SensorDisplay(void)
{
    TaskID id = VAL_TO_ID(sysCreate(sysPriority(sysTid())-1, &SensorDisplayPoll));

    U8 index = 0;
    initSensorMapping();

    MessageEnvelope env;
    TaskID sender;

    for(;;)
    {
        sysReceive(&sender.value, &env);

        if (sender.value == id.value)
        {
            if (index != 0)
            {
                updateSensorUi(buffer, owner, decmod(bufferHead, SENSOR_LIST_COUNT), index); 
            }
        }
        else
        {
            index = env.message.MessageU8.body;
            if (index != 0)
            {
                initSensorUi(index);
            }
        }

        sysReply(sender.value, &env);
    }
}
