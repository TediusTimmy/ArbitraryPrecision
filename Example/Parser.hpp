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
#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"

class ParserClass /* Syntax Analyzer */ /* Analyser for the Bri'ish */
 {
   private:

      void expect (Tokens::TokenType);
      void expectationError (const char *);
       /*
         Convention: function enters w/ next input already read.
       */
      long program (void);
      long statement (void);
      long variable (void);
      long boolean (void);
      long clause (void);
      long predicate (void);
      long equivalence (void);
      long relation (void);
      long relate (void);
      long _or_ (void);
      long _xor_ (void);
      long _and_ (void);
      long shift (void);
      long simple (void);
      long term (void);
      long factor (void);
      long primary (void);
      long identifier (void);
      long constant (void);

      void GetNext (void); // Wrapper function that prints stuff out.
      void GNT (void); // Wrapper function that throws an EOFError.

       /*
         An error has occurred.
         The purpose of this variable is to flag not executing the erroneous
         code when we get to code generation.
       */
      bool error;
       /*
         Count of errors.
       */
      long errorC;

       /*
         Dummy value to return for now.
       */
      const static int NoError;
       /*
         Location in the symbol table of our three constants.
       */
      const static int SymZero;
      const static int SymOne;
      const static int SymStringZero;
      const static int SymTen;
       /*
         Used for tracking if a label has been declared and defined.
       */
      const static Integer UsedLabel;

       /*
         Exception classes.
       */
      class ParseError { };
      class EOFError { };

      LexerClass * src;
      OpTable * dest;

      Tokens::TokenType Internal (void) { return src->Internal(); }

   public:

      bool Error (void) const { return error; }

      void Parse (LexerClass &, OpTable &);

 };

#endif /* PARSER_HPP */
