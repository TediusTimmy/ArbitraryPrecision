/*
   This file is part of DB12.

   Copyright (C) 2011 Thomas DiModica <ricinwich@yahoo.com>

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
#ifndef NUMBER_HPP
#define NUMBER_HPP

#include <vector>
#include "../Integer.hpp"

using std::vector;
using BigInt::Integer;

class Number
 {
   private:
      class NumberHolder
       {
         public:
            vector<Integer> Contents;
            mutable long Refs;

            NumberHolder ();
            NumberHolder (long elements);
            NumberHolder (const NumberHolder &);
            ~NumberHolder ();
       };

      bool Scalar;
      Integer ScalarValue;
      NumberHolder * ArrayValue;

   public:
      Number ();
      Number (long elements);
      Number (const Integer &);
      Number (const Number &);
      ~Number ();

      size_t elements (void)
       { if (Scalar) return 1; return ArrayValue->Contents.size(); }

      void resize (const Number &);

      void setElement (const Number &, const Number &);

      Integer getElement (long) const;
      Integer getElement (const Number &) const;

      bool isZero (void) const
       { return this->getElement(1).isZero(); }

      Number & operator += (const Number & rval)
       { return (*this = *this + rval); }
      Number & operator -= (const Number & rval)
       { return (*this = *this - rval); }
      Number & operator *= (const Number & rval)
       { return (*this = *this * rval); }
      Number & operator /= (const Number & rval)
       { return (*this = *this / rval); }
      Number & operator %= (const Number & rval)
       { return (*this = *this % rval); }

      Number & operator &= (const Number & rval)
       { return (*this = *this & rval); }
      Number & operator |= (const Number & rval)
       { return (*this = *this | rval); }
      Number & operator ^= (const Number & rval)
       { return (*this = *this ^ rval); }

      Number & operator <<= (const Number & rval)
       { return (*this = *this << rval); }
      Number & operator >>= (const Number & rval)
       { return (*this = *this >> rval); }

      friend Number operator + (const Number &, const Number &);
      friend Number operator - (const Number &, const Number &);
      friend Number operator * (const Number &, const Number &);
      friend Number operator / (const Number &, const Number &);
      friend Number operator % (const Number &, const Number &);

      friend Number operator & (const Number &, const Number &);
      friend Number operator | (const Number &, const Number &);
      friend Number operator ^ (const Number &, const Number &);

      friend Number operator << (const Number &, const Number &);
      friend Number operator >> (const Number &, const Number &);

      Number & operator = (const Number &);

      Number operator ~ (void) const
       { return Number(~this->getElement(1)); }
      Number operator - (void) const
       { return Number(-this->getElement(1)); }
 };

   Number operator + (const Number &, const Number &);
   Number operator - (const Number &, const Number &);
   Number operator * (const Number &, const Number &);
   Number operator / (const Number &, const Number &);
   Number operator % (const Number &, const Number &);

   Number operator & (const Number &, const Number &);
   Number operator | (const Number &, const Number &);
   Number operator ^ (const Number &, const Number &);

   Number operator << (const Number &, const Number &);
   Number operator >> (const Number &, const Number &);

   bool operator > (const Number &, const Number &);
   bool operator < (const Number &, const Number &);
   bool operator >= (const Number &, const Number &);
   bool operator <= (const Number &, const Number &);
   bool operator == (const Number &, const Number &);
   bool operator != (const Number &, const Number &);

   Number pow (const Number &, const Number &);
   Number abs (const Number &);
   Number div (const Number &, const Number &);
   Number mod (const Number &, const Number &);

#endif /* NUMBER_HPP */
