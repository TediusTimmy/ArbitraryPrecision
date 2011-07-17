/*
   This file is part of TDLang.

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


       /*
         Useful Enums.
       */
enum Instruction
 {
   No_Op = 0,
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
   SquareRoot_Op,
   Maximum_Op,
   Minimum_Op,
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
   Load_Op,
   Store_Op,
   BranchUnconditional_Op,
   BranchConditional_Op,
   BranchIndirect_Op,
   CallIndirect_Op,
   Return_Op,
   Print_Op,
   PrintString_Op,
   PrintPadded_Op,
   Read_Op
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
   A holder for the program counter.
   Like a real machine, the PC is incremented after fetching the instruction.
   This makes implementing branches easier.
 */
class ProgramCounter
 {
   private:
      size_t pc; //The real program counter.

   public:
      size_t getPC (void)
       {
         return pc++;
       }
      void setPC (size_t newPC)
       {
         pc = newPC;
       }
      size_t peekPC (void) const //Return the current pc.
       {
         return pc;
       }

      ProgramCounter () : pc (0) {}
 };


 /*
   A container class for symbols.
 */
class Symbol
 {
   public:
      string name; //The name of the symbol.
      SymbolType type; //The type of symbol.
      Integer value; //The value associated with that symbol.

       /*
         Some random crap that I would rather not define, but
         C++ refuses to treat structs in the same way C does.

         I define all of these because I can't remember C++'s rules
         for what default functions it generates, given that I've
         defined one that it usually generates.
       */
      Symbol () : name (""), type (Undefined_t), value (0LL) { }
      Symbol (const char * a, SymbolType b, Integer c) :
         name (a), type (b), value (c) { }
      Symbol (const string & a, SymbolType b, Integer c) :
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
      Integer getValue (long index) const
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
      void setValue (long index, Integer newValue)
       {
         Symbols[index].value = newValue;
       }

       /*
         Get the index of the symbol whose name is "name".
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
            if (Symbols[i].name == name)
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
         long ret = lookupSymbol(me.name);
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
 };


 /*
   A management class for reserved words.
 */
class ReserveTable
 {
   private:
      vector<string> Symbols; //Holder for the reserved words.

   public:
       /*
         Adds a new reserved word and returns its index.
       */
      long add (const string & me)
       {
         size_t ret = Symbols.size();
         Symbols.push_back(me);
         return ret;
       }

       /*
         Get the word of the reserved word at index.
         If index is OOB, return "".
       */
      string lookupCode (long index) const
       {
         string result;
         if ((index >= 0) && ((size_t)index < Symbols.size()))
            result = Symbols[index];
         return result;
       }

       /*
         Get the index of the reserved word whose word is "name".
         Return -1 if there is no such word.
         Uses a simple linear search, as we don't sort our reserved words.
         That would be BAD for opcode mnemonics.
       */
      long lookupName (const string & name) const
       {
         size_t i = 0;
         long ret = -1;
         for (bool found = false; (i < Symbols.size()) && (found == false); i++)
          {
            if (Symbols[i] == name)
             {
               ret = i;
               found = true;
             }
          }
         return ret;
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
         Get the address of the next instruction.
         To be used in code generation in defining labels.
       */
      size_t nextOp (void) const
       {
         return opTable.size();
       }
 };


 /*
   Now, the VM:
 */
class VirtualMachine
 {
   private:
      ProgramCounter PC; //Program Counter: our only register.
      SymbolTable symbolTable; //I hate this variable naming convention.
      OpTable opTable; //This is a cat.

   public:
       /*
         Allow access to our symbol table: variable memory.
       */
      SymbolTable & getSymbolTable (void)
       {
         return symbolTable;
       }

       /*
         I have never done this before.
         basic_string has this, and I need it.
       */
      const SymbolTable & getSymbolTable (void) const
       {
         return symbolTable;
       }

       /*
         Allow access to our quad table: program memory.
       */
      OpTable & getOpTable (void)
       {
         return opTable;
       }

      const OpTable & getOpTable (void) const
       {
         return opTable;
       }

       /*
         Execute the VM.
         Load symbolTable and opTable before calling this.
       */
      void interpretOps (void);
 };

#endif /* SHARED_HPP */
