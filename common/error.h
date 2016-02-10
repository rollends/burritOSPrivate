#ifndef ERROR_H
#define ERROR_H

#define OK              0

#define ERROR_GENERIC   -1
#define ERROR_NULL      -2
#define ERROR_LEN       -3
#define ERROR_FULL      -4

#ifdef NULL_CHECK
    #define IS_NOT_NULL(x) do {if (x == 0) return ERROR_NULL;} while(0);
#else
    #define IS_NOT_NULL(x)
#endif

#ifdef BOUNDS_CHECK
    #define IS_POW_2(x) do {if (!(x && !(x & (x - 1)))) return ERROR_LEN;} while(0)
    #define IS_ZERO(x) do {if (x != 0) return ERROR_LEN;} while(0)
    #define IS_NOT_ZERO(x) do {if (x == 0) return ERROR_LEN;} while(0)
#else
    #define IS_POW_2(x)
    #define IS_ZERO(x)
    #define IS_NOT_ZERO(x)
#endif

#endif
