/*
Copyright (c) 2011 Thomas DiModica.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of Thomas DiModica nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THOMAS DIMODICA AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THOMAS DIMODICA OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#ifndef FLOAT_HPP
#define FLOAT_HPP

#include <string>
#include "Fixed.hpp"

namespace BigInt
 {

   class Float
    {

      private:
         Fixed Data;
         bool Sign;
         long Exponent;

         bool Infinity;
         bool NaN;

          /*
            Arguments:
               1 + 2 Numbers to Add or Subtract
               3 If True Then Subtract
            Returns:
               -1 Will Overflow Negative (UnderFlow)
               0 No Exception
               1 Will Overflow Positive
          */
         static int willOverflow (long, long, bool);

      public:

         Float (const Float & from) :
            Data (from.Data), Sign (from.Sign), Exponent (from.Exponent),
            Infinity (from.Infinity), NaN (from.NaN) { }
         Float () :
            Data (), Sign (false), Exponent(0), Infinity (false), NaN (false)
            { }
         Float (const std::string &);
         Float (const char *);
         ~Float () { /* This has nothing to do. */ }

         unsigned long getPrecision (void) const
            { return Data.getPrecision(); }
         unsigned long setPrecision (unsigned long newPrecision)
            { return Data.setPrecision(newPrecision); }

         void changePrecision (unsigned long newPrecision)
            { Data.changePrecision(newPrecision); }

         long exponent (void) const { return Exponent; }

         bool isSigned (void) const { return Sign; }
         bool isNegative (void) const
            { if (Data.isZero()) return false; return Sign; }
         bool isInfinity (void) const { return Infinity; }
         bool isNaN (void) const { return NaN; }
         bool isZero (void) const
            { if (Infinity || NaN) return false; return Data.isZero(); }

         bool isUnSpecial (void) const
            { return !(Data.isZero() || NaN || Infinity); }

         bool isInteger (void) const;
         bool isOdd (void) const;
         bool isEven (void) const;

         std::string toString (void) const;

         void fromString (const std::string & src)
            { fromString(src.c_str()); }
         void fromString (const char *);

         Float & negate (void)
            { Sign = !Sign; return *this; }
         Float & abs (void)
            { Sign = false; return *this; }
         Float & copySign (const Float & of)
            { Sign = of.Sign; return *this; }
         Float & setSign (bool toThis)
            { Sign = toThis; return *this; }

         friend Float operator + (const Float &, const Float &);
         friend Float operator - (const Float &, const Float &);
         friend Float operator * (const Float &, const Float &);
         friend Float operator / (const Float &, const Float &);

         Float operator - (void) const
          { Float returnedFloat(*this); return returnedFloat.negate(); }
         bool operator ! (void) const { return isZero(); }

          /*
            There are two ways to think of Floats, and thus two assignment
            statements. 1) A Float is like a float or double in that any data
            moved into the variable has to be coerced to fit into that
            variable. 2) A Float is a class and assignment copies the data
            inside that class structure.

            The first assignment preserves the "data type" of the
            left-hand-side; the second preserves the structure of the Float:
            that it is a class with internal state being assigned.

            I found that I was using the first assignment most frequently,
            which is why it is the easier one to use.
          */

            // Data type preserving assignment
         Float & operator = (const Float &);
            // Structure preserving assignment
         Float & operator |= (const Float &);

            //Only works for finite, nonzero _numbers_
            // i.e. isUnSpecial() returns _true_
         int compare (const Float &) const;

            //Cowlishaw doesn't do normalization, but I need it to make
            //toString work correctly.
         Float & normalize (void);

         friend Float sqrt (const Float &);
         friend Float exp (const Float &);
         friend Float log (const Float &);

    }; /* class Float */

   bool operator > (const Float &, const Float &);
   bool operator < (const Float &, const Float &);
   bool operator >= (const Float &, const Float &);
   bool operator <= (const Float &, const Float &);
   bool operator == (const Float &, const Float &);
   bool operator != (const Float &, const Float &);

   Float & operator += (Float &, const Float &);
   Float & operator -= (Float &, const Float &);
   Float & operator *= (Float &, const Float &);
   Float & operator /= (Float &, const Float &);

   bool change (const Float &, const Float &);
   void match (Float &, Float &);

    /*
      All of the "important" scientific functions.
      __I__ don't really use the hyperbolic functions, so they are not here.
      They are easily derivable, however. Just steal the code from SlowCalc.
      Implement expm1() and use it instead for sinh() and cosh().
      Every function marked with * is fundemental,
         all other functions are derived from them.
    */
   Float sqrt (const Float &); // *

   Float exp (const Float &); // *
   Float log (const Float &); // *

   Float pow (const Float &, const Float &);

   Float sin (const Float &); // *
   Float cos (const Float &); // *
   Float tan (const Float &);

   Float atan (const Float &); // *
   Float asin (const Float &);
   Float acos (const Float &);

   Float exp10 (const Float &); // These two are here for
   Float log10 (const Float &); // historical reasons only

   Float atan2 (const Float &, const Float &);

 } /* namespace BigInt */

#endif /* FLOAT_HPP */
