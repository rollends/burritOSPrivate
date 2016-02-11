#include "common/string.h"

U8 isdigit(char c)
{
    return (c >= '0' && c <= '9');
}

void strtolower(String c)
{
    while(*c)
    {
        *c += ( *c >= 'A' && *c <= 'Z' ? ('a' - 'A') : 0 );
        ++c;
    }
}

U32 stratoui(ConstString* s)
{
    U32 i = 0;
    while(**s && isdigit(**s))
    {
        i = 10 * i + (*((*s)++) - '0');
    }
    return i;
}

U8 iswhitespace(char c)
{
    return (c <= ' ');
}

void strcpy(String d, ConstString s)
{
    while( (*(d++) = *(s++)) );
}

void strskipws(ConstString* str)
{
    while(**str && iswhitespace(**str)) { (*str)++; }
}

U32 strlen(ConstString str )
{
    ConstString c = str;
    while( *c ) c++;
    return c - str;
}
