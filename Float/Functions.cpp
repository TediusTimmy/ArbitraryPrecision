/*
   This file contains code that has been removed from SlowCalc,
   also by Thomas DiModica.

   Copyright (C) 2010, 2011 Thomas DiModica <ricinwich@yahoo.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Float.hpp"
#include "Constants.hpp"

namespace BigInt
 {

#define SERIESEXTRA 8
#define DERIVEDEXTRA 4

/*
==============================================================================
   Function: square root
------------------------------------------------------------------------------
   INPUT: number to take the square root of
   OUTPUT: the square root of the input
   NOTES:
      Caved to IEEE: sqrt(-0) = -0 now.

      Newton-Rhapson is much faster than x ^ .5,
      which becomes e ^ (.5 * ln(x)).
==============================================================================
*/
   Float sqrt (const Float & opp)
    {
      Float copyOpp (opp);

      if (opp.isZero()) return opp;
      if (opp.isSigned()) return (copyOpp = M_NaN);
      if (opp.isNaN() || opp.isInfinity()) return opp;

      copyOpp.changePrecision(opp.getPrecision() + SERIESEXTRA);

      long diff, term = opp.getPrecision() + DERIVEDEXTRA;

      Float temp (copyOpp);
      Float curApprox (copyOpp);
      Float lastApprox (copyOpp);

         //This helps to produce a good first estimate, and is why
         //this function has to be a friend.
      curApprox.Exponent /= 2;
      do
       {
         lastApprox = curApprox;

          /*
            As this is the first occurrance of this major change, I will
            describe it. As the DecFloat from SlowCalc didn't escalate
            precision, these functions were written with that in mind. It is
            part of how they behave: they return an value with the same
            precision as their argument. They also expect basic math
            operations to return a value with a precision that is the max of
            the two argument's precisions.

            Here, it is difficult to track the escalation, so let us consider
            pow: we don't do exp(log(base) * exponent), as that would make exp
            produce a result with 2 * DERIVEDEXTRA extra digits of precision
            instead of the desired DERIVEDEXTRA. The same escalation is at
            work here; so, we unwind the math to keep this escalation in
            check.
          */
         temp = curApprox * curApprox;
         temp = temp + copyOpp;
         curApprox += curApprox;
         curApprox = temp / curApprox;

         temp = curApprox - lastApprox;

         diff = curApprox.Exponent - temp.Exponent;
         if (diff < 0) diff = -diff;
       }
      while ((diff <= term) && !temp.isZero());

      curApprox.changePrecision(opp.getPrecision());
      return curApprox;
    }

/*
==============================================================================
   Function: change
------------------------------------------------------------------------------
   NOTES:
      Returns true while adding arg1 and arg2 does not return either
      arg1 or arg2 unchanged. It is used to control loops.
==============================================================================
*/
   bool change (const Float & arg1, const Float & arg2)
    {
      if (!arg1.isUnSpecial() || !arg2.isUnSpecial())
         return false;

      unsigned long prec, diff;

      prec = arg1.getPrecision() >= arg2.getPrecision() ?
         arg1.getPrecision() :
         arg2.getPrecision();

      diff = arg1.exponent() > arg2.exponent() ?
         arg1.exponent() - arg2.exponent() :
         arg2.exponent() - arg1.exponent();

         //Account for a rounding digit.
      if (diff > (prec + 1)) return false;
      return true;
    }

/*
==============================================================================
   Function: exponential function, Euler's constant to a real power
------------------------------------------------------------------------------
   NOTES:
      Basically: the Maclaurin series approximation for e ^ x

      I made one slight change: I noticed poor convergence and
      premature loop termination with negative exponents, so I
      force the exponent to be positive, then divide one by
      the result if it was originally negative.
      This has fixed my problem.

      I had a similar problem with sine and cosine with all large
      numbers (numbers with great magnitude), and the problem
      is/was that as the series goes on, the terms become the very
      small difference between very large numbers, and so there is
      a very big error due to round-off.

      I had an epiphany today, thanks to R. Brent in "Fast Multiple-Precision
      Evaluation of Elementary Functions":
         exp(x * 10 ^ y) = (exp(x * 10 ^ (y - 1))) ^ 10 = (exp(x)) ^ (10 ^ y)
      This works SO MUCH BETTER for base 2. It reduces the range and would be
      easy to implement as loops. My concern is the accuracy after repeated
      multiplications (probably negligable) and if it really speeds things up.
==============================================================================
*/
   Float exp (const Float & opp)
    {
      Float one (opp);

      if (opp.isZero()) return (one = M_1);
      else if (opp.isNaN()) return opp;
      else if (opp.isInfinity())
       {
         if (!opp.isSigned()) return opp;
         one = Float("0");
         return one;
       }

      one.changePrecision(opp.getPrecision() + SERIESEXTRA * 2);
      one = M_1;

      Float currentFactorial (one);
      Float currentDivisor (one);
      Float currentPower (one);
      Float copyOpp (one);
      Float res (one);
      Float temp (one);
      bool special = opp.Exponent > -3;//Three is specially chosen to maximize
      long loops = opp.Exponent + 3;//speed within the current extra precision

      currentFactorial = Float("2");//while maintaining accuracy
      currentDivisor = Float("2");
      currentPower = opp;
      copyOpp = opp;

      currentPower.abs();
      copyOpp.abs();
      if (special)
       {
         currentPower.Exponent = -3;
         copyOpp.Exponent = -3;
       }


      res += currentPower;
      currentPower *= copyOpp;

      temp = currentPower / currentDivisor;
      while (change(temp, res))
       {
         res += temp;
         currentFactorial += one;
         currentDivisor *= currentFactorial;
         currentPower *= copyOpp;
         temp = currentPower / currentDivisor;
       }

      if (special)
       {
         /*
            This loop can only execute ten times before the exponent exceeds
            1000000000. It requires the extra precision given.
         */
         while (loops-- && !res.Infinity)
          {
            temp = res;
            temp *= temp; //2
            temp *= temp; //4
            res *= temp;  //5
            res *= res;   //10
          }
       }

      if (opp.isSigned()) res = one / res;

      res.changePrecision(opp.getPrecision());
      return res;
    }

/*
==============================================================================
   Function: natural logrithm
------------------------------------------------------------------------------
   NOTES:
      log(x) = 2 * sum (i = 1 to Inf)  of
         (((i - 1) / (i + 1)) ^ (2 * i - 1)) / (2 * i - 1)

      This started general-purpose, but the series for ln has poor
      convergence outside of (.5,2). Remove all areas labeled new code
      to get the log function that can compute ln(10) itself.
      You should probably use the equation in the header, though,
      it converges faster and can be used here,
      without changing the function.

      So, by basic laws of logrithms:
         ln(x * 10 ^ y) = ln(x) + y * ln(10)
      Predefining ln(10) may limit the precision to that definition,
      but it speeds things up.

      As convergence is best at one and decays around one such that
      the convergence of x is as bad as 1 / x, we try to force
      numbers into an area of better convergence. As it is,
      x > sqrt(10) converges slower than x / 10. To speed things up,
      if (x > 3) we use x / 10 instead of x. This puts pi in the
      region of worst convergence, but everywhere else will be faster.

      Update: we will use one of three modes:
         [1, 1.78)      ln(x) + y * ln(10)
         [1.78, 5.62)   ln(x / sqrt(10)) + (y + .5) * ln(10)
         [5.62, 10)     ln(x / 10) + (y + 1) * ln(10)

      We can continue to use reductions to the argument to keep the actual
      calculation within some range of fast convergence, however it has
      diminishing returns.
==============================================================================
*/
   Float log (const Float & opp)
    {
      Float one (opp);

      if (opp.isZero()) return (one = -M_Inf);
      if (opp.isSigned()) return (one = M_NaN);
      if (opp.isNaN() || opp.isInfinity()) return opp;

      one.changePrecision(opp.getPrecision() + SERIESEXTRA);
      one = M_1;

      Float currentDivisor (one);
      currentDivisor = Float("3");
      Float copyOpp (one);
      copyOpp = opp;

      Float currentPower (one);
      Float res (one);
      Float temp (one);
      long mode;

      Fixed msDigits;

      msDigits = opp.Data;
      msDigits.changePrecision(msDigits.getPrecision() + 8);
      msDigits.setPrecision(msDigits.getPrecision() - 8);

      mode = msDigits.roundToInteger().toInt();

      if (mode < 177827941) /* 100000000 .. 177827940 */
       {
         copyOpp.Exponent = 0;
         mode = 1;
       }
      else if (mode < 562341325) /* 177827941 .. 562341324 */
       {
         copyOpp.Exponent = 0;
         temp = M_1_SQRT10;
         copyOpp *= temp;
         mode = 2;
       }
      else /* 562341325 .. 999999999 */
       {
         copyOpp.Exponent = -1;
         mode = 3;
       }

      res = copyOpp - one;
      temp = copyOpp + one;

      currentPower = res / temp;
      copyOpp = currentPower;
      res = currentPower;
      one = Float("2");

      copyOpp *= copyOpp;
      currentPower *= copyOpp;

      temp = currentPower / currentDivisor;
      while (change(temp, res))
       {
         res += temp;
         currentDivisor += one; //which is now 2
         currentPower *= copyOpp;
         temp = currentPower / currentDivisor;
       }
      res *= one;

      switch (mode)
       {
         case 1:
            if (opp.Exponent == 0) break;
            temp = Float(Integer((long long) opp.Exponent).toString());
            one = M_LN10;

            temp *= one; //which is now ln(10)
            res += temp;
            break;
         case 2:
            temp = Float(Integer((long long) opp.Exponent).toString());
            temp += Float("5E-1");
            one = M_LN10;

            temp *= one;
            res += temp;
            break;
         case 3:
            if (opp.Exponent == -1) break;
            temp = Float(Integer((long long) opp.Exponent + 1).toString());
            one = M_LN10;

            temp *= one;
            res += temp;
            break;
       }

      res.changePrecision(opp.getPrecision());
      return res;
    }

/*
==============================================================================
   Function: exponentiation
------------------------------------------------------------------------------
   NOTES:
      This is simply x ^ y = e ^ (y * ln(x))
      It doesn't work for -x, so (-1) ^ 2 = NaN.

      1^Inf becomes an NaN naturally by this definition.

      We define 0^0 = 1 because:
         1. It follows that this must be the case from the empty product
            definition for x ^ 0.
         2. The functions exp and cos use this identity in their Maclaurin
            series definitions implicitly.

      This now works rudimentarily for negative numbers. To be better, it
      would have to rationalize the fractional part and decide if that
      exponent is real, however this implies that the rational part is
      precise, which is incorrect for most purposes.
==============================================================================
*/
   Float pow (const Float & left, const Float & right)
    {
      Float base (left), exponent (right), res;
      match(base, exponent);

         //exceptions to the rules
      if (left.isInfinity() && right.isZero()) return (base = M_NaN);
      if (left.isZero() && right.isInfinity() && right.isSigned())
         return (base = M_Inf);

      if (right.isZero()) return (base = M_1);
      if (left.isZero()) return (base = Float("0"));

      if (left.isSigned() && !right.isInteger()) return (base = M_NaN);

      res.setPrecision(base.getPrecision());

      base.changePrecision(base.getPrecision() + DERIVEDEXTRA);
      exponent.changePrecision(exponent.getPrecision() + DERIVEDEXTRA);

      exponent = log(base) * exponent;
      if (left.isSigned())
       {
         base.abs();
         if (right.isOdd()) res = -exp(exponent);
         else res = exp(exponent);
       }
      else
         res = exp(exponent);

      return res;
    }

/*
==============================================================================
   Function: reduce (source file scope only)
------------------------------------------------------------------------------
   NOTES:
      Reduce for sine and cosine.

      This doesn't produce accurate results for numbers with great magnitude.
      One needs exponent extra digits of precision to get the correct
      accuracy.
==============================================================================
*/
   static void reduce (Float & arg)
    {
      Fixed_Round_Mode temp;

      temp = Fixed::getRoundMode();
      Fixed::setRoundMode(ROUND_ZERO);

      Float twopi (arg), oneovertwopi (arg), res (arg);
      unsigned long prec = res.getPrecision();

      twopi = M_2PI;
      oneovertwopi = M_1_2PI;

      for (int loops = 0; (loops < 64) && (arg > twopi); loops++)
       {
         res = arg * oneovertwopi;
         res.changePrecision(res.exponent());
         res.changePrecision(prec);

         arg -= (res * twopi);
       }

      if (arg > twopi)
       {
         arg *= oneovertwopi;
         res = arg;
         res.changePrecision(res.exponent());
         res.changePrecision(prec);
         arg -= res;
         arg *= twopi;
       }

      Fixed::setRoundMode(temp);
    }

/*
==============================================================================
   Function: sine of an angle in radians
------------------------------------------------------------------------------
   NOTES:
      An implementation of the Maclaurin series for sine.
==============================================================================
*/
   Float sin (const Float & opp)
    {
      Float one (opp);

      if (opp.isNaN() || opp.isInfinity()) return (one = M_NaN);

      one.changePrecision(opp.getPrecision() + SERIESEXTRA);
      one = M_1;

      Float currentFactorial (one);
      currentFactorial = Float("3");
      Float currentDivisor (one);
      currentDivisor = Float("6");

      Float temp (one);
      Float copyOpp (one);

      copyOpp = opp;
      reduce(copyOpp.abs()); //We will replace the sign later.

      Float currentPower (copyOpp);
      Float res (copyOpp);

      copyOpp *= copyOpp;
      currentPower *= copyOpp;

      bool alt = true;
      temp = currentPower / currentDivisor;
      while (change(temp, res))
       {
         if (alt) res -= temp;
         else res += temp;
         alt = !alt;

         currentFactorial += one;
         currentDivisor *= currentFactorial;
         currentFactorial += one;
         currentDivisor *= currentFactorial;

         currentPower *= copyOpp;

         temp = currentPower / currentDivisor;
       }

      if (opp.isSigned()) res.negate(); //Sign replaced.

      res.changePrecision(opp.getPrecision());
      return res;
    }

/*
==============================================================================
   Function: cosine of an angle in radians
------------------------------------------------------------------------------
   NOTES:
      An implementation of the Maclaurin series for cosine.
==============================================================================
*/
   Float cos (const Float & opp)
    {
      Float one (opp);

      if (opp.isNaN() || opp.isInfinity()) return (one = M_NaN);

      one.changePrecision(opp.getPrecision() + SERIESEXTRA);
      one = M_1;

      Float currentFactorial (one);
      currentFactorial = Float("2");
      Float currentDivisor (one);
      currentDivisor = Float("2");

      Float temp (one);
      Float copyOpp (one);

      copyOpp = opp; //We don't need to replace the sign as
      reduce(copyOpp.abs()); //cos is symmetric about 0.

      Float currentPower (copyOpp);
      Float res (one);

      currentPower *= copyOpp;
      copyOpp *= copyOpp;

      bool alt = true;
      temp = currentPower / currentDivisor;
      while (change(temp, res))
       {
         if (alt) res -= temp;
         else res += temp;
         alt = !alt;

         currentFactorial += one;
         currentDivisor *= currentFactorial;
         currentFactorial += one;
         currentDivisor *= currentFactorial;

         currentPower *= copyOpp;

         temp = currentPower / currentDivisor;
       }

      res.changePrecision(opp.getPrecision());
      return res;
    }

/*
==============================================================================
   Function: tangent of an angle in radians
------------------------------------------------------------------------------
   NOTES:
      tan(x) = sin(x) / cos(x).
==============================================================================
*/
   Float tan (const Float & opp)
    {
      Float copyOpp (opp);

      if (opp.isNaN() || opp.isInfinity()) return (copyOpp = M_NaN);

      copyOpp.changePrecision(opp.getPrecision() + DERIVEDEXTRA);

      reduce(copyOpp.abs()); //Reduce only once.
      if (opp.isSigned()) copyOpp.negate();

      copyOpp = sin(copyOpp) / cos(copyOpp);

      copyOpp.changePrecision(opp.getPrecision());
      return copyOpp;
    }

/*
==============================================================================
   Function: principle arctangent
------------------------------------------------------------------------------
   NOTES:
      Returns +- pi / 4 on +- 1
      Computes:
         |x| < 1 : sum (i = 1 to Inf)
            ((-1) ^ (i + 1)) * (x ^ (2 * i - 1)) / (2 * i - 1)
         |x| > 1 : pi / 2 - sum (i = 1 to Inf)
            ((-1) ^ i) / (x ^ (2 * i - 1)) / (2 * i - 1)

      Yes, atan really is more important than asin and acos.
      Both will be derived from atan.
==============================================================================
*/
   Float atan (const Float & opp)
    {
      Float one (opp);

      if (opp.isZero()) return one;
      if (opp.isNaN()) return (one = M_NaN);
      if (opp.isInfinity())
       {
         if (opp.isSigned()) return (one = -M_PI_2);
         return (one = M_PI_2);
       }

      one.changePrecision(one.getPrecision() + SERIESEXTRA);
      one = M_1;

      Float currentFactor (one);
      currentFactor = Float("3");

      Float copyOpp (one);
      copyOpp = opp;
      copyOpp.abs(); //Sign will be restored later.
      int Series = copyOpp.compare(one);

      Float res (copyOpp);
      Float currentPower (copyOpp);
      copyOpp *= copyOpp;
      currentPower *= copyOpp;

      Float two (one);
      two = Float("2");

      Float temp (one);
      temp = M_PI_2;

      if (Series == 0)
       {
         //Both of the following series can compute this,
         //but it takes FOREVER!
         res = M_PI_4;
       }
      else if (Series > 0)
       {
         bool alt = false;
         res = one / res;
         res = temp - res;

         temp = currentPower * currentFactor;
         temp = one / temp;
         while (change(temp, res))
          {
            if (alt) res -= temp;
            else res += temp;
            alt = !alt;

            currentFactor += two;

            currentPower *= copyOpp;

            temp = currentPower * currentFactor;
            temp = one / temp;
          }
       }
      else
       {
         bool alt = true;
         temp = currentPower / currentFactor;
         while (change(temp, res))
          {
            if (alt) res -= temp;
            else res += temp;
            alt = !alt;

            currentFactor += two;

            currentPower *= copyOpp;

            temp = currentPower / currentFactor;
          }
       }

      if (opp.isSigned()) res.negate(); //restore sign

      res.changePrecision(opp.getPrecision());
      return res;
    }

/*
==============================================================================
   Function: principle inverse sine
------------------------------------------------------------------------------
   NOTES:
      asin(x) = atan(x / sqrt(1 - x ^ 2))
==============================================================================
*/
   Float asin (const Float & opp)
    {
      Float one (opp);

      if (opp.isNaN() || opp.isInfinity()) return (one = M_NaN);

      one.changePrecision(opp.getPrecision() + DERIVEDEXTRA);
      one = M_1;

      Float copyOpp (one);
      copyOpp = opp;

      if (copyOpp == one)
       {
         Float res (opp);
         res = M_PI_2;
         return res;
       }
      if (copyOpp == -one)
       {
         Float res (opp);
         res = -M_PI_2;
         return res;
       }

      Float res (copyOpp);

      res = copyOpp * copyOpp;
      res = one - res;
      res = copyOpp / sqrt(res);
      res = atan(res);

      res.changePrecision(opp.getPrecision());
      return res;
    }

/*
==============================================================================
   Function: principle inverse cosine
------------------------------------------------------------------------------
   NOTES:
      acos(x) = atan(sqrt(1 - x ^ 2) / x)
==============================================================================
*/
   Float acos (const Float & opp)
    {
      Float one (opp);

      if (opp.isNaN() || opp.isInfinity()) return (one = M_NaN);

      one.changePrecision(opp.getPrecision() + DERIVEDEXTRA);
      one = M_1;

      Float copyOpp (one);
      copyOpp = opp;

         //This is necessary, or acos(-0) would return (-pi/2)
      if (copyOpp.isZero()) copyOpp.abs();

      if (copyOpp == one)
       {
         Float res (opp);
         res = Float("0");
         return res;
       }
      if (copyOpp == -one)
       {
         Float res (opp);
         res = M_PI;
         return res;
       }

      Float res (copyOpp);

      res = copyOpp * copyOpp;
      res = one - res;
      res = sqrt(res) / copyOpp;
      res = atan(res);

      res.changePrecision(opp.getPrecision());
      return res;
    }

/*
==============================================================================
   Function: common (base 10) logarithm
------------------------------------------------------------------------------
   NOTES:
      log10(x) = log(x) / log(10) = log10(e) * log(x)
==============================================================================
*/
   Float log10 (const Float & opp)
    {
      Float top (opp);
      top.changePrecision(opp.getPrecision() + DERIVEDEXTRA);

      Float bottom (top);
      bottom = M_LOG10E;

      top = bottom * log(top);

      top.changePrecision(opp.getPrecision());
      return top;
    }

/*
==============================================================================
   Function: ten to a real power
------------------------------------------------------------------------------
   NOTES:
      a ^ u = e ^ (u * log(a))
         so
      10 ^ x = e ^ (x * log(10))

      I like the name alog: antilogrithm.
      The name may be antiquated, but it gets to the point and is,
      in my opinion, more self-descriptive than pow and better looking
      than exp10 or pow10.

      Funny side note: as of March 2010, the help files for gcalctool,
      the GNOME calculator, states that it does not support antilogarithms.
      The calculator has 10 ^ x and e ^ x buttons, however, so only the
      mathematically disinclined are fooled.
==============================================================================
*/
   Float exp10 (const Float & opp)
    {
      Float copyOpp (opp);
      copyOpp.changePrecision(opp.getPrecision() + DERIVEDEXTRA);

      Float multiplier (copyOpp);
      multiplier = M_LN10;

      copyOpp *= multiplier;
      copyOpp = exp(copyOpp);

      copyOpp.changePrecision(opp.getPrecision());
      return copyOpp;
    }

/*
==============================================================================
   Function: two argument inverse tangent
------------------------------------------------------------------------------
   NOTES:
      The arctangent of top / bottom, situated in the correct quadrant.
      Returns a value in [-pi, pi].

      Written to comply with, as best as possible, the description
      of the fpatan instruction in the _Intel 64 and IA-32 Architectures
      Software Developers' Manual: Volume 2A: Instruction Set Reference
      A-M_, order number 253666. I write as best as possible, as there is
      an undefined symbol (p) in the table that I guess stands for pi,
      in my pdf copy of the text aforementioned.

         My interpretation of the results table:
                -Inf    -X    -0    +0    +X  +Inf
         -Inf -3pi/4 -pi/2 -pi/2 -pi/2 -pi/2 -pi/4
           -X    -pi     * -pi/2 -pi/2     *    -0
           -0    -pi   -pi   -pi    -0    -0    -0
           +0    +pi   +pi   +pi    +0    +0    +0
           +X    +pi     * +pi/2 +pi/2     *    +0
         +Inf +3pi/4 +pi/2 +pi/2 +pi/2 +pi/2 +pi/4

            **top is rows, bottom is columns
      Of the 36 entries on this table, only the four marked * will
      actually be computed.
==============================================================================
*/
   Float atan2 (const Float & top, const Float & bottom)
    {
      Float one (top), two (bottom);
      match(one, two);

      if (top.isNaN() || bottom.isNaN()) return (one = M_NaN);

      if (top.isInfinity())
       {
         if (bottom.isInfinity())
          {
            one = M_PI_4;
            if (bottom.isSigned())
             {
               two = Float("3");
               one *= two;
             }
            if (top.isSigned()) one.negate();
            return one;
          }
         else
          {
            one = M_PI_2;
            if (top.isSigned()) one.negate();
            return one;
          }
       }  //twelve down

      if (top.isZero())
       {
         if (bottom.isSigned()) one = M_PI;
         one.abs();
         if (top.isSigned()) one.negate();
         return one;
       } //now twenty-four!

      if (bottom.isInfinity())
       {
         if (bottom.isSigned()) one = M_PI;
         one.abs();
         if (top.isSigned()) one.negate();
         return one;
       } //twenty-eight

      if (bottom.isZero())
       {
         one = M_PI_2;
         if (top.isSigned()) one.negate();
         return one;
       } //All thirty-two precomputed entries.

      one.changePrecision(one.getPrecision() + DERIVEDEXTRA);
      two.changePrecision(two.getPrecision() + DERIVEDEXTRA);

      one /= two;
      one = atan(one);

         //Now fix the result based on the signs of the opperands.
      if (bottom.isSigned())
       {
         two = top.isSigned() ? -M_PI : M_PI;
         one += two;
       }

      one.changePrecision(one.getPrecision() - DERIVEDEXTRA);
      return one;
    }


 } /* namespace BigInt */
