/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_ASSEMBLER_MIPS_PARSER_HPP_INCLUDED
# define YY_YY_SRC_ASSEMBLER_MIPS_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    COMMA = 258,                   /* COMMA  */
    SYSCALL = 259,                 /* SYSCALL  */
    TEXT_SECTION = 260,            /* TEXT_SECTION  */
    DATA_SECTION = 261,            /* DATA_SECTION  */
    LAOPCODE = 262,                /* LAOPCODE  */
    LUIOPCODE = 263,               /* LUIOPCODE  */
    OBRACKET = 264,                /* OBRACKET  */
    CBRACKET = 265,                /* CBRACKET  */
    DATATAG = 266,                 /* DATATAG  */
    LABELTAG = 267,                /* LABELTAG  */
    LABELJUMP = 268,               /* LABELJUMP  */
    REGISTER = 269,                /* REGISTER  */
    FREGISTER = 270,               /* FREGISTER  */
    IOPCODE = 271,                 /* IOPCODE  */
    JOPCODE = 272,                 /* JOPCODE  */
    ROPCODE = 273,                 /* ROPCODE  */
    R1OPCODE = 274,                /* R1OPCODE  */
    R2OPCODE = 275,                /* R2OPCODE  */
    FOPCODE = 276,                 /* FOPCODE  */
    F2ROPCODE = 277,               /* F2ROPCODE  */
    FBROPCODE = 278,               /* FBROPCODE  */
    IMOPCODE = 279,                /* IMOPCODE  */
    FMOPCODE = 280,                /* FMOPCODE  */
    INTVALUE = 281,                /* INTVALUE  */
    FLOATVALUE = 282,              /* FLOATVALUE  */
    TEXTVALUE = 283,               /* TEXTVALUE  */
    EOL = 284                      /* EOL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 79 "src/assembler/mips_assembler.y"

  uint32_t ival;
  char * tval;

#line 98 "src/assembler/mips_parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SRC_ASSEMBLER_MIPS_PARSER_HPP_INCLUDED  */
