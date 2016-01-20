#include "print.h"
#include "uart.h"

char x2c(int c)
{
    if (c <= 9)
    {
        return '0' + c;
    }

    return 'a' + c - 10;
}

int printHex(U32 value)
{
    int byte;
    char chh, chl;
    unsigned char *str = (unsigned char*)&value;

    uartWriteByte(UART_PORT_2, '0');
    uartWriteByte(UART_PORT_2, 'x');

    for (byte = 3; byte >= 0; byte--)
    {
        unsigned char c = str[byte];
        chh = x2c(c / 16);
        chl = x2c(c % 16);

        uartWriteByte(UART_PORT_2, chh);
        uartWriteByte(UART_PORT_2, chl);
    }

    uartWriteByte(UART_PORT_2, '\r');
    uartWriteByte(UART_PORT_2, '\n');

    return 0;
}

int printString(char* str)
{
    while (*str)
    {
        uartWriteByte(UART_PORT_2, *str);
        str++;
    }

    return 0;
}
