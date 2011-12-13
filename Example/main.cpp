/*
   TDLang: Timmy's Dumb Language.
      This started out as a compiler programming assignment for a
      compiler class with Al Brouillette at UCCS. Most of the structure,
      however, he took from Wirth, which is why the language is so Pascal-
      like. He dumbed it down a little, notably removing functions and
      arrays, which are still absent from this.

      The BigInts and stack machine were not a part of the assignment and
      were added later, by me. The hack that allows strings was also by me.

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
   "SquareRoot_Op",
   "Maximum_Op",
   "Minimum_Op",
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
   "Load_Op",
   "Store_Op",
   "BranchUnconditional_Op",
   "BranchConditional_Op",
   "BranchIndirect_Op",
   "CallIndirect_Op",
   "Return_Op",
   "Print_Op",
   "PrintString_Op",
   "PrintPadded_Op",
   "Read_Op",
   "StrCat_Op"
};
#endif

int main (int argc, char ** argv)
 {
   VirtualMachine Gojira;
   LexerClass Mothra;
   ParserClass Hedorah;

   if (argc < 2)
    {
      Mothra.Error("No input file specified.");
    }
   else if (!Mothra.InitializeFile(argv[1], Gojira.getSymbolTable()))
    {
      Mothra.Error("Cannot open input file.");
    }
   else
    {
      Hedorah.Parse(Mothra, Gojira.getOpTable());

#ifdef DEBUG
      for (size_t i = 0; i < Gojira.getSymbolTable().size(); i++)
       {
         std::cout << i << " : " << Gojira.getSymbolTable().getName(i) <<
            "\t" << Gojira.getSymbolTable().getValue(i).toString() <<
            std::endl;
       }
      for (size_t i = 0; i < Gojira.getOpTable().nextOp(); i++)
       {
         std::cout << i << " : " <<
            Table[Gojira.getOpTable().getOp(i).opcode] <<
            "\t" << Gojira.getOpTable().getOp(i).arg << std::endl;
       }
#endif

      if (!Hedorah.Error()) Gojira.interpretOps();
    }

   return 0;
 }
