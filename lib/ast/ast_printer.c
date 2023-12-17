#include <stdio.h>
#include "ast_printer.h"



const char *op_names[] = {
  [OP_PLUS]     = "+",
  [OP_MINUS]    = "-",
  [OP_MUL]      = "*",
  [OP_TRUE_DIV] = "/",
  [OP_LT]       = "<",
  [OP_GT]       = ">",
  [OP_LE]       = "=<",
  [OP_GE]       = ">=",
  [OP_EQ]       = "==",
  [OP_ASSIGN]   = "=",
  [OP_UN_NOT]   = "not",
};


static const char *literal_names[] = {
  [LT_BOOL] = "bool", [LT_STR] = "str", [LT_CHAR] = "char",
  [LT_BITS] = "bits", [LT_DEC] = "dec", [LT_HEX]  = "hex",
};

static const char *typeref_names[] = {
  [TR_IDENT] = "ident", [TR_BOOL] = "bool", [TR_BYTE] = "byte",
  [TR_INT]   = "int",   [TR_UINT] = "uint", [TR_LONG] = "long",
  [TR_ULONG] = "ulong", [TR_CHAR] = "char", [TR_STR]  = "str",
  [TR_ARR]   = "array", [TR_NONE] = "none",
};

static const char *ant_names[] = {
  [AST_IDENT]       = "identifier",
  [AST_LITERAL]     = "literal",
  [AST_EXPR]        = "expression",
  [AST_EXPR_LIST]   = "expression_list",
  [AST_IDENT_LIST]  = "ident_list",
  [AST_UN_EXPR]     = "unary-expression",
  [AST_REPEAT]      = "repeat-until",
  [AST_STMT]        = "statement",
  [AST_STMTS_LIST]  = "statements-list",
  [AST_SOURCE]      = "program",
  [AST_CALL_OR_IND] = "call-or-indexer",
  [AST_BRANCH]      = "branch",
  [AST_WHILE]       = "while",
  [AST_ARGDEF_LIST] = "arg-def-list",
  [AST_ARRAY]       = "array",
  [AST_LIT_LIST]    = "literal-list",
  [AST_FUNC]        = "func",
  [AST_FUNC_SIGN]   = "func-sign",
  [AST_BODY]        = "body",
  [AST_TYPE_REF]    = "type-ref",
  [AST_TYPE_REF_LT] = "type-ref-literal",
  [AST_BIN_EXPR]    = "binary-expression",
  [AST_BREAK]       = "break",
  [AST_FUNC_LIST]   = "",
};


static void preprint_ast(AST *node, size_t depth) {
  if (depth >= 1) {
    for (size_t i = 0; i < 2 * depth; ++i) {
      fprintf(stdout, " ");
    }
  }
  fprintf(stdout, "%s ", ant_names[node->tag]);
  switch (node->tag) {
    case AST_SOURCE: {
      fprintf(stdout, "source\n");
      break;
    }
    case AST_ARRAY: {
        fprintf(stdout, "[dimensions: %zu]\n", node->AST_ARRAY.dimensions);
        break;
    }
    case AST_EXPR_LIST:
    case AST_ARGDEF_LIST:
    case AST_LIT_LIST:
    case AST_IDENT_LIST:
    case AST_STMTS_LIST: {
      fprintf(stdout, "\n");
      break;
    }
    case AST_LITERAL: {
      fprintf(stdout, "[val: %s, type: %s]\n", node->AST_LITERAL.value,
              literal_names[node->AST_LITERAL.type->AST_TYPE_REF.type]);
      break;
    }
    case AST_IDENT: {
      fprintf(stdout, "[ident name: %s]\n", node->AST_IDENT.name);
      break;
    }
    case AST_TYPE_REF: {
      fprintf(stdout, "[type: %s]\n", typeref_names[node->AST_TYPE_REF.type]);
      break;
    }
    case AST_CALL_OR_IND:
    case AST_BREAK:
    case AST_EXPR:
    case AST_WHILE:
    case AST_BRANCH:
    case AST_STMT:
    case AST_BODY:
    case AST_FUNC:
    case AST_FUNC_LIST:
    case AST_FUNC_SIGN: {
      fprintf(stdout, "\n");
      break;
    }
    case AST_UN_EXPR: {
      fprintf(stdout, "[type: %s]\n", op_names[node->AST_UN_EXPR.op]);
      break;
    }
    case AST_BIN_EXPR: {
      fprintf(stdout, "[type: %s]\n", op_names[node->AST_BIN_EXPR.op]);
      break;
    }
    default: {
      fprintf(stdout, "<unknown-node>\n");
      break;
    }
  }
}

static void post_dummy(AST *node, size_t depth) { return; }

int print_ast(AST *root) {
  dfs(root, 0, preprint_ast, post_dummy);
  return 1;
}