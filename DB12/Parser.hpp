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
      long statement_seq (vector<BackRef> &);
      long statement (vector<BackRef> &);
      long label (void);
      long variable (void);
      long expression (void);
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

       /*
         Wrapper that handles LL(2) constructs and Invalid input.
       */
      void GNT (void);

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
         The parser's "token" of lookahead.
       */
      Tokens::TokenType Internal;
      string NextSymbol;
      long Location;
      size_t LineNo;

       /*
         Dummy value to return for now.
       */
      const static int NoError;
       /*
         Location in the symbol table of our three constants.
       */
      const static int SymZero;
      const static int SymOne;

       /*
         Exception classes.
       */
      class ParseError { };

      LexerClass * src;
      OpTable * dest;

   public:

      bool Error (void) const { return error; }

      void Parse (LexerClass &, OpTable &);

 };

#endif /* PARSER_HPP */
