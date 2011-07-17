/*
   AltSlowCalc: a slow decimal, floating-point calculator for 8 to 512 digits

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
   File: main.cpp
      This was originally for a semester project in a C++ programming class.

      It wasn't necessary for the project to be this complex, however, I
      was only taking the class for credit, and the project was on my mind.

      This is based on an inferior calculator I wrote in Java, for the purpose
      of showing myself that I could program in Java, and that calculator
      helped speed the completion of this one.

      Pros and cons: In Java, I didn't have to worry about memory
      deallocation. I mitigated this in C++ with the Column class,
      that probably generates alot more memory copying than the Java version.
      I have operator overloading in C++, which makes complex calculations
      in the functions easier to look at. The Java version, if ported directly
      into C++, would be faster, however, as the layers of functions allowed
      me to jump past some bounds checks when I knew they were unnecessary.
*/

//HEADER:
// Calculator includes most everything we need.
// string and iostream make up the rest.
#include <iostream>
#include <string>
#include "Calculator.hpp"

/*
==============================================================================
   Function: main
------------------------------------------------------------------------------
   NOTES:
      Takes no arguments: there are no command line behavior changes.

      Drives the calculator class, which does all of the work.
==============================================================================
*/
int main (void)
 {
   Calculator MAINCALC;
   std::string input;

   std::cerr << "Reverse Polish Notation calculator, alternate version."
      << std::endl << "Programmed by Thomas DiModica." << std::endl
      << "Type 'print' to see the top item on the stack,"
      << std::endl << "and 'exit' or 'quit' to quit." << std::endl;

   std::cin >> input;
   while ((input != "exit") && (input != "quit"))
    {
      MAINCALC.doStuff(input);
      std::cin >> input;
    }

   return 0;
 }
