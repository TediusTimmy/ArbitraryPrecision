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

#ifndef DECFLOAT_HPP
#define DECFLOAT_HPP

#include "Float.hpp"

 /*
   Why DecFloat?
      It enforces that the result of a simple mathematical operation has a
         precision that is the maximum of the precisions of the arguments.
      It provides a guaranteed minimum precision.
      It caps the maximum precision, so the library functions remain valid.
      It provides tuning for truncation to zero on underflow and declaring a
         value infinity on overflow.
 */

namespace DecFloat
 {

   class Float
    {

      private:
         static unsigned long minPrecision;
         static unsigned long maxPrecision;
         static long minExponent;
         static long maxExponent;

      public:
         static unsigned long getMinPrecision (void) { return minPrecision; }
         static unsigned long setMinPrecision (unsigned long newPrecision)
          { return (minPrecision = newPrecision); }

         static unsigned long getMaxPrecision (void) { return maxPrecision; }
         static unsigned long setMaxPrecision (unsigned long newPrecision)
          { return (maxPrecision = newPrecision); }

         static long getMinExponent (void) { return minExponent; }
         static long setMinExponent (long newExponent)
          { return (minExponent = newExponent); }

         static long getMaxExponent (void) { return maxExponent; }
         static long setMaxExponent (long newExponent)
          { return (maxExponent = newExponent); }

      private:
         BigInt::Float Data;

      public:

         Float (const Float & from) : Data (from.Data) { }
         Float (const BigInt::Float &);
         Float () : Data () { }
         Float (const std::string & from);
         Float (const char * from);
         ~Float () { /* This has nothing to do. */ }

         operator BigInt::Float () const { return Data; }

         unsigned long getPrecision (void) const
          { return Data.getPrecision(); }
         unsigned long setPrecision (unsigned long newPrecision)
          {
            if (newPrecision < minPrecision) newPrecision = minPrecision;
            if (newPrecision > maxPrecision) newPrecision = maxPrecision;
            return Data.setPrecision(newPrecision);
          }

         void changePrecision (unsigned long newPrecision)
          {
            if (newPrecision < minPrecision) newPrecision = minPrecision;
            if (newPrecision > maxPrecision) newPrecision = maxPrecision;
            Data.changePrecision(newPrecision);
          }

         long exponent (void) const { return Data.exponent(); }

         bool isSigned (void) const { return Data.isSigned(); }
         bool isNegative (void) const { return Data.isNegative(); }
         bool isZero (void) const { return Data.isZero(); }
         bool isInfinity (void) const { return Data.isInfinity(); }
         bool isNaN (void) const { return Data.isNaN(); }

         bool isInteger (void) const { return Data.isInteger(); }
         bool isOdd (void) const { return Data.isOdd(); }
         bool isEven (void) const { return Data.isEven(); }

         bool isUnSpecial (void) const { return Data.isUnSpecial(); }

         std::string toString (void) const { return Data.toString(); }

         void fromString (const std::string & src)
            { fromString(src.c_str()); }
         void fromString (const char *);

         Float & negate (void)
          { Data.negate(); return *this; }
         Float & abs (void)
          { Data.abs(); return *this; }
         Float & copySign (const Float & of)
          { Data.copySign(of.Data); return *this; }
         Float & setSign (bool toThis)
          { Data.setSign(toThis); return *this; }

         friend Float operator + (const Float &, const Float &);
         friend Float operator - (const Float &, const Float &);
         friend Float operator * (const Float &, const Float &);
         friend Float operator / (const Float &, const Float &);

         Float & operator += (const Float & rhs)
          { return (*this = *this + rhs); }
         Float & operator -= (const Float & rhs)
          { return (*this = *this - rhs); }
         Float & operator *= (const Float & rhs)
          { return (*this = *this * rhs); }
         Float & operator /= (const Float & rhs)
          { return (*this = *this / rhs); }

         friend bool operator > (const Float &, const Float &);
         friend bool operator < (const Float &, const Float &);
         friend bool operator >= (const Float &, const Float &);
         friend bool operator <= (const Float &, const Float &);
         friend bool operator == (const Float &, const Float &);
         friend bool operator != (const Float &, const Float &);

         Float operator - (void) const
          { Float returnedFloat(*this); return returnedFloat.negate(); }
         bool operator ! (void) const { return Data.isZero(); }

          /*
            DecFloat flips the types of assignments.
          */

            // Structure preserving assignment
         Float & operator = (const Float & src)
          { Data |= src.Data; return *this; }
            // Data type preserving assignment
         Float & operator |= (const Float & src)
          { Data = src.Data; return *this; }

            //Only works for finite, nonzero _numbers_
            // i.e. isUnSpecial() returns _true_
         int compare (const Float & to) const
          { return Data.compare(to.Data); }

         friend bool change (const Float &, const Float &);
         friend void match (Float &, Float &);

    }; /* class Float */

 } /* namespace DecFloat */

#endif /* DECFLOAT_HPP */
