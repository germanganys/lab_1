#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "mylang.tab.h"
#include "mylang.lex.h"
#include "lib/ast/ast.h"
#include "lib/ast/ast_printer.h"


static const char * const HELP_MESSAGE = "USAGE"
                                    "\n\t./lab1 [-h] [-f <input file>]"
                                    "\nARGUMENTS"
                                    "\n\t-h, --help # show help message"
                                    "\n\t-f, --file # specify the input file, default: stdin\n"
                                    ;

static const struct option ARG_OPTS[] = {
        { "file",    required_argument, NULL, 'f' },
        { "help",    no_argument,       NULL, 'h' },
};

int scan_input_args(int argc, char** argv, yyscan_t *scanner) {
    int option_index = 0; char brief_option;
    brief_option = getopt_long(argc, argv, "vhf:o:", ARG_OPTS, &option_index);
    switch (brief_option) {
        case 'h':
            printf(HELP_MESSAGE); return 1;
        case 'f':
            yyset_in(fopen(optarg, "r"), *scanner); return 0;
        case -1:
            return 0;
        default: {
            fprintf(stderr, "can't open file"); return 1;
        }
    }
}

int main(int argc, char** argv) {
    yyscan_t scanner;
    int i;
    if ((i = yylex_init(&scanner)) != 0)
        exit(i);

    int result = scan_input_args(argc, argv, &scanner);
    if (result != 0)
        return -1;

    struct AST *root = NULL;
    char *err_msg = NULL;
	int err = yyparse(&root, &err_msg, scanner);
	
	if (err == 0) {
		print_ast(root);
		free_ast(root);
	} else {
        fprintf(stderr, "%s\nerr code=%d\n", err_msg, err);
	}
	yylex_destroy(scanner);
	return err;
}