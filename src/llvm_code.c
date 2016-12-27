#include "llvm_code.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


#include "type.h"
#include "scope.h"
#include "hash.h"

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

char* load_int(int reg, int value){
    char* type_name = type_of(llvm__convert(T_INT));
    char* code = malloc(strlen(type_name) + 12 + 20);
    //"%<reg> = add i32 0, <value>";
    sprintf(code, "%%x%d = add %s 0, %d", reg, type_name, value);
    return code;
}

char* load_double(int reg, double value){
    return ""; //TODO
    //return "%<reg> = fadd 0x0000000000000000, <value en je sais pas quoi>";
}
char* add_regs(int reg_dest, int reg1, int reg2, enum TYPE type){
    char* type_name = type_of(llvm__convert(type));
    char* code = malloc(strlen(type_name) + 16 + 20);
    switch (type) {
        case T_INT:
            sprintf(code, "%%x%d = add %s %%x%d, %%x%d", reg_dest, type_name, reg1, reg2);
            break;
        case T_DOUBLE:
            sprintf(code, "%%x%d = addf %s %%x%d, %%x%d", reg_dest, type_name, reg1, reg2);
    }
    return code;
}
char* mul_regs(int reg_dest, int reg1, int reg2, enum TYPE type){
    char* type_name = type_of(llvm__convert(type));
    char* code = malloc(strlen(type_name) + 16 + 20);
    switch (type) {
        case T_INT:
            sprintf(code, "%%x%d = mul %s %%x%d, %%x%d", reg_dest, type_name, reg1, reg2);
            break;
        case T_DOUBLE:
            sprintf(code, "%%x%d = mulf %s %%x%d, %%x%d", reg_dest, type_name, reg1, reg2);
    }
    return code;
}

char* operate_on_regs(enum COND_OPERATOR op, int reg_dest, int reg1, int reg2, enum TYPE type){
    switch (op){
        case OP_ADD:
            return add_regs(reg_dest, reg1, reg2, type);
        case OP_MUL:
            return mul_regs(reg_dest, reg1, reg2, type);
        default:
            return "TODO operation on regs :-)";
    }
}

char* load_var(int reg, char* id){
    enum TYPE type = hash__get_item(&scope, id).declarator.variable.type;
    char* type_name = type_of(llvm__convert(type));
    char* code = malloc(strlen(id) + strlen(type_name)*2 + 16 + 10);
    sprintf(code, "%%x%d = load %s, %s* %%%s", reg, type_name, type_name, id);
    return code;
}

struct computed_expression* generate_code(struct Expression* e, hash_t* loaded){
    struct computed_expression* ret = malloc(sizeof(struct computed_expression));
    llvm__init_program(&ret->code);

    if(e->type == E_CONDITIONAL && e->conditional_expression.type == C_LEAF){
        struct expr_operand* o = &e->conditional_expression.leaf;
        switch(o->type){
            case O_INT:
                ret->reg = new_register();
                ret->type = T_INT;
                llvm__program_add_line(&ret->code,load_int(ret->reg, o->operand.int_value));
            case O_DOUBLE:
                //mm chose avec load_double
                break;
            case O_VARIABLE:
                ret->reg = hash_lookup(loaded, o->operand.variable);
                ret->type = hash__get_item(&scope, o->operand.variable).declarator.variable.type;
                if(ret->reg == HASH_FAIL) {
                    ret->reg = new_register();
                    llvm__program_add_line(&ret->code,load_var(ret->reg, o->operand.variable));
                    hash_insert(loaded,o->operand.variable, ret->reg); // new register of variable
                }
        }
    }else if(e->type == E_CONDITIONAL){
        struct computed_expression* left = generate_code(e->conditional_expression.branch.e_left, loaded);
        struct computed_expression* right = generate_code(e->conditional_expression.branch.e_right, loaded);
        enum COND_OPERATOR operator = e->conditional_expression.branch.operator;
        llvm__fusion_programs(&ret->code, &left->code);
        llvm__fusion_programs(&ret->code, &right->code);

        if(left->type != right->type){
            printf("ahahahah. mdr. la conversion de type, t'es un rigolo toi.\n");
        }else{
            ret->reg = new_register();
            ret->type = left->type;

            llvm__program_add_line(&ret->code, operate_on_regs(operator, ret->reg, left->reg, right->reg, ret->type));
        }
    }else{
        printf("démerde toi :-)\n");
        llvm__program_add_line(&ret->code, "démerde toi :-)");
    }
    return ret;
}

void llvm__print(struct llvm__program* program){
    for(int i = 0; i < program->line_number; ++i){
        printf("%s\n", program->code[i]);
    }
}

