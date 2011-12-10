/*
   This file is part of FENZERO4FLOAT.

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
#include <stdio.h>
#include <string.h>
#include "FENREAD.h"
#include "FENFAIL.h"

 /*
   <equation> := <expr> ";"
   <expr> := <term> {("+" | "-") <term>}*
   <term> := <factor> {("*" | "/" | "%") <factor>}*
   <factor> := <primary> [("^" | "**" | "E") <factor>]
   <primary> := ("+" | "-") <primary>
              | <number> | "e" | "pi" | "x"
              | [("abs" | "sin" | "cos" | "tan" | "asin" | "acos" | "atan" |
                  "log" | "ln" | "sqrt" | "exp" | "pow")] "(" <expr> ")"
 */

#define INBUFMAX 1024

int CurTok;
Float CurVal;
char tok [INBUFMAX];

void GNT (void)
{
   int i;

ReBoot:
   i = 0;

   tok[i] = fgetc(stdin);

   while ((tok[i] == ' ') || (tok[i] == '\t') || (tok[i] == '\n'))
      tok[i] = fgetc(stdin);

   switch (tok[i])
    {
      case '(':
      case ')':
      case '+':
      case '-':
      case '/':
      case '^':
      case 'E':
      case ';':
         i++;
         break;

      case '*':
         i++;
         tok[i] = fgetc(stdin);
         if (tok[i] == '*') i++;
         else ungetc(tok[i], stdin);
         break;

      default:
         if ((tok[i] >= '0') && (tok[i] <= '9'))
          {
            while ((tok[i] >= '0') && (tok[i] <= '9'))
               tok[++i] = fgetc(stdin);
            if (tok[i] == '.')
             {
               i++;
               tok[i] = fgetc(stdin);
               while ((tok[i] >= '0') && (tok[i] <= '9'))
                  tok[++i] = fgetc(stdin);
             }
            ungetc(tok[i], stdin);
          }
         else if (tok[i] == '.')
          {
            tok[i++] = '0';
            tok[i++] = '.';
            tok[i] = fgetc(stdin);
            while ((tok[i] >= '0') && (tok[i] <= '9'))
               tok[++i] = fgetc(stdin);
            ungetc(tok[i], stdin);
          }
         else if ((tok[i] >= 'a') && (tok[i] <= 'z'))
          {
            while ((tok[i] >= 'a') && (tok[i] <= 'z'))
               tok[++i] = fgetc(stdin);
            ungetc(tok[i], stdin);
          }
         else
          {
            printf("Parse Error: invalid character '%c'\n", tok[i]);
            goto ReBoot;
          }
    }

   tok[i] = '\0';

   if (!strcmp(tok, "abs")) CurTok = 256;
   else if (!strcmp(tok, "-")) CurTok = 257;
   else if (!strcmp(tok, "sin")) CurTok = 258;
   else if (!strcmp(tok, "cos")) CurTok = 259;
   else if (!strcmp(tok, "tan")) CurTok = 260;
   else if (!strcmp(tok, "asin")) CurTok = 261;
   else if (!strcmp(tok, "acos")) CurTok = 262;
   else if (!strcmp(tok, "atan")) CurTok = 263;
   else if (!strcmp(tok, "log")) CurTok = 264;
   else if (!strcmp(tok, "ln")) CurTok = 265;
   else if (!strcmp(tok, "sqrt")) CurTok = 266;
   else if (!strcmp(tok, "exp")) CurTok = 267;
   else if (!strcmp(tok, "pow")) CurTok = 268;

   else if (!strcmp(tok, "+")) CurTok = 512;
//   else if (!strcmp(tok, "-")) CurTok = 513; //Wot?
   else if (!strcmp(tok, "/")) CurTok = 514;
   else if (!strcmp(tok, "*")) CurTok = 516;
   else if (!strcmp(tok, "**")) CurTok = 517;
   else if (!strcmp(tok, "^")) CurTok = 517;
   else if (!strcmp(tok, "E")) CurTok = 518;

   else if (!strcmp(tok, "e")) CurTok = 1024;
   else if (!strcmp(tok, "pi")) CurTok = 1025;
   else if (!strcmp(tok, "x")) CurTok = 1026;

   else if (!strcmp(tok, "(")) CurTok = 2048;
   else if (!strcmp(tok, ")")) CurTok = 2049;
   else if (!strcmp(tok, ";")) CurTok = 2050;

   else
    {
      CurTok = 255;
      CurVal.fromString(tok);
    }

   return;
}

pair * expr (void);
pair * term (void);
pair * factor (void);
pair * primary (void);

pair * read_pair (void)
{
   pair * leaf;

   GNT();
   leaf = expr();

   if (CurTok != 2050)
      printf("Parse Warning: no final ';'\n");

   return leaf;
}

pair * expr (void)
{
   pair * leaf, * nleaf;

   leaf = term();

   while ((CurTok == 512) || (CurTok == 257))
    {
      nleaf = new pair;
      if (nleaf == NULL) parse_panic("Out of Memory");

      if (CurTok == 257) CurTok = 513;
      nleaf->token = CurTok;

      GNT();

      nleaf->arg1 = leaf;
      nleaf->arg2 = term();

      leaf = nleaf;
    }

   return leaf;
}

pair * term (void)
{
   pair * leaf, * nleaf;

   leaf = factor();

   while ((CurTok == 514) || (CurTok == 515) || (CurTok == 516))
    {
      nleaf = new pair;
      if (nleaf == NULL) parse_panic("Out of Memory");

      nleaf->token = CurTok;

      GNT();

      nleaf->arg1 = leaf;
      nleaf->arg2 = factor();

      leaf = nleaf;
    }

   return leaf;
}

pair * factor (void)
{
   pair * leaf, * nleaf;

   leaf = primary();

   if ((CurTok == 517) || (CurTok == 518))
    {
      nleaf = new pair;
      if (nleaf == NULL) parse_panic("Out of Memory");

      nleaf->token = CurTok;

      GNT();

      nleaf->arg1 = leaf;
      nleaf->arg2 = factor();

      leaf = nleaf;
    }

   return leaf;
}

pair * primary (void)
{
   pair * leaf;

   if (CurTok == 512)
    {
      GNT();

      return primary();
    }

   if (CurTok == 2048)
    {
      GNT();
      leaf = expr();

      if (CurTok != 2049)
         printf("Parse Warning: missing ')'\n");
      else
         GNT();

      return leaf;
    }

   leaf = new pair;
   if (leaf == NULL) parse_panic("Out of Memory");

   switch (CurTok)
    {
      case 257:
         leaf->token = CurTok;

         GNT();

         leaf->arg1 = primary();
         leaf->arg2 = NULL;
         break;

      case 256:
      case 258:
      case 259:
      case 260:
      case 261:
      case 262:
      case 263:
      case 264:
      case 265:
      case 266:
      case 267:
      case 268:
         leaf->token = CurTok;

         GNT();

         if (CurTok != 2048)
            printf("Parse Warning: missing '('");
         else
            GNT();

         leaf->arg1 = expr();
         leaf->arg2 = NULL;

         if (CurTok != 2049)
            printf("Parse Warning: missing ')'");
         else
            GNT();

         break;

      case 1024:
      case 1025:
      case 1026:
         leaf->token = CurTok;
         leaf->arg1 = NULL;
         leaf->arg2 = NULL;

         GNT();
         break;

      case 255:
         leaf->token = CurTok;
         leaf->value = CurVal;
         leaf->arg1 = NULL;
         leaf->arg2 = NULL;

         GNT();
         break;

      default:
         printf("Parse Error: expected primary expression, got \"%s\"\n",tok);
         parse_panic("Parse Failure");
    }

   return leaf;
}
