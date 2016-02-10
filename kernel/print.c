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

int printDecimal(U32 value, U32 width, const char delim, const char comma)
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
    U8 ti = i + (i-1) / 3;
    while( width > ti )
    {
        uartWriteByteBlock(UART_2, delim);
        width--;
    }
    for(ci = 0; ci < i; ci++)
    {
        U32 index = i - ci - 1;
        uartWriteByteBlock( UART_2, buffer[index] );
        if (index > 0 && index % 3 == 0 && comma != 0)
        {
            uartWriteByteBlock(UART_2, ',');
        }
    }
    return 0;
}

int printHex(U32 value)
{
    int byte;
    char chh, chl;
    unsigned char *str = (unsigned char*)&value;

    uartWriteByteBlock(UART_2, '0');
    uartWriteByteBlock(UART_2, 'x');

    for (byte = 3; byte >= 0; byte--)
    {
        unsigned char c = str[byte];
        chh = x2c(c / 16);
        chl = x2c(c % 16);

        uartWriteByteBlock(UART_2, chh);
        uartWriteByteBlock(UART_2, chl);
    }

    return 0;
}

int printHexByte(U8 value)
{
    uartWriteByteBlock(UART_2, '0');
    uartWriteByteBlock(UART_2, 'x');
    uartWriteByteBlock(UART_2, x2c(value / 16));
    uartWriteByteBlock(UART_2, x2c(value % 16));

    return 0;
}

int printStringNoFormat(char const * str)
{
    char c;
    while( (c = *(str++)) )
    {
        uartWriteByteBlock(UART_2, c);
    }

    return 0;
}

int printBlocking(char const * format, ...)
{
    va_list va;
    va_start(va, format);

    enum { PLAIN, FORMAT, ESCAPE } state = PLAIN;

	U32 widthFlag = 0;
    char ch;
    while ((ch = *(format++)))
    {
        switch( state )
        {
        case ESCAPE:
        {
            uartWriteByteBlock(UART_2, ch);
            break;
        }

        case PLAIN:
        {
            if (ch == '%')
            {
                state = FORMAT;
                widthFlag = 0;
            }
            else if( ch == '\\' )
                state = ESCAPE;
            else
                uartWriteByteBlock(UART_2, ch);
            break;
        }
        case FORMAT:
        {
            if( ch >= '0' && ch <= '9' )
                widthFlag = widthFlag * 10 + (ch - '0');
            else
            {
                switch( ch )
                {
                case 'd':
                    printDecimal(va_arg(va, U32), widthFlag, '0', 0);
                    break;

                case 'n':
                    printDecimal(va_arg(va, U32), widthFlag, ' ', 1);
                    break;

                case 's':
                    printStringNoFormat( va_arg(va, char const *) );
                    break;

                case 'c':
                    uartWriteByteBlock(UART_2, va_arg(va, S32));
                    break;

                case 'x':
                    printHex(va_arg(va, U32));
                    break;

                case 'b':
                    printHexByte(va_arg(va, U8));
                    break;

                case '%':
                    uartWriteByteBlock(UART_2, '%');
                    break;

                default:
                    uartWriteByteBlock(UART_2, '%');
                    printDecimal(widthFlag, 0, '0', 0);
                    uartWriteByteBlock(UART_2, ch);
                    break;
                }
                state = PLAIN;
            }
            break;
        }
        }
    }

    va_end(va);
    return (state == PLAIN ? 0 : -1);
}

void printTaco()
{
    printStringNoFormat("\r\n .................... \r\n");
    printStringNoFormat(" .......╭╯╭╯╭╯....... \r\n");
    printStringNoFormat(" ......______   ..... \r\n");
    printStringNoFormat(" .....╱      \\╮╲..... \r\n");
    printStringNoFormat(" ....╱  x  x  ╲╮╲.... \r\n");
    printStringNoFormat(" ....▏  ____  ▕╮▕.... \r\n");
    printStringNoFormat(" ....▏ /    \\ ▕╮▕.... \r\n");
    printStringNoFormat(" ....╲_________╲╱.... \r\n");
    printStringNoFormat(" .................... \r\n");
    printStringNoFormat(" .....SAD..TACO...... \r\n");
    printStringNoFormat(" .................... \r\n\r\n");
}
