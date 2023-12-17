#ifndef AST_TRAVERSE_H
#define AST_TRAVERSE_H

#include "ast.h"

typedef void(visit)(AST *node, size_t depth);

void dfs(AST *node, size_t depth, visit visitor, visit after_visitor);

void free_ast(AST *root);

#endif