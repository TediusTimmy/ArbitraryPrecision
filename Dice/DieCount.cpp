 /*
   DiceCount.cpp: does 'sum xdy' for 1<y<101 and 0<x<1025
      To use the Troll command.

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
#include <string>
#include <set>
#include <cstdlib>
#include "../Integer.hpp"

 /*
   How this program works:
      Define the normal form of a roll to be the set of values on the face of
      each die, ordered such that the set is non-increasing.

      Define the maximal form of a roll of sum 's' to be the normal form of
      a roll with the maximum number of faces with the maximum face value,
      and the maximum number of faces with the value one.

      Note that the minimum sum is the number of dice,
      the maximum sum is the number of dice times the maximum face value,
      and the expected value is the average of these two values.

      For sum is minimum to the expected value:
         Generate the maximal form for the sum.

         Recursively:
            If this form is unique,
               then compute the combinations for this form
               and call it the sum.
            Else stop recursing and return zero.

            Mutate the roll.

            Add the total combinations of this mutation and
            all of its children mutations to the sum by
            recursively evaluating this new roll.

            Return the sum.

      Fill in the other half of the sums by noting that the distribution is
      symmetric about the expected value.
 */

using namespace std;
using BigInt::Integer;
using BigInt::Unit;


/*
   Meat and Potatos.
*/
Integer compute (const char * roll, int dice)
 {
   int i;
   Integer result ((Unit) 1);

   while (dice)
    {
      for (i = 1; (i < dice) && (roll[i] == roll[0]); i++) ;

      result *= combination(Integer((Unit)dice), Integer((Unit)i));

      roll += i;
      dice -= i;
    }

   return result;
 }

bool register_new (string roll)
 {
   static set <string> rolls;

   if (roll == "")
    {
      rolls.clear();
      return false;
    }

   if (rolls.find(roll) != rolls.end())
    {
      return false;
    }
   else
    {
      rolls.insert(roll);
      return true;
    }
 }

Integer permute_and_accumulate (string roll)
 {
   string new_roll;
   Integer count;
   int i, l, runs;

      //Figure the number of permutations/combinations on this roll.
   if (register_new(roll))
      count = compute(roll.c_str(), roll.length());
   else
      //If this isn't new, than no modifications upon it will be either.
      return Integer((Unit)0);

      //Compute the next rolls in the sequence:
   runs = 0;
   while (1)
    {
         //Find the first die that has the same number
         //as all of the last dice.
      for (l = roll.length() - 1; (l > 0) && (roll[l - 1] == roll[l]); l--) ;
      for (i = runs; i > 0; i--)
         for (l--; (l > 0) && (roll[l - 1] == roll[l]); l--) ;
         //If we are at a termination condition (all dice the same), quit.
      if (l == 0) return count;
         //Find the first die that has a difference of two or more than
         //this die.
      for (i = l - 1; (i >= 0) && (roll[i] <= (roll[l] + 1)); i--) ;
         //If no such die exists, quit.
      if ((i < 0) || (roll[i] <= (roll[l] + 1))) return count;
         //Modify roll, and permute and accumulate.
      new_roll = roll;
      new_roll[i]--;
      new_roll[l]++;
      count += permute_and_accumulate(new_roll);
      runs++;
    }
 }

Integer * count_rolls (int dice, int faces)
 {
   Integer * counts;
   string rolls;
   int totals, workingpool, i;

   /*
      Base roll is numdice.
      Max roll is faces * numdice.
      Total rolls is faces * numdice - numdice + 1 [as it is a range],
         or (faces - 1) * numdice + 1.
   */
   counts = new Integer [(faces - 1) * dice + 1];

   for (totals = dice; totals < ((((faces - 1) * dice) / 2 + 1) + dice); totals++)
    {
      workingpool = totals;
      rolls.clear();
      rolls.resize(dice, '1');
      workingpool -= dice;
      for (i = 0; workingpool != 0; i++)
       {
         if (workingpool < faces)
          {
            rolls[i] += workingpool;
            workingpool = 0;
          }
         else
          {
            rolls[i] += (faces - 1);
            workingpool -= (faces - 1);
          }
       }
      counts[totals - dice] = permute_and_accumulate(rolls);
      register_new("");
    }

   /*
      The distrobution is normal about the expected value.
   */
   for (i = (faces - 1) * dice; i >= 0; i--)
      counts[i] = counts[(faces - 1) * dice - i];

   return counts;
 }

int main (int argc, char ** argv)
 {
   int numdice, faces, i;
   Integer * counts;

   if (argc != 3)
    {
      cout << "usage: " << argv[0] << " number_of_dice faces_per_die" << endl;
      return 0;
    }

   numdice = atoi(argv[1]);
   faces = atoi(argv[2]);

   if ((numdice > 1024) || (numdice < 1))
    {
      cout << "Too many dice! Or too few!" << endl;
      return 0;
    }

   if ((faces > 100) || (faces < 2))
    {
      cout << "What kind of dice are you rolling?" << endl;
      return 0;
    }

   counts = count_rolls(numdice, faces);

   for (i = 0; i < ((faces - 1) * numdice + 1); i++)
      cout << (i + numdice) << ": " << counts[i].toString() << endl;

   delete [] counts;

   return 0;
 }

