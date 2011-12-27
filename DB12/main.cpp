/*
   DB12: DUMBBASIC 12.
      This started out as a compiler programming assignment for a
      compiler class with Al Brouillette at UCCS. I hacked in the
      structure of DUMBBASIC, taking it from a Pascal-like language
      to something more sinister.

      I'm greatly amused that I added break and continue to the compiler.
      The only use break has in this code is to not fall-through cases
      in a switch construct. Continue is absent. In that spirit, however,
      I did remove GOTO and replaced it with Java break/continue.
      Al would still be disappointed. He has religious devotion to structured
      programming, and break/continue are demons in his (and students) code.

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
#include <iostream>
#include "Shared.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

#ifdef DEBUG
char * Table [] = {
   "No_Op",
   "StrCat_Op",
   "Add_Op",
   "Subtract_Op",
   "Multiply_Op",
   "Divide_Op",
   "Modulus_Op",
   "StrictDivide_Op",
   "StrictModulus_Op",
   "Exponent_Op",
   "Negate_Op",
   "AbsoluteValue_Op",
   "And_Op",
   "Or_Op",
   "Xor_Op",
   "Not_Op",
   "ShiftLeft_Op",
   "ShiftRight_Op",
   "TestEqual_Op",
   "TestNotEqual_Op",
   "TestLessThan_Op",
   "TestGreaterThan_Op",
   "TestLessThanOrEqual_Op",
   "TestGreaterThanOrEqual_Op",
   "LogicalAnd_Op",
   "LogicalOr_Op",
   "LogicalNot_Op",
   "LoadConstant_Op",
   "LoadVariable_Op",
   "Store_Op",
   "BranchUnconditional_Op",
   "BranchConditional_Op",
   "CallIndirect_Op",
   "CallVIndirect_Op",
   "Call_Op",
   "CallV_Op",
   "Return_Op",
   "End_Op",
   "Print_Op",
   "Read_Op",
   "Copy_Op",
   "Pop_Op"
};
#endif

vector<Integer> Constants;
vector<Integer> Variables;

void interpretOps (OpTable &);

int main (int argc, char ** argv)
 {
   SymbolTable Gojira;
   LexerClass Mothra;
   ParserClass Hedorah;
   OpTable Tokyo;
   

   if (argc < 2)
    {
      Mothra.Error("No input file specified.");
    }
   else if (!Mothra.InitializeFile(argv[1], Gojira))
    {
      Mothra.Error("Cannot open input file.");
    }
   else
    {
      Hedorah.Parse(Mothra, Tokyo);

#ifdef DEBUG
      for (size_t i = 0; i < Gojira.size(); i++)
       {
         std::cout << i << " : " << Gojira.getName(i) <<
            "\t" << Gojira.getValue(i) <<
            std::endl;
       }
      for (size_t i = 0; i < Tokyo.nextOp(); i++)
       {
         std::cout << i << " : " <<
            Table[Tokyo.getOp(i).opcode] <<
            "\t" << Tokyo.getOp(i).arg << std::endl;
       }
#endif

      if (!Hedorah.Error()) interpretOps(Tokyo);
    }

   return 0;
 }
