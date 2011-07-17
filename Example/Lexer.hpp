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
#ifndef LEXER_HPP
#define LEXER_HPP

#include "Shared.hpp"
#include "Tokens.hpp"
#include <string>
#include <fstream>

using std::string;
using std::fstream;

class LexerClass /* Lex Me Up, Scotty */
 {
   private:
       /*
         Line input buffer.
       */
      string currentLine;
       /*
         Input file.
       */
      fstream inFile;
       /*
         Location in input file.
       */
      size_t linePosition;
       /*
         TokenType we are returning.
       */
      Tokens::TokenType internal;
       /*
         Token's string representation.
       */
      string nextSymbol;
       /*
         Identifier/literal location.
       */
      long location;
       /*
         Tables for character matching.
       */
      ReserveTable OneChar,
                   StartsTwoChar,
                   TwoChar,
                   ReservedWords;
       /*
         Current Line Number.
       */
      size_t lineNumber;

       /*
         Internal functions for operating on buffered input.
       */
      void GetNextLine (void);
      char GetNextNonWhite (void);
      char GetNextChar (void);
      void UnGetNextChar (void);

       /*
         Internal initialization functions.
       */
      void LoadOneChar (void);
      void LoadStartsTwoChar (void);
      void LoadTwoChar (void);
      void LoadReservedWords (void);
      void LoadAllTables (void)
       {
         LoadOneChar();
         LoadStartsTwoChar();
         LoadTwoChar();
         LoadReservedWords();
       }

   public:

       /*
         Symbol table to load.
       */
      SymbolTable * dest;

       /*
         Initialize input file with name and symbol table to fill.
         Returns false on error, true on success.
       */
      bool InitializeFile (const char *, SymbolTable &);

       /*
         The all-important Get Next Token function.
       */
      void GetNextToken (void);

       /*
         Prints some sort of error/warning message to the screen.
       */
      void Error (const char *) const;

       /*
         Returns true when the file's input is exhausted.
       */
      bool EndFile (void) const { return inFile.eof(); }

       /*
         Returns the next token's TokenType.
       */
      Tokens::TokenType Internal (void) const { return internal; }

       /*
         Returns the actual string representation of the token, the lexeme.
       */
      string NextSymbol (void) const { return nextSymbol; }

       /*
         Returns the location where identifiers and/or literals are put
         into the symbol table.
       */
      long Location (void) const { return location; }

       /*
         Returns the current line number.
       */
      size_t LineNo (void) const { return lineNumber; }
 };

#endif /* LEXER_HPP */
