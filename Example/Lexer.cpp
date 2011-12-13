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
#include "Lexer.hpp"
#include <cctype>
#include <cstdlib>

#include <iostream>
#include <iomanip>

 /*
   For All of these table loaders:
      ORDER IS IMPORTANT!
      They must match the tables in Tokens.hpp.
 */
void LexerClass::LoadOneChar (void)
 {
   OneChar.add("=");
   OneChar.add("+");
   OneChar.add("-");
   OneChar.add("*");
   OneChar.add("/");
   OneChar.add("%");
   OneChar.add("^");
   OneChar.add("#");
   OneChar.add("<");
   OneChar.add(">");
   OneChar.add("&");
   OneChar.add("|");
   OneChar.add("~");
   OneChar.add("!");
   OneChar.add(".");
   OneChar.add(",");
   OneChar.add(":");
   OneChar.add(";");
   OneChar.add("$");
   OneChar.add("(");
   OneChar.add(")");
   OneChar.add("{");
   OneChar.add("}");
   OneChar.add("[");
   OneChar.add("]");
   OneChar.add("\"");
   OneChar.add("'");
   OneChar.add("\n");
 }

void LexerClass::LoadStartsTwoChar (void)
 {
   StartsTwoChar.add("(");
   StartsTwoChar.add("*");
   StartsTwoChar.add(":");
   StartsTwoChar.add("<");
   StartsTwoChar.add("=");
   StartsTwoChar.add("/");
   StartsTwoChar.add("%");
   StartsTwoChar.add("!");
   StartsTwoChar.add("~");
   StartsTwoChar.add(">");
   StartsTwoChar.add("^");
   StartsTwoChar.add("&");
   StartsTwoChar.add("|");
 }

void LexerClass::LoadTwoChar (void)
 {
   TwoChar.add("(*");
   TwoChar.add("*)");
   TwoChar.add(":=");
   TwoChar.add("<-");
   TwoChar.add("==");
   TwoChar.add("//");
   TwoChar.add("%%");
   TwoChar.add("**");
   TwoChar.add("<>");
   TwoChar.add("!=");
   TwoChar.add("~=");
   TwoChar.add("<=");
   TwoChar.add(">=");
   TwoChar.add("^^");
   TwoChar.add("&&");
   TwoChar.add("||");
   TwoChar.add("<<");
   TwoChar.add(">>");
 }

void LexerClass::LoadReservedWords (void)
 {
   ReservedWords.add("xor");
   ReservedWords.add("and");
   ReservedWords.add("or");
   ReservedWords.add("not");
   ReservedWords.add("begin");
   ReservedWords.add("call");
   ReservedWords.add("downto");
   ReservedWords.add("else");
   ReservedWords.add("end");
   ReservedWords.add("for");
   ReservedWords.add("goto");
   ReservedWords.add("if");
   ReservedWords.add("max");
   ReservedWords.add("min");
   ReservedWords.add("print");
   ReservedWords.add("printf");
   ReservedWords.add("prints");
   ReservedWords.add("read");
   ReservedWords.add("reads");
   ReservedWords.add("repeat");
   ReservedWords.add("return");
   ReservedWords.add("sqrt");
   ReservedWords.add("step");
   ReservedWords.add("to");
   ReservedWords.add("until");
   ReservedWords.add("while");
   ReservedWords.add("var");
 }

 /*
   Now, for internal buffered IO commands.
 */
void LexerClass::GetNextLine (void)
 {
    //Get the next line of input, or a blank line
   if (!inFile.eof())
    {
      std::getline(inFile, currentLine);
      lineNumber++;
    }
   else currentLine = "";
   currentLine += '\n'; //getline strips the newline, which we'll use
   linePosition = 0;
 }

char LexerClass::GetNextNonWhite (void)
 {
   char c = GetNextChar();
   while (((c == ' ') || (c == '\t') || (c == '\n')) && !inFile.eof())
      c = GetNextChar();
   if (inFile.eof()) c = ' '; //Return a space on EOF.
   return c;
 }

char LexerClass::GetNextChar (void)
 {
   if (linePosition == currentLine.length()) GetNextLine();
   return currentLine[linePosition++];
 }

void LexerClass::UnGetNextChar (void)
 {
   linePosition--;
 }

 /*
   Now, the main initialization function for the Lexer.
 */
bool LexerClass::InitializeFile (const char * src, SymbolTable & Dest)
 {
   lineNumber = 0;
   dest = &Dest;
   LoadAllTables();
   inFile.open(src);
   if (!inFile.fail()) GetNextLine();
   return !inFile.fail();
 } //Wow, that was shorter than I thought it would be.

 /*
   As the Lexer knows all IO stuff, everything will tell the Lexer
   to print error messages.
 */
void LexerClass::Error (const char * message) const
 {
   std::cerr << "Line " << lineNumber << " " << message << std::endl;
 }

 /*
   Finally, the GetNextToken function.
 */
void LexerClass::GetNextToken (void)
 {
   string temp, temp2;
   long n;
   bool done;

    /*
      So, instead of creating a boolean variable that says when we are done,
      we just pretend that we just read in a comment, and continue with SOP.
      This makes sure that comments are eaten, and doesn't poliferate boolean
      variables, while still allowing comments to return to the Start state.
    */
   internal = Tokens::COMMENT;
   while (internal == Tokens::COMMENT)
    {
      temp = GetNextNonWhite();
      if (std::isalpha(temp[0])) //Read a letter
       { //Read in an identifier or keyword -- loop until not letter or number
         while (std::isalnum(n = GetNextChar())) temp += (char) n;
         UnGetNextChar();

         if ((n = ReservedWords.lookupName(temp)) != -1) //reserved word
          {
            internal = Tokens::KeyWords[n];
          }
         else
          {
            internal = Tokens::Identifier;
            location = dest->addSymbol( Symbol(temp, Undefined_t, 0LL) );
          }
       }
      else if (std::isdigit(temp[0])) //Read a number
       { //Read in a number -- loop until not a digit
         while (std::isdigit(n = GetNextChar())) temp += (char) n;
         UnGetNextChar();

         internal = Tokens::INTEGER;
         location = dest->addSymbol(Symbol("", Constant_t, Integer(temp)) );
       }
      else if (temp[0] == '@') //Read a special number
       {
         switch (GetNextChar())
          {
            case 'H':
            case 'h':
               temp += 'H';

               while (std::isxdigit(n = GetNextChar())) temp += (char) n;
               UnGetNextChar();

               internal = Tokens::INTEGER;
               location = dest->addSymbol( Symbol("", Constant_t,
                  Integer(temp.substr(2, temp.length()), 16) ) );

               break;

            case 'O':
            case 'o':
               temp += 'O';

               n = GetNextChar();
               while ((n >= '0') && (n <= '7'))
                {
                  temp += (char) n;
                  n = GetNextChar();
                }
               UnGetNextChar();

               internal = Tokens::INTEGER;
               location = dest->addSymbol( Symbol("", Constant_t,
                  Integer(temp.substr(2, temp.length()), 8) ) );

               break;

            case 'B':
            case 'b':
               temp += 'B';

               n = GetNextChar();
               while ((n >= '0') && (n <= '1'))
                {
                  temp += (char) n;
                  n = GetNextChar();
                }
               UnGetNextChar();

               internal = Tokens::INTEGER;
               location = dest->addSymbol( Symbol("", Constant_t,
                  Integer(temp.substr(2, temp.length()), 2) ) );

               break;

            default:
               UnGetNextChar();

               internal = Tokens::INVALID;
               break;
          }
       }
      else if (StartsTwoChar.lookupName(temp) != -1)
       { //Maybe a two-character token
         temp += GetNextChar();

         if ((n = TwoChar.lookupName(temp)) != -1)
          {
            internal = Tokens::TwoCharTokens[n];

             /*
               The code treats the comment marker as a two-character token
               so A) it can be in the two-character token table, and B) it
               simplifies the code by reducing character constants.
             */
            if (internal == Tokens::OpenComment2)
             {
               internal = Tokens::COMMENT;
               done = false;

               temp = TwoChar.lookupCode(Tokens::CloseComment2Index);

               while (!EndFile() && !done)
                {
                  n = GetNextChar();

                  while (!EndFile() && (n == temp[0]))
                   {
                     n = GetNextChar();

                     if (n == temp[1]) done = true;
                   }

                  if (EndFile())
                     Error("Warning: End of File found before comment "
                           "terminated.");
                }
             }
             /*
               The only drawback of this is that the close comment token
               becomes an acceptable token.
             */
            else if (internal == Tokens::CloseComment2)
             {
               internal = Tokens::INVALID;
             }
          }
         else
          { //Was not a two-character token
            temp = temp[0];
            UnGetNextChar();

            if ((n = OneChar.lookupName(temp)) != -1)
             { //Accept the one-character token
               internal = Tokens::OneCharTokens[n];
             }
            else
             {
               internal = Tokens::INVALID;
             }
          }
       }
       /*
         A one-character token
         We've already handled single chars that start a two-char token,
         so we can use the full one-char token table at this point safely.
       */
      else if ((n = OneChar.lookupName(temp)) != -1)
       {
         internal = Tokens::OneCharTokens[n];

         if ((internal == Tokens::String1) ||
             (internal == Tokens::String2))
          {
            temp = "";
            temp2 = GetNextChar();
            n = OneChar.lookupName(temp2);
            while ((n == -1) ||
                   ((Tokens::OneCharTokens[n] != internal) &&
                   (Tokens::OneCharTokens[n] != Tokens::EndOfLine)))
             {
               temp += temp2;
               temp2 = GetNextChar();
               n = OneChar.lookupName(temp2);
             }

             /*
               Internally, all lines are newline terminated, so there cannot
               be an EOF before an EndOfLine is read.
             */
            if (Tokens::OneCharTokens[n] == Tokens::EndOfLine)
               Error("Warning: End of Line found before string terminated.");

            internal = Tokens::INTEGER;
            location = dest->addSymbol(
               Symbol("", Constant_t, intify(temp)) );
          }
         else if (internal == Tokens::OpenComment)
          { //Agian, this was folded in to the one-char case.
            internal = Tokens::COMMENT;

            while (!EndFile() && ((n == -1) || ((n != -1) &&
                   (Tokens::OneCharTokens[n] != Tokens::CloseComment))))
             {
               temp = GetNextNonWhite();
               n = OneChar.lookupName(temp);

               if (EndFile())
                  Error("Warning: End of File found before comment "
                        "terminated.");
             }
          }
         else if (internal == Tokens::CloseComment)
          { //Again, close comment is invalid.
            internal = Tokens::INVALID;
          }
       }
      else //Must be an invalid character.
       {
         internal = Tokens::INVALID;
       }
    }
    /*
      Temp always holds the value that NextSymbol will get, so we do this
      assignment here, after we're sure we have the next symbol.
    */
   nextSymbol = temp;
 }
