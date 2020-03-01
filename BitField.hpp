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
   This is the backend of the Integer class. This class is an unsigned number
   expressed in binary. It handles all of the memory-management and a subset
   of operations that allow the Integer class to be cleaner, implementation-
   wise.

   It doesn't do alot of error checking, and will allow you to shoot yourself
   in the foot. You have been warned.

   So, BitField is like GMPs natural type, mpn. The only exception is that
   BitField does ALL memory management, so Integer doesn't have to worry about
   ANY of it.

   This originally started with each BitField having a private copy of its
   bit data, which was in an array that fit it perfectly. This incurred alot
   of unnecessary copying and reallocation. I decided to make it more like
   bc_num in GNU bc, and implemented COW. This makes the numerous temporary
   variables created by C++ more efficient. BitHolder holds a non-zero number
   in an array that may be larger than necessary.
*/

#ifndef BITFIELD_HPP
#define BITFIELD_HPP

#include <memory>

namespace BigInt
 {

   typedef unsigned int Unit;

   class BitField
    {

      private:
         class BitHolder
          {
            public:
               std::unique_ptr<Unit[]> Data;
               long Length;

               long Size;

               BitHolder ();
               BitHolder (const BitHolder &, long extra = 0);
               ~BitHolder ();
          };

         std::shared_ptr<BitHolder> Data;
         bool Zero;

      public:

         BitField ();
         BitField (Unit);
         BitField (const BitField &);

         ~BitField ();

         void operator <<= (long);
         void operator >>= (long);

         void operator -= (const BitField &);
         void operator += (const BitField &);
         void operator &= (const BitField &);
         void operator |= (const BitField &);
         void operator ^= (const BitField &);

         void operator -= (Unit);
         void operator += (Unit);

         void operator *= (Unit);
         Unit operator /= (Unit);

         void operator = (const BitField &);

         bool isZero (void) const
          { return Zero; }
         long length (void) const
          { if (Zero) return 0; return Data->Length; }
         long msb (void) const;

         void comp (void);

         bool operator ! (void) const { return isZero(); }

         Unit getDigit (long) const;

         Unit divApprox (Unit, Unit, bool) const;

         bool operator > (const BitField &) const;
         bool operator < (const BitField &) const;
         bool operator >= (const BitField &) const;
         bool operator <= (const BitField &) const;
         bool operator == (const BitField &) const;
         bool operator != (const BitField &) const;

         int compare (const BitField &) const;

         const static int bits;

    }; /* class BitField */

 } /* namespace BigInt */

#endif /* BITFIELD_HPP */
