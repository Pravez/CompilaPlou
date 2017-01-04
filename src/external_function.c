#include "external_function.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void init_external_functions_declaration(){
    external_functions.functions_number = 0;
    external_functions.size = DEFAULT_EXTERN_FUNC_QTY;
    external_functions.extern_functions = malloc(sizeof(struct Function)*DEFAULT_EXTERN_FUNC_QTY);
}

void add_external_function(struct Function func){
    if(external_functions.functions_number == external_functions.size -1) {
        external_functions.size *= 2;
        external_functions.extern_functions = realloc(external_functions.extern_functions,
                                                        sizeof(struct extern_function) * external_functions.size);
    }

    external_functions.extern_functions[external_functions.functions_number].function = func;
    external_functions.extern_functions[external_functions.functions_number++].to_add = 0;
}

void add_p5_functions(){
    register_external_function("createCanvas", T_VOID, 2, T_DOUBLE, T_DOUBLE);
    register_external_function("background", T_VOID, 1, T_DOUBLE);
    register_external_function("fill", T_VOID, 1, T_DOUBLE);
    register_external_function("stroke", T_VOID, 1, T_DOUBLE);
    register_external_function("point", T_VOID, 2, T_DOUBLE, T_DOUBLE);
    register_external_function("line", T_VOID, 4, T_DOUBLE, T_DOUBLE, T_DOUBLE, T_DOUBLE);
    register_external_function("ellipse", T_VOID, 4, T_DOUBLE, T_DOUBLE, T_DOUBLE, T_DOUBLE);
    register_external_function("log10", T_DOUBLE, 1, T_DOUBLE);
    register_external_function("cos", T_DOUBLE, 1, T_DOUBLE);
    register_external_function("sin", T_DOUBLE, 1, T_DOUBLE);
}

void register_external_function(char* name, enum TYPE return_type, int args_qty, ...){
    va_list args_list;
    va_start(args_list, args_qty);

    struct Function func;
    func.identifier = name;
    func.return_type = return_type;
    func.var_list_size = args_qty;

    for(int i = 0;i < args_qty; i++){
        func.var_list[i] = (struct Variable){ .type = va_arg(args_list, enum TYPE) };
    }

    add_external_function(func);
}

int add_if_registered_as_external(struct Scope *hashmap, char* name){
    for(int i = 0;i < external_functions.functions_number;i++){
        if(strcmp(name, external_functions.extern_functions[i].function.identifier) == 0){
            external_functions.extern_functions[i].to_add = 1;
            return hash__add_item_extern_function(hashmap, name, (struct Declarator){
                .decl_type = FUNCTION, .declarator.function = external_functions.extern_functions[i].function });
        }
    }

    return 0;
}

int is_registered_external(char* function){
    for(int i=0;i< external_functions.functions_number;i++){
        if(strcmp(function, external_functions.extern_functions[i].function.identifier) == 0)
            return 1;
    }

    return 0;
}