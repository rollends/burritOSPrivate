#include "user/display/SensorDisplay.h"
#include "user/services/services.h"
#include "user/messageTypes.h"

#define LOG_LIST_COUNT 20
static char rollingBuffer[LOG_LIST_COUNT][256];
static U8 head;
static U8 previousHead;

extern U16 millis;
extern U16 seconds;
extern U16 minutes;

void logMessage(ConstString format, ...)
{
    MessageEnvelope env;
    char buffer[512];
    char final[512];

    va_list va;
    va_start(va, format);
    vsprintf(buffer, format, va);
    sprintf(final, "[%2d:%2d:%d] %80s", minutes, seconds, millis, buffer);
    va_end(va);

    env.message.MessageArbitrary.body = (U32*)final;
    sysSend(nsWhoIs(Logger).value, &env, &env);
}

void logError(ConstString format, ...)
{
    MessageEnvelope env;
    char buffer[512];
    char final[512];

    va_list va;
    va_start(va, format);
    vsprintf(buffer, format, va);
    sprintf(final, "[%2d:%2d:%d] \033[31m%80s\033[m", minutes, seconds, millis, buffer);
    va_end(va);

    env.message.MessageArbitrary.body = (U32*)final;
    sysSend(nsWhoIs(Logger).value, &env, &env);
}

void logWarn(ConstString format, ...)
{
    MessageEnvelope env;
    char buffer[512];
    char final[512];

    va_list va;
    va_start(va, format);
    vsprintf(buffer, format, va);
    sprintf(final, "[%2d:%2d:%d] \033[33m%80s\033[m", minutes, seconds, millis, buffer);
    va_end(va);

    env.message.MessageArbitrary.body = (U32*)final;
    sysSend(nsWhoIs(Logger).value, &env, &env);
}

void LogServer(void)
{
    TaskID parent = VAL_TO_ID(sysPid());
    
    U16 i = 0;
    head = 0;
    previousHead = 0;
    for(i = 0; i < LOG_LIST_COUNT; i++)
    {
        memset(rollingBuffer[i], 0, sizeof(char)*256);
    }

    TaskID from;
    MessageEnvelope env;
    nsRegister(Logger);
    for(;;)
    {
        sysReceive(&from.value, &env);
         
        strcpy(rollingBuffer[head], (ConstString)env.message.MessageArbitrary.body);
        
        sysReply(from.value, &env);

        sysSend(parent.value, &env, &env);
    }
}

void updateLogUi(U8 index)
{
    printf("\033[s\033[%d;2H%100s\033[u", index + 1 + previousHead, rollingBuffer[previousHead]);
    printf("\033[s\033[%d;2H\033[2K\033[1m%100s\033[m\r\n\033[2K\033[u", index + 1 + head, rollingBuffer[head]);
    
    previousHead = head;
    head = (head + 1) % LOG_LIST_COUNT;
}

void LogDisplay(void)
{
    TaskID id = VAL_TO_ID(sysCreate(sysPriority(sysTid())-1, &LogServer));

    U8 index = 0;

    MessageEnvelope env;
    TaskID sender;

    for(;;)
    {
        sysReceive(&sender.value, &env);

        if (sender.value == id.value)
        {
            if (index != 0)
            { 
                updateLogUi(index); 
            }
        }
        else
        {
            if (env.type == MESSAGE_NOTIFY)
            {
                index = env.message.MessageU8.body;
                if (index != 0)
                {
                    printf("\033[s\033[%d;80H\033[1mLOG\033[m\033[u", index);
                }
            }
        }

        sysReply(sender.value, &env);
    }
}
