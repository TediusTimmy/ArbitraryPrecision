/*
   FENZERO4FLOAT: The fourth iteration of FENZERO.
      FENZERO is a program for finding the zeros of a function of a single
      variable. This version is a C program with a C++ class as its primary
      data type (Hence the FLOAT).

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
#include <stdlib.h>
#include "FEN.h"
#include "FENEVAL.h"
#include "FENREAD.h"
#include "FENFREE.h"
#include "FENPRINT.h"
#include "FENFOLD.h"

#include "../Float/Constants.hpp"

#define MAXITER 10000
#define MIBMAX 1024

int main (int argc, char ** argv)
 {
   pair * equ;
   Float zero, delta, last, prev, test;
   int slope, method, count = 0, done = 0;
   char main_inbuf [MIBMAX];
   long prec;

   if (argc > 1)
    {
      prec = atol(argv[1]);
      Float::setMinPrecision(prec);
      Float::setMaxPrecision(prec);
    }
   else
    {
      Float::setMinPrecision(16);
      Float::setMaxPrecision(16);
    }

   Float tenth ("1E-1");

   do
    {
      printf("Input the equation:\n");
      equ = read_pair();
      print_pair(equ);
      fold_pair(equ);
      putchar('\n');
      print_pair(equ);
      printf("\nWas that correct? (1 = no, 0 = yes) ");
      scanf("%d", &slope);
    }
   while (slope);

   printf("Do you want to use Method 0 or Method 1 "
      "(only if passes through x-axis): ");
   scanf("%d", &method);

   printf("What number is closest to the zero: ");
   scanf("%s", main_inbuf);
   zero.fromString(main_inbuf);

   printf("Is that guess high (-1) or low (1): ");
   scanf("%s", main_inbuf);
   delta.fromString(main_inbuf);
   delta *= tenth;

   if (method)
    {
      printf("Is the slope positive (1) or negative (-1) around the zero: ");
      scanf("%d", &slope);

      prev = BigInt::M_NaN;
      last = eval_pair(equ, zero);
      zero += delta;

      if ((slope < 0) && !delta.isSigned()) delta.negate();
      else if ((slope > 0) && delta.isSigned()) delta.negate();

      while (!done)
       {
         do
          {
            test = eval_pair(equ, zero);
            if (test.isNaN())
             {
               if (last.isNegative()) zero -= delta;
               else zero += delta;
               delta *= tenth;
               if (!change(zero, delta)) done = 1;
             }
            else
             {
               if (test.isNegative()) zero += delta;
               else zero -= delta;
             }
          }
         while (test.isNaN());

         if (test == prev)
          {
            delta *= tenth;
            if (!change(zero, delta)) done = 1;
          }

         prev = last;
         last = test;

         if (++count > MAXITER)
          {
            printf("A failure has occurred. %s\n", zero.toString().c_str());
            return 0;
          }
       }

    }
   else
    {

      if (delta.isSigned())
       {
         delta = -delta;
         slope = -1;
       }
      else slope = 1;

      last = eval_pair(equ, zero);
      last.abs();
      if (slope < 0) zero -= delta; else zero += delta;

      while (1)
       {
         do
          {
            test = eval_pair(equ, zero);
            if (test.isNaN())
             {
               if (slope < 0) zero += delta; else zero -= delta;
               delta *= tenth;
               if (slope < 0) zero -= delta; else zero += delta;
             }
            else
             {
               test.abs();
               if (test >= last)
                {
                  slope = -slope;
                  delta *= tenth;
                }
               if (slope < 0) zero -= delta; else zero += delta;
             }
          }
         while (test.isNaN());

         last = test;

         if (++count > MAXITER)
          {
            printf("A failure has occurred. %s\n", zero.toString().c_str());
            return 0;
          }
         if (!change(zero, delta)) break;
       }

    }

   printf("Zero: %s\n", zero.toString().c_str());

   return 0;
 }
