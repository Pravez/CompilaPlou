LEX=lex
YACC=bison
CFLAGS=-Wall -std=gnu99 -g
CC=gcc


all:clean parse

parse:grammar.c scanner.c type.c tools.c scope.c expression.c llvm_code.c reg_operation.c errors.c external_function.c
	$(CC) $(CFLAGS) -o ../$@ $^

grammar.c:grammar.y
	$(YACC) -t -d -o $@ --defines=grammar.tab.h $^

%.c:%.l
	$(LEX) -o $@ $^

srcTest: test-source-code/tst-tools.c tools.c
	$(CC) $(CFLAGS) -o $@ $^


clean:
	rm -f grammar.c scanner.c *~
	rm -f grammar.tab.h
	rm -f srcTest
	rm -f parse
