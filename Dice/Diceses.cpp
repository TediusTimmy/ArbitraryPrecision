 /*
   Diceses.cpp: does 'sum xdy' for 1<y<101 and 0<x<1025
      To use the Troll command.
      I think that this is O(d^2 * s^2),
         where d is the number of dice and s is the number of sides.

   Copyright (C) 2012 Thomas DiModica <ricinwich@yahoo.com>

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

#include <iostream>
#include <cstdlib>
#include "../Integer.hpp"

 /*
   How this program works:
      We use memoization:
         The base case is that for rolling one die, we have one
         way to roll a sum of each of the die face values.
         We return this basic table for the base case.

         We use that to generate a new table of the number of
         ways to roll the sums of two dice.

         This consecutive table building continues until we have
         reached the number of dice rolled.

      Given s sides and d dice:
         We, in general, use
            Sum (n : 1 .. d) s((n - 1)s - n + 2)
         space and time.

         Sum () (n-1)s^2 - ns + 2s
         s^2 * d(d-1)/2 - d(d+1)s/2 + 2ds
         (s^2(d^2-d) - sd^2 + ds + 4ds)/2
         (s^2 * d^2 - ds^2 - sd^2 + 5ds)/2
 */

using namespace std;
using BigInt::Integer;
using BigInt::Unit;

Integer * RollDice (int sides, int dice)
 {
   Integer * TempResults, * MyResults;
   int Die, Sum;
   MyResults = new Integer [sides * dice + 1];
   if (dice > 1)
    {
      TempResults = RollDice(sides, dice - 1);
      for (Die = 1; Die <= sides; Die++)
         for (Sum = dice - 1; Sum <= (sides * dice - sides); Sum++)
            MyResults[Die + Sum] += TempResults[Sum];
      delete [] TempResults;
    }
   else
    {
      for (Die = 1; Die <= sides; Die++)
         MyResults[Die] = Integer((Unit)1);
    }
   return MyResults;
 }

int main (int argc, char ** argv)
 {
   int dice, sides, sum;
   Integer * results;
   if (argc < 3)
    {
      cerr << "usage: Roller dice sides" << endl;
      return 1;
    }

   dice = atoi(argv[1]);
   sides = atoi(argv[2]);

   if ((sides < 2) || (sides > 100) || (dice < 1) || (dice > 1024))
    {
      cerr << "2 <= sides <= 100 , 1 <= dice <= 1024" << endl;
      return 1;
    }

   results = RollDice(sides, dice);

   for (sum = dice; sum <= (dice * sides); sum++)
      cout << sum << " : " << results[sum].toString() << endl;

   delete [] results;

   return 0;
 }

