Bison = bison
Flex = flex

CFLAGS = -Wunused-function -O0

all: clear build_prep parser lexer mylibs
	gcc ./build/mylang.tab.o ./build/mylang.lex.o ./build/ast.o ./build/traverse.o ./build/printer.o ./build/main.o -o lab1

build_prep:
	mkdir -p build

parser:
	$(Bison) -Wcounterexamples -Wother -b mylang -d bison.y

lexer:
	$(Flex) -o mylang.lex.c --header-file=mylang.lex.h flex.l

mylibs:
	gcc -c mylang.tab.c -o build/mylang.tab.o $(CFLAGS)
	gcc -c mylang.lex.c -o build/mylang.lex.o $(CFLAGS)
	gcc -c ./lib/ast/ast.c -o build/ast.o $(CFLAGS)
	gcc -c ./lib/ast/ast_traverse.c -o build/traverse.o $(CFLAGS)
	gcc -c ./lib/ast/ast_printer.c -o  build/printer.o $(CFLAGS)
	gcc -c main.c -o build/main.o

clear:
	rm -f build/*.o mylang.* lab1

