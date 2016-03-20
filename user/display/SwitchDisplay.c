#include "user/messageTypes.h"
#include "user/display/SwitchDisplay.h"
#include "user/services/services.h"

typedef struct
{
    U8 r;
    U8 c;
    char curved;
    char straight;
} SwitchMapEntry;

static SwitchMapEntry sMapping[22];
static U8 switchState[22];

static U8 id2Sw(U8 id)
{
    if (id <= 0x11)
    {
        return (id + 1);
    }
    else if (id >= 0x12 && id <= 0x15)
    {
        return (id - 0x12) + 0x99;
    }
    else
    {
        return -1;
    }
}

static void updateSwitchUi(U8 index)
{
    U8 i;
    for(i = 0; i < 22; ++i )
    {
        char symbol = (switchState[i] == eCurved ? 'C' : 'S');
        char marker = (switchState[i] == eCurved ? sMapping[i].curved : sMapping[i].straight);
        printf("\033[s\033[%d;17H%c\033[u", i + index, symbol);
        printf("\033[s\033[%d;%dH\033[1m%c\033[m\033[u",
                    sMapping[i].r + index,
                    sMapping[i].c + 25,
                    marker);
    }
}

static void initSwitchUi(U8 index)
{
    printf("\033[s\033[%d;25H______________________________________\033[u",          4 + index);
    printf("\033[s\033[%d;25H_______/  /                           \\\033[u",          5 + index);
    printf("\033[s\033[%d;25H  ____/  /_____________________________\\\033[u",        6 + index);
    printf("\033[s\033[%d;25H        /            \\     /            \\\033[u",    7 + index);
    printf("\033[s\033[%d;25H       /              \\ | /              \\\033[u",    8 + index);
    printf("\033[s\033[%d;25H       |               \\|/               |\033[u",      9 + index);
    printf("\033[s\033[%d;25H       |                |                |\033[u",       10 + index);
    printf("\033[s\033[%d;25H       |               /|\\               |\033[u",      11 + index);
    printf("\033[s\033[%d;25H       \\              / | \\              /\033[u",    12 + index);
    printf("\033[s\033[%d;25H  _____ \\____________/_____\\____________/\033[u",   13 + index);
    printf("\033[s\033[%d;25H_______\\ \\_____________________________/\033[u",       14 + index);
    printf("\033[s\033[%d;25H________\\        \\               /\033[u",             15 + index);
    printf("\033[s\033[%d;25H_________\\________\\_____________/____________\033[u",   16 + index);

    U8 i;
    for(i = 0; i < 22; ++i )
    {
        U8 sw = id2Sw(i);
        char symbol = (switchState[i] == eCurved ? 'C' : 'S');
        char marker = (switchState[i] == eCurved ? sMapping[i].curved : sMapping[i].straight);
        printf("\033[s\033[%d;2H| %3d | %b | %c |\033[u", i + index, sw, sw, symbol);
        printf("\033[s\033[%d;%dH\033[1m%c\033[m\033[u",
                    sMapping[i].r + index,
                    sMapping[i].c + 25,
                    marker);
    }
}

static void initSwitchMapping()
{
    U8 i;
    for (i = 0; i < 22; i++)
    {
        sMapping[i].r = 1;
        sMapping[i].c = 1;
        sMapping[i].curved = ' ';
        sMapping[i].straight = ' ';
    }

    sMapping[0].r = 14;
    sMapping[0].c = 7;
    sMapping[0].curved = '\\';
    sMapping[0].straight = '_';

    sMapping[1].r = 15;
    sMapping[1].c = 8;
    sMapping[1].curved = '\\';
    sMapping[1].straight = '_';

    sMapping[2].r = 16;
    sMapping[2].c = 9;
    sMapping[2].curved = '\\';
    sMapping[2].straight = '_';

    sMapping[3].r = 6;
    sMapping[3].c = 6;
    sMapping[3].curved = '/';
    sMapping[3].straight = ' ';

    sMapping[4].r = 16;
    sMapping[4].c = 32;
    sMapping[4].curved = '/';
    sMapping[4].straight = '_';

    sMapping[5].r = 15;
    sMapping[5].c = 17;
    sMapping[5].curved = '\\';
    sMapping[5].straight = ' ';

    sMapping[6].r = 15;
    sMapping[6].c = 33;
    sMapping[6].curved = '/';
    sMapping[6].straight = ' ';

    sMapping[7].r = 14;
    sMapping[7].c = 39;
    sMapping[7].curved = '_';
    sMapping[7].straight = '/';

    sMapping[8].r = 6;
    sMapping[8].c = 39;
    sMapping[8].curved = '_';
    sMapping[8].straight = '\\';

    sMapping[9].r = 6;
    sMapping[9].c = 27;
    sMapping[9].curved = '-';
    sMapping[9].straight = '_';

    sMapping[10].r = 5;
    sMapping[10].c = 10;
    sMapping[10].curved = '/';
    sMapping[10].straight = '-';

    sMapping[11].r = 5;
    sMapping[11].c = 7;
    sMapping[11].curved = '/';
    sMapping[11].straight = '-';

    sMapping[12].r = 6;
    sMapping[12].c = 21;
    sMapping[12].curved = '-';
    sMapping[12].straight = '_';

    sMapping[13].r = 6;
    sMapping[13].c = 9;
    sMapping[13].curved = '_';
    sMapping[13].straight = '/';

    sMapping[14].r = 14;
    sMapping[14].c = 9;
    sMapping[14].curved = ' ';
    sMapping[14].straight = '\\';

    sMapping[15].r = 13;
    sMapping[15].c = 22;
    sMapping[15].curved = '-';
    sMapping[15].straight = '_';

    sMapping[16].r = 13;
    sMapping[16].c = 26;
    sMapping[16].curved = '-';
    sMapping[16].straight = '_';

    sMapping[17].r = 16;
    sMapping[17].c = 18;
    sMapping[17].curved = '\\';
    sMapping[17].straight = '_';

    sMapping[18].r = 11;
    sMapping[18].c = 23;
    sMapping[18].curved = '/';
    sMapping[18].straight = ']';

    sMapping[19].r = 11;
    sMapping[19].c = 25;
    sMapping[19].curved = '\\';
    sMapping[19].straight = '[';

    sMapping[20].r = 9;
    sMapping[20].c = 25;
    sMapping[20].curved = '/';
    sMapping[20].straight = '[';

    sMapping[21].r = 9;
    sMapping[21].c = 23;
    sMapping[21].curved = '\\';
    sMapping[21].straight = ']';
}

void SwitchDisplayPoll(void)
{
    TaskID sSwitchOffice = nsWhoIs(TrainSwitchOffice);
    TaskID clock = nsWhoIs(Clock);
    TaskID parent = VAL_TO_ID(sysPid());

    U8 i = 0;
    MessageEnvelope env;

    for(i = 0; i < 22; i++)
    {
        switchState[i] = 0;
    }

    for(;;)
    {
        U8 dirty = 0;
        for (i = 0; i< 22; i++)
        {
            env.message.MessageU32.body = i;
            env.type = MESSAGE_SWITCH_READ;
            sysSend(sSwitchOffice.value, &env, &env);

            SwitchState sw = (SwitchState)env.message.MessageU32.body;
            if (switchState[i] != sw)
            {
                switchState[i] = sw;
                dirty = 1;
            }
        }

        if (dirty != 0)
        {
            sysSend(parent.value, &env, &env);
        }

        clockLongDelayBy(clock, 3);
    }
}

void SwitchDisplay(void)
{
    TaskID id = VAL_TO_ID(sysCreate(sysPriority()-1, &SwitchDisplayPoll));

    U8 index = 0;
    MessageEnvelope env;
    TaskID sender;

    initSwitchMapping();

    for(;;)
    {
        sysReceive(&sender.value, &env);

        if (sender.value == id.value)
        {
            if (index != 0)
            {
               updateSwitchUi(index); 
            }
        }
        else
        {
            index = env.message.MessageU8.body;
            if (index != 0)
            {
                initSwitchUi(index);
            }
        }

        sysReply(sender.value, &env);
    }
}
