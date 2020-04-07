#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <values.h>

#define RT_FIX_OV 1
#define RT_FIX_UV 2
#define RT_FIX_UNDEFINED 4
#define RT_FIXWL 31

typedef struct _rt_fix {
    long _value;         /* value of this object                          */
    unsigned short _iwl; /* integer word-length => bits for the mantissa  */
    unsigned short _errorset; /* set for recording exceptions */
} rt_fix;
long elapse(struct timespec start, struct timespec end)
{
    return ((long) 1.0e+9 * end.tv_sec + end.tv_nsec) -
           ((long) 1.0e+9 * start.tv_sec + start.tv_nsec);
}
short rt_fix_errmergeset(rt_fix *target, rt_fix source)
{
    if (!target) {
        return -1;
    }
    target->_errorset |= source._errorset;
    return 0;
}
short rt_fix_sign(rt_fix arg)
{
    if (arg._value &= MINLONG) {
        return 1;
    } else {
        return 0;
    }
}
void rt_fix_min_iwl(rt_fix *fix)
{
    unsigned long mask = 1073741824; /* = 2^30 */

    if (fix) {
        /*
         * integer word length of fix is minimal
         */
        if (fix->_iwl < 2) {
            return;
        }
        /*
         * test if fix is negative
         */
        if (fix->_value < 0) {
            fix->_value = ~fix->_value;
            while (!(fix->_value & mask) && fix->_iwl > 1) {
                fix->_value <<= 1;
                fix->_iwl--;
            }
            fix->_value = ~fix->_value;
        } else {
            while (!(fix->_value & mask) && fix->_iwl > 1) {
                fix->_value <<= 1;
                fix->_iwl--;
            }
        }
    }
}
double rt_fix_tod(rt_fix fix)
{
    double result;

    result = (double) fix._value;
    return (result / (1 << (RT_FIXWL - fix._iwl)));
}
rt_fix rt_fix_mul(rt_fix x, rt_fix y, int i)
{
    unsigned short iwl = x._iwl + y._iwl;

    /*
     * let the result appear in x.
     */
    struct timespec t1, t2;

    rt_fix_errmergeset(&x, y);
    /*if(iwl > RT_FIXWL) {
        if(rt_fix_sign(x) ^ rt_fix_sign(y)) {
            x = rt_fix_min;
        } else {
            x = rt_fix_max;
        }
        rt_fix_errmergeset(&x, y);
        rt_fix_erraddset(&x, RT_FIX_OV);
        return x;
    }*/
    clock_gettime(CLOCK_REALTIME, &t1);
    if (y._iwl < iwl) {
        y._value >>= x._iwl;
    }
    if (x._iwl < iwl) {
        x._value >>= y._iwl;
    }
    x._iwl = iwl;
    __asm__(
        "imull %%ebx\n\t" /* Multiply a * b */
        "shrdl %%cl, %%edx, %%eax\n\t"
        : "=a"(x._value)
        : "a"(x._value), "b"(y._value), "c"(RT_FIXWL - x._iwl)
        : "edx");
    rt_fix_min_iwl(&x);
    // printf("- %ld -",x._value);
    clock_gettime(CLOCK_REALTIME, &t2);
    printf("%d ", i);
    printf("%ld\n", elapse(t1, t2));
    return x;
}

short rt_fix_erraddset(rt_fix *fix, unsigned short errnum)
{
    if (!fix) {
        return -1;
    }
    switch (errnum) {
    case RT_FIX_OV:
    case RT_FIX_UV:
    case RT_FIX_UNDEFINED:
        fix->_errorset |= errnum;
        return 0;
    default:
        return -1;
    }
}
void fromfix(int tar)
{
    int num = tar >> 4;
    int frac = tar & 15;
    int neg = (frac & 8) >> 3;
    if (neg > 0)
        frac = -1 * ((~frac & 15) + 1);
    // printf("%lf\n",(double)num * pow(10,(double)frac));
}

double rt_fromfix(int tar)
{
    double result;
    result = (double) tar;
    return result / (1 << (31 - 14));
    // printf("%lf ",result);
}

int main()
{
    for (int i = 0; i < 1000; i++) {
        rt_fix a = {0, 0, 0}, b = {0, 0, 0}, c;
        a._value = 61526515;
        a._iwl = 14;
        b._iwl = 14;
        b._value = 54545454;
        // printf("%lf * %lf = ",rt_fix_tod(a),rt_fix_tod(b));
        c._iwl = 14;
        c = rt_fix_mul(a, b, i);
        // printf("%lf\n",rt_fix_tod(c));
    }
    return 0;
}