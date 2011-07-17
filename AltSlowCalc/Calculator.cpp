/*
   This file is part of AltSlowCalc.

   Copyright (C) 2010, 2011 Thomas DiModica <ricinwich@yahoo.com>

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
/*
   File: Calculator.cpp
      The primary member-function definitions for the calculator class.
*/

//HEADER:
// Calculator is the structure we're dealing with.
// Constants has constants we need
// Round allows us to do rounding manipulations
// string is a data type we'll use
// iostream define cin, cout, and cerr to do IO
// fstream is used by help for my help file
#include "Calculator.hpp"
#include "../Float/Constants.hpp"
#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::ifstream;

const long Calculator::totalStacks = actualTotalStacks;

/*
==============================================================================
   Function: no argument Calculator constructor
==============================================================================
*/
Calculator::Calculator() : currentStack(0) { }

/*
==============================================================================
   Function: doStuff
------------------------------------------------------------------------------
   INPUT: string command to do
   NOTES:
      This is the main driving function of the Calculator class.
      It could be more client/server and never have cin or cout,
      but that would add complexity that is unnecessary at this point.
==============================================================================
*/
void Calculator::doStuff (string doThis)
 {
   Float left, right; //To be used in the math
   long someThing; //Has random uses.

   if (doThis == "+")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left + right);
    }
   else if (doThis == "-")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left - right);
    }
   else if (doThis == "*")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left * right);
    }
   else if (doThis == "/")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left / right);
    }
   else if (doThis == "pow")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::pow(left, right));
    }
   else if (doThis == "sqrt")
    {
      //If I had an atomic top/pop, then these would all just be one line.
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::sqrt(left));
    }
   else if (doThis == "exp")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::exp(left));
    }
   else if (doThis == "ln")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::log(left));
    }
   else if (doThis == "sin")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::sin(left));
    }
   else if (doThis == "cos")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::cos(left));
    }
   else if (doThis == "tan")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::tan(left));
    }
   else if (doThis == "asin")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::asin(left));
    }
   else if (doThis == "acos")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::acos(left));
    }
   else if (doThis == "atan")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::atan(left));
    }
   else if (doThis == "log")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::log10(left));
    }
   else if (doThis == "alog")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::exp10(left));
    }
   else if (doThis == "atan2")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(BigInt::atan2(left, right));
    }
   else if (doThis == "neg")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left.negate());
    }
   else if (doThis == "abs")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left.abs());
    }
   else if (doThis == "sqr")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left * left);
    }
   else if (doThis == "shorten")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      cin >> someThing;
      left.changePrecision(left.getPrecision() - someThing);
      stacks[currentStack].push(left);
    }
   else if (doThis == "lengthen")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      cin >> someThing;
      left.changePrecision(left.getPrecision() + someThing);
      stacks[currentStack].push(left);
    }
   else if (doThis == "length")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      cin >> someThing;
      left.changePrecision(someThing);
      stacks[currentStack].push(left);
    }
   else if (doThis == "pop")
    {
      stacks[currentStack].pop();
    }
   else if (doThis == "flush")
    {
      stacks[currentStack].flush();
    }
   else if (doThis == "flushall")
    {
      for (someThing = 0; someThing < totalStacks; someThing++) stacks[someThing].flush();
    }
   else if (doThis == "copy")
    {
      stacks[currentStack].push(stacks[currentStack].top());
    }
   else if (doThis == "swap")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(right);
      stacks[currentStack].push(left);
    }
   else if (doThis == "print")
    {
      if (stacks[currentStack].isEmpty())
         cout << "Stack is empty." << endl;
      else
         cout << stacks[currentStack].top().toString() << endl;
    }
   else if (doThis == "Print")
    {
      if (stacks[currentStack].isEmpty())
         cout << "Stack is empty." << endl;
      else
         cout << stacks[currentStack].top().toString() << endl;
      stacks[currentStack].pop();
    }
   else if (doThis == "pi")
    {
      right = BigInt::M_PI;
      right.changePrecision(32);
      stacks[currentStack].push(right);
    }
   else if (doThis == "Pi")
    {
      right = BigInt::M_PI;
      right.changePrecision(128);
      stacks[currentStack].push(right);
    }
   else if (doThis == "PI")
    {
      right = BigInt::M_PI;
      right.changePrecision(256);
      stacks[currentStack].push(right);
    }
   else if (doThis == "mode")
    {
      cin >> doThis;
      if (doThis == "ROUND_TO_NEAREST_TIES_TO_EVEN")
         stacks[currentStack].setRoundMode(BigInt::ROUND_TIES_EVEN);
      else if (doThis == "ROUND_TO_NEAREST_TIES_AWAY_FROM_ZERO")
         stacks[currentStack].setRoundMode(BigInt::ROUND_TIES_AWAY);
      else if (doThis == "ROUND_TO_POSITIVE_INFINITY")
         stacks[currentStack].setRoundMode(BigInt::ROUND_POSITIVE_INFINITY);
      else if (doThis == "ROUND_TO_NEGATIVE_INFINITY")
         stacks[currentStack].setRoundMode(BigInt::ROUND_NEGATIVE_INFINITY);
      else if (doThis == "ROUND_TO_ZERO")
         stacks[currentStack].setRoundMode(BigInt::ROUND_ZERO);
      else if (doThis == "ROUND_TO_NEAREST_TIES_TO_ODD")
         stacks[currentStack].setRoundMode(BigInt::ROUND_TIES_ODD);
      else if (doThis == "ROUND_TO_NEAREST_TIES_TO_ZERO")
         stacks[currentStack].setRoundMode(BigInt::ROUND_TIES_ZERO);
      else if (doThis == "ROUND_AWAY_FROM_ZERO")
         stacks[currentStack].setRoundMode(BigInt::ROUND_AWAY);
      else if (doThis == "ROUND_05_AWAY_FROM_ZERO")
         stacks[currentStack].setRoundMode(BigInt::ROUND_05_AWAY);
      else
         cerr << "Invalid round mode." << endl;
      stacks[currentStack].focus();
    }
   else if (doThis == "stack")
    {
      cin >> someThing;
      if ((someThing < 0) || (someThing >= totalStacks))
         cerr << "Invalid stack number." << endl;
      else
         currentStack = someThing;
      stacks[currentStack].focus();
    }
   else if (doThis == "pushto")
    {
      cin >> someThing;
      if ((someThing < 0) || (someThing >= totalStacks))
         cerr << "Invalid stack number." << endl;
      else
         stacks[someThing].push(stacks[currentStack].top());
    }
   else if (doThis == "popto")
    {
      cin >> someThing;
      if ((someThing < 0) || (someThing >= totalStacks))
         cerr << "Invalid stack number." << endl;
      else
       {
         stacks[someThing].push(stacks[currentStack].top());
         stacks[currentStack].pop();
       }
    }
   else if (doThis == "pushfrom")
    {
      cin >> someThing;
      if ((someThing < 0) || (someThing >= totalStacks))
         cerr << "Invalid stack number." << endl;
      else
         stacks[currentStack].push(stacks[someThing].top());
    }
   else if (doThis == "popfrom")
    {
      cin >> someThing;
      if ((someThing < 0) || (someThing >= totalStacks))
         cerr << "Invalid stack number." << endl;
      else
       {
         stacks[currentStack].push(stacks[someThing].top());
         stacks[someThing].pop();
       }
    }
   else
    {
      left = Float(doThis);
      stacks[currentStack].push(left);
    }
 }
