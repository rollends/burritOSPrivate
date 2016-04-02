#include "kernel/kernel.h"

#include "user/display/TacoDisplay.h"
#include "user/services/services.h"
#include "user/messageTypes.h"

void TacoDisplay()
{
    MessageEnvelope env;
    TaskID sender;

    for(;;)
    {
        sysReceive(&sender.value, &env);
        sysReply(sender.value, &env);

        if (env.type == MESSAGE_NOTIFY)
        {
            U8 index = env.message.MessageU8.body;
            if (index != 0)
            {
                printf("\033[s\033[%d;35H..................... \r\n\033[u", 4+index);
                printf("\033[s\033[%d;35H..................... \r\n\033[u", 5+index);
                printf("\033[s\033[%d;35H........\033[1m╭╯╭╯╭╯\033[m....... \r\n\033[u", 6+index);
                printf("\033[s\033[%d;35H.......\033[1m______\033[m   ..... \r\n\033[u", 7+index);
                printf("\033[s\033[%d;35H......\033[1m╱      \\╮╲\033[m..... \r\n\033[u", 8+index);
                printf("\033[s\033[%d;35H.....\033[1m╱  ^  ^  ╲╮\033[m╲.... \r\n\033[u", 9+index);
                printf("\033[s\033[%d;35H.....\033[1m▏        ▕╮▕\033[m.... \r\n\033[u", 10+index);
                printf("\033[s\033[%d;35H.....\033[1m▏ \\____/ ▕╮▕\033[m.... \r\n\033[u", 11+index);
                printf("\033[s\033[%d;35H.....\033[1m╲_________╲╱\033[m.... \r\n\033[u", 12+index);
                printf("\033[s\033[%d;35H..................... \r\n\033[u", 13+index);
                printf("\033[s\033[%d;35H.......\033[1mburritOS\033[m...... \r\n\033[u", 14+index);
            }
        }
    }
}


