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
#include "Lexer.hpp"
#include <cctype>
#include <cstdlib>

#include <iostream>
#include <iomanip>

const int LexerClass::NoLoc = -1;

 /*
   Gar... For the Symbol Table to use because we aren't passing an
   Integer to it anymore and we don't want to convert the thing into
   a string... agian.
 */
Integer LexerTemp;


 /*
   Internal buffered IO commands.
 */
void LexerClass::GetNextLine (void)
 {
   int c;
    //Get the next line of input, or a blank line
   do
    {
      currentLine = "";
      if (!inFile.eof())
       {
         //getline strips the newline, which is significant
         c = inFile.get();
         while (!inFile.eof() && (c != '\n'))
          {
            currentLine += c;
            c = inFile.get();
          }
         if (c == '\n') currentLine += c;
         lineNumber++;
       }
      else currentLine = "";
    }
   while (currentLine[0] == '#');
   linePosition = 0;
 }

char LexerClass::GetNextNonWhite (void)
 {
   char c = GetNextChar();
   while ((c == ' ') || (c == '\t'))
      c = GetNextChar(); // Return a '\0' on EOF
   return c;
 }

char LexerClass::GetNextChar (void)
 {
   char ret;
   if (linePosition == -1) ret = '\0';
   else
    {
      if ((size_t)linePosition >= currentLine.length()) GetNextLine();

      if (currentLine.length() == 0) ret = '\0';
      else ret = currentLine[linePosition++];
    }
   return ret;
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
   string temp;
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
      n = GetNextNonWhite();
      if (std::isalpha(n)) //Read a letter
       { //Read in an identifier or keyword -- loop until not letter or number
         temp = (char) n;
         while (std::isalnum(n = GetNextChar())) temp += (char) n;
         UnGetNextChar();

         if (temp.length() == 1)
          {
            if ((temp[0] >= 'a') || (temp[0] == 'U') || (temp[0] == 'V'))
             {
               internal = Tokens::INVALID;
             }
            else
             {
               internal = Tokens::Identifier;
               location = '@' - temp[0];
               if (location < -20) location += 2;
             }
          }
         else
          {
            for (n = 0, done = false; (n < Tokens::NumKeyWords) && !done; n++)
               if (temp == Tokens::KeyWords[n].Word)
                {
                  done = true;
                  n--;
                }

            if (n != Tokens::NumKeyWords) //reserved word
             {
               internal = Tokens::KeyWords[n].Token;
             }
            else
             {
               internal = Tokens::Identifier;
               location = dest->addSymbol( Symbol(temp, Undefined_t, NoLoc) );
             }
          }
       }
      else if (std::isdigit(n)) //Read a number
       { //Read in a number -- loop until not a digit
         temp = (char) n;
         while (std::isdigit(n = GetNextChar())) temp += (char) n;
         UnGetNextChar();

         internal = Tokens::INTEGER;
         LexerTemp = Integer(temp);
         location = dest->addSymbol( Symbol("", Constant_t, NoLoc) );
         if (dest->getValue(location) == NoLoc)
          {
            dest->setValue(location, Constants.size());
            Constants.push_back(LexerTemp);
          }
       }
      else if (n == '@') //Read a special number
       {
         switch (GetNextChar())
          {
            case 'H':
            case 'h':
               while (std::isxdigit(n = GetNextChar())) temp += (char) n;
               UnGetNextChar();

               if (temp == "")
                {
                  internal = Tokens::INVALID;
                }
               else
                {
                  internal = Tokens::INTEGER;
                  LexerTemp = Integer(temp, 16);
                  location = dest->addSymbol( Symbol("", Constant_t, NoLoc) );
                  if (dest->getValue(location) == NoLoc)
                   {
                     dest->setValue(location, Constants.size());
                     Constants.push_back(LexerTemp);
                   }
                }
               temp = "@H" + temp;

               break;

            case 'O':
            case 'o':
               n = GetNextChar();
               while ((n >= '0') && (n <= '7'))
                {
                  temp += (char) n;
                  n = GetNextChar();
                }
               UnGetNextChar();

               if (temp == "")
                {
                  internal = Tokens::INVALID;
                }
               else
                {
                  internal = Tokens::INTEGER;
                  LexerTemp = Integer(temp, 8);
                  location = dest->addSymbol( Symbol("", Constant_t, NoLoc) );
                  if (dest->getValue(location) == NoLoc)
                   {
                     dest->setValue(location, Constants.size());
                     Constants.push_back(LexerTemp);
                   }
                }
               temp = "@O" + temp;

               break;

            case 'B':
            case 'b':
               n = GetNextChar();
               while ((n == '0') || (n == '1'))
                {
                  temp += (char) n;
                  n = GetNextChar();
                }
               UnGetNextChar();

               if (temp == "")
                {
                  internal = Tokens::INVALID;
                }
               else
                {
                  internal = Tokens::INTEGER;
                  LexerTemp = Integer(temp, 2);
                  location = dest->addSymbol( Symbol("", Constant_t, NoLoc) );
                  if (dest->getValue(location) == NoLoc)
                   {
                     dest->setValue(location, Constants.size());
                     Constants.push_back(LexerTemp);
                   }
                }
               temp = "@B" + temp;

               break;

            case 'D':
            case 'd':
               while (std::isdigit(n = GetNextChar())) temp += (char) n;
               UnGetNextChar();

               if (temp == "")
                {
                  internal = Tokens::INVALID;
                }
               else
                {
                  internal = Tokens::INTEGER;
                  LexerTemp = Integer(temp);
                  location = dest->addSymbol( Symbol("", Constant_t, NoLoc) );
                  if (dest->getValue(location) == NoLoc)
                   {
                     dest->setValue(location, Constants.size());
                     Constants.push_back(LexerTemp);
                   }
                }
               temp = "@D" + temp;

               break;

            default:
               UnGetNextChar();

               internal = Tokens::INVALID;
               temp = '@';
               break;
          }
       }
      else
       { //DFA for all other tokens
         if ((n != '\0') && (n != '\n')) temp = (char) n;
         switch (n)
          {
            case '<':
               switch (GetNextChar())
                {
                  case '-':
                     temp += '-';
                     internal = Tokens::Assign;
                     break;
                  case '>':
                     temp += '>';
                     internal = Tokens::NotEqual;
                     break;
                  case '=':
                     temp += '=';
                     internal = Tokens::LessOrEqual;
                     break;
                  case '<':
                     temp += '<';
                     internal = Tokens::ShiftLeft;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::LessThan;
                     break;
                }
               break;

            case '*':
               switch (GetNextChar())
                {
                  case '*':
                     temp += '*';
                     internal = Tokens::Exponent;
                     break;
                  case ')':
                     temp += ')';
                     internal = Tokens::INVALID;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::Multiply;
                     break;
                }
               break;

            case '/':
               switch (GetNextChar())
                {
                  case '/':
                     temp += '/';
                     internal = Tokens::StrictDivide;
                     break;
                  case '\\':
                     temp += '\\';
                     internal = Tokens::AlgAnd;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::Divide;
                     break;
                }
               break;

            case '>':
               switch (GetNextChar())
                {
                  case '=':
                     temp += '=';
                     internal = Tokens::GreaterOrEqual;
                     break;
                  case '>':
                     temp += '>';
                     internal = Tokens::ShiftRight;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::GreaterThan;
                     break;
                }
               break;

            case '\\':
               switch (GetNextChar())
                {
                  case '/':
                     temp += '/';
                     internal = Tokens::AlgOr;
                     break;
                  case '\n':
                     internal = Tokens::COMMENT;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::INVALID;
                     break;
                }
               break;

            case ':':
               switch (GetNextChar())
                {
                  case '=':
                     temp += '=';
                     internal = Tokens::Assign;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::Colon;
                     break;
                }
               break;

            case '~':
               switch (GetNextChar())
                {
                  case '=':
                     temp += '=';
                     internal = Tokens::NotEqual;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::BoolNot;
                     break;
                }
               break;

            case '!':
               switch (GetNextChar())
                {
                  case '=':
                     temp += '=';
                     internal = Tokens::NotEqual;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::LogNot;
                     break;
                }
               break;

            case '#':
               switch (GetNextChar())
                {
                  case '#':
                     temp += '#';
                     internal = Tokens::StrCat;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::NotEqual;
                     break;
                }
               break;

            case '=':
               switch (GetNextChar())
                {
                  case '=':
                     temp += '=';
                     internal = Tokens::LEQ;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::Equals;
                     break;
                }
               break;

            case '%':
               switch (GetNextChar())
                {
                  case '%':
                     temp += '%';
                     internal = Tokens::StrictModulus;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::Modulus;
                     break;
                }
               break;

            case '&':
               switch (GetNextChar())
                {
                  case '&':
                     temp += '&';
                     internal = Tokens::LogAnd;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::BoolAnd;
                     break;
                }
               break;

            case '|':
               switch (GetNextChar())
                {
                  case '|':
                     temp += '|';
                     internal = Tokens::LogOr;
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::BoolOr;
                     break;
                }
               break;

            case '(':
               switch (GetNextChar())
                {
                  case '*':
                     temp = "";
                     internal = Tokens::COMMENT;
                     done = false;
                     while (!done)
                      {
                        n = GetNextChar();
                        while ((n != '*') && (n != '\0')) n = GetNextChar();
                        if (n != '\0') n = GetNextChar();
                        while ((n == '*') && (n != '\0')) n = GetNextChar();
                        if ((n == ')') || (n == '\0')) done = true;
                      }
                     if (n == '\0') Error("Warning: End of File found "
                        "before comment terminated.");
                     break;
                  default:
                     UnGetNextChar();
                     internal = Tokens::OpenParen;
                     break;
                }
               break;

            case '$':
            case '.':
               internal = Tokens::StrCat;
               break;

            case ',':
               internal = Tokens::Comma;
               break;

            case '+':
               internal = Tokens::Plus;
               break;

            case '-':
               internal = Tokens::Minus;
               break;

            case '^':
               internal = Tokens::Exponent;
               break;

            case '?':
               internal = Tokens::BoolXor;
               break;

            case ')':
               internal = Tokens::CloseParen;
               break;

            case '[':
               internal = Tokens::OpenBrack;
               break;

            case ']':
               internal = Tokens::CloseBrack;
               break;

            case '\0':
               temp = "__EOF__";
               internal = Tokens::TEOF;
               break;

            case ';':
               n = GetNextChar();
               while ((n != '\n') && (n != '\0')) n = GetNextChar();

            case '\n':
               temp = "__NEWLINE__";
               internal = Tokens::TEOL;
               break;

            case '{':
               internal = Tokens::COMMENT;
               n = GetNextChar();
               while ((n != '}') && (n != '\0')) n = GetNextChar();
               if (n == '\0') Error("Warning: End of File found before "
                  "comment terminated.");
               break;

            case '"':
               temp = "";
               n = GetNextChar();
               while ((n != '"') && (n != '\0') && (n != '\n'))
                {
                  temp += n;
                  n = GetNextChar();
                }
               if (n == '\n')
                {
                  Error("Warning: End of Line found before "
                     "string terminated.");
                  UnGetNextChar();
                }
               if (n == '\0') Error("Warning: End of File found before "
                  "string terminated.");

               internal = Tokens::INTEGER;
               LexerTemp = intify(temp);
               location = dest->addSymbol( Symbol("", Constant_t, NoLoc) );
               if (dest->getValue(location) == NoLoc)
                {
                  dest->setValue(location, Constants.size());
                  Constants.push_back(LexerTemp);
                }
               break;

            case '\'':
               temp = "";
               n = GetNextChar();
               while ((n != '\'') && (n != '\0') && (n != '\n'))
                {
                  temp += n;
                  n = GetNextChar();
                }
               if (n == '\n')
                {
                  Error("Warning: End of Line found before "
                     "string terminated.");
                  UnGetNextChar();
                }
               if (n == '\0') Error("Warning: End of File found before "
                  "string terminated.");

               internal = Tokens::INTEGER;
               LexerTemp = intify(temp);
               location = dest->addSymbol( Symbol("", Constant_t, NoLoc) );
               if (dest->getValue(location) == NoLoc)
                {
                  dest->setValue(location, Constants.size());
                  Constants.push_back(LexerTemp);
                }
               break;

            default:
               internal = Tokens::INVALID;
               break;
          }
       }
    }
    /*
      Temp always holds the value that NextSymbol will get, so we do this
      assignment here, after we're sure we have the next symbol.
    */
   nextSymbol = temp;
 }
