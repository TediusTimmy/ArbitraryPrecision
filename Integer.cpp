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

#include "Integer.hpp"

namespace BigInt
 {

   Integer::Integer () : Digits (), Sign (false) { }

    /*
      The assumption throughout all of this Integer code is based on a
      unsigned 64 bit Unit. We also assume here a 64 bit long long.

      Note: this DOES work for a two's complement LLONG_MIN.
    */
   Integer::Integer (long long input) : Digits (), Sign (false)
    {
      if (input < 0)
       {
         Sign = true;
         input = -input;
       }

      Digits += (Unit) input;
    }

   Integer::Integer (Unit input) : Digits (input), Sign (false) { }

   Integer::Integer (const std::string & input, int base) :
      Digits (), Sign (false)
    {
      fromString(input.c_str(), base);
    }

   Integer::Integer (const char * input, int base) : Digits (), Sign (false)
    {
      fromString(input, base);
    }

   Integer::Integer (const Integer & input) :
      Digits (input.Digits), Sign (input.Sign) { }

   Integer::~Integer ()
    {
      Sign = false;
    }



   bool Integer::isOdd (void) const
    {
      return Digits.isZero() ? false : !!(Digits.getDigit(0) & 1);
    }

   bool Integer::isEven (void) const
    {
      return Digits.isZero() ? true : !(Digits.getDigit(0) & 1);
    }



   long Integer::toInt (void) const //It works for its purpose.
    {
      if (Digits.length() > 1) return 0;
      return Sign ? -((long)Digits.getDigit(0)) : (long)Digits.getDigit(0);
    }



   Integer & Integer::negate (void)
    {
      if (isZero()) Sign = false;
      else Sign = !Sign;
      return *this;
    }

   Integer & Integer::abs (void)
    {
      Sign = false;
      return *this;
    }

   Integer & Integer::copySign (const Integer & src)
    {
      if (isZero()) Sign = false;
      else Sign = src.Sign;
      return *this;
    }

   Integer & Integer::setSign (bool newsign)
    {
      if (isZero()) Sign = false;
      else Sign = newsign;
      return *this;
    }



   Integer Integer::operator - (void) const
    {
      Integer returnedInteger(*this);
      return returnedInteger.negate();
    }

   Integer Integer::operator ~ (void) const
    {
      Integer returnedInteger(*this);
      returnedInteger.Sign = !Sign;
      returnedInteger.Digits.comp();
      if (returnedInteger.isZero()) returnedInteger.Sign = false;
      return returnedInteger;
    }



   Integer & Integer::operator = (const Integer & src)
    {
      if (&src == this) return *this;
      Digits = src.Digits;
      Sign = src.Sign;
      return *this;
    }



   int Integer::compare (const Integer & to) const
    {
       /*
         Positive or zero is greater than negative.
         After this test, we know that both numbers have the same sign.
       */
      if (Sign != to.Sign)
       {
         if (Sign) return -1;
         return 1;
       }

       /*
         Return the unsigned comparison, remembering that if we are
         negative, then the result is negated.
       */
      if (Sign) return -Digits.compare(to.Digits);
      return Digits.compare(to.Digits);
    }



   bool operator > (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) > 0;
    }

   bool operator < (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) < 0;
    }

   bool operator >= (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) >= 0;
    }

   bool operator <= (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) <= 0;
    }

   bool operator == (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) == 0;
    }

   bool operator != (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) != 0;
    }



    /*
      It may seem faster to have these operators define the non-assignment
      operators, but the code is structured in a manner that requires a copy.
    */
   Integer & Integer::operator += (const Integer & rval)
    {
      return (*this = *this + rval);
    }

   Integer & Integer::operator -= (const Integer & rval)
    {
      return (*this = *this - rval);
    }

   Integer & Integer::operator *= (const Integer & rval)
    {
      return (*this = *this * rval);
    }

   Integer & Integer::operator /= (const Integer & rval)
    {
      return (*this = *this / rval);
    }

   Integer & Integer::operator %= (const Integer & rval)
    {
      return (*this = *this % rval);
    }

   Integer & Integer::operator &= (const Integer & rval)
    {
      return (*this = *this & rval);
    }

   Integer & Integer::operator |= (const Integer & rval)
    {
      return (*this = *this | rval);
    }

   Integer & Integer::operator ^= (const Integer & rval)
    {
      return (*this = *this ^ rval);
    }

   Integer & Integer::operator <<= (const Integer & rval)
    {
      return (*this = *this << rval);
    }

   Integer & Integer::operator >>= (const Integer & rval)
    {
      return (*this = *this >> rval);
    }



   Integer Integer::adder (const Integer & lhs, const Integer & rhs)
    {
      Integer result;
      int oneCompare;

      if (lhs.isZero() && rhs.isZero()) return result;

      if (lhs.isZero()) return (result = rhs);
      if (rhs.isZero()) return (result = lhs);

      if (!lhs.Sign && !rhs.Sign)
       {
         result.Digits = lhs.Digits;
         result.Digits += rhs.Digits;
         result.Sign = false;
       }
      else if (lhs.Sign && rhs.Sign)
       {
         result.Digits = lhs.Digits;
         result.Digits += rhs.Digits;
         result.Sign = true;
       }
      else if (lhs.Sign && !rhs.Sign)
       {
         oneCompare = lhs.Digits.compare(rhs.Digits);
         switch (oneCompare)
          {
            case 1:
               result.Sign = true;
               result.Digits = lhs.Digits;
               result.Digits -= rhs.Digits;
               break;
            case -1:
               result.Sign = false;
               result.Digits = rhs.Digits;
               result.Digits -= lhs.Digits;
               break;
          }
       }
      else /* !lhs.sign && rhs.sign */
       {
         oneCompare = lhs.Digits.compare(rhs.Digits);
         switch (oneCompare)
          {
            case 1:
               result.Sign = false;
               result.Digits = lhs.Digits;
               result.Digits -= rhs.Digits;
               break;
            case -1:
               result.Sign = true;
               result.Digits = rhs.Digits;
               result.Digits -= lhs.Digits;
               break;
          }
       }
      return result;
    }



   Integer operator + (const Integer & lhs, const Integer & rhs)
    {
      return Integer::adder (lhs, rhs);
    }

   Integer operator - (const Integer & lhs, const Integer & rhs)
    {
      return Integer::adder (lhs, -rhs);
    }

    /*
      This is the standard O(n^2) algorithm for multiplication.
      Karatsuba multiplication is poorly implemented.
    */
#ifndef K_CUT
 #define K_CUT 56
#endif
   Integer operator * (const Integer & lhs, const Integer & rhs)
    {
      Integer result;
      BitField temp;
      long i;

       // 0 * x = x * 0 = 0
      if (lhs.isZero() || rhs.isZero()) return result;

      result.Sign = (lhs.isSigned() != rhs.isSigned());

       //Easy case 1: rhs is one digit
      if (rhs.Digits.length() == 1)
       {
         result.Digits = lhs.Digits;
         result.Digits *= rhs.Digits.getDigit(0);
         return result;
       }

       //Easy case 2: lhs is one digit
      if (lhs.Digits.length() == 1)
       {
         result.Digits = rhs.Digits;
         result.Digits *= lhs.Digits.getDigit(0);
         return result;
       }

       // Check for doing Karatsuba multiplication
      if ((rhs.Digits.length() < K_CUT) || (lhs.Digits.length() < K_CUT))
       {
          //Do long multiplication.
         if (rhs.Digits.length() >= lhs.Digits.length())
          { //iterate over lhs, as it is smaller
            for (i = 0; i < lhs.Digits.length(); i++)
             {
               temp = rhs.Digits;
               temp *= lhs.Digits.getDigit(i); //We should save time by putting
               temp <<= i * BitField::bits; //the multiply before the shift.
               result.Digits += temp;
             }
          }
         else
          { //iterate over rhs, as it is smaller
            for (i = 0; i < rhs.Digits.length(); i++)
             {
               temp = lhs.Digits;
               temp *= rhs.Digits.getDigit(i);
               temp <<= i * BitField::bits;
               result.Digits += temp;
             }
          }
       }
      else
       {
         const long cutoff = ((rhs.Digits.length() >= lhs.Digits.length()) ? rhs.Digits.length() : lhs.Digits.length()) / 2;
         Integer shift = Integer(((Unit)cutoff * BitField::bits));
         Integer shift2 = Integer(((Unit)2 * cutoff * BitField::bits));
         Integer A, B, C, D, Z0, Z2;
         lhs.Digits.split(A.Digits, B.Digits, cutoff);
         rhs.Digits.split(C.Digits, D.Digits, cutoff);

         Z2 = A * C;
         Z0 = B * D;
         result = (Z2 << shift2) + (((A + B) * (C + D) - Z2 - Z0) << shift) + Z0;
         result.Sign = (lhs.isSigned() != rhs.isSigned());
       }

      return result;
    }

   Integer operator / (const Integer & lhs, const Integer & rhs)
    {
      Integer q, r;
      Integer::divmod (lhs, rhs, q, r);
      return q;
    }

   Integer operator % (const Integer & lhs, const Integer & rhs)
    {
      Integer q, r;
      Integer::divmod (lhs, rhs, q, r);
      return r;
    }

   Integer operator & (const Integer & lhs, const Integer & rhs)
    {
      Integer result (lhs);

      result.Sign &= rhs.Sign;
      result.Digits &= rhs.Digits;

      if (result.isZero()) result.Sign = false;

      return result;
    }

   Integer operator | (const Integer & lhs, const Integer & rhs)
    {
      Integer result (lhs);

      result.Sign |= rhs.Sign;
      result.Digits |= rhs.Digits;

      return result;
    }

   Integer operator ^ (const Integer & lhs, const Integer & rhs)
    {
      Integer result (lhs);

      result.Sign ^= rhs.Sign;
      result.Digits ^= rhs.Digits;

      if (result.isZero()) result.Sign = false;

      return result;
    }

   Integer operator << (const Integer & lhs, const Integer & rhs)
    {
      Integer result = lhs;
      long shift = rhs.toInt();

      if (shift < 0) result.Digits >>= -shift;
      else result.Digits <<= shift;

      if (result.isZero()) result.Sign = false;

      return result;
    }

   Integer operator >> (const Integer & lhs, const Integer & rhs)
    {
      Integer result = lhs;
      long shift = rhs.toInt();

      if (shift < 0) result.Digits <<= -shift;
      else result.Digits >>= shift;

      if (result.isZero()) result.Sign = false;

      return result;
    }



    Integer Integer::operator++ ()
     {
       if (Sign)
        {
          Digits -= (Unit)1;
          if (Digits.isZero()) Sign = false;
        }
       else
        {
          Digits += (Unit)1;
        }
       return (*this);
     }

    Integer Integer::operator++ (int)
     {
       Integer ret (*this);
       if (Sign)
        {
          Digits -= (Unit)1;
          if (Digits.isZero()) Sign = false;
        }
       else
        {
          Digits += (Unit)1;
        }
       return ret;
     }

    Integer Integer::operator-- ()
     {
       if (Sign)
        {
          Digits += (Unit)1;
        }
       else if (Digits.isZero())
        {
          Digits += (Unit) 1;
          Sign = true;
        }
       else
        {
          Digits -= (Unit)1;
        }
       return (*this);
     }

    Integer Integer::operator-- (int)
     {
       Integer ret (*this);
       if (Sign)
        {
          Digits += (Unit)1;
        }
       else if (Digits.isZero())
        {
          Digits += (Unit) 1;
          Sign = true;
        }
       else
        {
          Digits -= (Unit)1;
        }
       return ret;
     }



    /*
      This is floor(64 * log(2) / log(index + 2)). [63 for powers of 2]
      It will be used in toString and fromString to unpack and pack the
      BigInt "faster".
    */
   static signed char maxdigits [35] =
    {
      63, 40, 31, 27, 24, 22, 21, 20, 19, 18, 17, 17, 16, 16, 15, 15, 15,
      15, 14, 14, 14, 14, 13, 13, 13, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12
    };
    /*
      These are the powers that correspond to these numbers of digits.
      They are used in toString to pack Units.
    */
   static Unit powers [35] =
    {
      9223372036854775808ULL, 12157665459056928801ULL,  4611686018427387904ULL,  7450580596923828125ULL, 4738381338321616896ULL,
      3909821048582988049ULL,  9223372036854775808ULL, 12157665459056928801ULL, 10000000000000000000ULL, 5559917313492231481ULL,
      2218611106740436992ULL,  8650415919381337933ULL,  2177953337809371136ULL,  6568408355712890625ULL, 1152921504606846976ULL,
      2862423051509815793ULL,  6746640616477458432ULL, 15181127029874798299ULL,  1638400000000000000ULL, 3243919932521508681ULL,
      6221821273427820544ULL, 11592836324538749809ULL,   876488338465357824ULL,  1490116119384765625ULL, 2481152873203736576ULL,
      4052555153018976267ULL,  6502111422497947648ULL, 10260628712958602189ULL, 15943230000000000000ULL,  787662783788549761ULL,
      1152921504606846976ULL,  1667889514952984961ULL,  2386420683693101056ULL,  3379220508056640625ULL, 4738381338321616896ULL
    };



   void Integer::fromString (const std::string & src, int base)
    {
      fromString(src.c_str(), base);
    }

   void Integer::fromString (const char * src, int base)
    {
      int curchar, digits;
      Unit fatdigit, power;
      const char * iter = src;
      bool done = false;

      if ((base < 2) || (base > 36)) return;

      if (*iter == '-')
       {
         Sign = true;
         iter++;
       }
      else if (*iter == '+')
       {
         Sign = false;
         iter++;
       }
      else Sign = false;

      Digits = BitField();

       /*
         This digit packing algorithm should save alot of time for big
         numbers, and shouldn't add appreciable time to small numbers.
       */
      for (; !done && (*iter != '\0');)
       {
         digits = maxdigits[base - 2];
         fatdigit = 0;
         power = 1;

         for (; (*iter != '\0') && digits; iter++, digits--)
          {
            curchar = *iter;

            if ((curchar >= '0') && (curchar <= '9')) //Only handles ASCII
               curchar = curchar - '0';
            else if ((curchar >= 'A') && (curchar <= 'Z'))
               curchar = curchar - 'A' + 10;
            else if ((curchar >= 'a') && (curchar <= 'z'))
               curchar = curchar - 'a' + 10;
            else
             {
               done = true;
               break;
             }

             /*
               Adopt the bc model, so that 'zzz' is the largest 3 digit number
               invariant of base.
             */
            if (curchar >= base) curchar = base - 1;

            fatdigit *= base;
            fatdigit += curchar;

            power *= base;
          }

         Digits *= power;
         Digits += fatdigit;
       }

      if (isZero()) Sign = false;
    }



    /*
      Digit packing is NECESSARY for toString to be efficient.
      Consider this example: 2^2^20;
         2^2^20 is represented by 32769 Units and 315653 digits.
         Digit by digit, we do about 315653 long divisions as the 32769 Units
         reduce to zero. So, the total number of divides is approximated by
         the integral from 0 to 315653 of -(32769 / 315653) * x + 32769.
         We get 32769 * 315653 / 2 or about 5171816578 divides. Five BILLION.
         2^2^20 is represented by only 35073 groups of 9 digits.
         We take the integral from 0 to 35073 of -(32769 / 35073) * x + 32769.
         This comes out to 32769 * 35073 / 2, and we add the 9 * 35073
         divisions needed to unpack each of those groups, for a total of
         32787 * 35073 / 2 or about 574969225 divides. We have decreased the
         number of divides by almost an order of magnitude.
    */
   std::string Integer::toString (int base) const
    {
      std::string result;
      BitField cpy (Digits);
      Unit digit;
      char table [] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", * temp = NULL;
      int i, d;
      const int buffsize = 2048;

      if ((base < 2) || (base > 36))
       {
         result = "";
       }
      else if (isZero())
       {
         result = "0";
       }
      else
       {
         temp = new char [buffsize];
         temp [buffsize - 1] = '\0';
         i = buffsize - 2;
         while (!cpy.isZero())
          {
            digit = (cpy /= powers[base - 2]);
            d = maxdigits[base - 2];

            while (d--)
             {
               temp[i--] = table[digit % base];
               digit /= base;
               if (i == -1)
                {
                  if (cpy.isZero() && (digit == 0))
                   {
                     i++;
                     while (temp[i] == '0') i++;
                     result = (temp + i) + result;
                     i = buffsize - 2;
                     break;
                   }
                  else
                   {
                     result = temp + result;
                     i = buffsize - 2;
                   }
                }
             }
          }
         if (i != (buffsize - 2))
          {
            i++;
            while (temp[i] == '0') i++;
            result = (temp + i) + result;
          }
         if (isSigned()) result = "-" + result;
         delete [] temp;
         temp = NULL;
       }

      return result;
    }



   void Integer::divmod (const Integer & lhs, const Integer & rhs,
                         Integer & q, Integer & r)
    {
      Integer dd (lhs) /* DividenD */, dr (rhs) /* DivisoR */;
      BitField temp, subr;
      Unit smallr, test, top, secd;
      int oneCompare, shift;
      long digit;

       // Zero divided by zero?!... Do NOTHING!!!!!
      if (dd.isZero() && dr.isZero()) return;

       // Something else divided by zero?!... Do NOTHING!!!!!
       // Actually, return q and r such that dr * q + r = dd
      if (dr.isZero())
       {
         q = dr;
         r = dd;
         return;
       }

       // 0 / x = 0 rem 0
      if (dd.isZero())
       {
         q = dd;
         r = dd;
         return;
       }

      q.Sign = (dd.isSigned() != dr.isSigned());
      r.Sign = dd.isSigned();

      oneCompare = dd.Digits.compare(dr.Digits);

       // x / x = 1 rem 0
      if (oneCompare == 0)
       {
         q.Digits = BitField((Unit)1);
         r = Integer();
         return;
       }
       // 2 / 5 = 0 rem 2  <- Cases like these
      else if (oneCompare < 0)
       {
         q = Integer();
         r.Digits = dd.Digits;
         return;
       }

       // We now know that: dd != 0, dr != 0, and dd > dr.
       // Let's do the simple case first.
      if (dr.Digits.length() == 1)
       {
         smallr = (dd.Digits /= dr.Digits.getDigit(0));

         q.Digits = dd.Digits;
         r.Digits = BitField(smallr);

          //What an annoying bug to figure out.
         if (q.Digits.isZero()) q.Sign = false;
         if (r.Digits.isZero()) r.Sign = false;

         return;
       }

       //Our only choice now is long division.
       /*
         Let's implement Knuth's Division.
         I don't think I understood this reading it in TAOCP. I had to read
         several implementations to get it right.

         I find it best to actually try this out on paper to see how it
         works.
         Take 12|34|56|78 / 17|69
         1234 / 17 = 72          1769 * 72 * 100 = 12736800
                                 1769 * 71 * 100 = 12559900
                                 1769 * 70 * 100 = 12383000
                                 1769 * 69 * 100 = 12206100
         12345678 - 12206100 = 139578
         1395 / 17 = 82          1769 * 82 * 1 = 145058
                                 1769 * 81 * 1 = 143289
                                 1769 * 80 * 1 = 141520
                                 1769 * 79 * 1 = 139751
                                 1769 * 78 * 1 = 137982
         139578 - 137982 = 1596
         Quotient = 6978 (69|78) Remainder = 1596
       */
      q.Digits = BitField();

       //we normalize dr so that it's msb is bit 31
      shift = BitField::bits - dr.Digits.msb() % BitField::bits - 1;

       //shift up our divisor and dividend
      dr.Digits <<= shift;
      dd.Digits <<= shift;

       //get the most significant units of the divisor
      top = dr.Digits.getDigit(dr.Digits.length() - 1);
      secd = dr.Digits.getDigit(dr.Digits.length() - 2);

       //get the place value of the first digit
       //Generate alot of leading zeros to make sure that
       //we never lose a leading one.
      digit = dd.Digits.length() - dr.Digits.length();

       // subr will hold the shifted divisor
      subr = dr.Digits;
      subr <<= digit * BitField::bits;

      while (digit >= 0)
       {
          //first, compute an approximation of the current digit of q
         test = dd.Digits.divApprox(top, secd,
          //If the lengths are equal, we have the possibility of generating
          //a guess that is astronomically higher than it should be,
          //effectively causing the program to hang. Very obscure bug.
            dd.Digits.length() < (dr.Digits.length() + digit + 1));
         temp = dr.Digits;
         temp *= test;
         temp <<= digit * BitField::bits;

          //make sure our approximation isn't too high
         if (temp > dd.Digits)
          {
            test--;
            temp -= subr;
          }

          //add to quotient, and subtract from running remainder
         q.Digits <<= BitField::bits;
         q.Digits += test;
         dd.Digits -= temp;

          // shift down our shifted divisor and decrement the place value
         subr >>= BitField::bits;
         digit--;
       }

       //assign the remainder to r
      r.Digits = dd.Digits;

       //shift remainder back down
      r.Digits >>= shift;

       //don't let either result be -0
      if (q.Digits.isZero()) q.Sign = false; //q should never be 0 here
      if (r.Digits.isZero()) r.Sign = false;
    }



    /*
      It may be faster to figure out the minimal multiplications to produce
      the exponent, but the overhead might not be worth it until exponents are
      large. Binary exponentiation works here.

      I implemented this in base 10, with everything being raised to the 10th
      power after each iteration. Optimize as I could, it was still slow.
    */
   Integer pow (const Integer & lhs, const Integer & rhs)
    {
      Integer temp (lhs), result;

      if (rhs.isSigned()) return result;

      result = Integer((Unit) 1);

      if (rhs.isZero()) return result;

      for (long i = 0; /* I moved this down. */ ; i++)
       {
         if (rhs.Digits.getDigit(i / BitField::bits)
              & (((Unit)1) << (i % BitField::bits))) result *= temp;

          /*
            We should get a speed boost putting this test here,
            as we aren't doing one too many multiplications (which matters
            for cases like 2^2^20).
          */
         if (i == rhs.Digits.msb()) break;

         temp *= temp;
       }

      return result;
    }



    /*
      Find the truncated square root via Newton's Method.
    */
   Integer sqrt (const Integer & src)
    {
      Integer temp (src), last;

       // Range errors? My stove's fine.
      if (src.isSigned() || src.isZero()) return last;

       /*
         Generate a good first guess.
         Invariant of base, the square root of a number will have about
         half as many digits as the number. For Integers. Generalized to
         Reals (in Scientific Notation), the square root of a number has an
         exponent that is about half of the exponent of the number.
       */
      temp.Digits >>= (temp.Digits.msb() / 2);

      while ((temp - last).abs() > Integer((Unit) 1))
       {
         last = temp;
         temp = temp + src / temp; //I took this from Rossi's dfp (Decimal
         temp.Digits >>= 1; //Floating Point) class, where it made less sense
       }

       /*
         Newton's Method, in this form, converges on the wrong number for one
         less than the perfect square of an odd number, and we want the
         nearest integer whose square is less than or equal to the target
         number.
       */
      if ((temp * temp) > src) --temp;

      return temp;
    }



    /*
      Factorial. Return 0 on negative arg.
    */
   Integer fact (const Integer & ofThis)
    {
      Integer i (ofThis), product ((Unit) 1);

      if (ofThis.isSigned()) return Integer((Unit) 0);

      while (!(!i)) { product *= i; --i; }

      return product;
    }

   Integer permutation (const Integer & items, const Integer & taken)
    {
      Integer i (items), product ((Unit) 1), limit (items - taken);

      if (items.isSigned() || taken.isSigned() || limit.isSigned())
         return Integer((Unit) 0);

      while (i > limit) { product *= i; --i; }

      return product;
    }

   Integer combination (const Integer & items, const Integer & taken)
    {
      Integer p;

      p = permutation(items, taken);

      if (p.isZero()) return p;

      return p / fact (taken);
    }

    /*
      Basic implementation of Euclid's Algorithm.
    */
   Integer gcd (const Integer & lhs, const Integer & rhs)
    {
      Integer temp1 (lhs), temp2 (rhs);

      while (!temp1.isZero() && !temp2.isZero())
       {
         if (temp1 >= temp2) temp1 %= temp2;
         else temp2 %= temp1;
       }

      if (temp1.isZero()) return temp2;
      return temp1;
    }

   Integer lcm (const Integer & lhs, const Integer & rhs)
    {
      return (lhs / gcd(lhs, rhs)) * rhs;
    }

    /*
      Use a minimal form of the Extended Euclidean Algorithm to compute
      the Modular Inverse.
    */
   Integer modInv (const Integer & lhs, const Integer & rhs)
    {
      Integer temp1 (lhs), temp2 (rhs), quot, result;
      Integer resulta ((Unit) 1), resultb ((Unit) 0);

      while (!temp1.isZero() && !temp2.isZero())
       {
         if (temp1 >= temp2)
          {
            Integer::divmod(temp1, temp2, quot, temp1);
            resulta -= quot * resultb;
            result = resultb;
          }
         else
          {
            Integer::divmod(temp2, temp1, quot, temp2);
            resultb -= quot * resulta;
            result = resulta;
          }
       }

      if (temp1.isZero() && (temp2 != Integer ((Unit) 1)))
         return Integer ((Unit) 0);
      else if (temp2.isZero() && (temp1 != Integer ((Unit) 1)))
         return Integer ((Unit) 0);

      result %= rhs;
      if (result.isSigned()) result += rhs;
      return result;
    }

   void fibMult (Integer lhs[3], const Integer rhs[3])
    {
      Integer result[3];

      result[0] = lhs[0] * rhs[0] + lhs[1] * rhs[1];
      result[1] = lhs[0] * rhs[1] + lhs[1] * rhs[2];
      result[2] = lhs[1] * rhs[1] + lhs[2] * rhs[2];

      lhs[0] = result[0];
      lhs[1] = result[1];
      lhs[2] = result[2];
    }

   Integer fib (const Integer & number)
    {
      Integer result[3], one[3], temp (number);

      if (number.isSigned() || number.isZero()) return Integer((Unit) 0);

      result[0] = Integer((Unit) 1);
      result[2] = result[0];

      one[0] = result[0];
      one[1] = result[0];

      --temp;
      if (temp.isZero()) return result[0];

      for (long i = temp.msb(); /* I moved this down. */ ; --i)
       {
         if (temp.getDigit(i / BitField::bits)
              & (((Unit)1) << (i % BitField::bits))) fibMult(result, one);

          /*
            We should get a speed boost putting this test here,
            as we aren't doing one too many multiplications.
          */
         if (i == 0) break;

         fibMult(result, result);
       }

      return result[0];
    }

    /*
      I put these in the header and the compiler exploded.
    */
   Integer abs (const Integer & of)
      { return (of.isSigned()) ? -of: of; }
   Integer min (const Integer & a, const Integer & b)
      { return (b < a) ? b : a; }
   Integer max (const Integer & a, const Integer & b)
      { return (a < b) ? b : a; }

    /*
      Mathematically proper division.
    */
   Integer div (const Integer & lhs, const Integer & rhs)
    {
      Integer result, remainder;
      Integer::divmod(lhs, rhs, result, remainder);

       /*
          By the Division Algorithm: 0 <= remainder < |divisor|
          So, if the remainder is negative, we increment the magnitude of the
          quotient.
          We only break  0 <= remainder < |divisor|  when the divisor is zero.
          Then the quotient is zero, and the remainder is the dividend,
          retaining the relation:
             divident = divisor * quotient + remainder
       */
      if (remainder.isSigned() && !(rhs.isZero()))
       {
         if (lhs.isSigned() != rhs.isSigned()) --result;
         else ++result;
       }

      return result;
    }

    /*
      Mathematically proper modulo.
    */
   Integer mod (const Integer & lhs, const Integer & rhs)
    {
      Integer result;

      result = lhs % rhs;

       /*
          By the Division Algorithm: 0 <= remainder < |divisor|
          So, if this is negative, we reinterpret the result as its positive
          value mod the absolute value of the divisor.
       */
      if (result.isSigned())
       {
         if (rhs.isSigned()) result -= rhs;
         else result += rhs;
       }

      return result;
    }

   void divmod (const Integer & lhs, const Integer & rhs,
      Integer & quot, Integer & rem)
    {
      Integer Rhs (rhs);
      bool lhsS;

      lhsS = lhs.isSigned();

      Integer::divmod(lhs, rhs, quot, rem);


      if (rem.isSigned() && !(Rhs.isZero()))
       {
         if (lhsS != Rhs.isSigned()) --quot;
         else ++quot;
       }

      if (rem.isSigned())
       {
         if (Rhs.isSigned()) rem -= Rhs;
         else rem += Rhs;
       }

      return;
    }

   Integer powMod (const Integer & base, const Integer & exp,
      const Integer & Mod)
    {
      Integer temp, result, nexp (exp);
      bool invert = false;

      result = Integer((Unit) 1);

      if (nexp.isZero()) return result;

      if (nexp.isSigned())
       {
         nexp.setSign(false);
         invert = true;
       }

      temp = mod(base, Mod);

      for (long i = 0; /* This is in the middle of the loop. */ ; i++)
       {
         if (nexp.getDigit(i / BitField::bits) & (((Unit)1) << (i % BitField::bits)))
            result = mod(result * temp, Mod);

         if (i == nexp.msb()) break;

         temp = mod(temp * temp, Mod);
       }

      if (invert) result = modInv(result, Mod);

      return result;
    }

    /*
      Extended Euclidean Algorithm:
         a * lhs + b * rhs = gcd
    */
   void eEuclid (const Integer & lhs, const Integer & rhs,
      Integer & gcd, Integer & a, Integer & b)
    {
      Integer c0 (lhs), c1 (rhs), quot;
      Integer a0 ((Unit) 1), a1 ((Unit) 0), b0 ((Unit) 0), b1 ((Unit) 1);

      while (!c0.isZero() && !c1.isZero())
       {
         if (c0 >= c1)
          {
            divmod(c0, c1, quot, c0);
            a0 -= quot * a1;
            b0 -= quot * b1;
          }
         else
          {
            divmod(c1, c0, quot, c1);
            a1 -= quot * a0;
            b1 -= quot * b0;
          }
       }

      if (c0.isZero())
       {
         a = a1;
         b = b1;
         gcd = c1;
       }
      if (c1.isZero())
       {
         a = a0;
         b = b0;
         gcd = c0;
       }

      return;
    }



 } /* namespace BigInt */
