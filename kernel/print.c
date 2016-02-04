#include "common/vaList.h"
#include "hardware/hardware.h"
#include "kernel/print.h"

char x2c(int c)
{
    if (c <= 9)
    {
        return '0' + c;
    }

    return 'a' + c - 10;
}

int printDecimal(U32 value)
{
    char buffer[16];
    U8 i = 0;
    do
    {
        U8 d = value % 10;
        buffer[i++] = x2c(d);
        value /= 10;
    } while( value );

    U8 ci = 0;
    for(ci = 0; ci < i; ci++)
    {
        uartWriteByte( UART_2, buffer[i - ci - 1] );
    }
    return 0;
}

int printHex(U32 value)
{
    int byte;
    char chh, chl;
    unsigned char *str = (unsigned char*)&value;

    uartWriteByte(UART_2, '0');
    uartWriteByte(UART_2, 'x');

    for (byte = 3; byte >= 0; byte--)
    {
        unsigned char c = str[byte];
        chh = x2c(c / 16);
        chl = x2c(c % 16);

        uartWriteByte(UART_2, chh);
        uartWriteByte(UART_2, chl);
    }

    return 0;
}

int printHexByte(U8 value)
{
    uartWriteByte(UART_2, '0');
    uartWriteByte(UART_2, 'x');
    uartWriteByte(UART_2, x2c(value / 16));
    uartWriteByte(UART_2, x2c(value % 16));

    return 0;
}

int printString(char* format, ...)
{
    va_list va;
    va_start(va, format);

    char ch;
    while ((ch = *(format++)))
    {
        int ret = 0;

        if (ch != '%')
        {
            ret = uartWriteByte(UART_2, ch);
        }
        else
        {
            ch = *(format++);
            switch (ch)
            {
                case 'c':
                    ret = uartWriteByte(UART_2, va_arg(va, S32));
                    break;

                case 'd':
                    ret = printDecimal(va_arg(va, U32));
                    break;

                case 'x':
                    ret = printHex(va_arg(va, U32));
                    break;

                case 'b':
                    ret = printHexByte(va_arg(va, U8));
                    break;

                case '%':
                    ret = uartWriteByte(UART_2, ch);
                    break;

                default:
                    break;
            }
        }
    }

    va_end(va);
    return 0;
}
