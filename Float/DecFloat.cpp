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

#include "DecFloat.hpp"

namespace DecFloat
 {

    /*
      Remember: you will get digits + 1 of actual precision.
    */
   unsigned long Float::minPrecision = 7;
   unsigned long Float::maxPrecision = 511;

   long Float::minExponent = -1000000;
   long Float::maxExponent = 1000000;


   Float::Float (const std::string & from)
    {
      fromString(from);
    }

   Float::Float (const char * from)
    {
      fromString(from);
    }

   Float::Float (const BigInt::Float & from)
    {
      if (from.exponent() > maxExponent)
       {
         Data |= BigInt::Float("Inf");
         Data.copySign(from);
       }
      else if (from.exponent() < minExponent)
       {
         Data |= BigInt::Float("0");
         Data.copySign(from);
       }
      else Data |= from;

      if (Data.getPrecision() > maxPrecision)
         Data.changePrecision(maxPrecision);
      else if (Data.getPrecision() < minPrecision)
         Data.changePrecision(minPrecision);
    }


   bool operator > (const Float & lhs, const Float & rhs)
    {
      return lhs.Data > rhs.Data;
    }

   bool operator < (const Float & lhs, const Float & rhs)
    {
      return lhs.Data < rhs.Data;
    }

   bool operator >= (const Float & lhs, const Float & rhs)
    {
      return lhs.Data >= rhs.Data;
    }

   bool operator <= (const Float & lhs, const Float & rhs)
    {
      return lhs.Data <= rhs.Data;
    }

   bool operator == (const Float & lhs, const Float & rhs)
    {
      return lhs.Data == rhs.Data;
    }

   bool operator != (const Float & lhs, const Float & rhs)
    {
      return lhs.Data != rhs.Data;
    }


   void match (Float & left, Float & right)
    {
      BigInt::match(left.Data, right.Data);
    }


   bool change (const Float & lhs, const Float & rhs)
    {
      return BigInt::change(lhs.Data, rhs.Data);
    }


   Float operator + (const Float & lhs, const Float & rhs)
    {
      Float temp;
      unsigned long usePrec, diff;

      usePrec = lhs.getPrecision() > rhs.getPrecision() ?
         lhs.getPrecision() :
         rhs.getPrecision();

      diff = lhs.Data.exponent() >= rhs.Data.exponent() ?
         lhs.Data.exponent() - rhs.Data.exponent() :
         rhs.Data.exponent() - lhs.Data.exponent();

      if ((diff > (usePrec + 1)) && rhs.isUnSpecial() && lhs.isUnSpecial())
         temp.Data = lhs.Data.exponent() > rhs.Data.exponent() ?
            lhs.Data : rhs.Data;
      else
         temp.Data |= lhs.Data + rhs.Data;

      temp.Data.changePrecision(usePrec);

      if (temp.Data.exponent() > Float::maxExponent)
         temp.Data = (BigInt::Float("Inf")).copySign(temp.Data);
      else if (temp.Data.exponent() < Float::minExponent)
         temp.Data = (BigInt::Float("0")).copySign(temp.Data);

      return temp;
    }

   Float operator - (const Float & lhs, const Float & rhs)
    {
      Float temp;
      unsigned long usePrec, diff;

      usePrec = lhs.getPrecision() > rhs.getPrecision() ?
         lhs.getPrecision() :
         rhs.getPrecision();

      diff = lhs.Data.exponent() >= rhs.Data.exponent() ?
         lhs.Data.exponent() - rhs.Data.exponent() :
         rhs.Data.exponent() - lhs.Data.exponent();

      if ((diff > (usePrec + 1)) && rhs.isUnSpecial() && lhs.isUnSpecial())
         temp.Data = lhs.Data.exponent() > rhs.Data.exponent() ?
            lhs.Data : -rhs.Data;
      else
         temp.Data |= lhs.Data - rhs.Data;

      temp.Data.changePrecision(usePrec);

      if (temp.Data.exponent() > Float::maxExponent)
         temp.Data = (BigInt::Float("Inf")).copySign(temp.Data);
      else if (temp.Data.exponent() < Float::minExponent)
         temp.Data = (BigInt::Float("0")).copySign(temp.Data);

      return temp;
    }

   Float operator * (const Float & lhs, const Float & rhs)
    {
      Float temp;
      unsigned long usePrec;

      usePrec = lhs.getPrecision() > rhs.getPrecision() ?
         lhs.getPrecision() :
         rhs.getPrecision();

      temp.Data |= lhs.Data * rhs.Data;

      temp.Data.changePrecision(usePrec);

      if (temp.Data.exponent() > Float::maxExponent)
         temp.Data = (BigInt::Float("Inf")).copySign(temp.Data);
      else if (temp.Data.exponent() < Float::minExponent)
         temp.Data = (BigInt::Float("0")).copySign(temp.Data);

      return temp;
    }

   Float operator / (const Float & lhs, const Float & rhs)
    {
      Float temp;
      unsigned long usePrec;

      usePrec = lhs.getPrecision() > rhs.getPrecision() ?
         lhs.getPrecision() :
         rhs.getPrecision();

      temp.Data |= lhs.Data / rhs.Data;

      temp.Data.changePrecision(usePrec);

      if (temp.Data.exponent() > Float::maxExponent)
         temp.Data = (BigInt::Float("Inf")).copySign(temp.Data);
      else if (temp.Data.exponent() < Float::minExponent)
         temp.Data = (BigInt::Float("0")).copySign(temp.Data);

      return temp;
    }


   void Float::fromString (const char * src)
    {
      Data.fromString(src);

      Data.normalize();

      if (Data.exponent() > maxExponent)
         Data = (BigInt::Float("Inf")).copySign(Data);
      else if (Data.exponent() < minExponent)
         Data = (BigInt::Float("0")).copySign(Data);

      if (Data.getPrecision() > maxPrecision)
         Data.changePrecision(maxPrecision);
      else if (Data.getPrecision() < minPrecision)
         Data.changePrecision(minPrecision);
    }


 } /* namespace DecFloat */
