#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ast_traverse.h"

void dfs(AST *node, size_t depth, visit visitor, visit after_visitor) {
  if (node == NULL || node->visited)
    return;

  visitor(node, depth);
  node->visited = true;

  switch (node->tag) {
    case AST_SOURCE: {
      dfs(node->AST_SOURCE.child, depth, visitor, after_visitor);
      break;
    }
    case AST_EXPR_LIST:
    case AST_ARGDEF_LIST:
    case AST_LIT_LIST:
    case AST_IDENT_LIST:
    case AST_STMTS_LIST:
    case AST_FUNC_LIST: {
      dfs(node->AST_LIST.current, depth + 1, visitor, after_visitor);
      dfs(node->AST_LIST.next, depth, visitor, after_visitor);
      return;
    }
    case AST_ARRAY: {
        dfs(node->AST_ARRAY.type, depth + 1, visitor, after_visitor);
        break;
    }
    case AST_WHILE:
    case AST_REPEAT: {
      dfs(node->AST_REPEAT.test, depth + 1, visitor, after_visitor);
      dfs(node->AST_REPEAT.body, depth + 1, visitor, after_visitor);
      break;
    }
    case AST_BRANCH: {
      dfs(node->AST_BRANCH.test, depth + 1, visitor, after_visitor);
      dfs(node->AST_BRANCH.consequent, depth + 1, visitor, after_visitor);
      dfs(node->AST_BRANCH.alternate, depth + 1, visitor, after_visitor);
      break;
    }
    case AST_BIN_EXPR: {
      dfs(node->AST_BIN_EXPR.arg1, depth + 1, visitor, after_visitor);
      dfs(node->AST_BIN_EXPR.arg2, depth + 1, visitor, after_visitor);
      break;
    }
    case AST_UN_EXPR: {
      dfs(node->AST_UN_EXPR.argument, depth + 1, visitor, after_visitor);
      break;
    }
    case AST_CALL_OR_IND: {
      dfs(node->AST_CALL_OR_IND.expr, depth + 1, visitor, after_visitor);
      dfs(node->AST_CALL_OR_IND.expr_list, depth + 1, visitor, after_visitor);
      break;
    }
    case AST_FUNC_SIGN: {
      dfs(node->AST_FUNC_SIGN.ident, depth + 1, visitor, after_visitor);
      dfs(node->AST_FUNC_SIGN.arg_list, depth + 1, visitor, after_visitor);
      break;
    }
    case AST_FUNC: {
      dfs(node->AST_FUNC.func_sign, depth + 1, visitor, after_visitor);
      dfs(node->AST_FUNC.body, depth + 1, visitor, after_visitor);
      break;
    }
    case AST_IDENT: {
      dfs(node->AST_IDENT.type, depth + 1,  visitor, after_visitor);
      break;
    }
    case AST_BODY: {
      dfs(node->AST_BODY.statement, depth + 1, visitor, after_visitor);
      break;
    }
    case AST_STMT: {
      dfs(node->AST_STMT.some_node, depth + 1, visitor, after_visitor);
      break;
    }
    case AST_EXPR: {
      dfs(node->AST_EXPR.some_node, depth + 1, visitor, after_visitor);
      break;
    }
    case AST_TYPE_REF: {
      if (node->AST_TYPE_REF.type == TR_IDENT || node->AST_TYPE_REF.type == TR_ARR) {
        dfs(node->AST_TYPE_REF.sub_field, depth + 1, visitor, after_visitor);
      }
      break;
    }
    default: {
      break;
    }
  }
  after_visitor(node, depth);
}

static void dummy(AST *node, size_t depth) { }
static void free_node(AST *node, size_t depth) { free(node); }

void free_ast(AST *root) {
    dfs(root, 0, dummy, free_node);
}
