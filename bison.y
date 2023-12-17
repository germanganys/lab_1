%define api.pure full
%define api.prefix {yy}
%define parse.error verbose
%locations
%code top {
	/* XOPEN for strdup */
	#define _XOPEN_SOURCE 600
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
}

%parse-param {struct AST **root}
%parse-param {char **errmsg}
%param {void *scanner}

%destructor { free($$); } <value>
%destructor { free_ast($$); } <node>


%code {
    #define YYLEX_PARAM &yylval, &yylloc
    void yyerror(YYLTYPE *locp, struct AST **root, char **errmsg, yyscan_t scanner, char const *msg)
    {
        // Passing parsing error into main program
        size_t sz = snprintf(*errmsg, 0, "%s, location: linenum=%d\n", msg, locp->last_line);
        *errmsg = malloc((sz + 1) * sizeof(char));
        snprintf(*errmsg, sz + 1, "%s, location: linenum=%d", msg, locp->last_line);
    }
	int yylex(YYSTYPE *lval, YYLTYPE *locp, yyscan_t scanner);
}

%code requires {
  #include <stdbool.h>
  #include "lib/ast/ast_traverse.h"
  typedef void * yyscan_t;
}

%union {
  AST*                   node;
  char*                 value;
  size_t             arr_dime;
  enum operation_type      op;
}

%precedence        PREC_EXPR_IN_CP
%precedence        PREC_FUNC_CALL

%left               CP AS
%precedence         WHILE DO
%token      <value> IDENT
%precedence <value> BOOL CHR HEX BITS DEC STR 
%right              ASSIGN
%left               LT LE GT GE EQ
%left               NOT
%left               PLUS MINUS
%left               MUL TRUE_DIV 
%precedence         UNTIL
%token              IF VAR DIM END WEND FUNCTION COLON BREAK LOOP ERROR
%token              BOOLEAN BYTE INT UINT LONG ULONG CHAR STRING ARRAY
%precedence         THEN ELSE
%left               COMMA OP SEMICOL

%type <node>        statement statement_list expr expr_list typeRef
%type <node>        literal funcDef array callOrInd atom bracesExpr
%type <node>        program argDef argDef_list funcSignature ident_list source_list
%type <arr_dime>    array_dimension

%start program                                                                    
%%

program: source_list { $$ = *root = $1; return 0; };

source_list: funcDef source_list      { $$ = insert_list(&$2, $1, AST_FUNC_LIST);    }
           |                          { $$ = NULL;   }
           ;


typeRef: IDENT     { $$ = create_typeref_ident(create_ident($1, create_typeref(TR_NONE)));}
       | BOOLEAN   { $$ = create_typeref(TR_BOOL);                                     }
       | BYTE      { $$ = create_typeref(TR_BYTE);                                     }
       | INT       { $$ = create_typeref(TR_INT);                                      }
       | UINT      { $$ = create_typeref(TR_UINT);                                     }
       | LONG      { $$ = create_typeref(TR_LONG);                                     }
       | ULONG     { $$ = create_typeref(TR_ULONG);                                    }
       | CHAR      { $$ = create_typeref(TR_CHAR);                                     }
       | STRING    { $$ = create_typeref(TR_STR);                                      }
       | array     { $$ = create_typeref_array($1);                                    }
       ;

ident_list: IDENT COMMA ident_list { $$ = insert_list(&$3, create_ident($1, create_typeref(TR_NONE)), AST_IDENT_LIST);  }
          | IDENT                  { $$ = create_list(create_ident($1, create_typeref(TR_NONE)), NULL, AST_IDENT_LIST); }
          ;

argDef: IDENT AS typeRef { $$ = create_ident ($1, $3); }
      | IDENT            { $$ = create_ident ($1, create_typeref(TR_NONE)); }
      ;

argDef_list: argDef COMMA argDef_list { $$ = insert_list(&$3, $1, AST_ARGDEF_LIST);    }
           | argDef                   { $$ = create_list($1, NULL, AST_ARGDEF_LIST);   }
           |                          { $$ = create_list(NULL, NULL, AST_ARGDEF_LIST); }
           ;


array: typeRef OP array_dimension CP  { $$ = create_type_array($1, $3); }
     ;

array_dimension: COMMA array_dimension   { $$ = $2 + 1;  }
               | COMMA                   { $$ = 1;       }
               ;

funcDef: FUNCTION funcSignature statement_list END FUNCTION  { $$ = create_func($2, $3);   }
       | FUNCTION funcSignature SEMICOL  { $$ = create_func($2, NULL); }
       ;

funcSignature: IDENT OP argDef_list CP AS typeRef { $$ = create_func_sign (create_ident($1, $6), $3);                      }
             | IDENT OP argDef_list CP            { $$ = create_func_sign (create_ident($1, create_typeref(TR_NONE)), $3); }
             ;

statement: IF expr THEN statement_list END IF                     { $$ = create_statement(create_branch($2, $4, NULL )); }
         | IF expr THEN statement_list ELSE statement_list END IF { $$ = create_statement(create_branch($2, $4, $6 ));   }
         | WHILE expr statement_list WEND                         { $$ = create_statement(create_while($2, $3));         }
         | DIM ident_list AS typeRef                              { $$ = create_statement(create_typed_ident_decl($2, $4));}
         | DO statement_list LOOP WHILE expr                      { $$ = create_statement(create_while($5, $2));         }
         | DO statement_list LOOP UNTIL expr                      { $$ = create_statement(create_repeat($2, $5));        }
         | BREAK                                                  { $$ = create_statement(create_break());               }
         | expr SEMICOL                                           { $$ = create_statement($1);                           }
         ;

statement_list: statement statement_list { $$ = insert_list(&$2, $1, AST_STMTS_LIST); }
              |                          { $$ = NULL;  }
              ;

literal: BOOL  { $$ =  create_literal($1, create_typeref_lt(LT_BOOL)); }
       | STR   { $$ =  create_literal($1, create_typeref_lt(LT_STR));  } 
       | CHR   { $$ =  create_literal($1, create_typeref_lt(LT_CHAR)); }
       | BITS  { $$ =  create_literal($1, create_typeref_lt(LT_BITS)); }
       | HEX   { $$ =  create_literal($1, create_typeref_lt(LT_HEX));  }
       | DEC   { $$ =  create_literal($1, create_typeref_lt(LT_DEC));  }
       ;

bracesExpr: OP expr CP { $$ = $2;}

atom: IDENT                    { $$ = create_expr(create_ident($1, create_typeref(TR_NONE))); }
    | literal                  { $$ = create_expr($1);                                        }
    | bracesExpr               { $$ = create_expr($1);                                        }

expr_list: expr COMMA expr_list   { $$ = insert_list(&$3, $1, AST_EXPR_LIST);    }
         | expr                   { $$ = create_list($1, NULL, AST_EXPR_LIST);   }
         ;

callOrInd: atom OP expr_list CP { $$ = create_expr_index_or_call($1, $3); }
         | atom OP CP           { $$ = create_expr_index_or_call($1, NULL); }

expr: expr MINUS expr          { $$ = create_binexpr(OP_MINUS   , $1, $3); }
    | expr PLUS expr           { $$ = create_binexpr(OP_PLUS    , $1, $3); }
    | expr MUL expr            { $$ = create_binexpr(OP_MUL     , $1, $3); }
    | expr TRUE_DIV expr       { $$ = create_binexpr(OP_TRUE_DIV, $1, $3); }
    | expr LT expr             { $$ = create_binexpr(OP_LT      , $1, $3); }
    | expr GT expr             { $$ = create_binexpr(OP_GT      , $1, $3); }
    | expr LE expr             { $$ = create_binexpr(OP_LE      , $1, $3); }
    | expr GE expr             { $$ = create_binexpr(OP_GE      , $1, $3); }
    | expr EQ expr             { $$ = create_binexpr(OP_EQ      , $1, $3); }
    | expr ASSIGN expr         { $$ = create_binexpr(OP_ASSIGN  , $1, $3); }
    | NOT expr                 { $$ = create_unexpr(OP_UN_NOT  , $2);      } 
    | callOrInd  %prec  PREC_FUNC_CALL        { $$ = $1; }
    | atom       %prec  PREC_EXPR_IN_CP       { $$ = $1; }
    ;

%%