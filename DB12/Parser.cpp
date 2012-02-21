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
#include "Parser.hpp"
#include <iostream>


const int ParserClass::NoError = 0;
const int ParserClass::SymZero = 0;
const int ParserClass::SymOne = 1;


 /*
   This is the parser's entry point. It catches all exceptions thrown 
   (by the parsing code), and calls GetNextToken for the first time.

   It also now adds zero and one to the symbol table as the first entries.
   Negative one isn't needed.
 */
void ParserClass::Parse (LexerClass & tokenSrc, OpTable & codeDest)
 {
   error = false;
   errorC = 0;

   src = &tokenSrc;
   dest = &codeDest;

    // Push the first constants.
   LexerTemp = Integer(0LL);
   Constants.push_back(LexerTemp);
   src->dest->addSymbol(Symbol("0", Constant_t, 0));
   LexerTemp = Integer(1LL);
   Constants.push_back(LexerTemp);
   src->dest->addSymbol(Symbol("1", Constant_t, 1));
    // Push a dummy variable.
   Variables.push_back(Constants[0]);

   try
    {
      src->GetNextToken();
      GNT();

      program();
    }

   catch (ParseError) { }

    /*
      Now, resolve all indirect branches into direct branches.
    */
   for (size_t i = 0; i < dest->nextOp(); i++)
    {
      long location;
      OpCode temp;

      temp = dest->getOp(i);
      if (temp.opcode == CallIndirect_Op)
       {
         location = src->dest->getValue(temp.arg);
         if (location == LexerClass::NoLoc)
          {
            std::cerr << "Error: label \"" << src->dest->getName(temp.arg) <<
               "\" used but not defined." << std::endl;
            error = true;
            errorC++;
          }
         else
          {
            dest->setOp(i, Call_Op);
            dest->setArg(i, location);
          }
       }
      else if (temp.opcode == CallVIndirect_Op)
       {
         location = src->dest->getValue(temp.arg);
         if (location == LexerClass::NoLoc)
          {
            std::cerr << "Error: label \"" << src->dest->getName(temp.arg) <<
               "\" used but not defined." << std::endl;
            error = true;
            errorC++;
          }
         else
          {
            dest->setOp(i, CallV_Op);
            dest->setArg(i, location);
          }
       }
    }

#ifndef DEBUG
   src->dest->clear();
#endif

   std::cerr << "Total Errors: " << errorC << std::endl;
 }

 /*
   A wrapper that handles the LL(2) constructs and Invalid Input.
 */
void ParserClass::GNT (void)
 {
   Internal = src->Internal();
   NextSymbol = src->NextSymbol();
   Location = src->Location();
   LineNo = src->LineNo();

   src->GetNextToken();

   while (Internal == Tokens::INVALID)
    {
      std::cerr << "Line " << LineNo <<
         " invalid lexeme discarded: " << NextSymbol << std::endl;
      error = true;
      errorC++;

      Internal = src->Internal();
      NextSymbol = src->NextSymbol();
      Location = src->Location();
      LineNo = src->LineNo();

      src->GetNextToken();
    }

   switch (Internal)
    {
      case Tokens::And:
         if (src->Internal() == Tokens::Case)
          {
            Internal = Tokens::Choice;
            NextSymbol = NextSymbol + " " + src->NextSymbol();

            src->GetNextToken();

            if (src->Internal() == Tokens::Else)
             {
               Internal = Tokens::Default;
               NextSymbol = NextSymbol + " " + src->NextSymbol();

               src->GetNextToken();
             }
          }
         else if (src->Internal() == Tokens::Then)
          {
            Internal = Tokens::LogAnd;
            NextSymbol = NextSymbol + " " + src->NextSymbol();

            src->GetNextToken();
          }
         else
          {
            Internal = Tokens::AlgAnd;
          }
         break;

      case Tokens::Or:
         if (src->Internal() == Tokens::Else)
          {
            Internal = Tokens::LogOr;
            NextSymbol = NextSymbol + " " + src->NextSymbol();

            src->GetNextToken();
          }
         else
          {
            Internal = Tokens::AlgOr;
          }
         break;

      case Tokens::Case:
         if (src->Internal() == Tokens::Else)
          {
            Internal = Tokens::CElse;
            NextSymbol = NextSymbol + " " + src->NextSymbol();

            src->GetNextToken();
          }
         break;

      case Tokens::End:
         switch (src->Internal())
          {
            case Tokens::Select:
               Internal = Tokens::ESelect;
               NextSymbol = NextSymbol + " " + src->NextSymbol();

               src->GetNextToken();
               break;

            case Tokens::If:
               Internal = Tokens::Fi;
               NextSymbol = NextSymbol + " " + src->NextSymbol();

               src->GetNextToken();
               break;

            case Tokens::While:
               Internal = Tokens::Wend;
               NextSymbol = NextSymbol + " " + src->NextSymbol();

               src->GetNextToken();
               break;

            case Tokens::Until:
               Internal = Tokens::Repeat;
               NextSymbol = NextSymbol + " " + src->NextSymbol();

               src->GetNextToken();
               break;

            case Tokens::For:
               Internal = Tokens::Next;
               NextSymbol = NextSymbol + " " + src->NextSymbol();

               src->GetNextToken();
               break;

            default:
                /*
                  STFU Compiler!
                */
               break;
          }
         break;

      default:
          /*
            Seriously, STFU!
          */
         break;
    }
 }

 /*
   A commonly occurring sequence of code related to expecting a certain token.
 */
void ParserClass::expect (Tokens::TokenType me)
 {
   if (me != Internal)
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
   std::cerr << "Line " << LineNo << " expected: " << s <<
      " found \"" << NextSymbol << "\"" << std::endl;
 }

long ParserClass::program (void)
 {
   long location;
   vector<BackRef> base;

   while (Internal == Tokens::TEOL)
    { // Eat leading newlines.
      GNT();
    }

   while (Internal == Tokens::Dim)
    {
      GNT();

      location = identifier();

      src->dest->setType(location, Variable_t);
      src->dest->setValue(location, Variables.size());
      Variables.push_back(Constants[0]);

      while ((Internal == Tokens::Comma) ||
             (Internal == Tokens::Identifier))
       {
         if (Internal == Tokens::Comma) GNT();
         else
          {
            expectationError("\",\"");
            errorC++;
          }

         location = identifier();

         if (src->dest->getType(location) == Undefined_t)
          {
            src->dest->setType(location, Variable_t);
            src->dest->setValue(location, Variables.size());
            Variables.push_back(Constants[0]);
          }
         else
          {
            std::cerr << "Reuse of identifier \"" <<
               src->dest->getName(location) << "\"" << std::endl;
            error = true;
            errorC++;
          }
       }

      try { expect(Tokens::TEOL); }
      catch (ParseError)
       {
         expectationError("NewLine");
       }

      while (Internal == Tokens::TEOL)
       { // Eat superfluous newlines.
         GNT();
       }

    }

   statement_seq(base);

   if (Internal != Tokens::TEOF)
    {
      error = true;
      while (Internal != Tokens::TEOF)
       {
          /* Prevent a dumb infinite loop. */
         if ((Internal == Tokens::Fi) || (Internal == Tokens::Wend) ||
             (Internal == Tokens::Repeat) || (Internal == Tokens::ESelect) ||
             (Internal == Tokens::Else) || (Internal == Tokens::Loop) ||
             (Internal == Tokens::Next) || (Internal == Tokens::Case) ||
             (Internal == Tokens::Choice) || (Internal == Tokens::CElse) ||
             (Internal == Tokens::Default)) GNT();
         statement_seq(base);
       }
    }

   if (base.size() != 0)
    {
      error = true;
      errorC += base.size();

      for (size_t i = 0; i < base.size(); i++)
       {
         if (base[i].JumpLabel)
            std::cerr << "Line " << base[i].LineNo << " : " <<
               (base[i].isBreak ? "break" : "continue") <<
               " to label " << src->dest->getName(base[i].JumpLabel) <<
               " invalid." << std::endl;
         else
            std::cerr << "Line " << base[i].LineNo << " : " <<
               (base[i].isBreak ? "break" : "continue") <<
               " outside of loop." << std::endl;
       }
    }

    /*
      Well, the program is over, so here should be the best place to put this.
    */
   dest->addOp( OpCode(End_Op, 0) );

   return NoError;
 }

long ParserClass::statement_seq (vector<BackRef> & fill)
 {
   while ((Internal != Tokens::Fi) && (Internal != Tokens::Wend) &&
          (Internal != Tokens::Repeat) && (Internal != Tokens::ESelect) &&
          (Internal != Tokens::Else) && (Internal != Tokens::Loop) &&
          (Internal != Tokens::Next) && (Internal != Tokens::Case) &&
          (Internal != Tokens::Choice) && (Internal != Tokens::CElse) &&
          (Internal != Tokens::Default) && (Internal != Tokens::TEOF))
    {
      statement(fill);

      try { expect(Tokens::TEOL); }
      catch (ParseError)
       {
         expectationError("NewLine");
       }
    }

   return NoError;
 }

long ParserClass::statement (vector<BackRef> & fill)
 {
   long location, location2, lcv, downto, upto, tiptop;
   OpCode temp;
   bool bit = false;
   vector<BackRef> pass;
   size_t i;

   try
    {
      switch (Internal)
       {
         case Tokens::Colon: //Label definition
            GNT();

            location = identifier();

            if (location < 0)
             {
               std::cerr << "Auto variable used as label at " <<
                  LineNo << std::endl;
               error = true;
               errorC++;
             }
            else if (src->dest->getType(location) == Undefined_t)
             {
               src->dest->setType(location, Label_t);
               src->dest->setValue(location, dest->nextOp());
             }
            else if (src->dest->getType(location) == Label_t)
             {
               if (src->dest->getValue(location) == LexerClass::NoLoc)
                {
                  src->dest->setValue(location, dest->nextOp());
                }
               else
                {
                  std::cerr << "Redefinition of label \"" <<
                     src->dest->getName(location) << "\"" << " at " <<
                     LineNo << std::endl;
                  error = true;
                  errorC++;
                }
             }
            else
             {
               std::cerr << "Variable \"" << src->dest->getName(location) <<
                  "\" used as label at " << LineNo << std::endl;
               error = true;
               errorC++;
             }

//            statement(fill);

            try { expect(Tokens::Colon); }
            catch (ParseError)
             {
               expectationError("\":\"");
               throw;
             }

            break;

         case Tokens::Identifier: //Assignment Statement
            location = variable();
            if (location < 0)
               temp.arg = location;
            else
               temp.arg = src->dest->getValue(location);

            if (Internal == Tokens::OpenBrack)
             {
               GNT();

               expression();

               try { expect(Tokens::CloseBrack); }
               catch (ParseError)
                {
                  expectationError("\"]\"");
                }

               temp.opcode = StoreIndirect_Op;
             }
            else
               temp.opcode = Store_Op;

            if ((Internal != Tokens::Assign) &&
                (Internal != Tokens::Equals))
             {
               expectationError("\":=\", \"=\", or \"<-\"");
               errorC++;
               throw ParseError ();
             }
            GNT();

            expression();

            dest->addOp(temp);

            break;

         case Tokens::If:
            GNT();

            expression();
             //Save the quad that we need to backfill.
            location = dest->nextOp();

             //Produce the jump.
            temp.opcode = BranchConditional_Op;
            dest->addOp(temp);

            try { expect(Tokens::Then); }
            catch (ParseError)
             {
               expectationError("then");
             }

            if (Internal != Tokens::TEOL)
             {
                /*
                  Path 1: if <exppression> then <statement> [else <statement>]
                */
               statement(fill);

               if (Internal == Tokens::Else)
                {
                  GNT();

                   //Backfill the previous jump
                  dest->setArg(location, dest->nextOp() + 1);

                   //Save the location that we need to backfill
                  location = dest->nextOp();

                   //Produce an unconditional jump
                  temp.opcode = BranchUnconditional_Op;
                  dest->addOp(temp);

                  statement(fill);
                }
             }
            else
             {
                /*
                  Path 2: if <expression> then
                             <statement_seq>
                          [else
                             <statement_seq>]
                          fi
                */
               GNT();

               statement_seq(fill);

               if (Internal == Tokens::Else)
                {
                  GNT();

                   //Backfill the previous jump
                  dest->setArg(location, dest->nextOp() + 1);

                   //Save the location that we need to backfill
                  location = dest->nextOp();

                   //Produce an unconditional jump
                  temp.opcode = BranchUnconditional_Op;
                  dest->addOp(temp);

                   /*
                     Path 3: if <expression> then
                                <statement_seq>
                             {else if <expression> then
                                <statement_seq>}
                             [else
                                <statement_seq>]
                             fi
                   */
                  if (Internal == Tokens::If)
                   {
                     statement(fill);
                     bit = true;
                   }
                  else
                   {
                     try { expect(Tokens::TEOL); }
                     catch (ParseError)
                      {
                        expectationError("NewLine");
                      }

                     statement_seq(fill);
                   }
                }

               if (!bit)
                {
                  try { expect(Tokens::Fi); }
                  catch (ParseError)
                   {
                     expectationError("fi OR end if");
                   }
                }
             }

             //Backfill the jump
            dest->setArg(location, dest->nextOp());

            break;

         case Tokens::Until:
            bit = true;

         case Tokens::While:
            GNT();

            temp.arg = dest->nextOp();

            expression();

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

            try { expect(Tokens::TEOL); }
            catch (ParseError)
             {
               expectationError("NewLine");
             }

            statement_seq(pass);

             //Produce the jump to the top of the loop.
             //Arg is correct from before.
            temp.opcode = BranchUnconditional_Op;
            dest->addOp(temp);

             //Backfill the jump out of the loop.
            dest->setArg(location, dest->nextOp());

             //Backfill the breaks and continues
            for (i = 0; i < pass.size(); i++)
             {
               if ((pass[i].JumpLabel == 0) ||
                   (src->dest->getValue(pass[i].JumpLabel) == (long)temp.arg))
                {
                  if (pass[i].isBreak)
                     dest->setArg(pass[i].CodeLoc, dest->nextOp());
                  else
                     dest->setArg(pass[i].CodeLoc, temp.arg);
                }
               else //Was to an outer loop?
                {
                  fill.push_back(pass[i]);
                }
             }

            if (bit)
             {
               try { expect(Tokens::Repeat); }
               catch (ParseError)
                {
                  expectationError("repeat OR end until");
                }
             }
            else
             {
               try { expect(Tokens::Wend); }
               catch (ParseError)
                {
                  expectationError("wend OR end while");
                }
             }

            break;

         case Tokens::Do:
            GNT();

            try { expect(Tokens::TEOL); }
            catch (ParseError)
             {
               expectationError("NewLine");
             }

             //We jump up to location.
            temp.arg = dest->nextOp();

            statement_seq(pass);

            try { expect(Tokens::Loop); }
            catch (ParseError)
             {
               expectationError("loop");
             }

             //while or until is OPTIONAL
            if ((Internal != Tokens::While) &&
                (Internal != Tokens::Until))
             {
                //Produce the jump to the top of the loop.
               temp.opcode = BranchUnconditional_Op;
               dest->addOp(temp);

               location = temp.arg;
             }
            else
             {
               if (Internal == Tokens::While) bit = true;
               GNT();

                //Unlike previous DUMBBASICs,
                //this one will jump to the condition of the loop.
               location = dest->nextOp();

               expression();

               if (bit)
                {
                  temp.opcode = LogicalNot_Op;
                  dest->addOp(temp);
                }

                //Produce the jump to the top of the loop.
               temp.opcode = BranchConditional_Op;
               dest->addOp(temp);
             }

             //Backfill the breaks and continues
            for (i = 0; i < pass.size(); i++)
             {
               if ((pass[i].JumpLabel == 0) ||
                   (src->dest->getValue(pass[i].JumpLabel) == (long)temp.arg))
                {
                  if (pass[i].isBreak)
                     dest->setArg(pass[i].CodeLoc, dest->nextOp());
                  else
                     dest->setArg(pass[i].CodeLoc, location);
                }
               else //Was to an outer loop?
                {
                  fill.push_back(pass[i]);
                }
             }

            break;

         case Tokens::Break:
            bit = true;

         case Tokens::Continue:
            GNT();

            if (Internal == Tokens::Identifier)
             {
               location = label();

               if (src->dest->getValue(location) == LexerClass::NoLoc)
                {
                  std::cerr << "Line " << LineNo << " : " <<
                     (bit ? "break" : "continue") <<
                     " to label " << src->dest->getName(location) <<
                     " invalid." << std::endl;
                  error = true;
                  errorC++;
                }
             }
            else
               location = 0;

            temp.opcode = BranchUnconditional_Op;
            fill.push_back(BackRef(dest->nextOp(), bit, location, LineNo));
            dest->addOp(temp);

            break;

         case Tokens::For:
            GNT();

            tiptop = dest->nextOp();

             // lcv - loop control variable
            lcv = variable();
            if (lcv >= 0) lcv = src->dest->getValue(lcv);

            if ((Internal != Tokens::Assign) &&
                (Internal != Tokens::Equals))
             {
               expectationError("\":=\", \"=\", or \"<-\"");
               errorC++;
               throw ParseError ();
             }
            GNT();

            expression();

            temp.arg = lcv;
            temp.opcode = Store_Op;
            dest->addOp(temp);

            if ((Internal != Tokens::To) &&
                (Internal != Tokens::DownTo))
             {
               expectationError("\"to\" or \"downto\"");
               errorC++;
               throw ParseError ();
             }

            if (Internal == Tokens::DownTo) bit = true;
            GNT();

             /*
               Loop semantics:
                  Loop controls are re-evaluated each iteration.
             */

             // Save the top of the loop.
            location = dest->nextOp();

            expression();

             // Jump over the step.
            downto = dest->nextOp();

            temp.opcode = BranchUnconditional_Op;
            dest->addOp(temp);

             // The top of the step.
            upto = dest->nextOp();

             //Push the increment to the stack.
            if (Internal == Tokens::Step)
             {
               GNT();

               expression();
             }
            else
             {
               temp.opcode = LoadConstant_Op;
               temp.arg = SymOne;
               dest->addOp(temp);
             }

             // Backfill the jump over and set the new jump location.
            dest->setArg(downto, dest->nextOp() + 1);
            downto = dest->nextOp();

            temp.opcode = BranchUnconditional_Op;
            dest->addOp(temp);

            temp.arg = lcv;
            temp.opcode = LoadVariable_Op;
            dest->addOp(temp);

             // Do comparison and branch
            if (bit) temp.opcode = TestLessThanOrEqual_Op;
            else temp.opcode = TestGreaterThanOrEqual_Op;

            dest->addOp(temp);

            location2 = dest->nextOp();

            temp.opcode = BranchConditional_Op;
            dest->addOp(temp);

            try { expect(Tokens::TEOL); }
            catch (ParseError)
             {
               expectationError("NewLine");
             }

            statement_seq(pass);

             //Backfill the continues
            for (i = 0; i < pass.size(); i++)
             {
               if ((pass[i].JumpLabel == 0) ||
                   (src->dest->getValue(pass[i].JumpLabel) == tiptop))
                {
                  if (!pass[i].isBreak)
                     dest->setArg(pass[i].CodeLoc, dest->nextOp());
                }
             }

            temp.arg = lcv;
            temp.opcode = LoadVariable_Op;
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

             //Backfill the breaks
            for (i = 0; i < pass.size(); i++)
             {
               if ((pass[i].JumpLabel == 0) ||
                   (src->dest->getValue(pass[i].JumpLabel) == tiptop))
                {
                  if (pass[i].isBreak)
                     dest->setArg(pass[i].CodeLoc, dest->nextOp());
                }
               else //Was to an outer loop?
                {
                  fill.push_back(pass[i]);
                }
             }

            try { expect(Tokens::Next); }
            catch (ParseError)
             {
               expectationError("next OR end for");
             }

            break;

         case Tokens::Call:
            bit = true;

         case Tokens::Return:
            GNT();

            if (bit)
             {
               location = identifier();

               if (src->dest->getType(location) == Undefined_t)
                {
                  src->dest->setType(location, Label_t);
                }
               else if (src->dest->getType(location) == Variable_t)
                {
                  std::cerr << "Variable \"" <<
                     src->dest->getName(location) << "\" used as label at " <<
                     LineNo << std::endl;
                  error = true;
                  errorC++;
                }

               temp.arg = location;
             }
            else
 /*
   We don't kneed this line, but the code analyzer for GCC 4.1.1 (or 4.1.2?)
   can't tell that bit is invariant and that location will only be used when
   it is set, and therefore location is initialized.

   Yes, this is to quell an incorrect warning.
 */
               location = 0;

            if (Internal != Tokens::TEOL)
             {
               expression();

               try { expect(Tokens::Comma); }
               catch (ParseError)
                {
                  expectationError("\",\"");
                  throw;
                }

               expression();

               try { expect(Tokens::Comma); }
               catch (ParseError)
                {
                  expectationError("\",\"");
                  throw;
                }

               expression();

               try { expect(Tokens::Comma); }
               catch (ParseError)
                {
                  expectationError("\",\"");
                  throw;
                }

               expression();

               if (bit)
                {
                  if (src->dest->getValue(location) != LexerClass::NoLoc)
                   {
                     temp.arg = src->dest->getValue(location);
                     temp.opcode = CallV_Op;
                   }
                  else
                     temp.opcode = CallVIndirect_Op;
                }
               else
                {
                  temp.arg = 1;
                  temp.opcode = Return_Op;
                }
             }
            else
             {
               if (bit)
                {
                  if (src->dest->getValue(location) != LexerClass::NoLoc)
                   {
                     temp.arg = src->dest->getValue(location);
                     temp.opcode = Call_Op;
                   }
                  else
                     temp.opcode = CallIndirect_Op;
                }
               else
                {
                  temp.arg = 0;
                  temp.opcode = Return_Op;
                }
             }

            dest->addOp(temp);

            break;

         case Tokens::Print:
            bit = true;

         case Tokens::Write:
            GNT();

            expression();

            temp.opcode = Print_Op;

            if (bit) temp.arg = 1;
            else temp.arg = 0;

            dest->addOp(temp);

            break;

         case Tokens::Input:
            bit = true;

         case Tokens::Read:
            GNT();

            location = variable();

            if (Internal == Tokens::OpenBrack)
             {
               GNT();

               expression();

               try { expect(Tokens::CloseBrack); }
               catch (ParseError)
                {
                  expectationError("\"]\"");
                }

               lcv = 1;
             }
            else
               lcv = 0;

            temp.opcode = Read_Op;

            if (bit) temp.arg = 1;
            else temp.arg = 0;

            dest->addOp(temp);

            if (location < 0)
               temp.arg = location;
            else
               temp.arg = src->dest->getValue(location);

            if (lcv == 0) temp.opcode = Store_Op;
            else temp.opcode = StoreIndirect_Op;

            dest->addOp(temp);

            break;

         case Tokens::End:
            GNT();

            temp.opcode = End_Op;
            dest->addOp(temp);

            break;

          /*
            And now for something new and different.
            I was originally going to have the interpreter assist in handling
            this, but I will just sacrifice having Select be a looping
            construct. Break will not work in it. Use:
            label: do
               select ...
                  break label
               end select
            loop while 0
          */
         case Tokens::Select:
            GNT();

             /*
               Generate the value to test.
             */
            expression();

            try { expect(Tokens::TEOL); }
            catch (ParseError)
             {
               expectationError("NewLine");
             }

             /*
               Jump around the generated code.
             */
            location = dest->nextOp();

            temp.opcode = BranchUnconditional_Op;

            dest->addOp(temp);

             /*
               Eat any garbage before our first CASE.
             */
            if ((Internal != Tokens::Case) && (Internal != Tokens::Choice) &&
               (Internal != Tokens::CElse) && (Internal != Tokens::Default) &&
                (Internal != Tokens::ESelect))
             {
               expectationError("case, and case, "
                  "case else, OR, and case else");
               error = true;
               errorC++;

               while ((Internal != Tokens::Case) &&
                      (Internal != Tokens::Choice) &&
                      (Internal != Tokens::CElse) &&
                      (Internal != Tokens::Default) &&
                      (Internal != Tokens::ESelect) &&
                      (Internal != Tokens::TEOF))
                {
                  GNT();
                }
             }

             /*
               Generate the code for each case.
               Save the locations and constant values for later.
             */
            while ((Internal == Tokens::Case) || (Internal == Tokens::Choice))
             {
               if (Internal == Tokens::Case) bit = true;
               else bit = false;
               GNT();

               lcv = src->dest->getValue(constant());

               if (bit)
                {
                  pass.push_back(BackRef(dest->nextOp(), true, lcv, LineNo));
                  temp.opcode = BranchUnconditional_Op;
                  dest->addOp(temp);
                }
               else
                {
                  pass.push_back(BackRef(dest->nextOp() + 1, false,
                     lcv, LineNo));
                  temp.opcode = BranchUnconditional_Op;
                  temp.arg = dest->nextOp() + 2;
                  dest->addOp(temp);
                }

               temp.opcode = Pop_Op;
               dest->addOp(temp);

               try { expect(Tokens::TEOL); }
               catch (ParseError)
                {
                  expectationError("NewLine");
                }

               statement_seq(fill);
             }

             /*
               There can only be one final case, and it must be at the end.
             */
            if ((Internal == Tokens::CElse) || (Internal == Tokens::Default))
             {
               if (Internal == Tokens::CElse)
                {
                  pass.push_back(BackRef(dest->nextOp(), true,
                     LexerClass::NoLoc, LineNo));
                  temp.opcode = BranchUnconditional_Op;
                  dest->addOp(temp);
                }
               else
                  pass.push_back(BackRef(dest->nextOp(), false,
                     LexerClass::NoLoc, LineNo));

               GNT();

               temp.opcode = Pop_Op;
               dest->addOp(temp);

               try { expect(Tokens::TEOL); }
               catch (ParseError)
                {
                  expectationError("NewLine");
                }

               statement_seq(fill);
             }

             /*
               Jump around the decision code.
             */
            location2 = dest->nextOp();

            temp.opcode = BranchUnconditional_Op;
            dest->addOp(temp);

             /*
               Fill the jump to the start of our decision code.
             */
            dest->setArg(location, dest->nextOp());

             /*
               For each case:
             */
            for (i = 0; i < (pass.size() - 1); i++)
             {
                /*
                  We need to copy the value, always.
                */
               temp.opcode = Copy_Op;
               dest->addOp(temp);

                /*
                  Load the comparator.
                */
               temp.opcode = LoadConstant_Op;
               temp.arg = pass[i].JumpLabel;
               dest->addOp(temp);

                /*
                  Test if NOT equal, as conditional branch branches on FALSE.
                */
               temp.opcode = TestNotEqual_Op;
               dest->addOp(temp);

                /*
                  Generate the branch.
                  If the case was breaking, mind the branch.
                */
               temp.opcode = BranchConditional_Op;
               temp.arg = pass[i].CodeLoc;
               if (pass[i].isBreak) temp.arg++;
               dest->addOp(temp);
             }

             /*
               Treat the last case specially.
             */
            if ((pass.size() != 0) &&
                ((long) pass[i].JumpLabel != LexerClass::NoLoc))
             { //Normal last case
                /*
                  We STILL need to copy the value.
                */
               temp.opcode = Copy_Op;
               dest->addOp(temp);

               temp.opcode = LoadConstant_Op;
               temp.arg = pass[i].JumpLabel;
               dest->addOp(temp);

               temp.opcode = TestNotEqual_Op;
               dest->addOp(temp);

               temp.opcode = BranchConditional_Op;
               temp.arg = pass[i].CodeLoc;
               if (pass[i].isBreak) temp.arg++;
               dest->addOp(temp);

                /*
                  We finally remove the value from the stack.
                  This code has control pass-by if there
                  are no matching cases and is no default case.
                */
               temp.opcode = Pop_Op;
               dest->addOp(temp);
             }
            else if (pass.size() != 0)
             { //Default case
               temp.opcode = BranchUnconditional_Op;
               temp.arg = pass[i].CodeLoc;
               if (pass[i].isBreak) temp.arg++;
               dest->addOp(temp);
             }

             /*
               Fill the branch out
             */
            dest->setArg(location2, dest->nextOp());

             /*
               Fill the internal branches.
             */
            for (i = 0; i < pass.size(); i++)
             {
               if (pass[i].isBreak)
                  dest->setArg(pass[i].CodeLoc, dest->nextOp());
             }

             /*
               Eat the end select
             */
            try { expect(Tokens::ESelect); }
            catch (ParseError)
             {
               expectationError("end select");
             }

            break;

         case Tokens::ReDim:
            GNT();

            location = variable();
            if (location < 0)
               temp.arg = location;
            else
               temp.arg = src->dest->getValue(location);

            try { expect(Tokens::Comma); }
            catch (ParseError)
             {
               expectationError("\",\"");
               throw;
             }

            expression();

            temp.opcode = ReDim_Op;
            dest->addOp(temp);

            break;

         case Tokens::Else:
         case Tokens::TEOL:
         case Tokens::TEOF:
             /*
               An empty statement is fine, too.
             */
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
         DUMBBASIC does have one nice attribute:
            The follow set for <statement> is Tokens::Else and Tokens::TEOL.
       */
      while ((Internal != Tokens::Else) && (Internal != Tokens::TEOL) &&
             (Internal != Tokens::TEOF))
       {
         GNT();
       }
    }

   return NoError;
 }

long ParserClass::label (void)
 {
   long location = identifier();

   if (location < 0)
    {
      std::cerr << "Auto variable used as label at " << LineNo << std::endl;
      error = true;
      errorC++;

      location = 0; //No explosions!
    }
   else if (src->dest->getType(location) == Undefined_t)
    {
      std::cerr << "Undeclared label \"" <<
         src->dest->getName(location) << "\" at " << LineNo <<
         std::endl;
      error = true;
      errorC++;

       // Set it to Label to quiet this error further.
      src->dest->setType(location, Label_t);
       // Set its Value to 0 to not break break/continue.
      src->dest->setValue(location, 0);
    }
   else if (src->dest->getType(location) == Variable_t)
    {
      std::cerr << "Variable \"" << src->dest->getName(location) <<
         "\" used as label at " << LineNo << std::endl;
      error = true;
      errorC++;
    }

   return location;
 }

long ParserClass::variable (void)
 {
   long location = identifier();

   if (location >= 0)
    {
      if (src->dest->getType(location) == Undefined_t)
       {
         std::cerr << "Undeclared identifier \"" <<
            src->dest->getName(location) << "\" at " << LineNo <<
            std::endl;
         error = true;
         errorC++;

          // Set it to Variable to quiet this error further.
         src->dest->setType(location, Variable_t);
          // Set to the dummy first variable.
         src->dest->setValue(location, 0);
       }
      else if (src->dest->getType(location) == Label_t)
       {
         std::cerr << "Label \"" << src->dest->getName(location) <<
            "\" used as variable at " << LineNo << std::endl;
         error = true;
         errorC++;
       }
    }

   return location;
 }

long ParserClass::expression (void)
 {
   OpCode temp;

   boolean();

   while ((Internal == Tokens::AlgAnd) || (Internal == Tokens::AlgOr))
    {
      if (Internal == Tokens::AlgAnd) temp.opcode = LogicalAnd_Op;
      else temp.opcode = LogicalOr_Op;
      GNT();

      boolean();

      dest->addOp(temp);
    }

   return NoError;
 }

long ParserClass::boolean (void)
 {
   OpCode temp;
   vector<size_t> back;

   clause();

   while (Internal == Tokens::LogOr)
    {
      GNT();

      temp.opcode = Copy_Op;
      dest->addOp(temp);

      temp.opcode = LogicalNot_Op;
      dest->addOp(temp);

      back.push_back(dest->nextOp());
      temp.opcode = BranchConditional_Op;
      dest->addOp(temp);

      clause();

      temp.opcode = LogicalOr_Op;
      dest->addOp(temp);
    }

   for (size_t i = 0; i < back.size(); i++)
      dest->setArg(back[i], dest->nextOp());

   return NoError;
 }

long ParserClass::clause (void)
 {
   OpCode temp;
   vector<size_t> back;

   predicate();

   while (Internal == Tokens::LogAnd)
    {
      GNT();

      temp.opcode = Copy_Op;
      dest->addOp(temp);

      back.push_back(dest->nextOp());
      temp.opcode = BranchConditional_Op;
      dest->addOp(temp);

      predicate();

      temp.opcode = LogicalAnd_Op;
      dest->addOp(temp);
    }

   for (size_t i = 0; i < back.size(); i++)
      dest->setArg(back[i], dest->nextOp());

   return NoError;
 }

long ParserClass::predicate (void)
 {
   OpCode temp;

   relation();

   while ((Internal == Tokens::LEQ) ||
       (Internal == Tokens::Equals) ||
       (Internal == Tokens::NotEqual))
    {
      if (Internal == Tokens::NotEqual) temp.opcode = TestNotEqual_Op;
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

   while ((Internal == Tokens::LessThan) ||
       (Internal == Tokens::GreaterThan) ||
       (Internal == Tokens::LessOrEqual) ||
       (Internal == Tokens::GreaterOrEqual))
    {
      switch (Internal)
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

   while (Internal == Tokens::BoolOr)
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

   while (Internal == Tokens::BoolXor)
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

   while ((Internal == Tokens::BoolAnd) || (Internal == Tokens::And))
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

   while ((Internal == Tokens::ShiftLeft) ||
      (Internal == Tokens::ShiftRight))
    {
      if (Internal == Tokens::ShiftLeft) temp.opcode = ShiftLeft_Op;
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

   while ((Internal == Tokens::Plus) || (Internal == Tokens::Minus) ||
          (Internal == Tokens::StrCat))
    {
      if (Internal == Tokens::Plus) temp.opcode = Add_Op;
      else if (Internal == Tokens::Minus) temp.opcode = Subtract_Op;
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

   while ((Internal == Tokens::Multiply) || (Internal == Tokens::Divide) ||
       (Internal == Tokens::Modulus) || (Internal == Tokens::StrictDivide) ||
       (Internal == Tokens::StrictModulus))
    {
      switch (Internal)
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

   if (Internal == Tokens::Exponent)
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

   switch (Internal)
    {
      case Tokens::Plus:
      case Tokens::Minus:
      case Tokens::BoolNot:
      case Tokens::LogNot:

         switch(Internal)
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
         if ((long) temp.arg >= 0) temp.arg = src->dest->getValue(temp.arg);

           //Subscripted?
         if (Internal == Tokens::OpenBrack)
          {
            GNT();

            expression();

            try { expect(Tokens::CloseBrack); }
            catch (ParseError)
             {
               expectationError("\"]\"");
             }

            temp.opcode = LoadIndirect_Op;
          }
         else
            temp.opcode = LoadVariable_Op;

         dest->addOp(temp);
         break;

      case Tokens::INTEGER:
         temp.arg = src->dest->getValue(constant());
         temp.opcode = LoadConstant_Op;
         dest->addOp(temp);
         break;

      case Tokens::OpenParen:
         GNT();

         expression();

         try { expect(Tokens::CloseParen); }
         catch (ParseError)
          {
            expectationError("\")\"");
          }

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
   long location = Location;

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
   because they are undefined in <program>, where they get defined.

   <variable>, and break and continue of <statement> are all that need to
   check for a valid identifier.
 */
long ParserClass::identifier (void)
 {
   long location = Location;

   try { expect(Tokens::Identifier); }
   catch (ParseError)
    {
      expectationError("identifier");
      throw;
    }

   return location;
 }
