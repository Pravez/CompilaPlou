LEX=lex
YACC=bison
CFLAGS=-Wall -std=gnu99
CC=gcc

all:clean parse

parse:grammar.c scanner.c type.c tools.c hash.c expression.c llvm_code.c
	$(CC) $(CFLAGS) -o $@ $^

grammar.c:grammar.y
	$(YACC) -t -d -o $@ --defines=grammar.tab.h $^

%.c:%.l
	$(LEX) -o $@ $^

clean:
	rm -f grammar.c scanner.c *~
