#ifndef __TOKEN_H
#define __TOKEN_H
#include <sys/types.h>
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INTEGER = 258,                 /* INTEGER  */
    NUMBER = 259,                  /* NUMBER  */
    STRING = 260,                  /* STRING  */
    ID = 261,                      /* ID  */
    INT = 262,                     /* INT  */
    LONG = 263,                    /* LONG  */
    SHORT = 264,                   /* SHORT  */
    CHAR = 265,                    /* CHAR  */
    FLOAT = 266,                   /* FLOAT  */
    DOUBLE = 267,                  /* DOUBLE  */
    IF = 268,                      /* IF  */
    FOR = 269,                     /* FOR  */
    ELSE = 270,                    /* ELSE  */
    CASE = 271,                    /* CASE  */
    SWITCH = 272,                  /* SWITCH  */
    STRUCT = 273,                  /* STRUCT  */
    UNION = 274,                   /* UNION  */
    ENUM = 275,                    /* ENUM  */
    TYPEDEF = 276,                 /* TYPEDEF  */
    CONST = 277,                   /* CONST  */
    UNSIGNED = 278,                /* UNSIGNED  */
    SIGNED = 279,                  /* SIGNED  */
    EXTERN = 280,                  /* EXTERN  */
    REGISTER = 281,                /* REGISTER  */
    STATIC = 282,                  /* STATIC  */
    VOLATILE = 283,                /* VOLATILE  */
    VOID = 284,                    /* VOID  */
    DO = 285,                      /* DO  */
    WHILE = 286,                   /* WHILE  */
    GOTO = 287,                    /* GOTO  */
    CONTINUE = 288,                /* CONTINUE  */
    BREAK = 289,                   /* BREAK  */
    DEFAULT = 290,                 /* DEFAULT  */
    SIZEOF = 291,                  /* SIZEOF  */
    RETURN = 292,                  /* RETURN  */
    INCLUDE = 293,                 /* INCLUDE  */
    DEFINE = 294,                  /* DEFINE  */
    GT = 295,                      /* GT  */
    GE = 296,                      /* GE  */
    LT = 297,                      /* LT  */
    LE = 298,                      /* LE  */
    EQ = 299,                      /* EQ  */
    NE = 300,                      /* NE  */
    BIT_AND = 301,                 /* BIT_AND  */
    BIT_OR = 302,                  /* BIT_OR  */
    BIT_XOR = 303,                 /* BIT_XOR  */
    BIT_NOT = 304,                 /* BIT_NOT  */
    LOGIC_AND = 305,               /* LOGIC_AND  */
    LOGIC_OR = 306,                /* LOGIC_OR  */
    LOGIC_NOT = 307,               /* LOGIC_NOT  */
    LP = 308,                      /* LP  */
    RP = 309,                      /* RP  */
    LB = 310,                      /* LB  */
    RB = 311,                      /* RB  */
    LBB = 312,                     /* LBB  */
    RBB = 313,                     /* RBB  */
    SQ = 314,                      /* SQ  */
    DQ = 315,                      /* DQ  */
    ADD = 316,                     /* ADD  */
    MINUS = 317,                   /* MINUS  */
    STAR = 318,                    /* STAR  */
    DIV = 319,                     /* DIV  */
    POINT = 320,                   /* POINT  */
    DOT = 321,                     /* DOT  */
    SEM = 322,                     /* SEM  */
    COLON = 323,                   /* COLON  */
    MOD = 324,                     /* MOD  */
    COMMA = 325,                   /* COMMA  */
    INC = 326,                     /* INC  */
    DEC = 327,                     /* DEC  */
    ASSIGN = 328,                  /* ASSIGN  */
    ADD_ASSIGN = 329,              /* ADD_ASSIGN  */
    SUB_ASSIGN = 330,              /* SUB_ASSIGN  */
    QUMARK = 331,                  /* QUMARK  */
    NOT_ASSIGN = 332,              /* NOT_ASSIGN  */
    LEFT_SHIFT = 333,              /* LEFT_SHIFT  */
    RIGHT_SHIFT = 334,             /* RIGHT_SHIFT  */
    MUL_ASSIGN = 335,              /* MUL_ASSIGN  */
    DIV_ASSIGN = 336,              /* DIV_ASSIGN  */
    LSHIFT_ASSIGN = 337,           /* LSHIFT_ASSIGN  */
    RSHIFT_ASSIGN = 338,           /* RSHIFT_ASSIGN  */
    AND_ASSIGN = 339,              /* AND_ASSIGN  */
    OR_ASSIGN = 340,               /* OR_ASSIGN  */
    XOR_ASSIGN = 341,              /* XOR_ASSIGN  */
    MSTAR = 342,                   /* MSTAR  */
    NEXT = 343,                    /* NEXT  */
    UMINUS = 344,                  /* UMINUS  */
    GADDR = 345,                   /* GADDR  */
    GVALUE = 346                   /* GVALUE  */
  };
  typedef enum yytokentype yytoken_kind_t;
/* Token kinds.  */
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define INTEGER 258
#define NUMBER 259
#define STRING 260
#define ID 261
#define INT 262
#define LONG 263
#define SHORT 264
#define CHAR 265
#define FLOAT 266
#define DOUBLE 267
#define IF 268
#define FOR 269
#define ELSE 270
#define CASE 271
#define SWITCH 272
#define STRUCT 273
#define UNION 274
#define ENUM 275
#define TYPEDEF 276
#define CONST 277
#define UNSIGNED 278
#define SIGNED 279
#define EXTERN 280
#define REGISTER 281
#define STATIC 282
#define VOLATILE 283
#define VOID 284
#define DO 285
#define WHILE 286
#define GOTO 287
#define CONTINUE 288
#define BREAK 289
#define DEFAULT 290
#define SIZEOF 291
#define RETURN 292
#define INCLUDE 293
#define DEFINE 294
#define GT 295
#define GE 296
#define LT 297
#define LE 298
#define EQ 299
#define NE 300
#define BIT_AND 301
#define BIT_OR 302
#define BIT_XOR 303
#define BIT_NOT 304
#define LOGIC_AND 305
#define LOGIC_OR 306
#define LOGIC_NOT 307
#define LP 308
#define RP 309
#define LB 310
#define RB 311
#define LBB 312
#define RBB 313
#define SQ 314
#define DQ 315
#define ADD 316
#define MINUS 317
#define STAR 318
#define DIV 319
#define POINT 320
#define DOT 321
#define SEM 322
#define COLON 323
#define MOD 324
#define COMMA 325
#define INC 326
#define DEC 327
#define ASSIGN 328
#define ADD_ASSIGN 329
#define SUB_ASSIGN 330
#define QUMARK 331
#define NOT_ASSIGN 332
#define LEFT_SHIFT 333
#define RIGHT_SHIFT 334
#define MUL_ASSIGN 335
#define DIV_ASSIGN 336
#define LSHIFT_ASSIGN 337
#define RSHIFT_ASSIGN 338
#define AND_ASSIGN 339
#define OR_ASSIGN 340
#define XOR_ASSIGN 341
#define UMINUS 342
#define HEADER 345
#define UserDefineType 346
struct metadata_struct {
    int type;
    char *content;
    size_t len;
};
struct metadata_struct *set_content(int type);
extern struct metadata_struct *metadata_ptr;
#endif