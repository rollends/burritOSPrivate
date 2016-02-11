#ifndef ERROR_H
#define ERROR_H

#define OK              0

#define ERROR_GENERIC   -1
#define ERROR_NULL      -2
#define ERROR_LEN       -3
#define ERROR_FULL      -4
#define ERROR_EMPTY     -5
#define ERROR_RANGE     -6

#ifdef NULL_CHECK
    #define IS_NOT_NULL(x) do {if ((x) == 0) return ERROR_NULL;} while(0);
#else
    #define IS_NOT_NULL(x)
#endif

#ifdef BOUNDS_CHECK
    #define IS_POW_2(x) do {if (!((x) && !((x) & ((x) - 1)))) return ERROR_LEN;} while(0)
    #define IS_ZERO(x) do {if ((x) != 0) return ERROR_LEN;} while(0)
    #define IS_NOT_ZERO(x) do {if ((x) == 0) return ERROR_LEN;} while(0)
#else
    #define IS_POW_2(x)
    #define IS_ZERO(x)
    #define IS_NOT_ZERO(x)
    #define IS_FULL(x)
#endif

#ifdef RANGE_CHECK
    #define IS_IN_RANGE(x) do {if ((U32)(x) >= 0x01fdd000 || (U32)(x) <= 0x50000) return ERROR_RANGE;} while(0)
#else
    #define IS_IN_RANGE(x)
#endif

#ifdef RETURN_CODES
    #define IS_OK() return OK
#else
    #define IS_OK()
#endif
#endif
