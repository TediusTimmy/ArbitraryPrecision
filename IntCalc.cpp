/*
Copyright (c) 2010, 2011 Thomas DiModica.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of Thomas DiModica nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THOMAS DIMODICA AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THOMAS DIMODICA OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

/*
   Integer calculator. Supports the operations of addition, subtraction,
   multiplication, integer division, modulo, exponentiation, and factorial.
   Allow for arbitrarily nested parenthesis, and the use of brackets and
   braces as parenthesis. It cannot, however, differentiate between brackets,
   braces, and parenthesis when reporting an error. Use ';' as a stop, a break
   between calculations. Due to how data is read, spaces cannot be used to
   separate calculations.
*/

#include <iostream>
#include <string>
#include <stack>
#include <cstdlib>
#include "Integer.hpp"

using namespace std;
using BigInt::Integer;

const int Ops = 20;

 /*
   If the last token read was a number or a right parenthesis, then a minus
   should be a binary operator.
   If the last token read was an operator or a there is no last token, then a
   minus is either part of a number or a unary operator.
 */
bool LastNum = false;

 /*
   Have we had a "recoverable" error somewhere?
 */
bool Error = false;

/*
   Turn the ASCII value of the token read into a token number.
   Return the index of the token in "+-*""/%^!()~PCNMGLI|SF", if one of these.
   Return 98 if '@'.
   Return 99 if 0-9a-z.
   Else return -1.
*/
const int TokenTable [256] =
 {
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1,  6, -1, -1, -1,  4, -1, -1,  7,  8,  2,  0, -1, 01, -1,  3,
   99, 99, 99, 99, 99, 99, 99, 99, 99, 99, -1, -1, -1, -1, -1, -1,
   98, -1, -1, 11, -1, -1, 19, 14, -1, 16, -1, -1, 15, 13, 12, -1,
   10, -1, -1, 18, -1, -1, -1, -1, -1, -1, -1,  7, -1,  8,  5, -1,
   -1, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
   99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  7, 17,  8,  9, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
 };

 /*
   Returns the associativity of the operator.
   True if Left-to-Right, false otherwise.
   The tenth operator is unary negatation.
 */
const bool LRAssoc [Ops] =
 {
   /* + */ true,
   /* - */ true,
   /* * */ true,
   /* / */ true,
   /* % */ true,
   /* ^ */ false,
   /* ! */ false,
   /* ( */ true,
   /* ) */ true,
   /* ~ */ true,
   /* P */ true,
   /* C */ true,
   /* N */ true,
   /* M */ true,
   /* G */ true,
   /* L */ true,
   /* I */ true,
   /* | */ true,
   /* S */ true,
   /* F */ false
 };

 /*
   We'll give permute/combine the highest precedence and see how that goes.
   I'm just making crap up for the other "operators" as they should be
   functions.
 */
const int Precedence [Ops] =
 {
   /* + */ 0,
   /* - */ 0,
   /* * */ 1,
   /* / */ 1,
   /* % */ 1,
   /* ^ */ 2,
   /* ! */ 3,
   /* ( */ -1,
   /* ) */ -1,
   /* ~ */ 0,
   /* P */ 4,
   /* C */ 4,
   /* N */ 7,
   /* M */ 7,
   /* G */ 6,
   /* L */ 6,
   /* I */ 6,
   /* | */ 0,
   /* S */ 5,
   /* F */ 8
 };

 /*
   Do we have a Binary or Unary operation?
 */
const bool Binary [Ops] =
 {
   /* + */ true,
   /* - */ true,
   /* * */ true,
   /* / */ true,
   /* % */ true,
   /* ^ */ true,
   /* ! */ false,
   /* ( */ false,
   /* ) */ false,
   /* ~ */ false,
   /* P */ true,
   /* C */ true,
   /* N */ true,
   /* M */ true,
   /* G */ true,
   /* L */ true,
   /* I */ true,
   /* | */ false,
   /* S */ false,
   /* F */ false
 };

 /*
   Parse the input LALR(1).
 */
int Token, NextToken;

 /*
   End of computation character.
 */
const int EOC = ';';

void GetFirstToken (void)
 {
   char temp;
   cin >> temp;
   if (!(!cin))
    {
      Token = temp;
      cin >> temp;
      if (!(!cin)) NextToken = temp;
      else NextToken = -1;
    }
   else
    {
      Token = -1;
      NextToken = -1;
    }
 }

void GetNextToken (void)
 {
   char temp;
   Token = NextToken;
   if (Token == EOC) return;
   if (NextToken != -1)
    {
      cin >> temp;
      if (!(!cin)) NextToken = temp;
      else NextToken = -1;
    }
 }

string GetString (void)
 {
   string result;
   result = (char) Token;
   while ((NextToken != -1) && (TokenTable[NextToken] == 99))
    {
      GetNextToken();
      result += (char) Token;
    }
   return result;
 }

Integer operate (int operation, Integer * A, Integer * B)
 {
   Integer result, remainder;
   switch (operation)
    {
      case  0: return *A + *B;
      case  1: return *A - *B;
      case  2: return *A * *B;
      case  3: return div(*A, *B);
      case  4: return mod(*A, *B);
      case  5: return pow(*A, *B);
      case  6: return fact(*A);
      case  9: return -(*A);
      case 10: return permutation(*A, *B);
      case 11: return combination(*A, *B);
      case 12: return min(*A, *B);
      case 13: return max(*A, *B);
      case 14: return gcd(*A, *B);
      case 15: return lcm(*A, *B);
      case 16: return modInv(*A, *B);
      case 17: return abs(*A);
      case 18: return sqrt(*A);
      case 19: return fib(*A);
    }

   cerr << "Internal Error: " << __LINE__ << ". Aborting Calculator." << endl;
   exit(0);
 }

Integer pop (stack <Integer> & me)
 {
   Integer tos;
   if (me.empty())
    {
      cerr << "Parse error: operator without operands." << endl;
      Error = true;
      return Integer ((BigInt::Unit) 0);
    }
   tos = me.top();
   me.pop();
   return tos;
 }

 /*
   I'm going to try to implement Dijkstra's Shunting Yard Algorithm, with the
   exception that there is not going to be an output queue. Numbers will be
   pushed to a data stack, and operations will be performed on the data stack
   when the operators are popped off of the operator stack. So, it is the
   algorithm and the execution of the result at the same time. It just
   requires an extra stack.
 */
int main (int argc, char ** argv)
 {
   stack <Integer> DataStack;
   stack <int> OperatorStack;

   Integer lhs, rhs, last;
   int ibase = 10, obase = 10;

   if (argc > 1)
    {
      obase = atoi(argv[1]);
      if (argc > 2) ibase = atoi(argv[2]);

      if ((ibase < 2) || (ibase > 36) || (obase < 2) || (obase > 36))
       {
         cerr << argv[0] << " [output_base [input_base]]" << endl;
         cerr << "Input and output bases must be greater than one"
            " and less than thirty-seven." << endl;
       }
    }

   cerr << "Integer Playground by Thomas DiModica." << endl
        << "Copyright 2010, 2011." << endl
        << "This software comes with ABSOLUTELY NO WARRANTY." << endl;

   GetFirstToken();

   while (Token != -1)
    {

      do
       {
          // Parse the current Token
         switch (TokenTable[Token])
          {
            case  0:
                // '|' is absolute value. If we would interpret a '+' as such,
                // turn it into such. If the absolute value begins a number,
                // just read it in as part of the number.
               if (!LastNum)
                {
                   // I know everything that they say about goto, but does
                   // anyone see a better way to do this?
                  if (TokenTable[NextToken] == 99) goto ReadANumberInstead;
                  else Token = '|';
                }

               goto FlowThrough;

            case  1:
                // '~' is unary negate. If we would interpret a '-' as such,
                // turn it into such. If the unary negate begins a number,
                // just read it in as part of the number.
               if (!LastNum)
                {
                  if (TokenTable[NextToken] == 99) goto ReadANumberInstead;
                  else Token = '~';
                }

[[fallthrough]];
FlowThrough:    // Flow through to generalized operation.

            case  2: // These all flow through, as operation
            case  3: // is abstracted to encompass all operators.
            case  4:
            case  5:
            case  6:
            case  9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
               while (!OperatorStack.empty())
                {
                   // Do we evaluate the operator on the stack?
                  if (LRAssoc[TokenTable[Token]])
                     if (Precedence[TokenTable[Token]] >
                         Precedence[OperatorStack.top()])
                        break;
                  if (!LRAssoc[TokenTable[Token]])
                     if (Precedence[TokenTable[Token]] >=
                         Precedence[OperatorStack.top()])
                        break;

                   // Evaluate the operator on the stack.
                  if (Binary[OperatorStack.top()])
                   {
                     rhs = pop(DataStack);
                     lhs = pop(DataStack);
                     DataStack.push(operate(OperatorStack.top(), &lhs, &rhs));
                   }
                  else
                   {
                     lhs = pop(DataStack);
                     DataStack.push(operate(OperatorStack.top(), &lhs, NULL));
                   }
                  OperatorStack.pop();
                }

               OperatorStack.push(TokenTable[Token]);
               LastNum = false;
               break;

            case  7:

               if (LastNum)
                {
                  cerr << "Parse error: number after number." << endl;
                  Error = true;
                  break;
                }

               OperatorStack.push(TokenTable[Token]);
               LastNum = false;
               break;

            case  8:
               while (!OperatorStack.empty() && (OperatorStack.top() != 7))
                {
                  if (Binary[OperatorStack.top()])
                   {
                     rhs = pop(DataStack);
                     lhs = pop(DataStack);
                     DataStack.push(operate(OperatorStack.top(), &lhs, &rhs));
                   }
                  else
                   {
                     lhs = pop(DataStack);
                     DataStack.push(operate(OperatorStack.top(), &lhs, NULL));
                   }
                  OperatorStack.pop();
                }

               if (OperatorStack.empty())
                {
                  cerr << "Parse error: unmatched parenthesis." << endl;
                  Error = true;
                  break;
                }

               OperatorStack.pop();

               LastNum = true;
               break;

            case 98:

               if (LastNum)
                {
                  cerr << "Parse error: number after number." << endl;
                  Error = true;
                  break;
                }

               DataStack.push(last);
               LastNum = true;
               break;

            case 99:
ReadANumberInstead:

               if (LastNum)
                {
                  cerr << "Parse error: number after number." << endl;
                  Error = true;
                  break;
                }

               DataStack.push(Integer(GetString(), ibase));
               LastNum = true;
               break;

            case -1:
               cerr << "Bad token in token stream." << endl;
               Error = true;
               break;

            default:
                // We should NEVER reach this code.
               cerr << "Internal error: " << __LINE__
                    << ". Aborting calculator." << endl;
               return 0;
          }

         GetNextToken();

         if (Error)
          {
            while ((Token != -1) && (Token != EOC)) GetNextToken();
            Error = false;
          }
       }
      while ((Token != -1) && (Token != EOC));

       // Finalize the parsing
      while (!OperatorStack.empty())
       {
         if (OperatorStack.top() == 7)
          {
            cerr << "Parse error: unmatched parenthesis." << endl;
            break;
          }

         if (Binary[OperatorStack.top()])
          {
            rhs = pop(DataStack);
            lhs = pop(DataStack);
            DataStack.push(operate(OperatorStack.top(), &lhs, &rhs));
          }
         else
          {
            lhs = pop(DataStack);
            DataStack.push(operate(OperatorStack.top(), &lhs, NULL));
          }
         OperatorStack.pop();
       }

       // Print the result
      if (!DataStack.empty()) lhs = DataStack.top();
      else lhs = Integer((BigInt::Unit) 0);

      cout << lhs.toString(obase) << endl;

       // Clear the stacks, just to be sure. They SHOULD be empty right now.
      while (!DataStack.empty()) DataStack.pop();
      while (!OperatorStack.empty()) OperatorStack.pop();

       // Reset this. It's important.
      LastNum = false;
      last = lhs;

       // More input?
      GetFirstToken();
    }

   return 0;
 }
