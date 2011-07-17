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
#include <stdio.h>
#include "FEN.h"

void print_pair (pair * me)
{
   switch (me->token)
   {
      case 255:
         printf("%s", (me->value).toString().c_str());
         break;

      case 256:
         printf("abs(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 257:
         printf("-(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 258:
         printf("sin(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 259:
         printf("cos(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 260:
         printf("tan(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 261:
         printf("asin(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 262:
         printf("acos(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 263:
         printf("atan(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 264:
         printf("log(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 265:
         printf("ln(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 266:
         printf("sqrt(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 267:
         printf("exp(");
         print_pair(me->arg1);
         printf(")");
         break;
      case 268:
         printf("pow(");
         print_pair(me->arg1);
         printf(")");
         break;

      case 512:
         printf("(");
         print_pair(me->arg1);
         printf(")+(");
         print_pair(me->arg2);
         printf(")");
         break;
      case 513:
         printf("(");
         print_pair(me->arg1);
         printf(")-(");
         print_pair(me->arg2);
         printf(")");
         break;
      case 514:
         printf("(");
         print_pair(me->arg1);
         printf(")/(");
         print_pair(me->arg2);
         printf(")");
         break;
      case 516:
         printf("(");
         print_pair(me->arg1);
         printf(")*(");
         print_pair(me->arg2);
         printf(")");
         break;
      case 517:
         printf("(");
         print_pair(me->arg1);
         printf(")^(");
         print_pair(me->arg2);
         printf(")");
         break;
      case 518:
         printf("(");
         print_pair(me->arg1);
         printf(")E(");
         print_pair(me->arg2);
         printf(")");
         break;

      case 1024:
         printf("e");
         break;
      case 1025:
         printf("pi");
         break;
      case 1026:
         printf("x");
         break;
   }
   return;
}
