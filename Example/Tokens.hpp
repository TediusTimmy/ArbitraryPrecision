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
#ifndef TOKENS_HPP
#define TOKENS_HPP

namespace Tokens
 {
   enum TokenType
    {
      Identifier = 1,

      INTEGER,

      COMMENT,

      OpenComment,	/* { or [ */
      CloseComment,	/* } or ] */
      OpenComment2,	/* (* */
      CloseComment2,	/* *) */

      Assign,		/* := <- */
      Equals,		/* = */
      LEQ,		/* == */

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
      BoolXor,		/* ^^ xor */
      BoolOr,		/* | */
      BoolNot,		/* ~ */

      LogAnd,		/* && and */
      LogOr,		/* || or */
      LogNot,		/* ! not */

      ShiftLeft,	/* << */
      ShiftRight,	/* >> */

      Period,		/* . */
      Comma,		/* , */
      Colon,		/* : */
      SemiColon,	/* ; */

      OpenParen,	/* ( */
      CloseParen,	/* ) */

      Begin,		/* begin */
      Call,		/* call */
      DownTo,		/* downto */
      Else,		/* else */
      End,		/* end */
      For,		/* for */
      Goto,		/* goto */
      If,		/* if */
      Max,		/* max */
      Min,		/* min */
      Print,		/* print */
      PrintF,		/* printf */
      PrintS,		/* prints */
      Read,		/* read */
      ReadS,		/* reads */
      Repeat,		/* repeat */
      Return,		/* return */
      Sqrt,		/* sqrt */
      Step,		/* step */
      To,		/* to */
      Until,		/* until */
      While,		/* while */
      Var,		/* var */

      String1,		/* " */
      String2,		/* ' */
      EndOfLine,	/* \n */

      INVALID = 99
    };

    /*
      Given the table index of a one char token, return its TokenType.
    */
   const TokenType OneCharTokens [] =
    {
      Equals,
      Plus,
      Minus,
      Multiply,
      Divide,
      Modulus,
      Exponent,
      NotEqual,
      LessThan,
      GreaterThan,
      BoolAnd,
      BoolOr,
      BoolNot,
      LogNot,
      Period,
      Comma,
      Colon,
      SemiColon,
      OpenParen,
      CloseParen,
      OpenComment,
      CloseComment,
      OpenComment,
      CloseComment,
      String1,
      String2,
      EndOfLine
    };

    /*
      Sometimes, we need this index.
    */
   const int CloseComment2Index = 1;

    /*
      Given the table index of a two char token, return its TokenType.
    */
   const TokenType TwoCharTokens [] =
    {
      OpenComment2,
      CloseComment2,
      Assign,
      Assign,
      LEQ,
      StrictDivide,
      StrictModulus,
      Exponent,
      NotEqual,
      NotEqual,
      NotEqual,
      LessOrEqual,
      GreaterOrEqual,
      BoolXor,
      LogAnd,
      LogOr,
      ShiftLeft,
      ShiftRight
    };

    /*
      Given the table index of a keyword, return its TokenType.
    */
   const TokenType KeyWords [] =
    {
      BoolXor,
      LogAnd,
      LogOr,
      LogNot,
      Begin,
      Call,
      DownTo,
      Else,
      End,
      For,
      Goto,
      If,
      Max,
      Min,
      Print,
      PrintF,
      PrintS,
      Read,
      ReadS,
      Repeat,
      Return,
      Sqrt,
      Step,
      To,
      Until,
      While,
      Var
    };

 }

#endif /* TOKENS_HPP */
