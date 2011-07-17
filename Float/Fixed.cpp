/*
Copyright (c) 2010, 2011 Thomas DiModica.
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

/*
   This has been modified to produce results with escalated accuracy.
   It is now, essentially, an Exact with a different name, fixed fromString()
   function, and no % operator.

   I also added a little something to help internationalize this.
*/

#include <cstdlib>
#include <string>
#include "Fixed.hpp"

#ifndef SEPERATOR
 #define SEPERATOR '.'
#endif

namespace BigInt
 {


   unsigned long Fixed::defPrec = 7;

   Fixed_Round_Mode Fixed::mode = ROUND_TIES_EVEN;


   Fixed::Fixed (const std::string & from)
    {
      fromString(from);
    }

   Fixed::Fixed (const char * from)
    {
      fromString(from);
    }


   bool operator > (const Fixed & lhs, const Fixed & rhs)
    {
      return lhs.compare(rhs) > 0;
    }

   bool operator < (const Fixed & lhs, const Fixed & rhs)
    {
      return lhs.compare(rhs) < 0;
    }

   bool operator >= (const Fixed & lhs, const Fixed & rhs)
    {
      return lhs.compare(rhs) >= 0;
    }

   bool operator <= (const Fixed & lhs, const Fixed & rhs)
    {
      return lhs.compare(rhs) <= 0;
    }

   bool operator == (const Fixed & lhs, const Fixed & rhs)
    {
      return lhs.compare(rhs) == 0;
    }

   bool operator != (const Fixed & lhs, const Fixed & rhs)
    {
      return lhs.compare(rhs) != 0;
    }


   Fixed & Fixed::operator += (const Fixed & rval)
    {
      return (*this = *this + rval);
    }

   Fixed & Fixed::operator -= (const Fixed & rval)
    {
      return (*this = *this - rval);
    }

   Fixed & Fixed::operator *= (const Fixed & rval)
    {
      return (*this = *this * rval);
    }

   Fixed & Fixed::operator /= (const Fixed & rval)
    {
      return (*this = *this / rval);
    }


   Fixed Fixed::operator - (void) const
    {
      Fixed returnedFixed(*this);
      return returnedFixed.negate();
    }


   Fixed & Fixed::operator = (const Fixed & src)
    {
      if (&src == this) return *this;
      Data = src.Data;
      Digits = src.Digits;
      return *this;
    }


   void match (Fixed & lhs, Fixed & rhs)
    {
      if (lhs.Digits > rhs.Digits)
         rhs.changePrecision(lhs.Digits);
      else if (lhs.Digits < rhs.Digits)
         lhs.changePrecision(rhs.Digits);
    }


   int Fixed::compare (const Fixed & to) const
    {
      Fixed temp;

      if (Digits > to.Digits)
       {
         temp = to;
         temp.changePrecision(Digits);
         return Data.compare(temp.Data);
       }
      else if (Digits < to.Digits)
       {
         temp = *this;
         temp.changePrecision(to.Digits);
         return temp.Data.compare(Data);
       }

      return Data.compare(to.Data);
    }


   Fixed operator + (const Fixed & lhs, const Fixed & rhs)
    {
      Fixed temp;

      if (lhs.Digits > rhs.Digits)
       {
         temp = rhs;
         temp.changePrecision(lhs.Digits);
         temp.Data += lhs.Data;
       }
      else if (lhs.Digits < rhs.Digits)
       {
         temp = lhs;
         temp.changePrecision(rhs.Digits);
         temp.Data += rhs.Data;
       }
      else
       {
         temp.Data = lhs.Data + rhs.Data;
         temp.Digits = lhs.Digits;
       }

      return temp;
    }

   Fixed operator - (const Fixed & lhs, const Fixed & rhs)
    {
      Fixed temp1 (lhs), temp2 (rhs);
      match(temp1, temp2);

      temp1.Data -= temp2.Data;

      return temp1;
    }

   Fixed operator * (const Fixed & lhs, const Fixed & rhs)
    {
      Fixed temp (lhs);

      temp.Data *= rhs.Data;
      temp.Digits += rhs.Digits;

      return temp;
    }

    /*
      So, a fixed used to produce a result with a precision that was
      the max of the lhs and rhs precisions.
      Now, it produces a result of the sum of the lhs and rhs precisions.
    */
   Fixed operator / (const Fixed & lhs, const Fixed & rhs)
    {
      Fixed q = lhs;
      Fixed r = rhs;
      Integer d;
      bool s = lhs.Data.isSigned() ^ rhs.Data.isSigned();

      d = r.Data;
      d.abs();

      q.Data *= pow(Integer((Unit) 10), Integer((long long) (2 * r.Digits)));
      q.Digits += r.Digits;

      Integer::divmod (q.Data, r.Data, q.Data, r.Data);

      r.Data <<= Integer((Unit) 1);

      if (Fixed::decideRound(s, q.Data.isEven(),
                             d.compare(r.Data.abs()), r.Data.isZero()))
       {
         if (s) q.Data--;
         else q.Data++;
       }

      return q;
    }


   void Fixed::changePrecision (unsigned long newPrec)
    {
      Integer scale, rem;
      bool s = Data.isSigned();

      if (newPrec == Digits) return;
      if (newPrec > Digits)
       {
         scale = pow(Integer((Unit) 10), Integer((long long) (newPrec - Digits)));
         Data *= scale;
       }
      else
       {
         scale = pow(Integer((Unit) 10), Integer((long long) (Digits - newPrec)));
         Integer::divmod(Data, scale, Data, rem);

         rem <<= Integer((Unit) 1);

         if (decideRound(s, Data.isEven(),
                         scale.compare(rem.abs()), rem.isZero()))
          {
            if (s) Data--;
            else Data++;
          }
       }
      Digits = newPrec;
    }


   std::string Fixed::toString (void) const
    {
      std::string result;
      result = Data.toString();
      if (Digits == 0) return result;
      if (result.length() <= Digits)
       {
         while (result.length() < Digits) result = '0' + result;
         result = '0' + (SEPERATOR + result);
       }
      else
       {
         result = result.substr(0, result.length() - Digits) + SEPERATOR +
            result.substr(result.length() - Digits, Digits);
       }
      return result;
    }


   void Fixed::fromString (const char * src)
    {
      const char * iter = src;
      std::string conv;

      conv.reserve(2048);

      if ((*iter == '+') || (*iter == '-')) iter++;
      if (*iter != SEPERATOR)
       {
         while ((*iter >= '0') && (*iter <= '9')) iter++;

         if (*iter != SEPERATOR)
          {
            Data.fromString(src);
            Digits = 0;
            return;
          }
       }

      Digits = 0;
      iter++;
      while ((*iter >= '0') && (*iter <= '9')) { iter++; Digits++; }

      iter = src;

      if (*iter != SEPERATOR) conv += *(iter++);
      if (*iter != SEPERATOR)
         while ((*iter >= '0') && (*iter <= '9')) conv += *(iter++);
      iter++;
      while ((*iter >= '0') && (*iter <= '9')) conv += *(iter++);

      Data.fromString(conv);
    }


   Integer Fixed::roundToInteger (void) const
    {
      Fixed temp (*this);
      temp.changePrecision(0);
      return temp.Data;
    }


   bool Fixed::decideRound (bool sign, bool even, int comp, bool zero)
    {
      switch (mode)
       {
         case ROUND_TIES_EVEN:
            if ((comp < 0) || ((comp == 0) && !even)) return true;
            break;
         case ROUND_TIES_AWAY:
            if (comp <= 0) return true;
            break;
         case ROUND_POSITIVE_INFINITY:
            if (!sign && !zero) return true;
            break;
         case ROUND_NEGATIVE_INFINITY:
            if (sign && !zero) return true;
            break;
         case ROUND_ZERO:
            break;
         case ROUND_TIES_ODD:
            if ((comp < 0) || ((comp == 0) && even)) return true;
            break;
         case ROUND_TIES_ZERO:
            if (comp < 0) return true;
            break;
         case ROUND_AWAY:
            if (!zero) return true;
            break;
         case ROUND_05_AWAY:
            if (zero || (comp == 0)) return true;
            break;
       }
      return false;
    }


   bool Fixed::isInteger (void) const
    {
      Integer temp, scale;

      scale = pow(Integer((Unit) 10), Integer((long long) (Digits)));
      Integer::divmod(Data, scale, temp, scale);

      return scale.isZero();
    }

   bool Fixed::isOdd (void) const
    {
      Integer temp, scale;

      scale = pow(Integer((Unit) 10), Integer((long long) (Digits)));
      Integer::divmod(Data, scale, temp, scale);

      return scale.isZero() ? temp.isOdd() : false;
    }

   bool Fixed::isEven (void) const
    {
      Integer temp, scale;

      scale = pow(Integer((Unit) 10), Integer((long long) (Digits)));
      Integer::divmod(Data, scale, temp, scale);

      return scale.isZero() ? temp.isEven() : false;
    }


 } /* namespace BigInt */
