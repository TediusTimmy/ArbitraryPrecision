/*
   This file is part of FENZERO4FLOAT.

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
#include "FEN.h"
#include "FENFAIL.h"

#include "../Float/Constants.hpp"

Float eval_pair (pair * me, const Float & ind)
{
   Float arg1, arg2;
   switch (me->token)
   {
      case 255:
         return me->value;

      case 256:
         arg1 = eval_pair(me->arg1, ind);
         if (arg1.isSigned()) return -arg1;
         return arg1;
      case 257:
         return -eval_pair(me->arg1, ind);
      case 258:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::sin(arg1);
      case 259:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::cos(arg1);
      case 260:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::tan(arg1);
      case 261:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::asin(arg1);
      case 262:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::acos(arg1);
      case 263:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::atan(arg1);
      case 264:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::log10(arg1);
      case 265:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::log(arg1);
      case 266:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::sqrt(arg1);
      case 267:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::exp(arg1);
      case 268:
         arg1 = eval_pair(me->arg1, ind);
         return BigInt::exp10(arg1);

      case 512:
         arg1 = eval_pair(me->arg1, ind);
         arg2 = eval_pair(me->arg2, ind);
         return arg1 + arg2;
      case 513:
         arg1 = eval_pair(me->arg1, ind);
         arg2 = eval_pair(me->arg2, ind);
         return arg1 - arg2;
      case 514:
         arg1 = eval_pair(me->arg1, ind);
         arg2 = eval_pair(me->arg2, ind);
         return arg1 / arg2;
      case 516:
         arg1 = eval_pair(me->arg1, ind);
         arg2 = eval_pair(me->arg2, ind);
         return arg1 * arg2;
      case 517:
         arg1 = eval_pair(me->arg1, ind);
         arg2 = eval_pair(me->arg2, ind);
         return BigInt::pow(arg1, arg2);
      case 518:
         arg1 = eval_pair(me->arg1, ind);
         arg2 = eval_pair(me->arg2, ind);
         return arg1 * Float(BigInt::exp10(arg2));

      case 1024:
         arg1 |= BigInt::exp(BigInt::M_1);
         return arg1;
      case 1025:
         arg1 |= BigInt::M_PI;
         return arg1;
      case 1026:
         return ind;
   }
   parse_panic("Corrupted Token");
   return 0;
}
