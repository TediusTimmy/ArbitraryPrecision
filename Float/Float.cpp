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

#include <cstdlib>
#include <string>
#include <sstream>
#include <cstring>
#include "Float.hpp"

#ifndef SEPERATOR
 #define SEPERATOR '.'
#endif

namespace BigInt
 {


   Float::Float (const std::string & from)
    {
      fromString(from);
    }

   Float::Float (const char * from)
    {
      fromString(from);
    }


   bool operator > (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return false;
            else if (lhs.isSigned()) return false;
            else return true;
          }
         else if (lhs.isInfinity()) return !lhs.isSigned();
         else return !rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) > 0;
    }

   bool operator < (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return false;
            else if (lhs.isSigned()) return true;
            else return false;
          }
         else if (lhs.isInfinity()) return lhs.isSigned();
         else return rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) < 0;
    }

   bool operator >= (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return false;
            else if (lhs.isSigned()) return false;
            else return true;
          }
         else if (lhs.isInfinity()) return !lhs.isSigned();
         else return !rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) >= 0;
    }

   bool operator <= (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return false;
            else if (lhs.isSigned()) return true;
            else return false;
          }
         else if (lhs.isInfinity()) return lhs.isSigned();
         else return rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) <= 0;
    }

   bool operator == (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity()) return false;
      if (lhs.isZero() && rhs.isZero()) return true;
      return lhs.compare(rhs) == 0;
    }

   bool operator != (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() && rhs.isNaN()) return true;
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity()) return true;
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) != 0;
    }


   Float & operator += (Float & lval, const Float & rval)
    {
      return (lval = lval + rval);
    }

   Float & operator -= (Float & lval, const Float & rval)
    {
      return (lval = lval - rval);
    }

   Float & operator *= (Float & lval, const Float & rval)
    {
      return (lval = lval * rval);
    }

   Float & operator /= (Float & lval, const Float & rval)
    {
      return (lval = lval / rval);
    }


   Float & Float::operator = (const Float & src)
    {
      if (&src == this) return *this;
      unsigned long Precision;
      Precision = Data.getPrecision();
      Data = src.Data;
      Data.changePrecision(Precision);
      Sign = src.Sign;
      Exponent = src.Exponent;
      Infinity = src.Infinity;
      NaN = src.NaN;
      return *this;
    }

   Float & Float::operator |= (const Float & src)
    {
      if (&src == this) return *this;
      Data = src.Data;
      Sign = src.Sign;
      Exponent = src.Exponent;
      Infinity = src.Infinity;
      NaN = src.NaN;
      return *this;
    }


   int Float::compare (const Float & to) const
    {
      if (&to == this) return 0;

      if (Exponent == to.Exponent)
       {
         Fixed temp1 (Data);
         Fixed temp2 (to.Data);

         temp1.setSign(Sign);
         temp2.setSign(to.Sign);

         return temp1.compare(temp2);
       }
      else if (Sign && to.Sign)
       {
         if (Exponent > to.Exponent) return -1;
         else return 1;
       }
      else if (!Sign && to.Sign) return -1;
      else if (Sign && !to.Sign) return 1;
      else
       {
         if (Exponent > to.Exponent) return 1;
         else return -1;
       }
    }


   void match (Float & left, Float & right)
    {
      if (left.getPrecision() > right.getPrecision())
         right.changePrecision(left.getPrecision());
      else if (left.getPrecision() < right.getPrecision())
         left.changePrecision(right.getPrecision());
    }


   Float operator + (const Float & lhs, const Float & rhs)
    {
      Float temp, temp1 (lhs), temp2 (rhs);

         //First, handle NaNs, as they have the highest precedence.
      if (lhs.isNaN() || rhs.isNaN())
       {
         temp.NaN = true;
         return temp;
       }

         //Next, if both are infinities.
      if (lhs.isInfinity() && rhs.isInfinity())
       {
            //Inf - Inf = NaN
         if (lhs.isSigned() != rhs.isSigned())
          {
            temp.NaN = true;
            return temp;
          }

         temp.copySign(lhs);
         temp.Infinity = true;
         return temp;
       }

         //Anything else with infinity is infinity
      if (lhs.isInfinity())
       {
         temp.copySign(lhs);
         temp.Infinity = true;
         return temp;
       }
      if (rhs.isInfinity())
       {
         temp.copySign(rhs);
         temp.Infinity = true;
         return temp;
       }

         //Finally, zeros.
      if (lhs.isZero() && rhs.isZero() && (lhs.Sign != rhs.Sign))
         return temp;
      if (rhs.isZero()) return temp1;
      if (lhs.isZero()) return temp2;

         //Now, denormalize the Precisions to take account of the Exponents
      if (temp1.Exponent > temp2.Exponent)
       {
         temp.Exponent = temp1.Exponent;
         temp2.Data.setPrecision
            (temp2.Data.getPrecision() + temp1.Exponent - temp2.Exponent);
       }
      else if (temp1.Exponent < temp2.Exponent)
       {
         temp.Exponent = temp2.Exponent;
         temp1.Data.setPrecision
            (temp1.Data.getPrecision() + temp2.Exponent - temp1.Exponent);
       }
      else temp.Exponent = temp1.Exponent;

         //Assign the correct signs before operating
      temp1.Data.setSign(temp1.Sign);
      temp2.Data.setSign(temp2.Sign);

      temp.Data = temp1.Data + temp2.Data;

         //Clear the sign
      temp.Sign = temp.Data.isSigned();
      temp.Data.abs();

         //Return normalized result
      return temp.normalize();
    }

   Float operator - (const Float & lhs, const Float & rhs)
    {
      Float temp, temp1 (lhs), temp2 (rhs);

      if (lhs.isNaN() || rhs.isNaN())
       {
         temp.NaN = true;
         return temp;
       }

      if (lhs.isInfinity() && rhs.isInfinity())
       {
         if (lhs.isSigned() == rhs.isSigned())
          {
            temp.NaN = true;
            return temp;
          }

         temp.copySign(lhs);
         temp.Infinity = true;
         return temp;
       }

      if (lhs.isInfinity())
       {
         temp.copySign(lhs);
         temp.Infinity = true;
         return temp;
       }
      if (rhs.isInfinity())
       {
         temp.setSign(!rhs.Sign);
         temp.Infinity = true;
         return temp;
       }

      if (lhs.isZero() && rhs.isZero() && (lhs.Sign == rhs.Sign))
         return temp;
      if (rhs.isZero()) return temp1;
      if (lhs.isZero()) return -temp2;

      if (temp1.Exponent > temp2.Exponent)
       {
         temp.Exponent = temp1.Exponent;
         temp2.Data.setPrecision
            (temp2.Data.getPrecision() + temp1.Exponent - temp2.Exponent);
       }
      else if (temp1.Exponent < temp2.Exponent)
       {
         temp.Exponent = temp2.Exponent;
         temp1.Data.setPrecision
            (temp1.Data.getPrecision() + temp2.Exponent - temp1.Exponent);
       }
      else temp.Exponent = temp1.Exponent;

      temp1.Data.setSign(temp1.Sign);
      temp2.Data.setSign(!temp2.Sign); //Negate temp2 here.

      temp.Data = temp1.Data + temp2.Data;

      temp.Sign = temp.Data.isSigned();
      temp.Data.abs();

      return temp.normalize();
    }

   Float operator * (const Float & lhs, const Float & rhs)
    {
      Float temp;
      Fixed upper ((long long)10, 0);

         //Again, NaN has precedence
      if (lhs.isNaN() || rhs.isNaN())
       {
         temp.NaN = true;
         return temp;
       }

         //Inf*0 = NaN
      if ((lhs.isInfinity() && rhs.isZero()) ||
          (lhs.isZero() && rhs.isInfinity()))
       {
         temp.NaN = true;
         return temp;
       }

         //Put this here, as Zero and Infinity need the correct sign.
      temp.setSign(lhs.isSigned() ^ rhs.isSigned());

         //Infinity handling
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         temp.Infinity = true;
         return temp;
       }

         //Zero handling: return a zero with the correct sign
      if (lhs.isZero() || rhs.isZero()) return temp;

         //Do we need to do the multiply?
      switch (Float::willOverflow(lhs.Exponent, rhs.Exponent, false))
       {
         case 1:
            temp.Infinity = true;
            temp.Exponent = 0;
            temp.Data = Fixed((long long) 0);
            break;

         case -1:
            temp.Exponent = 0;
            temp.Data = Fixed((long long) 0);
            break;

         default:
            temp.Exponent = lhs.Exponent + rhs.Exponent;

               //Do the multiply
            temp.Data = lhs.Data * rhs.Data;

               //Normalize
            if (temp.Data >= upper)
             {
               temp.Data.setPrecision(temp.Data.getPrecision() + 1);
               switch (Float::willOverflow(temp.Exponent, 1, false))
                {
                  case 1:
                     temp.Infinity = true;
                     temp.Exponent = 0;
                     temp.Data = Fixed((long long) 0);
                     break;

                  default:
                     temp.Exponent++;
                }
             }
       }

      return temp;
    }

   Float operator / (const Float & lhs, const Float & rhs)
    {
      Float temp;
      Fixed lower ((long long)1, 0);

         //Finally, NaN has precedence
      if (lhs.isNaN() || rhs.isNaN())
       {
         temp.NaN = true;
         return temp;
       }

         //Handle the two Inf/Inf and 0/0 NaN cases
      if ((lhs.isZero() && rhs.isZero()) ||
          (lhs.isInfinity() && rhs.isInfinity()))
       {
         temp.NaN = true;
         return temp;
       }

         //Put this here, as Zero and Infinity need the correct sign.
      temp.setSign(lhs.isSigned() ^ rhs.isSigned());

         //Return an infinity
      if (lhs.isInfinity() || rhs.isZero())
       {
         temp.Infinity = true;
         return temp;
       }

         //Return a zero
      if (lhs.isZero() || rhs.isInfinity()) return temp;

         //Do we need to do the divide?
      switch (Float::willOverflow(lhs.Exponent, rhs.Exponent, true))
       {
         case 1:
            temp.Infinity = true;
            temp.Exponent = 0;
            temp.Data = Fixed((long long) 0);
            break;

         case -1:
            temp.Exponent = 0;
            temp.Data = Fixed((long long) 0);
            break;

         default:
            temp.Exponent = lhs.Exponent - rhs.Exponent;

               //Do the divide
               //The divide should generate enough extra digits
               //to do accurate rounding.
            temp.Data = lhs.Data / rhs.Data;

               //Normalize
            if (temp.Data < lower)
             {
               temp.Data.setPrecision(temp.Data.getPrecision() - 1);
               switch (Float::willOverflow(temp.Exponent, -1, false))
                {
                  case -1:
                     temp.Exponent = 0;
                     temp.Data = Fixed((long long) 0);
                     break;

                  default:
                     temp.Exponent--;
                }
             }
       }

      return temp;
    }


   std::string Float::toString (void) const
    {
      if (NaN) return std::string ("NaN");
      if (Infinity)
       {
         if (Sign) return std::string ("-Inf");
         else return std::string ("Inf");
       }
      std::string result ("");
      if (Sign) result += "-";
      result += Data.toString();
      result += "E";
      std::ostringstream temp;
      temp << Exponent;
      result += temp.str();
      return result;
    }


   void Float::fromString (const char * src)
    {
      if (!std::strcmp(src, "Inf") || !std::strcmp(src, "+Inf") ||
          !std::strcmp(src, "INF") || !std::strcmp(src, "+INF"))
       {
         Data = Fixed((long long)0, Data.getPrecision());
         Sign = false;
         Exponent = 0;
         Infinity = true;
         NaN = false;
         return;
       }

      if (!std::strcmp(src, "-Inf") || !std::strcmp(src, "-INF"))
       {
         Data = Fixed((long long)0, Data.getPrecision());
         Sign = true;
         Exponent = 0;
         Infinity = true;
         NaN = false;
         return;
       }

      if (!std::strcmp(src, "NaN") || !std::strcmp(src, "NAN"))
       {
         Data = Fixed((long long)0, Data.getPrecision());
         Sign = false;
         Exponent = 0;
         Infinity = false;
         NaN = true;
         return;
       }

      if (*src == '\0')
       {
         Data = Fixed((long long)0, Data.getPrecision());
         Sign = false;
         Exponent = 0;
         Infinity = false;
         NaN = false;
         return;
       }

      Sign = false;
      Infinity = false;
      NaN = false;

      const char * iter = src;
      std::string conv;
      bool computeExponent = true, insertSep = true;
      long newExponent = -1;

      conv.reserve(2048);

         // The Fixed is "signless".
      if (*iter == '-')
       {
         Sign = true;
         iter++;
       }
      else if (*iter == '+') iter++;

       /*
         We strip the seperator from the string and insert our own to create
         a normalized Fixed portion. Floats created from strings have string
         defined precision.
       */
      for (;(*iter != '\0') && (*iter != 'e') && (*iter != 'E'); iter++)
       {
         if (*iter != SEPERATOR) conv += *iter;
         else computeExponent = false;
         if (computeExponent) newExponent++;
         if (insertSep)
          {
            conv += SEPERATOR;
            insertSep = false;
          }
       }
      Data.fromString(conv);

      if ((*iter != '\0') && ((*iter != 'e') || (*iter != 'E')))
       {
         std::istringstream temp (iter + 1);
         temp >> Exponent;
         switch (willOverflow(Exponent, newExponent, false))
          {
            case 1:
               Infinity = true;
               Exponent = 0;
               Data = Fixed((long long) 0);
               break;

            case -1:
               Exponent = 0;
               Data = Fixed((long long) 0);
               break;

            default:
               Exponent += newExponent;
          }
       }
      else Exponent = newExponent;

      if (Data.isZero()) Exponent = 0;
    }


    /*
      This function seems to be orphaned now, as I inlined all
      of the normalization in +, -, *, and / to take advantage
      of a priori knowledge of the results.

      Not really, as + and - really need all this work.
      So does input.
    */
   Float & Float::normalize (void)
    {
      Fixed upper ((long long)10, 0), lower ((long long)1, 0);

      if (Data.isZero())
       {
         Exponent = 0;
         Data.setPrecision(Fixed::getDefaultPrecision());
       }

      else if (Data >= upper)
       {
         Data.setPrecision(Data.getPrecision() + 1);
         switch (willOverflow(Exponent, 1, false))
          {
            case 1:
               Infinity = true;
               Exponent = 0;
               Data = Fixed((long long) 0);
               break;

            default:
               Exponent++;
          }
       }
      else
       {
         while (Data < lower)
          {
            Data.setPrecision(Data.getPrecision() - 1);
            switch (willOverflow(Exponent, -1, false))
             {
               case -1:
                  Exponent = 0;
                  Data = Fixed((long long) 0);
                  break;

               default:
                  Exponent--;
             }
          }
       }

      return *this;
    }


   bool Float::isInteger (void) const
    {
      if (Exponent < 0) return false;
      if ((Exponent >= Data.getPrecision()) || Data.isZero()) return true;

      Fixed temp (Data);

      temp.setPrecision(temp.getPrecision() - Exponent);

      return temp.isInteger();
    }

   bool Float::isOdd (void) const
    {
      if (Exponent < 0) return false;
      if ((Exponent > Data.getPrecision()) || Data.isZero()) return false;

      Fixed temp (Data);

      temp.setPrecision(temp.getPrecision() - Exponent);

      return temp.isOdd();
    }

   bool Float::isEven (void) const
    {
      if (Exponent < 0) return false;
      if ((Exponent > Data.getPrecision()) || Data.isZero()) return true;

      Fixed temp (Data);

      temp.setPrecision(temp.getPrecision() - Exponent);

      return temp.isEven();
    }


    /*
      Returns -1 if overflows TO positive, ie negative overflow
               1 if overflows TO negative, positive overflow
               0 on no overflow
    */
   int Float::willOverflow (long a, long b, bool sub)
    {
       /* If b is zero, all is good. */
      if (b == 0) return 0;

      if (sub)
       {
          /* Only handle subtraction by most negative long specially. */
          /* This assumes a two's complement machine. */
         if (-b == b)
          {
             /* -a + b is good */
            if (a < 0) return 0;
             /* alternative will always overflow */
            return 1;
          }
          /* We now know that we can make this change. */
         else b = -b;
       }

       /* Handle a == 0 here just in case b is most negative long */
      if (a == 0) return 0;

       /* -a + b is always in range */
      if (((a > 0) && (b < 0)) || ((a < 0) && (b > 0))) return 0;

       /* a + b < a or b implies wrapped around to negative */
       /* This ugly casting happens because C/C++ compilers are allowed
          to assume that overflow NEVER occurs with signed arithmetic.
          So, the code may be compiled wrong. */
      if (a > 0)
         { if ((long)((unsigned long)a + (unsigned long)b) < a) return 1; }

       /* -a + -b > -a or -b implies wrapped around to positive */
      else
         { if ((long)((unsigned long)a + (unsigned long)b) > a) return -1; }

       /* All good */
      return 0;
    }


 } /* namespace BigInt */
