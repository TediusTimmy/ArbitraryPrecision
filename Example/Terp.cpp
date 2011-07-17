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
#include <iostream>
#include <stack>
#include "Shared.hpp"

#ifdef DEBUG
extern char * Table [];
#endif

 /*
   Takes in a string and returns it as an integer, by treating
   each character as a base 256 digit and the string as a little-endian
   number.
 */
#define UNIT_SIZE sizeof(BigInt::Unit)
Integer intify (const string & src)
 {
   Integer result;
   long curchar, digits, iter;
   BigInt::Unit fatdigit, shift;

   for (iter = src.length() - 1; iter >= 0;)
    {
      digits = UNIT_SIZE == 1 ? 1 : UNIT_SIZE - 1;
      fatdigit = 0;
      shift = 0;

      for (; (iter >= 0) && digits; iter--, digits--)
       {
         curchar = (unsigned char) src[iter];

         fatdigit <<= 8;
         fatdigit += curchar;

         shift += 8;
       }

      result <<= Integer(shift);
      result |= Integer(fatdigit);
    }

   return result;
 }

 /*
   Does the opposite of Intify.
 */
string stringify (const Integer & src)
 {
   string result;
   int digit, index;
   BigInt::Unit cur;
   char * temp;

   if (src.isZero()) return result;

   temp = new char [(src.msb() / (UNIT_SIZE * 8) + 2) * (UNIT_SIZE * 8)];
   digit = 0;
   index = 0;

   cur = src.getDigit(digit++);
   while (cur)
    {
      temp[index++] = cur;
      if (UNIT_SIZE > 1)
       {
         temp[index++] = cur >> 8;
         if (UNIT_SIZE > 2)
          {
            temp[index++] = cur >> 16;
            temp[index++] = cur >> 24;
#if 0
            if (UNIT_SIZE  > 4)
             {
               temp[index++] = cur >> 32;
               temp[index++] = cur >> 40;
               temp[index++] = cur >> 48;
               temp[index++] = cur >> 56;
             }
#endif
          }
       }
      cur = src.getDigit(digit++);
    }

   temp[index] ='\0';

   result = temp;
   delete [] temp;

   return result;
 }

string doPad (long base, const string & fill,
   size_t length, const Integer & src)
 {
   string temp = src.toString(base), temp2;
   long runs;

   if (temp.length() >= length) return temp;

   runs = (length - temp.length()) / fill.length();
   while (runs--) temp2 += fill;
   temp2 += fill.substr(0, (length - temp.length()) % fill.length());

   return temp2 + temp;
 }

void VirtualMachine::interpretOps (void)
 {
    /*
      The run-time environment.
    */
   OpCode ThisOp;
   std::stack<Integer> dataStack;
   std::stack<size_t> callStack;

    /*
      Temps.
    */
   string temp;
   Integer op, op2;
   long len, base;

    /*
      Stop the machine, I need to get off.
    */
   bool halt = false;

    /*
      Count of total instructions executed.
    */
   unsigned long long ic = 0;

    /*
      Useful constants.
    */
   static const Integer one (1LL), zero (0LL);


   while (!halt)
    {
      ThisOp = opTable.getOp(PC.getPC());

#ifdef DEBUG
      std::cerr << ic << " : " << PC.peekPC() - 1 << " : " <<
         Table[ThisOp.opcode] << "\t" << ThisOp.arg << "\t" <<
         dataStack.size() << std::endl;
#endif

      switch (ThisOp.opcode)
       {
         case No_Op:
            break;

         case Add_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() += op;

            break;

         case Subtract_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() -= op;

            break;

         case Multiply_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() *= op;

            break;

         case Divide_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() /= op;

            break;

         case Modulus_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() %= op;

            break;

         case StrictDivide_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() = BigInt::div(dataStack.top(), op);

            break;

         case StrictModulus_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() = BigInt::mod(dataStack.top(), op);

            break;

         case Exponent_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() = BigInt::pow(dataStack.top(), op);

            break;

         case Negate_Op:
            dataStack.top() = -dataStack.top();

            break;

         case AbsoluteValue_Op:
            dataStack.top() = BigInt::abs(dataStack.top());

            break;

         case SquareRoot_Op:
            dataStack.top() = BigInt::sqrt(dataStack.top());

            break;

         case Maximum_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() = BigInt::max(dataStack.top(), op);

            break;

         case Minimum_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() = BigInt::min(dataStack.top(), op);

            break;

         case And_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() &= op;

            break;

         case Or_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() |= op;

            break;

         case Xor_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() ^= op;

            break;

         case Not_Op:
            dataStack.top() = ~dataStack.top();

            break;

         case ShiftLeft_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() <<= op;

            break;

         case ShiftRight_Op:
            op = dataStack.top();
            dataStack.pop();
            dataStack.top() >>= op;

            break;

         case TestEqual_Op:
            op = dataStack.top();
            dataStack.pop();
            if (dataStack.top() == op) dataStack.top() = one;
            else dataStack.top() = zero;

            break;

         case TestNotEqual_Op:
            op = dataStack.top();
            dataStack.pop();
            if (dataStack.top() != op) dataStack.top() = one;
            else dataStack.top() = zero;

            break;

         case TestLessThan_Op:
            op = dataStack.top();
            dataStack.pop();
            if (dataStack.top() < op) dataStack.top() = one;
            else dataStack.top() = zero;

            break;

         case TestGreaterThan_Op:
            op = dataStack.top();
            dataStack.pop();
            if (dataStack.top() > op) dataStack.top() = one;
            else dataStack.top() = zero;

            break;

         case TestLessThanOrEqual_Op:
            op = dataStack.top();
            dataStack.pop();
            if (dataStack.top() <= op) dataStack.top() = one;
            else dataStack.top() = zero;

            break;

         case TestGreaterThanOrEqual_Op:
            op = dataStack.top();
            dataStack.pop();
            if (dataStack.top() >= op) dataStack.top() = one;
            else dataStack.top() = zero;

            break;

         case LogicalAnd_Op:
            op = dataStack.top();
            dataStack.pop();
            if (!dataStack.top().isZero() && !op.isZero())
               dataStack.top() = one;
            else dataStack.top() = zero;

            break;

         case LogicalOr_Op:
            op = dataStack.top();
            dataStack.pop();
            if (!dataStack.top().isZero() || !op.isZero())
               dataStack.top() = one;
            else dataStack.top() = zero;

            break;

         case LogicalNot_Op:
            if (dataStack.top().isZero()) dataStack.top() = one;
            else dataStack.top() = zero;

            break;

         case Load_Op:
            dataStack.push(symbolTable.getValue(ThisOp.arg));

            break;

         case Store_Op:
            symbolTable.setValue(ThisOp.arg, dataStack.top());
            dataStack.pop();

            break;

         case BranchUnconditional_Op:
            PC.setPC(ThisOp.arg);

            break;

         case BranchConditional_Op:
            op = dataStack.top();
            dataStack.pop();
            if (op.isZero()) PC.setPC(ThisOp.arg);

            break;

         case BranchIndirect_Op:
            PC.setPC(symbolTable.getValue(ThisOp.arg).toInt());

            break;

         case CallIndirect_Op:
            callStack.push(PC.peekPC());
            PC.setPC(symbolTable.getValue(ThisOp.arg).toInt());

            break;

         case Return_Op:
            if (callStack.size() == 0)
             {
               halt = true;
             }
            else
             {
               PC.setPC(callStack.top());
               callStack.pop();
             }

            break;

         case Print_Op:
            base = dataStack.top().toInt();
            dataStack.pop();
            std::cout << dataStack.top().toString(base);
            dataStack.pop();
            if (ThisOp.arg != 0) std::cout << std::endl;

            break;

         case PrintString_Op:
            std::cout << stringify(dataStack.top());
            dataStack.pop();
            if (ThisOp.arg != 0) std::cout << std::endl;

            break;

         case PrintPadded_Op:
            base = dataStack.top().toInt();
            dataStack.pop();
            temp = stringify(dataStack.top());
            dataStack.pop();
            len = dataStack.top().toInt();
            dataStack.pop();
            op = dataStack.top();
            dataStack.pop();

            std::cout << doPad(base, temp, len, op);
            if (ThisOp.arg != 0) std::cout << std::endl;

            break;

         case Read_Op:
            if (ThisOp.arg == 0)
             {
               base = dataStack.top().toInt();
               std::getline(std::cin, temp);
               dataStack.top().fromString(temp, base);
             }
            else
             {
               std::getline(std::cin, temp);
               dataStack.push(intify(temp));
             }

            break;

       }

      ic++;
    }

    /*
      If I want to run the same program more than once,
      I must reset the PC here, or the program EXPLODES.
    */
   PC.setPC(0);

   std::cout << "Instructions executed: " << ic << std::endl;

   return;
 }
