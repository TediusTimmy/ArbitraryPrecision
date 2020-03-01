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

#include "BitField.hpp"
#include <cstring>

namespace BigInt
 {

    /*
      Note to porters:

      Unit is defined in BitField.hpp. It is currently an "unsigned int".
      It should be an unsigned type. You can avoid using an unsigned type
      by decreasing BitField::bits.

      BitField::bits is the number of bits in our "Unit". It can be decreased
      so that an unsigned type isn't needed. Decreasing it can also allow you
      to remove the upcast in += and -=.

      If you are using less bits than your word size, uncomment the commented
      portion of mask. The comments are just to force the compiler to shut up.

      NEXT_TYPE is the type used for upcasting. /= uses "unsigned NEXT_TYPE",
      so I don't know if it can be a typedef. NEXT_TYPE should be a type that
      has at least BitField::bits * 2 bits in it.

      Humorously, in making changes for LP64: the only occurrance of "long"
      had to change to an "int", and all "int"s had to change to "long"s.
      It is a shame that GCC doesn't seem to have a 128-bit data type for
      64-bit machines, but that means the 32-bit and 64-bit code can be the
      same!
    */


   const int BitField::bits = 32;
   const static Unit mask = (Unit) /*(1 << BitField::bits)*/ - 1;

   #define NEXT_TYPE long long



   BitField::BitHolder::BitHolder () :
      Length (0), Size (0) { }

   BitField::BitHolder::BitHolder (const BitHolder & src, long extra) :
      Length (0), Size (0)
    {
         Data.reset(new Unit [src.Length + extra]);
         Length = src.Length;
         Size = src.Length + extra;

         std::memcpy(Data.get(), src.Data.get(), Length * sizeof(Unit));
    }

   BitField::BitHolder::~BitHolder ()
    {
       /*
         Alot of finalization is added before deallocation in case we need
         to hunt down bugs.
       */
      Data.reset(nullptr);
      Length = 0;
      Size = 0;
    }



   BitField::BitField () : Zero (true) { }

   BitField::BitField (Unit src) : Zero (true)
    {
      if (src != 0)
       {
         Zero = false;
         Data = std::make_shared<BitHolder>();

         Data->Data.reset(new Unit [1]);
         Data->Length = 1;
         Data->Size = 1;

         Data->Data[0] = src;
       }
    }

   BitField::BitField (const BitField & src) :
      Zero (src.Zero)
    {
      if (!src.isZero())
       {
         Data = src.Data;
       }
    }



   BitField::~BitField ()
    {
      if (!Zero)
       {
         Data.reset();
         Zero = true;
       }
    }



    /*
      Note that neither this nor >>= support negative amounts.
    */
   void BitField::operator <<= (long amount)
    {
      long addedUnits = amount / bits;
      int shiftAmount = amount % bits;
      int rightShift = bits - shiftAmount;
      long newLength;
      Unit carry = 0, oldCarry, * newData = nullptr;

       /* Are we essentially doing nothing? */
      if (Zero || (amount == 0)) return;

       /*
         As BitField is composed of only mutator methods, one of the first
         things we do is make sure we "own" the Data that we are mutating.
       */
      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data, addedUnits + 1);
       }

       /*
         Shift the data. We do the remainder mod bits here, as the quotient
         is just the number of Units we need to add to the number.
       */
      if (shiftAmount != 0)
       {
         for (long i = 0; i < Data->Length; i++)
          {
            oldCarry = carry;
            carry = Data->Data[i] >> rightShift;
            Data->Data[i] = (Data->Data[i] << shiftAmount) | oldCarry;
          }
       }

      if ((addedUnits != 0) || (carry != 0))
       {
         newLength = Data->Length + addedUnits + (carry == 0 ? 0 : 1);
         if (newLength > Data->Size)
          {
            newData = new Unit [newLength];

            std::memcpy(newData + addedUnits, Data->Data.get(),
               Data->Length * sizeof(Unit));
            if (addedUnits != 0)
               std::memset(newData, '\0', addedUnits * sizeof(Unit));

            Data->Data.reset(newData);
            newData = nullptr;

            Data->Size = newLength;
          }
         else
          {
            if (addedUnits != 0)
             {
               std::memmove(Data->Data.get() + addedUnits, Data->Data.get(),
                  Data->Length * sizeof(Unit));
               std::memset(Data->Data.get(), '\0', addedUnits * sizeof(Unit));
             }
          }

         Data->Length = newLength;
         if (carry != 0) Data->Data[Data->Length - 1] = carry;
       }
    }

   void BitField::operator >>= (long amount)
    {
      long lessUnits = amount / bits;
      int shiftAmount = amount % bits;
      int leftShift = bits - shiftAmount;
      Unit carry = 0, oldCarry;

      if ((Zero) || (amount == 0)) return;

       /* Are we just making ourself Zero? */
      if (lessUnits >= Data->Length)
       {
         Data.reset();
         Zero = true;
         return;
       }

      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data);
       }

      if (lessUnits != 0)
       {
         Data->Length -= lessUnits;
         std::memmove(Data->Data.get(), Data->Data.get() + lessUnits,
            Data->Length * sizeof(Unit));
       }

      if (shiftAmount != 0)
       {
         for (long i = Data->Length - 1; i >= 0 ; i--)
          {
            oldCarry = carry;
            carry = (Data->Data[i] << leftShift) & mask;
            Data->Data[i] = (Data->Data[i] >> shiftAmount) | oldCarry;
          }
       }

      if (Data->Data[Data->Length - 1] == 0)
       {
         Data->Length--;
         if (Data->Length == 0)
          {
            Data.reset();
            Zero = true;
          }
       }
    }



   void BitField::operator = (const BitField & src)
    {
      if (&src == this) return;
      if (!Zero) //then make us zero
       {
         Data.reset();
         Zero = true;
       }
      if (src.isZero()) return;
      Data = src.Data;
      Zero = false;
    }



   void BitField::operator -= (const BitField & rhs)
    {
      if (rhs.isZero()) return; //We only test if we are subtracting zero.
       /*
         We could add a compare here, and early-out if lhs == rhs : zero.
         We could then add an error check as rhs > lhs is not supported.
         Most of the time, though, this is just going to waste time, and
         Integer should never raise the error condition, so don't do it.
       */

      Unit carry = 0;
      NEXT_TYPE temp;
      long i;

      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data);
       }

      for (i = 0; i < rhs.Data->Length; i++)
       {
         temp = (NEXT_TYPE)Data->Data[i] - rhs.Data->Data[i] - carry;
         Data->Data[i] = (Unit)(temp & mask);
         carry = (Unit)(temp >> bits) & (Unit)1; // Borrow out is at most 1
       }

      for (; (i < Data->Length) && (carry != 0); i++)
       {
         temp = (NEXT_TYPE)Data->Data[i] - carry;
         Data->Data[i] = (Unit)(temp & mask);
         carry = (Unit)(temp >> bits) & (Unit)1;
       }

      if (Data->Data[Data->Length - 1] == 0)
       {
         for (i = Data->Length - 1; (i >= 0) && (Data->Data[i] == 0); i--) ;
         Data->Length = i + 1;

         if (i == -1) //Are we Zero?
          {
            Data.reset();
            Zero = true;
          }
       }
    }

   void BitField::operator += (const BitField & rhs)
    {
      if (rhs.isZero()) return; //Simple case 1: we are adding zero
      if (Zero) //Simple case 2: something is being added to zero
       {
         Data = rhs.Data;
         Zero = false;
         return;
       }

      Unit * newData = nullptr, carry = 0;
      NEXT_TYPE temp;
      long i;
      std::shared_ptr<BitHolder> Rhs;

      Rhs = rhs.Data;

       /*
         For simplicity's sake, we add the smaller number to the larger.
         This may require us to clone the larger, which we'd have to do
         anyway, but with more complex logic.
       */
      if (Data->Length < Rhs->Length)
       {
         Data.swap(Rhs);
       }

      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data, 1);
       }

      for (i = 0; i < Rhs->Length; i++)
       {
         temp = (NEXT_TYPE)Data->Data[i] + Rhs->Data[i] + carry;
         Data->Data[i] = (Unit)(temp & mask);
         carry = (Unit)(temp >> bits);
       }

      for (; (i < Data->Length) && (carry != 0); i++)
       {
         temp = (NEXT_TYPE)Data->Data[i] + carry;
         Data->Data[i] = (Unit)(temp & mask);
         carry = (Unit)(temp >> bits);
       }

      if (carry != 0)
       {
         if ((Data->Length + 1) > Data->Size)
          {
            newData = new Unit [Data->Length + 1];

            std::memcpy(newData, Data->Data.get(), Data->Length * sizeof(Unit));

            Data->Data.reset(newData);
            newData = nullptr;

            Data->Size++;
          }

         Data->Data[Data->Length] = carry;
         Data->Length++;
       }

      Rhs.reset();
    }

   void BitField::operator &= (const BitField & rhs)
    {
      if (Zero) return; //Simple case 1: 0 & x = 0
      if (rhs.isZero()) //Simple case 2: x & 0 = 0
       {
         Data.reset();
         Zero = true;
         return;
       }

      long i;
      std::shared_ptr<BitHolder> Rhs;

      Rhs = rhs.Data;

       /*
         We AND the larger number to the smaller (saving space),
         unless we own our Data.
       */
      if ((Data.use_count() != 1) && (Data->Length > Rhs->Length))
       {
         Data.swap(Rhs);
       }

      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data);
       }

      if (Data->Length > Rhs->Length)
       {
         Data->Length = Rhs->Length;
       }

      for (i = 0; i < Data->Length; i++)
         Data->Data[i] &= Rhs->Data[i];

       /*
         Handle cancellation.
       */
      if (Data->Data[Data->Length - 1] == 0)
       {
         for (i = Data->Length - 1; (i >= 0) && (Data->Data[i] == 0); i--) ;
         Data->Length = i + 1;

         if (i == -1) //Are we Zero?
          {
            Data.reset();
            Zero = true;
          }
       }

      Rhs.reset();
    }

   void BitField::operator |= (const BitField & rhs)
    {
      if (rhs.isZero()) return; //Simple case 1: x | 0 = x
      if (Zero) //Simple case 2: 0 | x = x
       {
         Data = rhs.Data;
         Zero = false;
         return;
       }

      long i;
      std::shared_ptr<BitHolder> Rhs;

      Rhs = rhs.Data;

       /*
         We OR the smaller number to the larger.
       */
      if (Data->Length < Rhs->Length)
       {
         Data.swap(Rhs);
       }

      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data);
       }

      for (i = 0; i < Rhs->Length; i++)
         Data->Data[i] |= Rhs->Data[i];

       /*
         OR neither cancels, nor increases a number's size.
       */
      Rhs.reset();
    }

   void BitField::operator ^= (const BitField & rhs)
    {
      if (rhs.isZero()) return; //Simple case 1: x ^ 0 = x
      if (Zero) //Simple case 2: 0 ^ x = x
       {
         Data = rhs.Data;
         Zero = false;
         return;
       }

      long i;
      std::shared_ptr<BitHolder> Rhs;

      Rhs = rhs.Data;

       /*
         We XOR the smaller number to the larger.
       */
      if (Data->Length < Rhs->Length)
       {
         Data.swap(Rhs);
       }

      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data);
       }

      for (i = 0; i < Rhs->Length; i++)
         Data->Data[i] ^= Rhs->Data[i];

       /*
         Handle cancellation,
         which will only occur if Rhs->Length == Data->Length.
       */
      if (Data->Data[Data->Length - 1] == 0)
       {
         for (i = Data->Length - 1; (i >= 0) && (Data->Data[i] == 0); i--) ;
         Data->Length = i + 1;

         if (i == -1) //Are we Zero?
          {
            Data.reset();
            Zero = true;
          }
       }

      Rhs.reset();
    }



   void BitField::operator -= (Unit carry)
    {
      if (carry == 0) return;

      NEXT_TYPE temp;
      long i;

      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data);
       }

      for (i = 0; (i < Data->Length) && (carry != 0); i++)
       {
         temp = (NEXT_TYPE)Data->Data[i] - carry;
         Data->Data[i] = (Unit)(temp & mask);
         carry = (Unit)(temp >> bits) & (Unit)1;
       }

      if (Data->Data[Data->Length - 1] == 0)
       {
         for (i = Data->Length - 1; (i >= 0) && (Data->Data[i] == 0); i--) ;
         Data->Length = i + 1;

         if (i == -1) //Are we Zero?
          {
            Data.reset();
            Zero = true;
          }
       }
    }

   void BitField::operator += (Unit carry)
    {
      if (carry == 0) return;

      Unit * newData = nullptr;
      NEXT_TYPE temp;
      long i;

      if (Zero)
       {
         Zero = false;
         Data = std::make_shared<BitHolder>();

         Data->Data.reset(new Unit [1]);
         Data->Length = 1;
         Data->Size = 1;

         Data->Data[0] = carry;
         return;
       }
      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data, 1);
       }

      for (i = 0; (i < Data->Length) && (carry != 0); i++)
       {
         temp = (NEXT_TYPE)Data->Data[i] + carry;
         Data->Data[i] = (Unit)(temp & mask);
         carry = (Unit)(temp >> bits);
       }

      if (carry != 0)
       {
         if ((Data->Length + 1) > Data->Size)
          {
            newData = new Unit [Data->Length + 1];

            std::memcpy(newData, Data->Data.get(), Data->Length * sizeof(Unit));

            Data->Data.reset(newData);
            newData = nullptr;

            Data->Size++;
          }

         Data->Data[Data->Length] = carry;
         Data->Length++;
       }
    }



   void BitField::operator *= (Unit mult)
    {
      if (Zero || (mult == 1)) return; // Simple cases 1 + 2
       // 1: 0 * x = 0
       // 2: x * 1 = x

      if (mult == 0) // Simple case 3: x * 0 = 0
       {
         if (!Zero)
          {
            Data.reset();
            Zero = true;
          }
         return;
       }

      Unit carry = 0, temp, * newData = nullptr;

      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data, 1);
       }

      for (long i = 0; i < Data->Length; i++)
       {
         temp = Data->Data[i];
         Data->Data[i] =
            (Unit)((((unsigned NEXT_TYPE)temp * mult) + carry) & mask);
         carry =
            (Unit)((((unsigned NEXT_TYPE)temp * mult) + carry) >> bits);
       }

      if (carry != 0)
       {
         if ((Data->Length + 1) > Data->Size)
          {
            newData = new Unit [Data->Length + 1];

            std::memcpy(newData, Data->Data.get(), Data->Length * sizeof(Unit));

            Data->Data.reset(newData);
            newData = nullptr;

            Data->Size++;
          }

         Data->Data[Data->Length] = carry;
         Data->Length++;
       }
    }

    /*
      This is a little wierd. We do the division, mutating the BitField,
      and return the remainder. This convention makes perfect sense in
      Integer::toString().
    */
   Unit BitField::operator /= (Unit divisor)
    {
      if (Zero) return 0; // There's really no good behavior for this.
       // Throwing exceptions is NOT good behavior.

      Unit rem = 0, temp;

      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data);
       }

      for (long i = Data->Length - 1; i >= 0; i--)
       {
         temp = Data->Data[i];
         Data->Data[i] =
            (Unit)((((unsigned NEXT_TYPE)rem << bits) | temp) / divisor);
         rem =
            (Unit)((((unsigned NEXT_TYPE)rem << bits) | temp) % divisor);
       }

      if (Data->Data[Data->Length - 1] == 0)
       {
         Data->Length--;
         if (Data->Length == 0)
          {
            Data.reset();
            Zero = true;
          }
       }

      return rem;
    }



   int BitField::compare (const BitField & rhs) const
    {
      if (Zero && rhs.isZero()) return 0;

      if (Zero && !rhs.isZero()) return -1;

      if (!Zero && rhs.isZero()) return 1;

      if (Data->Length > rhs.Data->Length) return 1;

      if (Data->Length < rhs.Data->Length) return -1;

      long i;
      for (i = Data->Length - 1;
         (i >= 0) && (Data->Data[i] == rhs.Data->Data[i]); i--) ;

      if (i == -1) return 0;

      if (Data->Data[i] > rhs.Data->Data[i]) return 1;
      return -1;
    }



   bool BitField::operator > (const BitField & rhs) const
    {
      return compare(rhs) > 0;
    }

   bool BitField::operator < (const BitField & rhs) const
    {
      return compare(rhs) < 0;
    }

   bool BitField::operator >= (const BitField & rhs) const
    {
      return compare(rhs) >= 0;
    }

   bool BitField::operator <= (const BitField & rhs) const
    {
      return compare(rhs) <= 0;
    }

   bool BitField::operator == (const BitField & rhs) const
    {
      return compare(rhs) == 0;
    }

   bool BitField::operator != (const BitField & rhs) const
    {
      return compare(rhs) != 0;
    }



    /*
      I added this for Division, but it seems that cryptographic packages
      implementing RSA also need this.
    */
   long BitField::msb (void) const
    {
      if (Zero) return -1;

      long result = Data->Length * bits - 1;

      for (Unit i = (1 << (bits - 1));
           !(Data->Data[Data->Length - 1] & i);
           i >>= 1, result--) ;

      return result;
    }



   Unit BitField::getDigit (long index) const
    {
      if (Zero) return 0;
      if ((index < 0) || (index >= Data->Length)) return 0;
      return Data->Data[index];
    }



    /*
      This is here because Integer::divmod has no idea what NEXT_TYPE is.
      shortGuess handles a strange bug in division: see division code.

      From Knuth, TAOCP vol. 2:
         approx is at most off by 2
         tempering will solve ALL instances off by 1 and most off by 2
    */
   Unit BitField::divApprox (Unit divisor, Unit sec, bool shortGuess) const
    {
      if (Zero) return 0;

      Unit approx;
      unsigned NEXT_TYPE lhs, rhs;

       /*
         Handle the need for a leading zero.
         Risk generating a leading zero to ensure that a leading 1 isn't
         dropped.
       */
      if (shortGuess)
       {
         approx = Data->Data[Data->Length - 1] / divisor;

         rhs = (((unsigned NEXT_TYPE)Data->Data[Data->Length - 1] %
            divisor) << bits) | Data->Data[Data->Length - 2];
       }
      else
       {
         approx = (Unit)((((unsigned NEXT_TYPE)Data->Data[Data->Length - 1] <<
            bits) | Data->Data[Data->Length - 2]) / divisor);

         rhs = (((((unsigned NEXT_TYPE)Data->Data[Data->Length - 1] << bits) |
            Data->Data[Data->Length - 2]) % divisor) << bits) |
            Data->Data[Data->Length - 3];
       }

      lhs = (unsigned NEXT_TYPE)approx * sec;

      if (lhs > rhs) approx--;

      return approx;
    }



    /*
      One's complement. I don't like how it works, but c'est la vie.
    */
   void BitField::comp (void)
    {
      if (Zero) return; //Do nothing.

      long i;
      Unit mask;

      if (Data.use_count() != 1)
       {
         Data = std::make_shared<BitHolder>(*Data);
       }

       /*
         Only complement up to the msb.
       */
      mask = 0;
      while (mask < Data->Data[Data->Length - 1]) mask = (mask << 1) + 1;

      for (i = 0; i < Data->Length; i++)
         Data->Data[i] = ~Data->Data[i];

      Data->Data[Data->Length - 1] &= mask;

       /*
         Handle cancellation.
       */
      if (Data->Data[Data->Length - 1] == 0)
       {
         for (i = Data->Length - 1; (i >= 0) && (Data->Data[i] == 0); i--) ;
         Data->Length = i + 1;

         if (i == -1) //Are we Zero?
          {
            Data.reset();
            Zero = true;
          }
       }
    }



 } /* namespace BigInt */
