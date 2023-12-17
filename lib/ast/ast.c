#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

AST *create_node(AST_T type) {
  AST *node = (AST *)malloc(sizeof(AST));
  if (node == NULL)
    fprintf(stderr, "Unable to allocate ast_node\n"), exit(1);
  node->tag = type;
  node->visited = false;
  return node;
}

AST *create_typeref_array(AST *node_arr) {
  AST *node = create_node(AST_TYPE_REF);
  node->AST_TYPE_REF.type = TR_ARR;
  node->AST_TYPE_REF.sub_field = node_arr;
  return node;
}

AST *create_typeref_ident(AST *node_ident) {
  AST *node = create_node(AST_TYPE_REF);
  node->AST_TYPE_REF.type = TR_IDENT;
  node->AST_TYPE_REF.sub_field = node_ident;
  return node;
}

AST *create_typeref(enum type_ref type) {
  AST *node = create_node(AST_TYPE_REF);
  node->AST_TYPE_REF.type = type;
  return node;
}

AST *create_typeref_lt(enum literal_type type) {
  AST *node = create_node(AST_TYPE_REF_LT);
  node->AST_TYPE_REF_LT.type = type;
  return node;
}

AST *create_ident(char *name, AST *typeref_node) {
  AST *ident = create_node(AST_IDENT);
  ident->AST_IDENT.name = name;
  ident->AST_IDENT.type = typeref_node;
  return ident;
}

AST *create_typed_ident_decl(AST *ident_list, AST *typeref_node) {
  AST *iter = ident_list;
  while (iter != NULL) {
      iter->AST_LIST.current->AST_IDENT.type = node_dup(typeref_node);
      iter = iter->AST_LIST.next;
  }
  free(typeref_node);
  return ident_list;
}

AST *create_func_sign(AST *name, AST *arg_list) {
  AST *fs = create_node(AST_FUNC_SIGN);
  fs->AST_FUNC_SIGN.ident = name;
  fs->AST_FUNC_SIGN.arg_list = arg_list;
  return fs;
}

AST *create_func(AST *sign, AST *body) {
  AST *func = create_node(AST_FUNC);
  func->AST_FUNC.func_sign = sign;
  func->AST_FUNC.body = body;
  return func;
}

AST *create_body(AST *statement) {
  AST *body = create_node(AST_BODY);
  body->AST_BODY.statement = statement;
  return body;
}

AST *create_type_array(AST *type, size_t dimensions) {
  AST *node = create_node(AST_ARRAY);
  node->AST_ARRAY.dimensions = dimensions;
  node->AST_ARRAY.type = type;
  return node;
}

AST *create_literal(char *value, AST *lt) {
  AST *constant = create_node(AST_LITERAL);
  constant->AST_LITERAL.value = value;
  constant->AST_LITERAL.type = lt;
  return constant;
}

AST *create_expr(AST *node) {
  AST *expr = create_node(AST_EXPR);
  expr->AST_EXPR.some_node = node;
  return expr;
}

AST *create_unexpr(OP_T op, AST *arg) {
  AST *unexpr = create_node(AST_UN_EXPR);
  unexpr->AST_UN_EXPR.op = op;
  unexpr->AST_UN_EXPR.argument = arg;
  return unexpr;
}

AST *create_binexpr(OP_T op, AST *arg1, AST *arg2) {
  AST *binexpr = create_node(AST_BIN_EXPR);
  binexpr->AST_BIN_EXPR.op = op;
  binexpr->AST_BIN_EXPR.arg1 = arg1;
  binexpr->AST_BIN_EXPR.arg2 = arg2;
  return binexpr;
}

AST *create_expr_index_or_call(AST *expr, AST *expr_list) {
  AST *node = create_node(AST_CALL_OR_IND);
  node->AST_CALL_OR_IND.expr = expr;
  node->AST_CALL_OR_IND.expr_list = expr_list;
  return node;
}

AST *create_break() {
  AST *node = create_node(AST_BREAK);
  return node;
}

AST *create_branch(AST *test, AST *consequent, AST *alternate) {
  AST *br = create_node(AST_BRANCH);
  br->AST_BRANCH.test = test;
  br->AST_BRANCH.consequent = consequent;
  br->AST_BRANCH.alternate = alternate;
  return br;
}

AST *create_while(AST *test, AST *body) {
  AST *wh = create_node(AST_WHILE);
  wh->AST_WHILE.test = test;
  wh->AST_WHILE.body = body;
  return wh;
}

AST *create_repeat(AST *test, AST *body) {
    AST *wh = create_node(AST_REPEAT);
    wh->AST_REPEAT.test = test;
    wh->AST_REPEAT.body = body;
    return wh;
}

AST *create_list(AST *head, AST *next, AST_T type) {                                            
  AST *list = create_node(type);                                     
  list->AST_LIST.current = head;                                                
  list->AST_LIST.next = next;                                                   
  return list;
}

AST *insert_list(AST **head_p, AST *first, AST_T type) {
  if (*head_p != NULL) {
    AST * const oldstart = create_list((*head_p)->AST_LIST.current, (*head_p)->AST_LIST.next, type);
    free(*head_p);
    *head_p = create_list(first, oldstart, type);
  } else {
    *head_p = create_list(first, NULL, type);  
  }
  return *head_p;
}

AST *create_statement(AST *node) {
  AST *stat = create_node(AST_STMT);
  stat->AST_STMT.some_node = node;
  return stat;
}

AST *create_source(AST *child) {
  AST *program = create_node(AST_SOURCE);
  program->AST_SOURCE.child = child;
  return program;
}

AST *node_dup(AST *node) {
    if (node == NULL) return NULL;

    AST *new_node = create_node(node->tag);

    switch (node->tag) {
        case AST_IDENT: {
            new_node->AST_IDENT.name = strdup(node->AST_IDENT.name);
            new_node->AST_IDENT.type = node_dup(node->AST_IDENT.type);
            return new_node;
        }
        case AST_LITERAL: {
            new_node->AST_LITERAL.value = strdup(node->AST_LITERAL.value);
            new_node->AST_LITERAL.type = node_dup(node->AST_LITERAL.type);
            return new_node;
        }
        case AST_EXPR: {
            new_node->AST_EXPR.some_node = node_dup(node->AST_EXPR.some_node);
            return new_node;
        }
        case AST_UN_EXPR: {
            new_node->AST_UN_EXPR.op = node->AST_UN_EXPR.op;
            new_node->AST_UN_EXPR.argument = node_dup(node->AST_UN_EXPR.argument);
            return new_node;
        }
        case AST_BIN_EXPR: {
            new_node->AST_BIN_EXPR.op = node->AST_BIN_EXPR.op;
            new_node->AST_BIN_EXPR.arg1 = node_dup(node->AST_BIN_EXPR.arg1);
            new_node->AST_BIN_EXPR.arg2 = node_dup(node->AST_BIN_EXPR.arg2);
            return new_node;
        }
        case AST_BRANCH: {
            new_node->AST_BRANCH.test = node_dup(node->AST_BRANCH.test);
            new_node->AST_BRANCH.consequent = node_dup(node->AST_BRANCH.consequent);
            new_node->AST_BRANCH.alternate = node_dup(node->AST_BRANCH.alternate);
            return new_node;
        }
        case AST_WHILE: {
            new_node->AST_WHILE.test = node_dup(node->AST_WHILE.test);
            new_node->AST_WHILE.body = node_dup(node->AST_WHILE.body);
            return new_node;
        }
        case AST_REPEAT: {
            new_node->AST_REPEAT.test = node_dup(node->AST_REPEAT.test);
            new_node->AST_REPEAT.body = node_dup(node->AST_REPEAT.body);
            return new_node;
        }
        case AST_SOURCE: {
            new_node->AST_SOURCE.child = node_dup(node->AST_SOURCE.child);
            return new_node;
        }
        case AST_CALL_OR_IND: {
            new_node->AST_CALL_OR_IND.expr = node_dup(node->AST_CALL_OR_IND.expr);
            new_node->AST_CALL_OR_IND.expr_list = node_dup(node->AST_CALL_OR_IND.expr_list);
            return new_node;
        }
        case AST_FUNC: {
            new_node->AST_FUNC.body = node_dup(node->AST_FUNC.body);
            new_node->AST_FUNC.func_sign = node_dup(node->AST_FUNC.func_sign);
            return new_node;
        }
        case AST_FUNC_SIGN: {
            new_node->AST_FUNC_SIGN.ident = node_dup(node->AST_FUNC_SIGN.ident);
            new_node->AST_FUNC_SIGN.arg_list = node_dup(node->AST_FUNC_SIGN.arg_list);
            return new_node;
        }
        case AST_BODY: {
            new_node->AST_BODY.statement = node_dup(node->AST_BODY.statement);
            return new_node;
        }
        case AST_BREAK: {
            return new_node;
        }
        case AST_STMT: {
            new_node->AST_STMT.some_node = node_dup(node->AST_STMT.some_node);
            return new_node;
        }
        case AST_TYPE_REF: {
            new_node->AST_TYPE_REF.type = node->AST_TYPE_REF.type;
            new_node->AST_TYPE_REF.sub_field = node_dup(node->AST_TYPE_REF.sub_field);
            return new_node;
        }
        case AST_TYPE_REF_LT: {
            new_node->AST_TYPE_REF_LT.type = node->AST_TYPE_REF_LT.type;
            new_node->AST_TYPE_REF_LT.sub_field = node_dup(node->AST_TYPE_REF_LT.sub_field);
            return new_node;
        }
        case AST_STMTS_LIST:
        case AST_LIT_LIST:
        case AST_ARGDEF_LIST:
        case AST_FUNC_LIST:
        case AST_EXPR_LIST:
        case AST_IDENT_LIST: {
            new_node->AST_LIST.current = node_dup(node->AST_LIST.current);
            new_node->AST_LIST.next = node_dup(node->AST_LIST.next);
            return new_node;
        }
        case AST_ARRAY: {
            new_node->AST_ARRAY.type = node_dup(node->AST_ARRAY.type);
            new_node->AST_ARRAY.dimensions = node->AST_ARRAY.dimensions;
            return new_node;
        }
        default: {
            return NULL;
        }
    }
}