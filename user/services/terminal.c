#include "common/string.h"
#include "kernel/kernel.h"
#include "user/messageTypes.h"
#include "user/services/terminal.h"

void putc(TaskID server, char c)
{
    MessageEnvelope envelope;
    envelope.type = MESSAGE_IO_PUTC;
    envelope.message.MessageU8.body = c;
    sysSend(server.value, &envelope, &envelope);
}

char getc(TaskID server)
{
    MessageEnvelope envelope;
    envelope.type = MESSAGE_IO_GETC;
    sysSend(server.value, &envelope, &envelope);
    return envelope.message.MessageU8.body;
}

static int putStr(TaskID server, ConstString str)
{
    MessageEnvelope envelope;
    envelope.type = MESSAGE_IO_PUTSTR;
    envelope.message.MessageArbitrary.body = (U32*)str;
    sysSend(server.value, &envelope, &envelope);
    return (envelope.message.MessageU32.body == 1 ? -1 : 0);
}

static char _x2c(int c)
{
    if (c <= 9)
    {
        return '0' + c;
    }

    return 'a' + c - 10;
}

static int _printDecimal(String* output, U32 value, U32 width, const char delim, const char comma)
{
    char buffer[16];
    U8 i = 0;
    do
    {
        U8 d = value % 10;
        buffer[i++] = _x2c(d);
        value /= 10;
    } while( value );

    U8 ci = 0;
    U8 ti = i + (i-1) / 3;
    while( width > ti )
    {
        *((*output)++) = delim;
        width--;
    }
    for(ci = 0; ci < i; ci++)
    {
        U32 index = i - ci - 1;
        *((*output)++) = buffer[index];
        if (index > 0 && index % 3 == 0 && comma != 0)
        {
            *((*output)++) = ',';
        }
    }
    return 0;
}

static int _printHex(String* output, U32 value)
{ 
    int byte;
    char chh, chl;
    unsigned char *str = (unsigned char*)&value;

    *((*output)++) = '0';
    *((*output)++) = 'x';

    for (byte = 3; byte >= 0; byte--)
    {
        unsigned char c = str[byte];
        chh = _x2c(c / 16);
        chl = _x2c(c % 16);

        *((*output)++) = chh;
        *((*output)++) = chl;
    }

    return 0;
}

static int _printHexByte(String *output, U8 value)
{
    *((*output)++) = '0';
    *((*output)++) = 'x';
    *((*output)++) = _x2c(value / 16);
    *((*output)++) = _x2c(value % 16);
    return 0;
}

static int _printStringNoFormat(String* output, char const * str)
{
    char c;
    while( (c = *(str++)) )
        *((*output)++) = c;
    return 0;
}

S32 printf(TaskID server, ConstString format, ...)
{    
    enum { PLAIN, FORMAT, ESCAPE } state = PLAIN;
	U32 widthFlag = 0;
    char ch;
    
    char OutputBuffer[512];
    String OutputBufferI = OutputBuffer;

    assert( strlen(format) <= 512 );

    va_list va;
    va_start(va, format);
    while ((ch = *(format++)))
    {
        switch( state )
        {
        case ESCAPE:
        {
            *(OutputBufferI++) = ch;
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
                *(OutputBufferI++) = ch;
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
                    _printDecimal(&OutputBufferI,va_arg(va, U32), widthFlag, '0', 0);
                    break;

                case 'n':
                    _printDecimal(&OutputBufferI,va_arg(va, U32), widthFlag, ' ', 1);
                    break;

                case 's':
                    _printStringNoFormat(&OutputBufferI, va_arg(va, ConstString) );
                    break;

                case 'c':
                    *(OutputBufferI++) = va_arg(va, S32);
                    break;

                case 'x':
                    _printHex(&OutputBufferI,va_arg(va, U32));
                    break;

                case 'b':
                    _printHexByte(&OutputBufferI,va_arg(va, U8));
                    break;

                case '%':
                    *(OutputBufferI++) = '%';
                    break;

                default:
                    *(OutputBufferI++) = '%';
                    _printDecimal(&OutputBufferI,widthFlag, 0, '0', 0);
                    *(OutputBufferI++) = ch;
                    break;
                }
                state = PLAIN;
            }
            break;
        }
        }
    }
    *OutputBufferI = '\0';

    assert( (OutputBufferI - OutputBuffer) <= 512 );
    
    va_end(va);

    assert( state == PLAIN );

    return putStr(server, OutputBuffer);
}
