#ifndef TRAIN_MATH_H
#define TRAIN_MATH_H

/**
 * Newton's method computation of sqrt
 */
static inline intSqrt(const U32 i)
{
    U32 k = i;

    do 
    {
        U32 temp = k;
        k = (k + (val/k)) >> 1;

        S32 delta = k - temp;
        if (delta < 1 && delta > -1)
            break;
    } while (1);

    return k;
}

#endif