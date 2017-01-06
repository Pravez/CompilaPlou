LEX=lex
YACC=bison
CFLAGS=-Wall -std=gnu99 -g
CC=gcc

#if the command asked is run
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

#if the command asked is emcc
ifeq (emcc,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

all:clean parse

emcc:
	emcc $(RUN_ARGS) --js-library ../p5-libraries/p5-wrap.js -s EXPORTED_FUNCTIONS="['_my_draw','_my_setup']" -o ../foo.js

run:
	./parse $(addprefix final-tests/, $(RUN_ARGS))

parse:grammar.c scanner.c type.c tools.c scope.c expression.c llvm_code.c reg_operation.c errors.c external_function.c
	$(CC) $(CFLAGS) -o $@ $^

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
