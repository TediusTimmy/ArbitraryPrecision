/*
Copyright (c) 2010 Thomas DiModica.
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

#ifndef FIXED_HPP
#define FIXED_HPP

#include "../Integer.hpp"

namespace BigInt
 {

   enum Fixed_Round_Mode
    {
      ROUND_TIES_EVEN,
      ROUND_TIES_AWAY,
      ROUND_POSITIVE_INFINITY,
      ROUND_NEGATIVE_INFINITY,
      ROUND_ZERO,
      ROUND_TIES_ODD,
      ROUND_TIES_ZERO,
      ROUND_AWAY,
      ROUND_05_AWAY
    };

   class Fixed
    {

      private:
         static unsigned long defPrec;
         static Fixed_Round_Mode mode;

         static bool decideRound (bool, bool, int, bool);

      public:
         static unsigned long getDefaultPrecision (void) { return defPrec; }
         static unsigned long setDefaultPrecision (unsigned long newPrecision)
          { return (defPrec = newPrecision); }

         static Fixed_Round_Mode getRoundMode (void) { return mode; }
         static Fixed_Round_Mode setRoundMode (Fixed_Round_Mode newMode)
          { return (mode = newMode); }

      private:
         Integer Data;
         unsigned long Digits;

      public:

         Fixed (const Fixed & from) :
            Data (from.Data), Digits (from.Digits) { }
         Fixed (unsigned long precision = defPrec) :
            Data (), Digits (precision) { }
         Fixed (long long i, unsigned long p = defPrec) :
            Data (i), Digits (p) { }
         Fixed (const std::string &);
         Fixed (const char *);
         ~Fixed () { /* This has nothing to do. */ }

         unsigned long getPrecision (void) const { return Digits; }
         unsigned long setPrecision (unsigned long newPrecision)
            { return (Digits = newPrecision); }

         void changePrecision (unsigned long); //changes Data to match

         bool isSigned (void) const { return Data.isSigned(); }
         bool isZero (void) const { return Data.isZero(); }

         bool isInteger (void) const;
         bool isOdd (void) const;
         bool isEven (void) const;

         std::string toString (void) const;

         void fromString (const std::string & src)
            { fromString(src.c_str()); }
         void fromString (const char *);

         Fixed & negate (void)
            { Data.negate(); return *this; }
         Fixed & abs (void)
            { Data.abs(); return *this; }
         Fixed & copySign (const Fixed & of)
            { Data.copySign(of.Data); return *this; }
         Fixed & setSign (bool toThis)
            { Data.setSign(toThis); return *this; }

         friend Fixed operator + (const Fixed &, const Fixed &);
         friend Fixed operator - (const Fixed &, const Fixed &);
         friend Fixed operator * (const Fixed &, const Fixed &);
         friend Fixed operator / (const Fixed &, const Fixed &);

         Fixed & operator = (const Fixed &);

         Fixed & operator += (const Fixed &);
         Fixed & operator -= (const Fixed &);
         Fixed & operator *= (const Fixed &);
         Fixed & operator /= (const Fixed &);

         Fixed operator - (void) const;
         bool operator ! (void) const { return isZero(); }

         int compare (const Fixed &) const;

         friend void match (Fixed &, Fixed &);

         Integer roundToInteger (void) const;

    }; /* class Fixed */

   Fixed operator + (const Fixed &, const Fixed &);
   Fixed operator - (const Fixed &, const Fixed &);
   Fixed operator * (const Fixed &, const Fixed &);
   Fixed operator / (const Fixed &, const Fixed &);

   bool operator > (const Fixed &, const Fixed &);
   bool operator < (const Fixed &, const Fixed &);
   bool operator >= (const Fixed &, const Fixed &);
   bool operator <= (const Fixed &, const Fixed &);
   bool operator == (const Fixed &, const Fixed &);
   bool operator != (const Fixed &, const Fixed &);

   void match (Fixed &, Fixed &);

 } /* namespace BigInt */

#endif /* FIXED_HPP */
