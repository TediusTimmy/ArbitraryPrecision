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
#include "Parser.hpp"
#include <iostream>


const int ParserClass::NoError = 0;
const int ParserClass::SymZero = 0;
const int ParserClass::SymOne = 1;
const int ParserClass::SymStringZero = 2;
const int ParserClass::SymTen = 3;
const Integer ParserClass::UsedLabel = Integer(-1LL);


 /*
   This is the parser's entry point. It checks for extra data in file,
   catches all exceptions thrown (by the parsing code), and calls
   GetNextToken for the first time.

   It also now adds zero and one to the symbol table as the first entries.
   Negative one isn't needed.
 */
void ParserClass::Parse (LexerClass & tokenSrc, OpTable & codeDest)
 {
   error = false;
   errorC = 0;

   src = &tokenSrc;
   dest = &codeDest;

   src->dest->addSymbol(Symbol("0", Constant_t, 0LL));
   src->dest->addSymbol(Symbol("1", Constant_t, 1LL));
   src->dest->addSymbol(Symbol("\"0", Constant_t, (long long)'0'));
   src->dest->addSymbol(Symbol("10", Constant_t, 10LL));

   try
    {
      GNT();

      program();
    }

   catch (EOFError)
    {
      std::cerr << "Error: unexpected end of file." << std::endl;
    }
   catch (ParseError) { }

    /*
      Now, check for label used but not defined errors.
      This scheme may seem complex, but using a label that never gets defined
         is a serious semantic error that shouldn't be a runtime error.
      Also, declaring a label but never defining or using it shouldn't be an
         error at all.
      This solves the problem while not generating spurious errors.
    */
   for (size_t i = 0; i < src->dest->size(); i++)
    {
      if ((src->dest->getType(i) == Label_t) &&
          (src->dest->getValue(i) == UsedLabel))
       {
         std::cerr << "Error: label \"" <<
            src->dest->getName(i) << "\" used but not defined." << std::endl;
         error = true;
         errorC++;
       }
    }

    /*
      So I can input programs from stdin, I adopt the brainfuck convention
      that a '!' seperates a program and its input.
    */
   if (!src->EndFile() && (Internal() != Tokens::LogNot))
    {
      std::cerr << "Warning: data past end of program ignored." << std::endl;
    }
   std::cout << "Total Errors: " << errorC << std::endl;
 }

 /*
   A wrapper for GetNextToken that prints out the token information.
 */
void ParserClass::GetNext (void)
 {
   src->GetNextToken();
 }

 /*
   A wrapper for the wrapper that throws an
   exception if EOF is reached.
 */
void ParserClass::GNT (void)
 {
   GetNext();

   if (src->EndFile())
    {
      error = true;
      errorC++;
      throw EOFError ();
    }

   while (Internal() == Tokens::INVALID)
    {
      std::cerr << "Line " << src->LineNo() <<
         " invalid lexeme discarded: " << src->NextSymbol() << std::endl;
      error = true;
      errorC++;
      GetNext();

      if (src->EndFile())
       {
         error = true;
         errorC++;
         throw EOFError ();
       }
    }
 }

 /*
   A commonly occurring sequence of code related to expecting a certain token.
 */
void ParserClass::expect (Tokens::TokenType me)
 {
   if (me != Internal())
    {
      error = true;
      errorC++;
      throw ParseError ();
    }
   GNT();
 }

 /*
   Another commonly occurring sequence of code, whining about expecting a
   certain token.
 */
void ParserClass::expectationError (const char * s)
 {
   error = true;
   std::cerr << "Line " << src->LineNo() << " expected: " << s <<
      " found \"" << src->NextSymbol() << "\"" << std::endl;
 }

long ParserClass::program (void)
 {
   long location;

   while (Internal() == Tokens::Var)
    {
      GNT();

      location = identifier();

      src->dest->setType(location, Variable_t);

      while ((Internal() == Tokens::Comma) ||
             (Internal() == Tokens::Identifier))
       {
         if (Internal() == Tokens::Comma) GNT();
         else
          {
            expectationError("\",\"");
            errorC++;
          }

         location = identifier();

         if (src->dest->getType(location) == Undefined_t)
          {
            src->dest->setType(location, Variable_t);
          }
         else
          {
            std::cerr << "Reuse of identifier \"" <<
               src->dest->getName(location) << "\"" << std::endl;
            error = true;
            errorC++;
          }
       }

      try { expect(Tokens::Period); }
      catch (ParseError)
       {
         expectationError("\".\"");
       }

    }

   statement();

   if (Internal() == Tokens::Period) // Can't use expect or GNT here,
    {                                // as I don't want an EOFError
      GetNext();                     // thrown.
    }
   else
    {
      expectationError("\".\"");
      errorC++;
    }

    /*
      Well, the program is over, so here should be the best place to put this.
    */
   dest->addOp( OpCode(Return_Op, 0) );

   return NoError;
 }

long ParserClass::statement (void)
 {
   size_t location, location2, lcv, downto, upto;
   OpCode temp;
   bool bit = false;

   try
    {
      switch (Internal())
       {
         case Tokens::Colon: //Label definition
            GNT();

            location = identifier();

            if (src->dest->getType(location) == Undefined_t)
             {
               src->dest->setType(location, Label_t);
               src->dest->setValue(location, (long long) dest->nextOp());
             }
            else if (src->dest->getType(location) == Label_t)
             {
               if (src->dest->getValue(location) == UsedLabel)
                {
                  src->dest->setValue(location, (long long) dest->nextOp());
                }
               else
                {
                  std::cerr << "Redefinition of label \"" <<
                     src->dest->getName(location) << "\"" << " at " <<
                     src->LineNo() << std::endl;
                  error = true;
                  errorC++;
                }
             }
            else
             {
               std::cerr << "Variable \"" << src->dest->getName(location) <<
                  "\" used as label at " << src->LineNo() << std::endl;
               error = true;
               errorC++;
             }

            statement();

            break;

         case Tokens::Identifier: //Assignment Statement
            temp.arg = variable();

            if ((Internal() != Tokens::Assign) &&
                (Internal() != Tokens::Equals))
             {
               expectationError("\":=\", \"=\", or \"<-\"");
               errorC++;
               throw ParseError ();
             }
            GNT();

            boolean();

            temp.opcode = Store_Op;
            dest->addOp(temp);

            break;

         case Tokens::Begin:
            GNT();

            while (Internal() != Tokens::End)
             {
               statement();

               try { expect(Tokens::Period); }
               catch (ParseError)
                {
                  expectationError("\".\"");
                }
             }

            GNT();

            break;

         case Tokens::If:
            GNT();

            boolean();
             //Save the quad that we need to backfill.
            location = dest->nextOp();

             //Produce the jump.
            temp.opcode = BranchConditional_Op;
            dest->addOp(temp);

            statement();

            if (Internal() == Tokens::Else)
             {
               GNT();

                //Backfill the previous jump
               dest->setArg(location, dest->nextOp() + 1);

                //Save the location that we need to backfill
               location = dest->nextOp();

                //Produce an unconditional jump
               temp.opcode = BranchUnconditional_Op;
               dest->addOp(temp);

               statement();
             }

             //Backfill the jump
            dest->setArg(location, dest->nextOp());

            break;

         case Tokens::Until:
            bit = true;

         case Tokens::While:
            GNT();

            temp.arg = dest->nextOp();

            boolean();

            if (bit)
             {
               temp.opcode = LogicalNot_Op;
               dest->addOp(temp);
             }

             //Save the location of the forward jump out of the loop.
            location = dest->nextOp();

             //Produce the jump out of the loop.
            temp.opcode = BranchConditional_Op;
            dest->addOp(temp);

            statement();

             //Produce the jump to the top of the loop.
             //Arg is correct from before.
            temp.opcode = BranchUnconditional_Op;
            dest->addOp(temp);

             //Backfill the jump out of the loop.
            dest->setArg(location, dest->nextOp());

            break;

         case Tokens::Repeat:
            GNT();

             //We jump up to location.
            temp.arg = dest->nextOp();

            statement();

             //while or until is OPTIONAL
            if ((Internal() != Tokens::While) &&
                (Internal() != Tokens::Until))
             {
                //Produce the jump to the top of the loop.
               temp.opcode = BranchUnconditional_Op;
               dest->addOp(temp);
             }
            else
             {
               if (Internal() == Tokens::While) bit = true;
               GNT();

               boolean();

               if (bit)
                {
                  temp.opcode = LogicalNot_Op;
                  dest->addOp(temp);
                }

                //Produce the jump to the top of the loop.
               temp.opcode = BranchConditional_Op;
               dest->addOp(temp);
             }

            break;

         case Tokens::For:
            GNT();

             // lcv - loop control variable
            lcv = variable();

            if ((Internal() != Tokens::Assign) &&
                (Internal() != Tokens::Equals))
             {
               expectationError("\":=\", \"=\", or \"<-\"");
               errorC++;
               throw ParseError ();
             }
            GNT();

            boolean();

            temp.arg = lcv;
            temp.opcode = Store_Op;
            dest->addOp(temp);

            if ((Internal() != Tokens::To) &&
                (Internal() != Tokens::DownTo))
             {
               expectationError("\"to\" or \"downto\"");
               errorC++;
               throw ParseError ();
             }

            if (Internal() == Tokens::DownTo) bit = true;
            GNT();

             /*
               Loop semantics:
                  Loop controls are re-evaluated each iteration.
             */

             // Save the top of the loop.
            location = dest->nextOp();

            boolean();

             // Jump over the step.
            downto = dest->nextOp();

            temp.opcode = BranchUnconditional_Op;
            dest->addOp(temp);

             // The top of the step.
            upto = dest->nextOp();

             //Push the increment to the stack.
            if (Internal() == Tokens::Step)
             {
               GNT();

               boolean();
             }
            else
             {
               temp.opcode = Load_Op;
               temp.arg = SymOne;
               dest->addOp(temp);
             }

             // Backfill the jump over and set the new jump location.
            dest->setArg(downto, dest->nextOp() + 1);
            downto = dest->nextOp();

            temp.opcode = BranchUnconditional_Op;
            dest->addOp(temp);

            temp.arg = lcv;
            temp.opcode = Load_Op;
            dest->addOp(temp);

             // Do comparison and branch
            if (bit) temp.opcode = TestLessThanOrEqual_Op;
            else temp.opcode = TestGreaterThanOrEqual_Op;

            dest->addOp(temp);

            location2 = dest->nextOp();

            temp.opcode = BranchConditional_Op;
            dest->addOp(temp);

            statement();

            temp.arg = lcv;
            temp.opcode = Load_Op;
            dest->addOp(temp);

             // Get the step.
            temp.arg = upto;
            temp.opcode = BranchUnconditional_Op;
            dest->addOp(temp);

             // Jump to the right place.
            dest->setArg(downto, dest->nextOp());

            if (bit) temp.opcode = Subtract_Op;
            else temp.opcode = Add_Op;
            dest->addOp(temp);

            temp.arg = lcv;
            temp.opcode = Store_Op;
            dest->addOp(temp);

            temp.arg = location;
            temp.opcode = BranchUnconditional_Op;
            dest->addOp(temp);

             // Backfill the jump.
            dest->setArg(location2, dest->nextOp());

            break;

         case Tokens::Call:
            bit = true;

         case Tokens::Goto:
            GNT();

            location = identifier();

            if (src->dest->getType(location) == Undefined_t)
             {
               src->dest->setType(location, Label_t);
               src->dest->setValue(location, UsedLabel);
             }
            else if (src->dest->getType(location) == Variable_t)
             {
               std::cerr << "Variable \"" << src->dest->getName(location) <<
                  "\" used as label at " << src->LineNo() << std::endl;
               error = true;
               errorC++;
             }

            temp.arg = location;
            if (bit) temp.opcode = CallIndirect_Op;
            else temp.opcode = BranchIndirect_Op;
            dest->addOp(temp);

             /*
               We track that a label is declared, used, but not defined
               so that we can throw an error later.
             */

            break;

         case Tokens::Return:
            GNT();

            temp.opcode = Return_Op;
            dest->addOp(temp);

            break;

         case Tokens::PrintS:
            bit = true;

         case Tokens::Print:
            GNT();

            boolean();

            if (!bit)
             {
               if (Internal() == Tokens::Comma)
                {
                  GNT();

                  boolean();
                }
               else
                {
                  temp.arg = SymTen;
                  temp.opcode = Load_Op;
                  dest->addOp(temp);
                }
             }

            if (Internal() == Tokens::SemiColon)
             {
               GNT();

               temp.arg = 0;
             }
            else temp.arg = 1;

            if (bit) temp.opcode = PrintString_Op;
            else temp.opcode = Print_Op;
            dest->addOp(temp);

            break;

         case Tokens::PrintF:
            GNT();

            boolean();

            try { expect(Tokens::Comma); }
            catch (ParseError)
             {
               expectationError("\",\"");
             }

            boolean();

            if (Internal() == Tokens::Comma)
             {
               GNT();

               boolean();
             }
            else
             {
               temp.arg = SymStringZero;
               temp.opcode = Load_Op;
               dest->addOp(temp);
             }

            if (Internal() == Tokens::Comma)
             {
               GNT();

               boolean();
             }
            else
             {
               temp.arg = SymTen;
               temp.opcode = Load_Op;
               dest->addOp(temp);
             }

            if (Internal() == Tokens::SemiColon)
             {
               GNT();

               temp.arg = 0;
             }
            else temp.arg = 1;

            temp.opcode = PrintPadded_Op;
            dest->addOp(temp);

            break;

         case Tokens::ReadS:
            bit = true;

         case Tokens::Read:
            GNT();

            location = variable();

            if (!bit)
             {
               if (Internal() == Tokens::Comma)
                {
                  GNT();

                  boolean();
                }
               else
                {
                  temp.arg = SymTen;
                  temp.opcode = Load_Op;
                  dest->addOp(temp);
                }
             }

            temp.opcode = Read_Op;

            if (bit) temp.arg = 1;
            else temp.arg = 0;

            dest->addOp(temp);

            temp.arg = location;
            temp.opcode = Store_Op;
            dest->addOp(temp);

            break;

         default:
            expectationError("statement");
            errorC++;
            throw ParseError ();
       }
    }
   catch (ParseError)
    {
       /*
         This finds either the end of the current statement,
         or the begining of the next statement.
       */
      while ((Internal() != Tokens::Colon) &&
             (Internal() != Tokens::Identifier) &&
             (Internal() != Tokens::Begin) &&
             (Internal() != Tokens::If) &&
             (Internal() != Tokens::While) &&
             (Internal() != Tokens::Until) &&
             (Internal() != Tokens::Repeat) &&
             (Internal() != Tokens::For) &&
             (Internal() != Tokens::Goto) &&
             (Internal() != Tokens::Call) &&
             (Internal() != Tokens::Return) &&
             (Internal() != Tokens::Read) &&
             (Internal() != Tokens::ReadS) &&
             (Internal() != Tokens::Print) &&
             (Internal() != Tokens::PrintS) &&
             (Internal() != Tokens::PrintF) &&
             (Internal() != Tokens::End) &&
             (Internal() != Tokens::Period))
       {
         GNT();
       }
    }

   return NoError;
 }

long ParserClass::variable (void)
 {
   long location = identifier();

   if (src->dest->getType(location) == Undefined_t)
    {
      std::cerr << "Undeclared identifier \"" <<
         src->dest->getName(location) << "\" at " << src->LineNo() <<
         std::endl;
      error = true;
      errorC++;

       // Set it to Variable to quiet this error further.
      src->dest->setType(location, Variable_t);
    }
   else if (src->dest->getType(location) == Label_t)
    {
      std::cerr << "Label \"" << src->dest->getName(location) <<
         "\" used as variable at " << src->LineNo() << std::endl;
      error = true;
      errorC++;
    }

   return location;
 }

long ParserClass::boolean (void)
 {
   OpCode temp;

   clause();

   temp.opcode = LogicalOr_Op;

   while (Internal() == Tokens::LogOr)
    {
      GNT();

      clause();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::clause (void)
 {
   OpCode temp;

   predicate();

   temp.opcode = LogicalAnd_Op;

   while (Internal() == Tokens::LogAnd)
    {
      GNT();

      predicate();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::predicate (void)
 {
   OpCode temp;

   relation();

   while ((Internal() == Tokens::LEQ) ||
       (Internal() == Tokens::Equals) ||
       (Internal() == Tokens::NotEqual))
    {
      if (Internal() == Tokens::NotEqual) temp.opcode = TestNotEqual_Op;
      else temp.opcode = TestEqual_Op;
      GNT();

      relation();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::relation (void)
 {
   OpCode temp;

   _or_();

   while ((Internal() == Tokens::LessThan) ||
       (Internal() == Tokens::GreaterThan) ||
       (Internal() == Tokens::LessOrEqual) ||
       (Internal() == Tokens::GreaterOrEqual))
    {
      switch (Internal())
       {
         case Tokens::LessThan:
            temp.opcode = TestLessThan_Op;
            break;

         case Tokens::GreaterThan:
            temp.opcode = TestGreaterThan_Op;
            break;

         case Tokens::LessOrEqual:
            temp.opcode = TestLessThanOrEqual_Op;
            break;

         case Tokens::GreaterOrEqual:
            temp.opcode = TestGreaterThanOrEqual_Op;
            break;

         default:
             /*
               This case will never happen, but I want G++ to shut up.
             */
            break;
       }
      GNT();

      _or_();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::_or_ (void)
 {
   OpCode temp;

   _xor_();

   temp.opcode = Or_Op;

   while (Internal() == Tokens::BoolOr)
    {
      GNT();

      _xor_();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::_xor_ (void)
 {
   OpCode temp;

   _and_();

   temp.opcode = Xor_Op;

   while (Internal() == Tokens::BoolXor)
    {
      GNT();

      _and_();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::_and_ (void)
 {
   OpCode temp;

   shift();

   temp.opcode = And_Op;

   while (Internal() == Tokens::BoolAnd)
    {
      GNT();

      shift();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::shift (void)
 {
   OpCode temp;

   simple();

   while ((Internal() == Tokens::ShiftLeft) ||
      (Internal() == Tokens::ShiftRight))
    {
      if (Internal() == Tokens::ShiftLeft) temp.opcode = ShiftLeft_Op;
      else temp.opcode = ShiftRight_Op;
      GNT();

      simple();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::simple (void)
 {
   OpCode temp;

   term();

   while ((Internal() == Tokens::Plus) || (Internal() == Tokens::Minus) ||
          (Internal() == Tokens::StrCat))
    {
      if (Internal() == Tokens::Plus) temp.opcode = Add_Op;
      else if (Internal() == Tokens::Minus) temp.opcode = Subtract_Op;
      else temp.opcode = StrCat_Op;
      GNT();

      term();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::term (void)
 {
   OpCode temp;

   factor();

   while ((Internal() == Tokens::Multiply) ||
       (Internal() == Tokens::Divide) ||
       (Internal() == Tokens::Modulus) ||
       (Internal() == Tokens::StrictDivide) ||
       (Internal() == Tokens::StrictModulus))
    {
      switch (Internal())
       {
         case Tokens::Multiply:
            temp.opcode = Multiply_Op;
            break;

         case Tokens::Divide:
            temp.opcode = Divide_Op;
            break;

         case Tokens::Modulus:
            temp.opcode = Modulus_Op;
            break;

         case Tokens::StrictDivide:
            temp.opcode = StrictDivide_Op;
            break;

         case Tokens::StrictModulus:
            temp.opcode = StrictModulus_Op;
            break;

         default:
            break;
       }
      GNT();

      factor();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::factor (void)
 {
   OpCode temp;

   primary();

   if (Internal() == Tokens::Exponent)
    {
      GNT();

      factor();

      temp.opcode = Exponent_Op;
      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::primary (void)
 {
   OpCode temp;
   bool bit = false;

   switch (Internal())
    {
      case Tokens::Plus:
      case Tokens::Minus:
      case Tokens::BoolNot:
      case Tokens::LogNot:

         switch(Internal())
          {
            case Tokens::Plus:
               temp.opcode = AbsoluteValue_Op;
               break;

            case Tokens::Minus:
               temp.opcode = Negate_Op;
               break;

            case Tokens::BoolNot:
               temp.opcode = Not_Op;
               break;

            case Tokens::LogNot:
               temp.opcode = LogicalNot_Op;
               break;

            default:
               break;
          }
         GNT();

         primary();

         dest->addOp(temp);

         break;

      case Tokens::Identifier:
         temp.arg = variable();
         temp.opcode = Load_Op;
         dest->addOp(temp);
         break;

      case Tokens::INTEGER:
         temp.arg = constant();
         temp.opcode = Load_Op;
         dest->addOp(temp);
         break;

      case Tokens::OpenParen:
         GNT();

         boolean();

         try { expect(Tokens::CloseParen); }
         catch (ParseError)
          {
            expectationError("\")\"");
            throw;
          }

         break;

      case Tokens::Sqrt:
         GNT();

         try { expect(Tokens::OpenParen); }
         catch (ParseError)
          {
            expectationError("\"(\"");
            throw;
          }

         boolean();

         try { expect(Tokens::CloseParen); }
         catch (ParseError)
          {
            expectationError("\")\"");
            throw;
          }

         temp.opcode = SquareRoot_Op;
         dest->addOp(temp);

         break;

      case Tokens::Max:
         bit = true;

      case Tokens::Min:
         GNT();

         try { expect(Tokens::OpenParen); }
         catch (ParseError)
          {
            expectationError("\"(\"");
            throw;
          }

         boolean();

         try { expect(Tokens::Comma); }
         catch (ParseError)
          {
            expectationError("\",\"");
            throw;
          }

         boolean();

         try { expect(Tokens::CloseParen); }
         catch (ParseError)
          {
            expectationError("\")\"");
            throw;
          }

         if (bit) temp.opcode = Maximum_Op;
         else temp.opcode = Minimum_Op;
         dest->addOp(temp);

         break;

      default:
         expectationError("sign, identifier, constant, or \"(\"");
         errorC++;
         throw ParseError ();
    }

   return NoError;
 }

long ParserClass::constant (void)
 {
   long location = src->Location();

   try { expect(Tokens::INTEGER); }
   catch (ParseError)
    {
      expectationError("integer value");
      throw;
    }

   return location;
 }

 /*
   We don't check for undefined identifiers in <identifier>,
   because they are undefined in <block>, where they get defined.

   <variable> and the $GOTO part of <statement> are all that need to
   check for a valid identifier.
 */
long ParserClass::identifier (void)
 {
   long location = src->Location();

   try { expect(Tokens::Identifier); }
   catch (ParseError)
    {
      expectationError("identifier");
      throw;
    }

   return location;
 }
