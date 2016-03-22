#include "common/common.h"
#include "user/display/OwnerDisplay.h"
#include "user/services/services.h"

#define SENSOR_LIST_COUNT 14

typedef struct
{
    U8 r;
    U8 c;
    char s;
} OwnerMapEntry;

static OwnerMapEntry sOwnerMapping[72];

static U8 ownerColor(U8 owner)
{
    switch (owner)
    {
        case 64:
            return 31;
        case 69:
            return 35;
        default:
            return 37;
    }
}

static void updateOwnerUi(U8 index, U8 node, U8 owner)
{
    U8 row = sOwnerMapping[node].r + index;
    U8 col = sOwnerMapping[node].c + 18;

    printf("\033[s\033[%d;%dH\033[%dm%c\033[m\033[u",
                    row, col, ownerColor(owner), sOwnerMapping[node].s);
}

static void initOwnerUi(U8 index)
{
    printf("\033[s\033[%d;80H\033[1mOWNERSHIP\033[m\033[u", index);

    printf("\033[s\033[%d;18H@______*________*_____________________*__\033[u",          3 + index);
    printf("\033[s\033[%d;18H@____*___/#  /#                          \\\033[u",          4 + index);
    printf("\033[s\033[%d;18H@__*____/#  /#__*________*___*________*__#\\\033[u",        5 + index);
    printf("\033[s\033[%d;18H           /           #\\     /#           \\*\033[u",    6 + index);
    printf("\033[s\033[%d;18H          /             *\\ @ /*            *\\\033[u",    7 + index);
    printf("\033[s\033[%d;18H         |*              *\\|/*              |\033[u",      8 + index);
    printf("\033[s\033[%d;18H         |                #|#               |\033[u",       9 + index);
    printf("\033[s\033[%d;18H         |                 |                |\033[u",       10 + index);
    printf("\033[s\033[%d;18H         |                #|#               |\033[u",       11 + index);
    printf("\033[s\033[%d;18H         |*              */|\\*              |\033[u",      12 + index);
    printf("\033[s\033[%d;18H          \\             */ @ \\*            */\033[u",    13 + index);
    printf("\033[s\033[%d;18H@_*_____   \\#___*______#/*___*\\#______*___#/*\033[u",   14 + index);
    printf("\033[s\033[%d;18H@_*__*__\\#  \\___*____*___________*____*___/\033[u",       15 + index);
    printf("\033[s\033[%d;18H@_*___*__\\#        \\#             #/\033[u",              16 + index);
    printf("\033[s\033[%d;18H@_*____*__\\#______*_\\#___________#/_*_________@\033[u",   17 + index);
}

static void initOwnerMapping()
{
    sOwnerMapping[0x00].r = 3;
    sOwnerMapping[0x00].c = 7;
    sOwnerMapping[0x00].s = '*';

    sOwnerMapping[0x01].r = 8;
    sOwnerMapping[0x01].c = 10;
    sOwnerMapping[0x01].s = '*';

    sOwnerMapping[0x02].r = 17;
    sOwnerMapping[0x02].c = 7;
    sOwnerMapping[0x02].s = '*';

    sOwnerMapping[0x03].r = 16;
    sOwnerMapping[0x03].c = 6;
    sOwnerMapping[0x03].s = '*';

    sOwnerMapping[0x04].r = 15;
    sOwnerMapping[0x04].c = 5;
    sOwnerMapping[0x04].s = '*';

    sOwnerMapping[0x05].r = 14;
    sOwnerMapping[0x05].c = 2;
    sOwnerMapping[0x05].s = '*';

    sOwnerMapping[0x06].r = 4;
    sOwnerMapping[0x06].c = 5;
    sOwnerMapping[0x06].s = '*';

    sOwnerMapping[0x07].r = 5;
    sOwnerMapping[0x07].c = 3;
    sOwnerMapping[0x07].s = '*';

    sOwnerMapping[0x08].r = 14;
    sOwnerMapping[0x08].c = 25;
    sOwnerMapping[0x08].s = '*';
    
    sOwnerMapping[0x09].r = 13;
    sOwnerMapping[0x09].c = 24;
    sOwnerMapping[0x09].s = '*';
    
    sOwnerMapping[0x0A].r = 5;
    sOwnerMapping[0x0A].c = 25;
    sOwnerMapping[0x0A].s = '*';
    
    sOwnerMapping[0x0B].r = 15;
    sOwnerMapping[0x0B].c = 2;
    sOwnerMapping[0x0B].s = '*';

    sOwnerMapping[0x0C].r = 17;
    sOwnerMapping[0x0C].c = 2;
    sOwnerMapping[0x0C].s = '*';

    sOwnerMapping[0x0D].r = 16;
    sOwnerMapping[0x0D].c = 2;
    sOwnerMapping[0x0D].s = '*';

    sOwnerMapping[0x0E].r = 12;
    sOwnerMapping[0x0E].c = 29;
    sOwnerMapping[0x0E].s = '*';

    sOwnerMapping[0x0F].r = 12;
    sOwnerMapping[0x0F].c = 10;
    sOwnerMapping[0x0F].s = '*';

    sOwnerMapping[0x10].r = 12;
    sOwnerMapping[0x10].c = 25;
    sOwnerMapping[0x10].s = '*';

    sOwnerMapping[0x11].r = 17;
    sOwnerMapping[0x11].c = 36;
    sOwnerMapping[0x11].s = '*';

    sOwnerMapping[0x12].r = 15;
    sOwnerMapping[0x12].c = 16;
    sOwnerMapping[0x12].s = '*';

    sOwnerMapping[0x13].r = 17;
    sOwnerMapping[0x13].c = 18;
    sOwnerMapping[0x13].s = '*';

    sOwnerMapping[0x14].r = 14;
    sOwnerMapping[0x14].c = 16;
    sOwnerMapping[0x14].s = '*';

    sOwnerMapping[0x15].r = 5;
    sOwnerMapping[0x15].c = 16;
    sOwnerMapping[0x15].s = '*';

    sOwnerMapping[0x16].r = 3;
    sOwnerMapping[0x16].c = 16;
    sOwnerMapping[0x16].s = '*';

    sOwnerMapping[0x17].r = 15;
    sOwnerMapping[0x17].c = 21;
    sOwnerMapping[0x17].s = '*';

    sOwnerMapping[0x18].r = 8;
    sOwnerMapping[0x18].c = 29;
    sOwnerMapping[0x18].s = '*';
    
    sOwnerMapping[0x19].r = 5;
    sOwnerMapping[0x19].c = 29;
    sOwnerMapping[0x19].s = '*';
    
    sOwnerMapping[0x1A].r = 7;
    sOwnerMapping[0x1A].c = 43;
    sOwnerMapping[0x1A].s = '*';
    
    sOwnerMapping[0x1B].r = 6;
    sOwnerMapping[0x1B].c = 44;
    sOwnerMapping[0x1B].s = '*';

    sOwnerMapping[0x1C].r = 14;
    sOwnerMapping[0x1C].c = 44;
    sOwnerMapping[0x1C].s = '*';

    sOwnerMapping[0x1D].r = 15;
    sOwnerMapping[0x1D].c = 33;
    sOwnerMapping[0x1D].s = '*';

    sOwnerMapping[0x1E].r = 14;
    sOwnerMapping[0x1E].c = 29;
    sOwnerMapping[0x1E].s = '*';

    sOwnerMapping[0x1F].r = 13;
    sOwnerMapping[0x1F].c = 30;
    sOwnerMapping[0x1F].s = '*';

    sOwnerMapping[0x20].r = 8;
    sOwnerMapping[0x20].c = 25;
    sOwnerMapping[0x20].s = '*';

    sOwnerMapping[0x21].r = 7;
    sOwnerMapping[0x21].c = 30;
    sOwnerMapping[0x21].s = '*';

    sOwnerMapping[0x22].r = 5;
    sOwnerMapping[0x22].c = 38;
    sOwnerMapping[0x22].s = '*';

    sOwnerMapping[0x23].r = 3;
    sOwnerMapping[0x23].c = 38;
    sOwnerMapping[0x23].s = '*';

    sOwnerMapping[0x24].r = 13;
    sOwnerMapping[0x24].c = 43;
    sOwnerMapping[0x24].s = '*';

    sOwnerMapping[0x25].r = 15;
    sOwnerMapping[0x25].c = 38;
    sOwnerMapping[0x25].s = '*';

    sOwnerMapping[0x26].r = 14;
    sOwnerMapping[0x26].c = 38;
    sOwnerMapping[0x26].s = '*';

    sOwnerMapping[0x27].r = 7;
    sOwnerMapping[0x27].c = 24;
    sOwnerMapping[0x27].s = '*';

    sOwnerMapping[0x28].r = 15;
    sOwnerMapping[0x28].c = 9;
    sOwnerMapping[0x28].s = '#';

    sOwnerMapping[0x29].r = 16; 
    sOwnerMapping[0x29].c = 10;  
    sOwnerMapping[0x29].s = '#';

    sOwnerMapping[0x2A].r = 17; 
    sOwnerMapping[0x2A].c = 11;  
    sOwnerMapping[0x2A].s = '#'; 

    sOwnerMapping[0x2B].r = 5; 
    sOwnerMapping[0x2B].c = 9;  
    sOwnerMapping[0x2B].s = '#';

    sOwnerMapping[0x2C].r = 17; 
    sOwnerMapping[0x2C].c = 33;  
    sOwnerMapping[0x2C].s = '#'; 

    sOwnerMapping[0x2D].r = 16; 
    sOwnerMapping[0x2D].c = 20;  
    sOwnerMapping[0x2D].s = '#'; 

    sOwnerMapping[0x2E].r = 16; 
    sOwnerMapping[0x2E].c = 34;  
    sOwnerMapping[0x2E].s = '#'; 

    sOwnerMapping[0x2F].r = 14; 
    sOwnerMapping[0x2F].c = 42;  
    sOwnerMapping[0x2F].s = '#';

    sOwnerMapping[0x30].r = 5; 
    sOwnerMapping[0x30].c = 41;  
    sOwnerMapping[0x30].s = '#';

    sOwnerMapping[0x31].r = 6;
    sOwnerMapping[0x31].c = 31;
    sOwnerMapping[0x31].s = '#';

    sOwnerMapping[0x32].r = 4;
    sOwnerMapping[0x32].c = 14;
    sOwnerMapping[0x32].s = '#';

    sOwnerMapping[0x33].r = 4;
    sOwnerMapping[0x33].c = 10;
    sOwnerMapping[0x33].s = '#';

    sOwnerMapping[0x34].r = 6;
    sOwnerMapping[0x34].c = 23;
    sOwnerMapping[0x34].s = '#';

    sOwnerMapping[0x35].r = 5;
    sOwnerMapping[0x35].c = 13;
    sOwnerMapping[0x35].s = '#';

    sOwnerMapping[0x36].r = 14;
    sOwnerMapping[0x36].c = 12;
    sOwnerMapping[0x36].s = '#';

    sOwnerMapping[0x37].r = 14;
    sOwnerMapping[0x37].c = 23;
    sOwnerMapping[0x37].s = '#';

    sOwnerMapping[0x38].r = 14;
    sOwnerMapping[0x38].c = 31;
    sOwnerMapping[0x38].s = '#';

    sOwnerMapping[0x39].r = 17;
    sOwnerMapping[0x39].c = 21;
    sOwnerMapping[0x39].s = '#';

    sOwnerMapping[0x3A].r = 11;
    sOwnerMapping[0x3A].c = 26;
    sOwnerMapping[0x3A].s = '#';

    sOwnerMapping[0x3B].r = 11; 
    sOwnerMapping[0x3B].c = 28;
    sOwnerMapping[0x3B].s = '#';

    sOwnerMapping[0x3C].r = 9; 
    sOwnerMapping[0x3C].c = 28;
    sOwnerMapping[0x3C].s = '#';

    sOwnerMapping[0x3D].r = 9; 
    sOwnerMapping[0x3D].c = 26;
    sOwnerMapping[0x3D].s = '#';
}

static U8 volatile * volatile ownershipGraph;
static volatile U8 ownershipLast[72];

void OwnerDisplayPoll(void)
{
    TaskID parent = VAL_TO_ID(sysPid());
    TaskID manager = nsWhoIs(TrackManager);
    TaskID clock = nsWhoIs(Clock);

    MessageEnvelope env;
    env.type = 2;

    sysSend(manager.value, &env, &env);
    ownershipGraph = (U8*)(env.message.MessageU32.body);

    U8 i;
    for (i = 0; i < 62; i++)
    {
        ownershipLast[i] = ownershipGraph[i];
    }

    for(;;)
    {
        clockDelayBy(clock, 10);
        sysSend(parent.value, &env, &env);
    }
}


void OwnerDisplay(void)
{
    TaskID id = VAL_TO_ID(sysCreate(sysPriority()-1, &OwnerDisplayPoll));

    U8 index = 0;
    initOwnerMapping();

    MessageEnvelope env;
    TaskID sender;

    for(;;)
    {
        sysReceive(&sender.value, &env);

        if (sender.value == id.value)
        {
            if (index != 0)
            {
                U8 i;
                for (i = 0; i < 62; i++)
                {
                    if (ownershipLast[i] != ownershipGraph[i])
                    {
                        ownershipLast[i] = ownershipGraph[i];
                        updateOwnerUi(index, i, ownershipGraph[i]);
                    }
                }
            }
        }
        else
        {
            index = env.message.MessageU8.body;
            if (index != 0)
            {
                initOwnerUi(index);
            }
        }

        sysReply(sender.value, &env);
    }
}
