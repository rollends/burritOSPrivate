#ifndef TYPES_H
#define TYPES_H

/// Unsigned 8 bit integer
typedef unsigned char   U8;

/// Signed 8 bit integer
typedef signed char     S8;

/// Unsigned 16 bit integer
typedef unsigned short  U16;

/// Signed 16 bit integer
typedef signed short    S16;

/// Unsigned 32 bit integer
typedef unsigned int    U32;

/// Signed 32 bit integer
typedef signed int      S32;

/// String
typedef char *          String;

/// Constant String 
typedef char const *    ConstString;

/// Read/write register
typedef volatile U32 * const RWRegister;

/// Read-only register
typedef volatile U32 const * const RORegister;


/// Function return type
#ifdef RETURN_CODES
    #define RETURN      S32
#else
    #define RETURN      void
#endif

#endif
