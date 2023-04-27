/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/assembler/mips_assembler.y"

  #include "../utils.h"
  #include "../mem.h"
  #include "../exception.h"
  #include "mips_assembler.h"

  #include <iostream>
  #include <sstream>
  #include <cstring>
  #include <cassert>
  #include <vector>
  #include <memory>
  #include <algorithm>

  #define MEM_TYPE_WORD   1
  #define MEM_TYPE_FLOAT  2
  #define MEM_TYPE_DOUBLE 3
  #define MEM_TYPE_STRING 4
  #define MEM_TYPE_SPACE  5

  using namespace std;
  using namespace mips_sim;

  int yyparse();
  int assemble_file(FILE *fp, shared_ptr<Memory> memory);

  extern "C"
  {
    #include "mips_parser_lex.hpp"

    int yylex( void );

    void yyerror(const char *str)
    {
      cerr << "error: " << str << endl;
    }

    int yywrap()
    {
      return 1;
    }
  }

  struct Label
  {
    std::string name;
    uint32_t line;
  };

  struct Instruction
  {
    uint32_t instcode;
    uint32_t line;
    bool has_branch_label;
    bool relative;
    int has_data_label; /* 1: high, 2: low */
    std::string label;
  };

  struct Memsection
  {
    string label;
    int type; /* 1: word, 2: float, 3: double, 4: asccii, 5: space */
    uint32_t position;
    uint32_t length;
    vector<string> values;
  };

  static vector<string> values;
  static vector<Label> labels;
  static vector<Instruction> instructions;
  static vector<Memsection> memsections;

  static uint32_t line;
  static uint32_t mem_pos;


#line 149 "src/assembler/mips_parser.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "mips_parser.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_COMMA = 3,                      /* COMMA  */
  YYSYMBOL_SYSCALL = 4,                    /* SYSCALL  */
  YYSYMBOL_NOPCODE = 5,                    /* NOPCODE  */
  YYSYMBOL_TEXT_SECTION = 6,               /* TEXT_SECTION  */
  YYSYMBOL_DATA_SECTION = 7,               /* DATA_SECTION  */
  YYSYMBOL_LAOPCODE = 8,                   /* LAOPCODE  */
  YYSYMBOL_LUIOPCODE = 9,                  /* LUIOPCODE  */
  YYSYMBOL_OBRACKET = 10,                  /* OBRACKET  */
  YYSYMBOL_CBRACKET = 11,                  /* CBRACKET  */
  YYSYMBOL_DATATAG = 12,                   /* DATATAG  */
  YYSYMBOL_LABELTAG = 13,                  /* LABELTAG  */
  YYSYMBOL_LABELJUMP = 14,                 /* LABELJUMP  */
  YYSYMBOL_REGISTER = 15,                  /* REGISTER  */
  YYSYMBOL_FREGISTER = 16,                 /* FREGISTER  */
  YYSYMBOL_IOPCODE = 17,                   /* IOPCODE  */
  YYSYMBOL_JOPCODE = 18,                   /* JOPCODE  */
  YYSYMBOL_ROPCODE = 19,                   /* ROPCODE  */
  YYSYMBOL_R1OPCODE = 20,                  /* R1OPCODE  */
  YYSYMBOL_R2OPCODE = 21,                  /* R2OPCODE  */
  YYSYMBOL_FOPCODE = 22,                   /* FOPCODE  */
  YYSYMBOL_F2ROPCODE = 23,                 /* F2ROPCODE  */
  YYSYMBOL_FBROPCODE = 24,                 /* FBROPCODE  */
  YYSYMBOL_IMOPCODE = 25,                  /* IMOPCODE  */
  YYSYMBOL_FMOPCODE = 26,                  /* FMOPCODE  */
  YYSYMBOL_INTVALUE = 27,                  /* INTVALUE  */
  YYSYMBOL_FLOATVALUE = 28,                /* FLOATVALUE  */
  YYSYMBOL_TEXTVALUE = 29,                 /* TEXTVALUE  */
  YYSYMBOL_EOL = 30,                       /* EOL  */
  YYSYMBOL_YYACCEPT = 31,                  /* $accept  */
  YYSYMBOL_program = 32,                   /* program  */
  YYSYMBOL_instructions_sequence = 33,     /* instructions_sequence  */
  YYSYMBOL_instruction = 34,               /* instruction  */
  YYSYMBOL_data_sequence = 35,             /* data_sequence  */
  YYSYMBOL_data_line = 36,                 /* data_line  */
  YYSYMBOL_array_values = 37               /* array_values  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  24
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   101

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  7
/* YYNRULES -- Number of rules.  */
#define YYNRULES  31
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  88

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   285


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   119,   119,   120,   121,   124,   129,   135,   143,   151,
     159,   167,   175,   183,   191,   199,   206,   214,   222,   230,
     243,   251,   256,   261,   269,   269,   272,   311,   315,   319,
     323,   327
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "COMMA", "SYSCALL",
  "NOPCODE", "TEXT_SECTION", "DATA_SECTION", "LAOPCODE", "LUIOPCODE",
  "OBRACKET", "CBRACKET", "DATATAG", "LABELTAG", "LABELJUMP", "REGISTER",
  "FREGISTER", "IOPCODE", "JOPCODE", "ROPCODE", "R1OPCODE", "R2OPCODE",
  "FOPCODE", "F2ROPCODE", "FBROPCODE", "IMOPCODE", "FMOPCODE", "INTVALUE",
  "FLOATVALUE", "TEXTVALUE", "EOL", "$accept", "program",
  "instructions_sequence", "instruction", "data_sequence", "data_line",
  "array_values", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-34)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       2,    25,    -9,    10,   -34,   -34,    -3,    12,   -34,    16,
      42,    37,    43,    44,     9,    24,     0,    45,    46,    -2,
     -34,    29,    22,   -34,   -34,    58,    60,    61,   -34,   -34,
      62,    63,    64,    65,    66,   -34,    67,    68,    -9,   -34,
      26,    25,   -34,    59,    47,    57,    69,    70,    71,    72,
      73,    48,    49,    -9,    74,    75,   -34,   -34,    25,   -34,
     -34,    76,    77,    78,   -34,    79,   -34,    80,    81,    26,
      26,    -1,    82,    56,    83,    85,    86,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,    84,    87,   -34,   -34
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,    21,    22,     0,     0,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       6,     0,     0,    25,     1,     0,     0,     0,    18,    20,
       0,     0,     0,     0,     0,    12,     0,     0,     0,     5,
       0,     0,    24,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     2,    29,    27,    31,    26,     3,    19,
      15,     0,     0,     0,     9,     0,    11,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    30,    28,    14,
      13,     7,     8,    10,     0,     0,    16,    17
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -34,   -34,    51,   -19,    55,   -21,   -33
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     3,    19,    20,    22,    23,    57
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      39,    42,     4,     5,    21,    38,     6,     7,     1,     2,
      24,     8,    25,    79,    35,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    33,    80,    26,    41,     4,
       5,    27,    42,     6,     7,    21,    77,    78,     8,    39,
      34,    40,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    30,    54,    55,    56,    28,    29,    31,    32,
      36,    43,    37,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    61,    59,    60,    67,    68,    69,    70,    71,
      72,    73,    74,    82,    62,    63,    64,     0,    65,    66,
      75,    76,    58,    53,     0,    86,     0,    81,    87,    83,
      84,    85
};

static const yytype_int8 yycheck[] =
{
      19,    22,     4,     5,    13,     7,     8,     9,     6,     7,
       0,    13,    15,    14,    14,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    16,    27,    15,     6,     4,
       5,    15,    53,     8,     9,    13,    69,    70,    13,    58,
      16,    12,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    15,    27,    28,    29,    14,    15,    15,    15,
      15,     3,    16,     3,     3,     3,     3,     3,     3,     3,
       3,     3,    15,    14,    27,    27,    27,     3,     3,     3,
       3,     3,     3,    27,    15,    15,    15,    -1,    16,    16,
      10,    10,    41,    38,    -1,    11,    -1,    15,    11,    16,
      15,    15
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     6,     7,    32,     4,     5,     8,     9,    13,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    33,
      34,    13,    35,    36,     0,    15,    15,    15,    14,    15,
      15,    15,    15,    16,    16,    14,    15,    16,     7,    34,
      12,     6,    36,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,    35,    27,    28,    29,    37,    33,    14,
      27,    15,    15,    15,    15,    16,    16,    27,    27,     3,
       3,     3,     3,     3,     3,    10,    10,    37,    37,    14,
      27,    15,    27,    16,    15,    15,    11,    11
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    31,    32,    32,    32,    33,    33,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    35,    35,    36,    37,    37,    37,
      37,    37
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     4,     4,     2,     2,     1,     6,     6,     4,
       6,     4,     2,     6,     6,     4,     7,     7,     2,     4,
       2,     1,     1,     1,     2,     1,     3,     1,     3,     1,
       3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_DATATAG: /* DATATAG  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 961 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_LABELTAG: /* LABELTAG  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 967 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_LABELJUMP: /* LABELJUMP  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 973 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_IOPCODE: /* IOPCODE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 979 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_JOPCODE: /* JOPCODE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 985 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_ROPCODE: /* ROPCODE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 991 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_R1OPCODE: /* R1OPCODE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 997 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_R2OPCODE: /* R2OPCODE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 1003 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_FOPCODE: /* FOPCODE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 1009 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_F2ROPCODE: /* F2ROPCODE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 1015 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_FBROPCODE: /* FBROPCODE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 1021 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_IMOPCODE: /* IMOPCODE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 1027 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_FMOPCODE: /* FMOPCODE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 1033 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_FLOATVALUE: /* FLOATVALUE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 1039 "src/assembler/mips_parser.cpp"
        break;

    case YYSYMBOL_TEXTVALUE: /* TEXTVALUE  */
#line 84 "src/assembler/mips_assembler.y"
            { free (((*yyvaluep).tval)); }
#line 1045 "src/assembler/mips_parser.cpp"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 5: /* instructions_sequence: instructions_sequence instruction  */
#line 125 "src/assembler/mips_assembler.y"
    {
      line++;
    }
#line 1317 "src/assembler/mips_parser.cpp"
    break;

  case 6: /* instructions_sequence: instruction  */
#line 130 "src/assembler/mips_assembler.y"
    {
      line++;
    }
#line 1325 "src/assembler/mips_parser.cpp"
    break;

  case 7: /* instruction: ROPCODE REGISTER COMMA REGISTER COMMA REGISTER  */
#line 136 "src/assembler/mips_assembler.y"
                {
      stringstream ss;
      ss << (yyvsp[-5].tval) << " $" << (yyvsp[-4].ival) << ", $" << (yyvsp[-2].ival) << ", $" << (yyvsp[0].ival);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, false, false, 0, ""});
      free((yyvsp[-5].tval));
		}
#line 1337 "src/assembler/mips_parser.cpp"
    break;

  case 8: /* instruction: R1OPCODE REGISTER COMMA REGISTER COMMA INTVALUE  */
#line 144 "src/assembler/mips_assembler.y"
                {
      stringstream ss;
      ss << (yyvsp[-5].tval) << " $" << (yyvsp[-4].ival) << ", $" << (yyvsp[-2].ival) << ", " << (yyvsp[0].ival);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, false, false, 0, ""});
      free((yyvsp[-5].tval));
		}
#line 1349 "src/assembler/mips_parser.cpp"
    break;

  case 9: /* instruction: R2OPCODE REGISTER COMMA REGISTER  */
#line 152 "src/assembler/mips_assembler.y"
                {
      stringstream ss;
      ss << (yyvsp[-3].tval) << " $" << (yyvsp[-2].ival) << ", $" << (yyvsp[0].ival);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, false, false, 0, ""});
      free((yyvsp[-3].tval));
		}
#line 1361 "src/assembler/mips_parser.cpp"
    break;

  case 10: /* instruction: FOPCODE FREGISTER COMMA FREGISTER COMMA FREGISTER  */
#line 160 "src/assembler/mips_assembler.y"
    {
      stringstream ss;
      ss << (yyvsp[-5].tval) << " $" << (yyvsp[-4].ival) << ", $" << (yyvsp[-2].ival) << ", $" << (yyvsp[0].ival);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, false, false, 0, ""});
      free((yyvsp[-5].tval));
    }
#line 1373 "src/assembler/mips_parser.cpp"
    break;

  case 11: /* instruction: F2ROPCODE FREGISTER COMMA FREGISTER  */
#line 168 "src/assembler/mips_assembler.y"
    {
      stringstream ss;
      ss << (yyvsp[-3].tval) << " $" << (yyvsp[-2].ival) << ", $" << (yyvsp[0].ival);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, false, false, 0, ""});
      free((yyvsp[-3].tval));
    }
#line 1385 "src/assembler/mips_parser.cpp"
    break;

  case 12: /* instruction: FBROPCODE LABELJUMP  */
#line 176 "src/assembler/mips_assembler.y"
    {
      stringstream ss;
      ss << (yyvsp[-1].tval) << " " << (yyvsp[0].tval);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, true, true, 0, (yyvsp[0].tval)});
      free((yyvsp[-1].tval)); free((yyvsp[0].tval));
    }
#line 1397 "src/assembler/mips_parser.cpp"
    break;

  case 13: /* instruction: IOPCODE REGISTER COMMA REGISTER COMMA INTVALUE  */
#line 184 "src/assembler/mips_assembler.y"
                {
      stringstream ss;
      ss << (yyvsp[-5].tval) << " $" << (yyvsp[-4].ival) << ", $" << (yyvsp[-2].ival) << ", " << (yyvsp[0].ival);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, false, false, 0, ""});
      free((yyvsp[-5].tval));
		}
#line 1409 "src/assembler/mips_parser.cpp"
    break;

  case 14: /* instruction: IOPCODE REGISTER COMMA REGISTER COMMA LABELJUMP  */
#line 192 "src/assembler/mips_assembler.y"
                {
      stringstream ss;
      ss << (yyvsp[-5].tval) << " $" << (yyvsp[-4].ival) << ", $" << (yyvsp[-2].ival) << ", " << (yyvsp[0].tval);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, true, true, 0, (yyvsp[0].tval)});
      free((yyvsp[-5].tval)); free((yyvsp[0].tval));
		}
#line 1421 "src/assembler/mips_parser.cpp"
    break;

  case 15: /* instruction: LUIOPCODE REGISTER COMMA INTVALUE  */
#line 200 "src/assembler/mips_assembler.y"
                {
      stringstream ss;
      ss << "lui $" << (yyvsp[-2].ival) << ", " << (yyvsp[0].ival);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, false, false, 0, ""});
		}
#line 1432 "src/assembler/mips_parser.cpp"
    break;

  case 16: /* instruction: IMOPCODE REGISTER COMMA INTVALUE OBRACKET REGISTER CBRACKET  */
#line 207 "src/assembler/mips_assembler.y"
                {
      stringstream ss;
      ss << (yyvsp[-6].tval) << " $" << (yyvsp[-5].ival) << ", " << (yyvsp[-3].ival) << "($" << (yyvsp[-1].ival) << ")";
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, false, false, 0, ""});
      free((yyvsp[-6].tval));
		}
#line 1444 "src/assembler/mips_parser.cpp"
    break;

  case 17: /* instruction: FMOPCODE FREGISTER COMMA INTVALUE OBRACKET REGISTER CBRACKET  */
#line 215 "src/assembler/mips_assembler.y"
    {
      stringstream ss;
      ss << (yyvsp[-6].tval) << " $" << (yyvsp[-5].ival) << ", " << (yyvsp[-3].ival) << "($" << (yyvsp[-1].ival) << ")";
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, false, false, 0, ""});
      free((yyvsp[-6].tval));
    }
#line 1456 "src/assembler/mips_parser.cpp"
    break;

  case 18: /* instruction: JOPCODE LABELJUMP  */
#line 223 "src/assembler/mips_assembler.y"
                {
      stringstream ss;
      ss << (yyvsp[-1].tval) << " " << (yyvsp[0].tval);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, true, false, 0, (yyvsp[0].tval)});
      free((yyvsp[-1].tval)); free((yyvsp[0].tval));
		}
#line 1468 "src/assembler/mips_parser.cpp"
    break;

  case 19: /* instruction: LAOPCODE REGISTER COMMA LABELJUMP  */
#line 231 "src/assembler/mips_assembler.y"
                {
      /* decompose in lui & ori */
      stringstream ss1, ss2;
      ss1 << "lui $" << (yyvsp[-2].ival) << ", 0";
      ss2 << "ori $" << (yyvsp[-2].ival) << ", $" << (yyvsp[-2].ival) << ", 0";
      uint32_t instcode = Utils::assemble_instruction(ss1.str());
      instructions.push_back({instcode, line, false, false, 1, (yyvsp[0].tval)});
      line++;
      instcode = Utils::assemble_instruction(ss2.str());
      instructions.push_back({instcode, line, false, false, 2, (yyvsp[0].tval)});
      free((yyvsp[0].tval));
		}
#line 1485 "src/assembler/mips_parser.cpp"
    break;

  case 20: /* instruction: JOPCODE REGISTER  */
#line 244 "src/assembler/mips_assembler.y"
                {
      stringstream ss;
      ss << (yyvsp[-1].tval) << " $" << (yyvsp[0].ival);
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      instructions.push_back({instcode, line, false, false, 0, ""});
      free((yyvsp[-1].tval));
		}
#line 1497 "src/assembler/mips_parser.cpp"
    break;

  case 21: /* instruction: SYSCALL  */
#line 252 "src/assembler/mips_assembler.y"
    {
      uint32_t instcode = Utils::assemble_instruction("syscall");
      instructions.push_back({instcode, line, false, false, 0, ""});
    }
#line 1506 "src/assembler/mips_parser.cpp"
    break;

  case 22: /* instruction: NOPCODE  */
#line 257 "src/assembler/mips_assembler.y"
                {
      uint32_t instcode = 0;
      instructions.push_back({instcode, line, false, false, 0, ""});
		}
#line 1515 "src/assembler/mips_parser.cpp"
    break;

  case 23: /* instruction: LABELTAG  */
#line 262 "src/assembler/mips_assembler.y"
    {
      labels.push_back({(yyvsp[0].tval), line});
      line--;
      free((yyvsp[0].tval));
    }
#line 1525 "src/assembler/mips_parser.cpp"
    break;

  case 26: /* data_line: LABELTAG DATATAG array_values  */
#line 273 "src/assembler/mips_assembler.y"
      {
        uint32_t last_pos = mem_pos;
        int type = 0;
        reverse(values.begin(), values.end());
        if (!strcmp((yyvsp[-1].tval), "word"))
        {
          type = MEM_TYPE_WORD;
          mem_pos += values.size() * 4;
        }
        else if (!strcmp((yyvsp[-1].tval), "float"))
        {
          type = MEM_TYPE_FLOAT;
          mem_pos += values.size() * 4;
        }
        else if (!strcmp((yyvsp[-1].tval), "double"))
        {
          type = MEM_TYPE_DOUBLE;
          mem_pos += values.size() * 8;
        }
        else if (!strcmp((yyvsp[-1].tval), "asciiz"))
        {
          type = MEM_TYPE_STRING;
          assert(values.size() == 1);
          /* round up to word size */
          mem_pos += Utils::address_align_4(values[0].length());
        }
        else if (!strcmp((yyvsp[-1].tval), "space"))
        {
          type = MEM_TYPE_SPACE;
          assert(values.size() == 1);
          mem_pos += stoi(values[0]);
        }
        memsections.push_back({(yyvsp[-2].tval), type, last_pos, mem_pos-last_pos, values});
        values.clear();
      }
#line 1565 "src/assembler/mips_parser.cpp"
    break;

  case 27: /* array_values: FLOATVALUE  */
#line 312 "src/assembler/mips_assembler.y"
      {
        values.push_back((yyvsp[0].tval));
      }
#line 1573 "src/assembler/mips_parser.cpp"
    break;

  case 28: /* array_values: FLOATVALUE COMMA array_values  */
#line 316 "src/assembler/mips_assembler.y"
      {
        values.push_back((yyvsp[-2].tval));
      }
#line 1581 "src/assembler/mips_parser.cpp"
    break;

  case 29: /* array_values: INTVALUE  */
#line 320 "src/assembler/mips_assembler.y"
      {
        values.push_back(to_string((yyvsp[0].ival)));
      }
#line 1589 "src/assembler/mips_parser.cpp"
    break;

  case 30: /* array_values: INTVALUE COMMA array_values  */
#line 324 "src/assembler/mips_assembler.y"
      {
        values.push_back(to_string((yyvsp[-2].ival)));
      }
#line 1597 "src/assembler/mips_parser.cpp"
    break;

  case 31: /* array_values: TEXTVALUE  */
#line 328 "src/assembler/mips_assembler.y"
      {
        string s = (yyvsp[0].tval);
        /* remove quotes */
        assert(s[0] == s[s.length()-1] && s[0] == '\"');
        values.push_back(s.substr(1, s.length()-2));
      }
#line 1608 "src/assembler/mips_parser.cpp"
    break;


#line 1612 "src/assembler/mips_parser.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 334 "src/assembler/mips_assembler.y"


namespace mips_sim
{

/******************************************************************************/

static void setup_memory(shared_ptr<Memory> memory)
{
  uint32_t next_address;
  uint32_t alloc_address;
  uint32_t words_to_mem[2];

  memory->lock();
  /* process data memory sections */
  next_address = MEM_DATA_START;
  for (Memsection memsection : memsections)
  {
    alloc_address = memory->allocate_space(memsection.length);
    assert(alloc_address == next_address);

    /* copy data */
    for (string s : memsection.values)
    {
      if (memsection.type == MEM_TYPE_WORD)
      {
        memory->mem_write_32(next_address,
                              stoi(s));
        next_address += 4;
      }
      else if (memsection.type == MEM_TYPE_FLOAT)
      {
        Utils::float_to_word(stof(s), words_to_mem);
        memory->mem_write_32(next_address,
                             words_to_mem[0]);
        next_address += 4;
      }
      else if (memsection.type == MEM_TYPE_DOUBLE)
      {
        Utils::double_to_word(stod(s), words_to_mem);
        memory->mem_write_32(next_address,
                             words_to_mem[0]);
        memory->mem_write_32(next_address + 4,
                             words_to_mem[1]);
        next_address += 8;
      }
      else if (memsection.type == MEM_TYPE_STRING)
      {
        uint32_t writevalue;
        uint32_t str_len = s.length();

        for (uint32_t i=0; i<str_len; i++)
        {
          memory->mem_write_8(next_address + i, static_cast<uint8_t>(s[i]));
        }

        next_address += memsection.length;
      }
      else if (memsection.type == MEM_TYPE_SPACE)
      {
        next_address += stoi(s);
      }
    }
  }

  /* process code memory */
  alloc_address = memory->allocate_space(static_cast<uint32_t>((instructions.size()+2) * 4),
                                         MEM_TEXT_START);
  assert(alloc_address == MEM_TEXT_START);

  next_address = MEM_TEXT_START;
  for (Instruction instruction : instructions)
  {
    /* Read in the program. */
    memory->mem_write_32(next_address, instruction.instcode);
    next_address += 4;
  }
}

int assemble_file(const char filename[], shared_ptr<Memory> memory)
{
  int retval = 1;

  /* reset stuff */
  memory->clear();
  values.clear();
  labels.clear();
  instructions.clear();
  memsections.clear();
  line = 0;
  mem_pos = 0;

  /* parse file */
  if (Utils::file_exists(filename))
  {
    yyin = fopen(filename, "r");
    retval = yyparse();
    fclose(yyin);

    yylex_destroy();
  }

  /* process file */
  if (retval)
  {
    return 1;
  }
  /* rebuild instructions */
  for (Instruction & instruction : instructions)
  {
    if (instruction.has_branch_label)
    {
      bool label_found = false;
      for (Label label : labels)
      {
        if (!instruction.label.compare(label.name))
        {
          if (label_found)
            throw Exception::e(PARSER_DUPLABEL_EXCEPTION, "Dublicated label '"+label.name + "'");
          label_found = true;
          if (instruction.relative)
          {
            int rel_branch = label.line - instruction.line - 1;
            instruction.instcode |= (static_cast<uint32_t>(rel_branch) & 0xFFFF);
          }
          else
          {
            uint32_t abs_branch = MEM_TEXT_START/4 + static_cast<uint32_t>(label.line);
            instruction.instcode |= (abs_branch & 0x3FFFFFF);
          }
        }
      }
      if (!label_found)
        throw Exception::e(PARSER_NOLABEL_EXCEPTION, "Undefined label '"+instruction.label + "'");
    }
    else if (instruction.has_data_label > 0)
    {
      assert(instruction.has_data_label <= 2);
      bool label_found = false;
      for (Memsection mem_section : memsections)
      {
        if (!instruction.label.compare(mem_section.label))
        {
          if (label_found)
            throw Exception::e(PARSER_DUPLABEL_EXCEPTION, "Dublicated label '"+mem_section.label + "'");
          label_found = true;
          uint32_t address = MEM_DATA_START + mem_section.position;
          if (instruction.has_data_label == 1)
            instruction.instcode |= (address >> 16 & 0xFFFF);
          else
            instruction.instcode |= (address & 0xFFFF);
        }
      }
      if (!label_found)
        throw Exception::e(PARSER_NOLABEL_EXCEPTION, "Undefined label '"+instruction.label + "'");
    }
  }

  setup_memory(memory);

  return 0;
}

void print_file(std::string output_file)
{
  for (Instruction & instruction : instructions)
  {
    cout << Utils::hex32(instruction.instcode)
         << " # " << Utils::decode_instruction(instruction.instcode)
         << endl;
  }
}

} /* namespace */
