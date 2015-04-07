/*-----------Fixed Lib--------------------------------
    This namespace defines a set of useful functions for
    our fixed point arithmetic.
*/
#ifndef FIXEDLIB
#define FIXEDLIB

#define MAX16    32768

namespace FixedLib
{
    /*---------Multiply 16----------------------------------------
      Performs a multiply on two INT16 numbers and returns an
      INT16 result
    */
    inline int mult16(int m1, int m2)
    {
        long temp;
        temp = (long)m1 * m2;
        return (temp + (1<<14)) >> 15;
    }

    /*---------Abs16---------------------------------------
      Takes the absolute value of a 16bit number using bitwise operations
    */
    inline int abs16(int x)
    {
        int mask = x >> 15;
        return (x ^ mask) - mask;
    }
}
#endif
