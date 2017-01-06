EMCC=usr/lib/emscripten/emcc

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

#if the command asked is compll
ifeq (compll,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

all:
	make -C sources/ all

clean:
	make -C sources/ clean
	rm ./parse

run:
	./parse $(addprefix final-tests/, $(RUN_ARGS))

compll:
	lli $(RUN_ARGS)

emcc:
	$(EMCC) $(RUN_ARGS) --js-library ../p5-libraries/p5-wrap.js -s EXPORTED_FUNCTIONS="['_my_draw','_my_setup']" -o ../foo.js

