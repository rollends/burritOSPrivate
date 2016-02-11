#ifndef STRING_H
#define STRING_H

#include "common/types.h"

U32 stratoui(ConstString*);

U32 strlen(ConstString);

void strcpy(String, ConstString);

void strtolower(String);

void strskipws(ConstString* str);

#endif
