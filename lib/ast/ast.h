#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <stdbool.h>

typedef enum operation_type OP_T;

enum operation_type {
  OP_PLUS,
  OP_MINUS,
  OP_MUL,
  OP_TRUE_DIV,
  OP_LT,
  OP_GT,
  OP_LE,
  OP_GE,
  OP_EQ,
  OP_ASSIGN,
  OP_UN_NOT,
};


typedef enum type_ref T_REF;

enum type_ref {
  TR_IDENT,
  TR_BOOL,
  TR_BYTE,
  TR_INT,
  TR_UINT,
  TR_LONG,
  TR_ULONG,
  TR_CHAR,
  TR_STR,
  TR_ARR,
  TR_NONE,
};

typedef enum literal_type L_TYPE;

enum literal_type {
  LT_BOOL, 
  LT_STR,
  LT_CHAR,
  LT_BITS, 
  LT_DEC,
  LT_HEX,
};

typedef struct AST AST;
typedef enum ast_node_type AST_T;

struct AST {
  bool visited;
  enum ast_node_type {
    AST_IDENT,
    AST_LITERAL,
    AST_EXPR,
    AST_UN_EXPR,
    AST_BIN_EXPR,
    AST_BRANCH,
    AST_REPEAT,
    AST_SOURCE,
    AST_CALL_OR_IND,
    AST_FUNC,
    AST_FUNC_SIGN,
    AST_BODY,
    AST_TYPE_REF,
    AST_TYPE_REF_LT,
    AST_STMT,
    AST_STMTS_LIST,
    AST_LIT_LIST,
    AST_ARGDEF_LIST,
    AST_FUNC_LIST,
    AST_EXPR_LIST,
    AST_IDENT_LIST,
    AST_WHILE,
    AST_ARRAY,
    AST_BREAK,
  } tag;
  union {
    struct AST_IDENT       { char *name; AST *type;                           } AST_IDENT;
    struct AST_LITERAL     { char *value; AST *type;                          } AST_LITERAL;
    struct AST_EXPR        { AST *some_node;                                  } AST_EXPR;
    struct AST_UN_EXPR     { OP_T op; AST *argument;                          } AST_UN_EXPR;
    struct AST_BIN_EXPR    { OP_T op; AST *arg1; AST *arg2;                   } AST_BIN_EXPR;
    struct AST_BRANCH      { AST *test; AST *consequent; AST *alternate;      } AST_BRANCH;
    struct AST_REPEAT      { AST *test; AST *body;                            } AST_REPEAT;
    struct AST_WHILE       { AST *test; AST *body;                            } AST_WHILE;
    struct AST_SOURCE      { AST *child;                                      } AST_SOURCE;
    struct AST_CALL_OR_IND { AST *expr; AST *expr_list;                       } AST_CALL_OR_IND;
    struct AST_FUNC        { AST *func_sign; AST *body;                       } AST_FUNC;
    struct AST_FUNC_SIGN   { AST *ident; AST *arg_list;                       } AST_FUNC_SIGN;
    struct AST_BODY        { AST *statement;                                  } AST_BODY;
    struct AST_TYPE_REF    { T_REF type; AST *sub_field;                      } AST_TYPE_REF;
    struct AST_TYPE_REF_LT { L_TYPE type; AST *sub_field;                     } AST_TYPE_REF_LT;
    struct AST_STMT        { AST *some_node;                                  } AST_STMT;
    struct AST_LIST        { AST *current; AST *next;                         } AST_LIST;
    struct AST_ARRAY       { size_t dimensions; AST *type;                    } AST_ARRAY;
  };
};

AST *create_typeref_array(AST *);

AST *create_typeref_ident(AST *);

AST *create_typeref_lt(enum literal_type type);

AST *create_typeref(enum type_ref);

AST *create_body(AST *);

AST *create_ident(char *, AST *);

AST *create_func(AST *, AST *);

AST *create_func_sign(AST *, AST *);

AST *create_type_array(AST *type, size_t dimensions);

AST *create_break();

AST *create_literal(char *, AST *);

AST *create_expr(AST *);

AST *create_unexpr(enum operation_type, AST *);

AST *create_typed_ident_decl(AST *ident_list, AST *typeref_node);

AST *create_binexpr(enum operation_type, AST *, AST *);

AST *create_expr_index_or_call(AST *, AST *);

AST *create_branch(AST *, AST *, AST *);

AST *create_while(AST *, AST *);

AST *create_repeat(AST *, AST*);

AST *create_statement(AST *);

AST *create_source(AST *);

AST *insert_list(AST **head_p, AST *first, AST_T type);

AST *create_list(AST *head, AST *next, AST_T type);

AST *create_node(AST_T type);

AST *node_dup(AST *node);

#endif
