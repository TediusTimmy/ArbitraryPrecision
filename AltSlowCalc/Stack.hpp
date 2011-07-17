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
   File: Stack.hpp
*/

#ifndef STACK_HPP
#define STACK_HPP

//HEADER:
// stack as this is a wrapper for it.
// Float, as that is all our stack will hold.
#include <stack>
#include "../Float/DecFloat.hpp"

using DecFloat::Float;

    /*
      This is a wrapper for the STL stack, as I don't like
      its default behavior when asked for top() of an empty
      stack: undefined. The STL stack also has undefined
      behavior when asked to pop an empty stack.

      This also wraps up the stack-based round mode.

      With the exception of the added round mode data,
      this is a drop-in replacement for the STL stack.
    */
class Stack
 {
   private:
      std::stack<Float> myStack;
      BigInt::Fixed_Round_Mode myMode;

   public:
      Float & top (void);
      void push (const Float &);
      void pop (void);

      BigInt::Fixed_Round_Mode getRoundMode (void);
      BigInt::Fixed_Round_Mode setRoundMode (BigInt::Fixed_Round_Mode);

      void focus (void); //Called when the stack is selected.

      void flush (void);

      bool isEmpty(void);

      Stack ();
      ~Stack ();
 };

#endif /* STACK_HPP */
