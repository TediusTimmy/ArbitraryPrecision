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
#ifndef SHARED_HPP
#define SHARED_HPP

#include <string>
#include <vector>
#include "../Integer.hpp"

using std::vector;
using std::string;
using BigInt::Integer;

string stringify (const Integer &);
Integer intify (const string &);

extern vector<Integer> Constants;
extern vector<Integer> Variables;

extern Integer LexerTemp;


       /*
         Useful Enums.
       */
enum Instruction
 {
   No_Op = 0,
   StrCat_Op,
   Add_Op,
   Subtract_Op,
   Multiply_Op,
   Divide_Op,
   Modulus_Op,
   StrictDivide_Op,
   StrictModulus_Op,
   Exponent_Op,
   Negate_Op,
   AbsoluteValue_Op,
   And_Op,
   Or_Op,
   Xor_Op,
   Not_Op,
   ShiftLeft_Op,
   ShiftRight_Op,
   TestEqual_Op,
   TestNotEqual_Op,
   TestLessThan_Op,
   TestGreaterThan_Op,
   TestLessThanOrEqual_Op,
   TestGreaterThanOrEqual_Op,
   LogicalAnd_Op,
   LogicalOr_Op,
   LogicalNot_Op,
   LoadConstant_Op,
   LoadVariable_Op,
   Store_Op,
   BranchUnconditional_Op,
   BranchConditional_Op,
   CallIndirect_Op, /* VM will never see this. */
   CallVIndirect_Op, /* VM will never see this. */
   Call_Op,
   CallV_Op,
   Return_Op,
   End_Op,
   Print_Op,
   Read_Op,
   Copy_Op,
   Pop_Op
 };

enum SymbolType
 {
   Undefined_t,
   Label_t,
   Variable_t,
   Constant_t
 };


    /*
      Data Type Declarations
    */


 /*
   Backreference filler holding class.
   We use 0 to indicate no label,
      as symbol table entry 0 is always Constant 0,
      and never a label anyway.
 */
class BackRef
 {
   public:
      size_t CodeLoc;
      bool isBreak;
      long JumpLabel;
      size_t LineNo;

      BackRef () : CodeLoc (0), isBreak (false), JumpLabel (0), LineNo (0) { }
      BackRef (size_t a, bool b, long c, size_t d) :
         CodeLoc (a), isBreak (b), JumpLabel (c), LineNo (d) { }
      BackRef (const BackRef & a) : CodeLoc (a.CodeLoc), isBreak (a.isBreak),
         JumpLabel (a.JumpLabel), LineNo (a.LineNo) { }

      BackRef & operator= (const BackRef & a)
       {
         if (&a != this)
          {
            CodeLoc = a.CodeLoc;
            isBreak = a.isBreak;
            JumpLabel = a.JumpLabel;
            LineNo = a.LineNo;
          }
         return (*this);
       }
 };


 /*
   A container class for symbols.
 */
class Symbol
 {
   public:
      string name; //The name of the symbol.
      SymbolType type; //The type of symbol.
      long value; //The value associated with that symbol.

       /*
         Some random crap that I would rather not define, but
         C++ refuses to treat structs in the same way C does.

         I define all of these because I can't remember C++'s rules
         for what default functions it generates, given that I've
         defined one that it usually generates.
       */
      Symbol () : name (""), type (Undefined_t), value (0) { }
      Symbol (const char * a, SymbolType b, long c) :
         name (a), type (b), value (c) { }
      Symbol (const string & a, SymbolType b, long c) :
         name (a), type (b), value (c) { }
      Symbol (const Symbol & a) :
         name (a.name), type (a.type), value (a.value) { }

      Symbol & operator= (const Symbol & a)
       {
         if (&a != this)
          {
            name = a.name;
            type = a.type;
            value = a.value;
          }
         return (*this);
       }
 };


 /*
   A management class for the symbol table.
 */
class SymbolTable
 {
   private:
      vector<Symbol> Symbols; //Holder for our Symbols.

   public:

       /*
         Get the name attribute of the symbol at index.
         Assume that index is valid.
       */
      string getName (long index) const
       {
         return Symbols[index].name;
       }

       /*
         Get the type attribute of the symbol at index.
         Assume that index is valid.
       */
      SymbolType getType (long index) const
       {
         return Symbols[index].type;
       }

       /*
         Get the value attribute of the symbol at index.
         Assume that index is valid.
       */
      long getValue (long index) const
       {
         return Symbols[index].value;
       }

       /*
         Set the type attribute of the symbol at index.
         Assume that index is valid.
       */
      void setType (long index, SymbolType newType)
       {
         Symbols[index].type = newType;
       }

       /*
         Set the value attribute of the symbol at index.
         Assume that index is valid.
       */
      void setValue (long index, long newValue)
       {
         Symbols[index].value = newValue;
       }

       /*
         Get the index of the symbol whose name is "name",
            but only from symbols whose type is not Constant_t.
         Return -1 if there is no such symbol.
         Uses a simple linear search, as we don't sort our symbol table.
         I think that sorting the table would break code generation.
       */
      long lookupSymbol (const string & name) const
       {
         size_t i = 0;
         long ret = -1;
         for (bool found = false; (i < Symbols.size()) && (found == false); i++)
          {
            if ((Symbols[i].type != Constant_t) && (Symbols[i].name == name))
             {
               ret = i;
               found = true;
             }
          }
         return ret;
       }

       /*
         Get the index of the symbol whose value is "value",
            but only from symbols whose type is Constant_t.
         Return -1 if there is no such symbol.
       */
      long lookupSymbol (const Integer & value) const
       {
         size_t i = 0;
         long ret = -1;
         for (bool found = false; (i < Symbols.size()) && (found == false); i++)
          {
            if ((Symbols[i].type == Constant_t) &&
                (Constants[Symbols[i].value] == value))
             {
               ret = i;
               found = true;
             }
          }
         return ret;
       }

       /*
         Adds a new symbol to the symbol table and returns its index.
         If the symbol is already there, just return its index.
       */
      long addSymbol (const Symbol & me)
       {
         long ret;
         if (me.type == Constant_t) ret = lookupSymbol(LexerTemp);
         else ret = lookupSymbol(me.name);
         if (ret == -1)
          {
            ret = Symbols.size();
            Symbols.push_back(me);
          }
         return ret;
       }

       /*
         Returns the number of symbols in the symbol table.
       */
      size_t size (void) const
       {
         return Symbols.size();
       }

       /*
         Clears the symbol table.
       */
      void clear (void)
       {
         Symbols.clear();
       }
 };


 /*
   A storage class for opcodes.
 */
class OpCode
 {
   public:
      Instruction opcode;
      size_t arg;

      OpCode () : opcode (No_Op), arg (0) { }
      OpCode (Instruction a, size_t b) :
         opcode (a), arg (b) { }
      OpCode (const OpCode & a) :
         opcode (a.opcode), arg (a.arg) { }

      OpCode & operator= (const OpCode & a)
       {
         if (&a != this)
          {
            opcode = a.opcode;
            arg = a.arg;
          }
         return (*this);
       }
 };


 /*
   The program memory portion of our Harvard machine.
   Manages the opcode table and provides services for code generation.
 */
class OpTable
 {
   private:
      vector<OpCode> opTable;

   public:
       /*
         Adds a new opcode to the optable and returns its index.
       */
      size_t addOp (const OpCode & me)
       {
         size_t ret = opTable.size();
         opTable.push_back(me);
         return ret;
       }

       /*
         Get the opcode at an index in the op table.
         Assume that index is valid.
         If index isn't valid, hopefully the machine will explode,
         indicating an error in our code generation.
       */
      OpCode getOp (size_t index) const
       {
         return opTable[index];
       }

       /*
         A minimum accessor to allow jump backfilling.
       */
      void setArg (size_t index, size_t arg)
       {
         opTable[index].arg = arg;
       }

       /*
         Another accessor for indirect branch elimination.
       */
      void setOp (size_t index, Instruction opcode)
       {
         opTable[index].opcode = opcode;
       }

       /*
         Get the address of the next instruction.
         To be used in code generation in defining labels.
       */
      size_t nextOp (void) const
       {
         return opTable.size();
       }
 };


#endif /* SHARED_HPP */
