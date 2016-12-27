#include "llvm_code.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define TO_LLVM_STRING(type) type_of(llvm__convert(type))

void llvm__init_program(struct llvm__program* program){
    program->line_number = 0;
    program->validity = 1;
    program->size = DEFAULT_SIZE;
    program->code = malloc(sizeof(char*) * DEFAULT_SIZE);
}

int llvm__program_add_line(struct llvm__program* program, char* line){
    program->code[program->line_number++] = line;

    if(program->line_number == program->size - 1){
        program->size*= 2;
        program->code = realloc(program->code, (size_t) program->size);
    }

    return 1;
}

char* llvm__create_constant(char* name, enum TYPE type, int size, void* value){
    char* constant;
    char* type_char = type_of(llvm__convert(type));
    asprintf(&constant, "@%s = constant [%d x %s] c\"%s\"", name, size, type_char, (char*) value);

    return constant;
}

void llvm__fusion_programs(struct llvm__program* main, const struct llvm__program* toappend){
    for(int i=0;i<toappend->line_number;i++){
        llvm__program_add_line(main, toappend->code[i]);
    }
}

char* llvm___create_function_def(struct Function function){
    char *definition;

    char* args = malloc(sizeof(char) * 256);

    for(int i=0;i< function.var_list_size;i++){
        strcat(args, TO_LLVM_STRING(function.var_list[i].type));
        strcat(args, " %");
        strcat(args, function.var_list[i].identifier);
        if(i != function.var_list_size - 1)
            strcat(args, ", ");
    }

    asprintf(&definition, "define %s @%s(%s)", TO_LLVM_STRING(function.return_type), function.identifier, args);


    return definition;
}

char* type_of(enum LLVM_TYPE type){
    switch(type){
        case LLVM_I32:
            return "i32";
        case LLVM_DOUBLE:
            return "double";
        case LLVM_I8:
            return "i8";
        default:
            break;
    }

    return NULL;
}

enum LLVM_TYPE llvm__convert(enum TYPE type){
    switch(type){
        case T_INT:
            return LLVM_I32;
        case T_DOUBLE:
            return LLVM_DOUBLE;
        case T_VOID:
            break;
    }

    return -1;
}