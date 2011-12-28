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

#define VARS 24

Integer strcat (const Integer & lhs, const Integer & rhs)
 {
   long shift;

   shift = ((lhs.msb() >> 3) + 1) << 3;

   return (lhs | (rhs << Integer((long long) shift)));
 }

void interpretOps (OpTable & opTable)
 {
    /*
      The run-time environment.
    */
   OpCode ThisOp;
   std::vector<Integer> dataStack; // I need random access.
   std::stack<size_t> callStack;
   size_t pc, bp, i;

    /*
      Temps.
    */
   string temp;
   Integer op;

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


    /*
      Initialization:
    */
   pc = 0;
   bp = VARS;
   dataStack.resize(bp);


   while (!halt)
    {
      ThisOp = opTable.getOp(pc++);

#ifdef DEBUG
      std::cerr << ic << " : " << pc - 1 << " : " <<
         Table[ThisOp.opcode] << "\t" << ThisOp.arg << "\t" <<
         dataStack.size() << std::endl;
#endif

      switch (ThisOp.opcode)
       {
         case No_Op:
            break;

         case StrCat_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() = strcat(dataStack.back(), op);

            break;

         case Add_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() += op;

            break;

         case Subtract_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() -= op;

            break;

         case Multiply_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() *= op;

            break;

         case Divide_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() /= op;

            break;

         case Modulus_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() %= op;

            break;

         case StrictDivide_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() = BigInt::div(dataStack.back(), op);

            break;

         case StrictModulus_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() = BigInt::mod(dataStack.back(), op);

            break;

         case Exponent_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() = BigInt::pow(dataStack.back(), op);

            break;

         case Negate_Op:
            dataStack.back() = -dataStack.back();

            break;

         case AbsoluteValue_Op:
            dataStack.back() = BigInt::abs(dataStack.back());

            break;

         case And_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() &= op;

            break;

         case Or_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() |= op;

            break;

         case Xor_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() ^= op;

            break;

         case Not_Op:
            dataStack.back() = ~dataStack.back();

            break;

         case ShiftLeft_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() <<= op;

            break;

         case ShiftRight_Op:
            op = dataStack.back();
            dataStack.pop_back();
            dataStack.back() >>= op;

            break;

         case TestEqual_Op:
            op = dataStack.back();
            dataStack.pop_back();
            if (dataStack.back() == op) dataStack.back() = one;
            else dataStack.back() = zero;

            break;

         case TestNotEqual_Op:
            op = dataStack.back();
            dataStack.pop_back();
            if (dataStack.back() != op) dataStack.back() = one;
            else dataStack.back() = zero;

            break;

         case TestLessThan_Op:
            op = dataStack.back();
            dataStack.pop_back();
            if (dataStack.back() < op) dataStack.back() = one;
            else dataStack.back() = zero;

            break;

         case TestGreaterThan_Op:
            op = dataStack.back();
            dataStack.pop_back();
            if (dataStack.back() > op) dataStack.back() = one;
            else dataStack.back() = zero;

            break;

         case TestLessThanOrEqual_Op:
            op = dataStack.back();
            dataStack.pop_back();
            if (dataStack.back() <= op) dataStack.back() = one;
            else dataStack.back() = zero;

            break;

         case TestGreaterThanOrEqual_Op:
            op = dataStack.back();
            dataStack.pop_back();
            if (dataStack.back() >= op) dataStack.back() = one;
            else dataStack.back() = zero;

            break;

         case LogicalAnd_Op:
            op = dataStack.back();
            dataStack.pop_back();
            if (!dataStack.back().isZero() && !op.isZero())
               dataStack.back() = one;
            else dataStack.back() = zero;

            break;

         case LogicalOr_Op:
            op = dataStack.back();
            dataStack.pop_back();
            if (!dataStack.back().isZero() || !op.isZero())
               dataStack.back() = one;
            else dataStack.back() = zero;

            break;

         case LogicalNot_Op:
            if (dataStack.back().isZero()) dataStack.back() = one;
            else dataStack.back() = zero;

            break;

         case LoadConstant_Op:
            dataStack.push_back(Constants[ThisOp.arg]);

            break;

         case LoadVariable_Op:
            if ((long) ThisOp.arg >= 0)
               dataStack.push_back(Variables[ThisOp.arg]);
            else
               dataStack.push_back(dataStack[bp + ThisOp.arg]);

            break;

         case Store_Op:
            if ((long) ThisOp.arg >= 0)
               Variables[ThisOp.arg] = dataStack.back();
            else
               dataStack[bp + ThisOp.arg] = dataStack.back();
            dataStack.pop_back();

            break;

         case BranchUnconditional_Op:
            pc = ThisOp.arg;

            break;

         case BranchConditional_Op:
            op = dataStack.back();
            dataStack.pop_back();
            if (op.isZero()) pc = ThisOp.arg;

            break;

         case CallIndirect_Op:
         case CallVIndirect_Op:
            std::cerr << "Call Indirect not replaced at " <<
               pc - 1 << std::endl;

            break;

         case Call_Op:
            callStack.push(bp);
            callStack.push(pc);

            bp = bp + VARS;
            dataStack.resize(bp);

            for (i = 1; i <= VARS; i++)
               dataStack[bp - i] = dataStack[bp - VARS - i];

            pc = ThisOp.arg;

            break;

         case CallV_Op:

            callStack.push(bp);
            callStack.push(pc);

            bp = bp + VARS + 4;
            dataStack.resize(bp);

            for (i = 1; i <= VARS; i++)
               dataStack[bp - i] = dataStack[bp - VARS - 4 - i];

            dataStack[bp - 17] = dataStack[bp - 28];
            dataStack[bp - 18] = dataStack[bp - 27];
            dataStack[bp - 19] = dataStack[bp - 26];
            dataStack[bp - 20] = dataStack[bp - 25];

            pc = ThisOp.arg;

            break;

         case Return_Op:
            if (callStack.size() == 0)
             {
               halt = true;
             }
            else
             {
               pc = callStack.top();
               callStack.pop();
               bp = callStack.top();
               callStack.pop();
               if (ThisOp.arg)
                {
                  dataStack[bp - VARS + 4] = dataStack[dataStack.size() - 1];
                  dataStack[bp - VARS + 5] = dataStack[dataStack.size() - 2];
                  dataStack[bp - VARS + 6] = dataStack[dataStack.size() - 3];
                  dataStack[bp - VARS + 7] = dataStack[dataStack.size() - 4];
                }
               else
                {
                  dataStack[bp - VARS + 4] = dataStack[bp + 0];
                  dataStack[bp - VARS + 5] = dataStack[bp + 1];
                  dataStack[bp - VARS + 6] = dataStack[bp + 2];
                  dataStack[bp - VARS + 7] = dataStack[bp + 3];
                }
               dataStack.resize(bp);
             }

            break;

         case End_Op:
            halt = true;

            break;

         case Print_Op:
            if (ThisOp.arg == 0)
             {
               std::cout << dataStack.back().toString();
               dataStack.pop_back();
             }
            else
             {
               std::cout << stringify(dataStack.back());
               dataStack.pop_back();
             }

            break;

         case Read_Op:
            if (ThisOp.arg == 0)
             {
               std::getline(std::cin, temp);
               dataStack.push_back(Integer(temp));
             }
            else
             {
               std::getline(std::cin, temp);
               dataStack.push_back(intify(temp));
             }

            break;

         case Copy_Op:
            dataStack.push_back(dataStack.back());

            break;

         case Pop_Op:
            dataStack.pop_back();

            break;

         default:
            std::cerr << "Unimplemented Instruction: " << ThisOp.opcode <<
               "at" << pc - 1 << std::endl;
            break;

       }

      ic++;
    }

   std::cerr << "Instructions executed: " << ic << std::endl;

   return;
 }
