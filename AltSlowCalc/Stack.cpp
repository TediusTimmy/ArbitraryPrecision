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
   File: Stack.cpp
*/

//HEADER:
// Stack defines the class that we are working on, and everything else we need.
#include "Stack.hpp"

/*
==============================================================================
   Function: no argument Stack constructor
==============================================================================
*/
   Stack::Stack () : myMode(BigInt::ROUND_TIES_EVEN) { }

/*
==============================================================================
   Function: Stack destructor
------------------------------------------------------------------------------
   NOTES:
      Flushes the stack on being destroyed.
==============================================================================
*/
   Stack::~Stack () { while (myStack.size() != 0) myStack.pop(); }

      //Zero constant for use here.
   const Float fZero ("0");

/*
==============================================================================
   Function: top
------------------------------------------------------------------------------
   OUTPUT: a reference to the top item on the stack
   NOTES:
      Pushes a zero and returns a reference to it if the stack is empty.
==============================================================================
*/
   Float & Stack::top (void)
    {
      //Push a zero onto the stack if it is empty.
      if (myStack.size() == 0)
         myStack.push(fZero);
      return myStack.top();
    }

/*
==============================================================================
   Function: pop
------------------------------------------------------------------------------
   NOTES:
      Removes the top item from the stack.
==============================================================================
*/
   void Stack::pop (void) { if (myStack.size() != 0) myStack.pop(); }

/*
==============================================================================
   Function: push
------------------------------------------------------------------------------
   INPUT: item to be pushed onto the stack
==============================================================================
*/
   void Stack::push (const Float & me) { myStack.push(me); }

/*
==============================================================================
   Function: focus
------------------------------------------------------------------------------
   NOTES:
      Sets the global round mode to the round mode of the stack when the
      stack is selected as the operating stack.
==============================================================================
*/
   void Stack::focus (void) { BigInt::Fixed::setRoundMode(myMode); }

/*
==============================================================================
   Function: flush
------------------------------------------------------------------------------
   NOTES:
      Removes all items from the stack.
==============================================================================
*/
   void Stack::flush (void) { while (myStack.size() != 0) myStack.pop(); }

/*
==============================================================================
   Function: isEmpty
------------------------------------------------------------------------------
   OUTPUT: true if the stack is empty, false otherwise.
==============================================================================
*/
   bool Stack::isEmpty (void) { return (myStack.size() == 0); }

/*
==============================================================================
   Function: getRoundMode
------------------------------------------------------------------------------
   OUTPUT: the stack's round mode
==============================================================================
*/
   BigInt::Fixed_Round_Mode Stack::getRoundMode (void) { return myMode; }

/*
==============================================================================
   Function: setRoundMode
------------------------------------------------------------------------------
   INPUT: the new round mode
   OUTPUT: if successful, the new round mode, else something else
==============================================================================
*/
   BigInt::Fixed_Round_Mode Stack::setRoundMode (BigInt::Fixed_Round_Mode toThis)
      { return (myMode = toThis); }
