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

static SwitchMapEntry sSwitchMapping[22];
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
        char marker = (switchState[i] == eCurved ? sSwitchMapping[i].curved : sSwitchMapping[i].straight);
        printf("\033[s\033[%d;17H%c\033[u", i + index, symbol);
        printf("\033[s\033[%d;%dH\033[1m%c\033[m\033[u",
                    sSwitchMapping[i].r + index,
                    sSwitchMapping[i].c + 25,
                    marker);
    }
}

static void initSwitchUi(U8 index)
{
    printf("\033[s\033[%d;80H\033[1mSWITCHES\033[m\033[u", index);

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
        char marker = (switchState[i] == eCurved ? sSwitchMapping[i].curved : sSwitchMapping[i].straight);
        printf("\033[s\033[%d;2H| %3d | %b | %c |\033[u", i + index, sw, sw, symbol);
        printf("\033[s\033[%d;%dH\033[1m%c\033[m\033[u",
                    sSwitchMapping[i].r + index,
                    sSwitchMapping[i].c + 25,
                    marker);
    }
}

static void initSwitchMapping()
{
    U8 i;
    for (i = 0; i < 22; i++)
    {
        sSwitchMapping[i].r = 1;
        sSwitchMapping[i].c = 1;
        sSwitchMapping[i].curved = ' ';
        sSwitchMapping[i].straight = ' ';
    }

    sSwitchMapping[0].r = 14;
    sSwitchMapping[0].c = 7;
    sSwitchMapping[0].curved = '\\';
    sSwitchMapping[0].straight = '_';

    sSwitchMapping[1].r = 15;
    sSwitchMapping[1].c = 8;
    sSwitchMapping[1].curved = '\\';
    sSwitchMapping[1].straight = '_';

    sSwitchMapping[2].r = 16;
    sSwitchMapping[2].c = 9;
    sSwitchMapping[2].curved = '\\';
    sSwitchMapping[2].straight = '_';

    sSwitchMapping[3].r = 6;
    sSwitchMapping[3].c = 6;
    sSwitchMapping[3].curved = '/';
    sSwitchMapping[3].straight = ' ';

    sSwitchMapping[4].r = 16;
    sSwitchMapping[4].c = 32;
    sSwitchMapping[4].curved = '/';
    sSwitchMapping[4].straight = '_';

    sSwitchMapping[5].r = 15;
    sSwitchMapping[5].c = 17;
    sSwitchMapping[5].curved = '\\';
    sSwitchMapping[5].straight = ' ';

    sSwitchMapping[6].r = 15;
    sSwitchMapping[6].c = 33;
    sSwitchMapping[6].curved = '/';
    sSwitchMapping[6].straight = ' ';

    sSwitchMapping[7].r = 14;
    sSwitchMapping[7].c = 39;
    sSwitchMapping[7].curved = '_';
    sSwitchMapping[7].straight = '/';

    sSwitchMapping[8].r = 6;
    sSwitchMapping[8].c = 39;
    sSwitchMapping[8].curved = '_';
    sSwitchMapping[8].straight = '\\';

    sSwitchMapping[9].r = 6;
    sSwitchMapping[9].c = 27;
    sSwitchMapping[9].curved = '-';
    sSwitchMapping[9].straight = '_';

    sSwitchMapping[10].r = 5;
    sSwitchMapping[10].c = 10;
    sSwitchMapping[10].curved = '/';
    sSwitchMapping[10].straight = '-';

    sSwitchMapping[11].r = 5;
    sSwitchMapping[11].c = 7;
    sSwitchMapping[11].curved = '/';
    sSwitchMapping[11].straight = '-';

    sSwitchMapping[12].r = 6;
    sSwitchMapping[12].c = 21;
    sSwitchMapping[12].curved = '-';
    sSwitchMapping[12].straight = '_';

    sSwitchMapping[13].r = 6;
    sSwitchMapping[13].c = 9;
    sSwitchMapping[13].curved = '_';
    sSwitchMapping[13].straight = '/';

    sSwitchMapping[14].r = 14;
    sSwitchMapping[14].c = 9;
    sSwitchMapping[14].curved = ' ';
    sSwitchMapping[14].straight = '\\';

    sSwitchMapping[15].r = 13;
    sSwitchMapping[15].c = 22;
    sSwitchMapping[15].curved = '-';
    sSwitchMapping[15].straight = '_';

    sSwitchMapping[16].r = 13;
    sSwitchMapping[16].c = 26;
    sSwitchMapping[16].curved = '-';
    sSwitchMapping[16].straight = '_';

    sSwitchMapping[17].r = 16;
    sSwitchMapping[17].c = 18;
    sSwitchMapping[17].curved = '\\';
    sSwitchMapping[17].straight = '_';

    sSwitchMapping[18].r = 11;
    sSwitchMapping[18].c = 23;
    sSwitchMapping[18].curved = '/';
    sSwitchMapping[18].straight = ']';

    sSwitchMapping[19].r = 11;
    sSwitchMapping[19].c = 25;
    sSwitchMapping[19].curved = '\\';
    sSwitchMapping[19].straight = '[';

    sSwitchMapping[20].r = 9;
    sSwitchMapping[20].c = 25;
    sSwitchMapping[20].curved = '/';
    sSwitchMapping[20].straight = '[';

    sSwitchMapping[21].r = 9;
    sSwitchMapping[21].c = 23;
    sSwitchMapping[21].curved = '\\';
    sSwitchMapping[21].straight = ']';
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
