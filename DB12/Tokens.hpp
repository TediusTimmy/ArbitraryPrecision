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
#ifndef TOKENS_HPP
#define TOKENS_HPP

namespace Tokens
 {
   enum TokenType
    {
      Identifier = 1,

      INTEGER,

      COMMENT,

      Assign,		/* := <- */
      Equals,		/* = */
      LEQ,		/* == */

      StrCat,		/* . ## */
      Plus,		/* + */
      Minus,		/* - */
      Multiply,		/* * */
      Divide,		/* / */
      Modulus,		/* % */
      StrictDivide,	/* // */
      StrictModulus,	/* %% */
      Exponent,		/* ^ ** */

      NotEqual,		/* != <> ~= # */
      LessThan,		/* < */
      GreaterThan,	/* > */
      LessOrEqual,	/* <= */
      GreaterOrEqual,	/* >= */

      BoolAnd,		/* & */
      BoolXor,		/* ? */
      BoolOr,		/* | */
      BoolNot,		/* ~ */

      LogAnd,		/* /\ && and */
      LogOr,		/* \/ || or */
      LogNot,		/* ! not */

      ShiftLeft,	/* << */
      ShiftRight,	/* >> */

      Comma,		/* , */
      Colon,		/* : */

      OpenParen,	/* ( */
      CloseParen,	/* ) */

      And,		/* and */
      Break,		/* break */
      Call,		/* call */
      Case,		/* case */
      CElse,		/* case else */ /* Parser, not scanner, generated */
      Choice,		/* and case */ /* Parser generated */
      Continue,		/* continue */
      Default,		/* and case else */ /* Parser generated */
      Dim,		/* dim */
      Do,		/* do */
      DownTo,		/* downto */
      Else,		/* else */
      End,		/* end */
      ESelect,		/* end select */ /* Parser generated */
      Fi,		/* fi */
      For,		/* for */
      Get,		/* get */
      If,		/* if */
      Loop,		/* loop */
      Next,		/* next */
      Print,		/* print */
      Put,		/* put */
      Read,		/* read */
      Repeat,		/* repeat */
      Return,		/* return */
      Select,		/* select */
      Step,		/* step */
      Then,		/* then */
      To,		/* to */
      Until,		/* until */
      Wend,		/* wend */
      While,		/* while */

      TEOL,		/* \n */
      TEOF,		/* EOF */

      INVALID = 99
    };

   struct KeyWordType
    {
      char * Word;
      TokenType Token;
    };

   const int NumKeyWords = 30;

   const KeyWordType KeyWords [] = 
    {
      { "and", And },
      { "break", Break },
      { "call", Call },
      { "case", Case },
      { "continue", Continue },
      { "dim", Dim },
      { "do", Do },
      { "downto", DownTo },
      { "else", Else },
      { "end", End },
      { "fi", Fi },
      { "for", For },
      { "get", Get },
      { "if", If },
      { "loop", Loop },
      { "not", LogNot },
      { "next", Next },
      { "or", LogOr },
      { "print", Print },
      { "put", Put },
      { "read", Read },
      { "repeat", Repeat },
      { "return", Return },
      { "select", Select },
      { "step", Step },
      { "then", Then },
      { "to", To },
      { "until", Until },
      { "wend", Wend },
      { "while", While }
    };

 }

#endif /* TOKENS_HPP */
