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
#include <stdlib.h>

#include "FEN.h"
#include "FENEVAL.h"
#include "FENFREE.h"

#define VARIABLE 1026
#define CONSTANT 255

 /*
   Returns 0 if the subtree can be folded (ie is all constants),
   and 1 if not.
 */
int can_fold (pair * top)
 {
   if (!top) return 1;
   if ((top->arg1 == NULL) && (top->arg2 == NULL))
    {
      if (top->token == VARIABLE) return 1;
      return 0;
    }
   else if (top->arg2 == NULL)
    {
      return can_fold(top->arg1);
    }
   else
    {
      return can_fold(top->arg1) | can_fold(top->arg2);
    }
   return 1;
 }

 /*
   Fold constants in the parse tree to remove unneccessary computations.

   Starts at the top of the tree (well, bottom, but trees grow down in
   Computer Science) and determines if it can be folded.
 */
void fold_pair (pair * top)
 {
   Float result;

   if (!top) return;
    /*
      If we can fold this subtree of the parse tree.
    */
   if (!can_fold(top))
    {
       /*
         If we already are a constant, then there is no need to do folding.
         And, we are a constant if arg1 is NULL.
       */
      if (top->arg1 != NULL)
       {
         result = eval_pair(top, Float("0"));

         if (top->arg1) free_pair(top->arg1);
         top->arg1 = NULL;
         if (top->arg2) free_pair(top->arg2);
         top->arg2 = NULL;

         top->token = CONSTANT;
         top->value = result;
       }
    }
    /*
      If we cannot, recurse down trying to find a subtree to fold.
    */
   else
    {
      if (top->arg1) fold_pair(top->arg1);
      if (top->arg2) fold_pair(top->arg2);
    }
   return;
 }
