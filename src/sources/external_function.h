#ifndef _P5_LIB_H_
#define _P5_LIB_H_

#include "type.h"
#include "scope.h"


#define DEFAULT_EXTERN_FUNC_QTY 10

struct extern_function{
    struct Function function;
    int to_add;
};

struct external_functions_declarations{
    struct extern_function* extern_functions;
    int functions_number;
    int size;
};

struct external_functions_declarations external_functions;

void init_external_functions_declaration();
void add_external_function(struct Function func);
void register_external_function(char* name, enum TYPE return_type, int args_qty, ...);
int add_if_registered_as_external(struct Scope* hashmap, char* name);
int is_registered_external(char* function);

void add_p5_functions();

#endif