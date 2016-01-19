#include "uart.h"

char x2c(int c)
{
    if (c <= 9)
        return '0' + c;

    return 'a' + c - 10;
}

int printHex(unsigned int value)
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

extern void load_vector(int dst, int src);

int main(int* pc)
{
    uartSpeed(UART_PORT_2, UART_SPEED_HI);
    uartConfig(UART_PORT_2, 0, 0, 0);                                                      

    int* mem = 0;
    printHex(*mem);

    return 0;
}
