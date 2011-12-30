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
#include "Number.hpp"

Number::NumberHolder::NumberHolder () : Refs (1) { }

Number::NumberHolder::NumberHolder (long elements) : Refs (1)
 {
   Contents.resize(elements);
 }

Number::NumberHolder::NumberHolder (const NumberHolder & src) :
   Contents (src.Contents), Refs (1) { }

Number::NumberHolder::~NumberHolder ()
 {
   Contents.clear();
   Refs = -1;
 }


Number::Number () : Scalar (true) { }

Number::Number (long elements) : Scalar (false)
 {
   if (elements == 1)
    {
      Scalar = true;
      ArrayValue = NULL;
    }
   else if (elements > 1) ArrayValue = new NumberHolder (elements);
 }

Number::Number (const Integer & src) :
   Scalar (true), ScalarValue (src), ArrayValue (NULL) { }

Number::Number (const Number & src) : Scalar (src.Scalar),
   ScalarValue (src.ScalarValue), ArrayValue (src.ArrayValue)
 {
   if (!Scalar) ArrayValue->Refs++;
 }

Number::~Number ()
 {
   if (!Scalar)
    {
      ArrayValue->Refs--;
      if (ArrayValue->Refs == 0) delete ArrayValue;
    }

   Scalar = true;
   ScalarValue = Integer();
   ArrayValue = NULL;
 }


void Number::resize (const Number & elements)
 {
   long telements = elements.getElement(1).toInt();

   if (telements > 1)
    {
      if (!Scalar)
       {
         if (ArrayValue->Refs == 1)
            ArrayValue->Contents.resize(telements);
         else
          {
            ArrayValue->Refs--;
            ArrayValue = new NumberHolder (*ArrayValue);

            ArrayValue->Contents.resize(telements);
          }
       }
      else
       {
         ArrayValue = new NumberHolder (telements);

         ArrayValue->Contents[0] = ScalarValue;
         ScalarValue = Integer();
         Scalar = false;
       }
    }
   else if (telements == 1)
    {
      if (!Scalar)
       {
         ScalarValue = ArrayValue->Contents[0];

         ArrayValue->Refs--;
         if (ArrayValue->Refs == 0) delete ArrayValue;

         Scalar = true;
         ArrayValue = NULL;
       }
    }
 }

void Number::setElement (const Number & position, const Number & value)
 {
   Integer avalue = value.getElement(1);
   long tposition = position.getElement(1).toInt();

   if (Scalar) { if (tposition == 1) ScalarValue = avalue; }
   else
    {
      if ((tposition > 0) && (tposition <= (long)ArrayValue->Contents.size()))
       {
         if (ArrayValue->Refs == 1)
            ArrayValue->Contents[tposition - 1] = avalue;
         else
          {
            ArrayValue->Refs--;
            ArrayValue = new NumberHolder (*ArrayValue);

            ArrayValue->Contents[tposition - 1] = avalue;
          }
       }
    }
 }

Integer Number::getElement (long position) const
 {
   if (Scalar) { if (position == 1) return ScalarValue; }
   else
    {
      if ((position > 0) && (position <= (long) ArrayValue->Contents.size()))
         return ArrayValue->Contents[position - 1];
    }
   return Integer();
 }

Integer Number::getElement (const Number & position) const
 {
   long tposition = position.getElement(1).toInt();

   if (Scalar) { if (tposition == 1) return ScalarValue; }
   else
    {
      if ((tposition > 0) && (tposition <= (long)ArrayValue->Contents.size()))
         return ArrayValue->Contents[tposition - 1];
    }
   return Integer();
 }

Number & Number::operator = (const Number & src)
 {
   if (&src != this)
    {
      if (!Scalar) //Make us a scalar to prevent memory leaks.
       {
         ArrayValue->Refs--;
         if (ArrayValue->Refs == 0) delete ArrayValue;
       }

      Scalar = src.Scalar;
      ScalarValue = src.ScalarValue;
      ArrayValue = src.ArrayValue;

      if (!Scalar) ArrayValue->Refs++;
    }
   return (*this);
 }



Number operator + (const Number & lhs, const Number & rhs)
 {
   return Number(lhs.getElement(1) + rhs.getElement(1));
 }

Number operator - (const Number & lhs, const Number & rhs)
 {
   return Number(lhs.getElement(1) - rhs.getElement(1));
 }

Number operator * (const Number & lhs, const Number & rhs)
 {
   return Number(lhs.getElement(1) * rhs.getElement(1));
 }

Number operator / (const Number & lhs, const Number & rhs)
 {
   return Number(lhs.getElement(1) / rhs.getElement(1));
 }

Number operator % (const Number & lhs, const Number & rhs)
 {
   return Number(lhs.getElement(1) % rhs.getElement(1));
 }


Number operator & (const Number & lhs, const Number & rhs)
 {
   return Number(lhs.getElement(1) & rhs.getElement(1));
 }

Number operator | (const Number & lhs, const Number & rhs)
 {
   return Number(lhs.getElement(1) | rhs.getElement(1));
 }

Number operator ^ (const Number & lhs, const Number & rhs)
 {
   return Number(lhs.getElement(1) ^ rhs.getElement(1));
 }


Number operator << (const Number & lhs, const Number & rhs)
 {
   return Number(lhs.getElement(1) << rhs.getElement(1));
 }

Number operator >> (const Number & lhs, const Number & rhs)
 {
   return Number(lhs.getElement(1) >> rhs.getElement(1));
 }


bool operator > (const Number & lhs, const Number & rhs)
 {
   return lhs.getElement(1) > rhs.getElement(1);
 }

bool operator < (const Number & lhs, const Number & rhs)
 {
   return lhs.getElement(1) < rhs.getElement(1);
 }

bool operator >= (const Number & lhs, const Number & rhs)
 {
   return lhs.getElement(1) >= rhs.getElement(1);
 }

bool operator <= (const Number & lhs, const Number & rhs)
 {
   return lhs.getElement(1) <= rhs.getElement(1);
 }

bool operator == (const Number & lhs, const Number & rhs)
 {
   return lhs.getElement(1) == rhs.getElement(1);
 }

bool operator != (const Number & lhs, const Number & rhs)
 {
   return lhs.getElement(1) != rhs.getElement(1);
 }

Number pow (const Number & base, const Number & exponent)
 {
   return Number(BigInt::pow(base.getElement(1), exponent.getElement(1)));
 }

Number abs (const Number & val)
 {
   return Number(BigInt::abs(val.getElement(1)));
 }

Number div (const Number & lhs, const Number & rhs)
 {
   return Number(BigInt::div(lhs.getElement(1), rhs.getElement(1)));
 }

Number mod (const Number & lhs, const Number & rhs)
 {
   return Number(BigInt::mod(lhs.getElement(1), rhs.getElement(1)));
 }
