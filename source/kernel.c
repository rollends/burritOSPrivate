#include "memory.h"
#include "types.h"
#include "uart.h"

extern U32 sys_call(U32, U32, U32, U32);
extern U32* enterTask(U32*);

char x2c(int c)
{
    if (c <= 9)
        return '0' + c;

    return 'a' + c - 10;
}

int printHex(U32 value)
{
    int byte;
    char chh, chl;
    unsigned char *str = (unsigned char*)&value;

    uartWriteChar(UART_PORT_2, '0');
    uartWriteChar(UART_PORT_2, 'x');

    for (byte = 3; byte >= 0; byte--)
    {
        unsigned char c = str[byte];
        chh = x2c(c / 16);
        chl = x2c(c % 16);

        uartWriteChar(UART_PORT_2, chh);
        uartWriteChar(UART_PORT_2, chl);
    }

    uartWriteChar(UART_PORT_2, '\r');
    uartWriteChar(UART_PORT_2, '\n');

    return 0;
}

void task1()
{
    U32 speed = 1;
    while (1)
    {
        uartWriteChar(UART_PORT_2, '\r');
        uartWriteChar(UART_PORT_2, '\n');
        uartWriteChar(UART_PORT_2, speed + '0');
        uartWriteChar(UART_PORT_2, '\r');
        uartWriteChar(UART_PORT_2, '\n');

        int test = 10;
        while (test-- > 0)
        {
            volatile int i;
            for (i = 0; i < 100000* speed; i++)
            {
                asm volatile("nop");
            }

            uartWriteChar(UART_PORT_2, 'a');

            for (i = 0; i < 100000* speed; i++)
            {
                asm volatile("nop");
            }

            uartWriteChar(UART_PORT_2, 'b');
        }


        uartWriteChar(UART_PORT_2, '\r');
        uartWriteChar(UART_PORT_2, '\n');

        speed = sys_call(0, 0, 0, 0);

        uartWriteChar(UART_PORT_2, '\r');
        uartWriteChar(UART_PORT_2, '\n');
        uartWriteChar(UART_PORT_2, speed + '0');
        uartWriteChar(UART_PORT_2, '\r');
        uartWriteChar(UART_PORT_2, '\n');
        test = 10;
        while (test-- > 0)
        {
            volatile int i;
            for (i = 0; i < 50000* speed; i++)
            {
                asm volatile("nop");
            }

            uartWriteChar(UART_PORT_2, 'c');

            for (i = 0; i < 50000* speed; i++)
            {
                asm volatile("nop");
            }

            uartWriteChar(UART_PORT_2, 'd');
        }

        uartWriteChar(UART_PORT_2, '\r');
        uartWriteChar(UART_PORT_2, '\n');
        
        speed = sys_call(0, 0, 0, 0);
    }
}

int kernelMain(int* pc)
{
    uartSpeed(UART_PORT_2, UART_SPEED_HI);
    uartConfig(UART_PORT_2, 0, 0, 0);                                                      

    uartWriteChar(UART_PORT_2, 27);
    uartWriteChar(UART_PORT_2, '[');
    uartWriteChar(UART_PORT_2, '2');
    uartWriteChar(UART_PORT_2, 'J');
    uartWriteChar(UART_PORT_2, '\r');

    U32 task_stack[512];
    task_stack[496] = 0x10;
    task_stack[497] = 15;
    task_stack[498] = 0;
    task_stack[499] = 1;
    task_stack[500] = 2;
    task_stack[501] = 3;
    task_stack[502] = 4;
    task_stack[503] = 5;
    task_stack[504] = 6;
    task_stack[505] = 7;
    task_stack[506] = 8;
    task_stack[507] = 9;
    task_stack[508] = 10;
    task_stack[509] = 11;
    task_stack[510] = 12;
    task_stack[511] = (U32)(&task1) + (U32)(pc);

    U32* sp = &(task_stack[496]);
    while(1)
    {
        S8 c;
        uartReadChar(UART_PORT_2, &c);

        if (c >= 0)
        {
            if (c == 0x0D)
            {
                uartWriteChar(UART_PORT_2, 0x0A);
            }

            if (c == 'g')
            {
                int i;
                for (i = 0; i < 16; i++)
                {
                    printHex(*(sp + i));
                }
                printHex((U32)sp);

                sp = enterTask(sp);
                
                for (i = 0; i < 16; i++)
                {
                    printHex(*(sp + i));
                }
                printHex((U32)sp);
            }
            else
            {
                uartWriteChar(UART_PORT_2, c);
                if (c == 'q')
                {
                    break;
                }
                else
                {
                    *(sp + 2) = c - '0';
                }
            }
        }
    }

    return 0;
}
