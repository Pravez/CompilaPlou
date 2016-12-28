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

struct computed_expression* generate_code(struct Expression* e){
    struct computed_expression* ret = malloc(sizeof(struct computed_expression));
    llvm__init_program(&ret->code);

    if(e->type == E_CONDITIONAL && e->conditional_expression.type == C_LEAF){
        struct expr_operand* o = &e->conditional_expression.leaf;
        switch(o->type){
            case O_INT:
                ret->reg = new_register();
                ret->type = T_INT;
                llvm__program_add_line(&ret->code,load_int(ret->reg, o->operand.int_value));
                break;
            case O_DOUBLE:
                ret->reg = new_register();
                ret->type = T_DOUBLE;
                llvm__program_add_line(&ret->code,load_double(ret->reg, o->operand.double_value));
                break;
            case O_VARIABLE:
                ret->reg = hash_lookup(&CURRENT_LOADED_REGS, o->operand.variable);
                ret->type = hash__get_item(&scope, o->operand.variable).declarator.variable.type;
                if(ret->reg == HASH_FAIL) {
                    ret->reg = new_register();
                    llvm__program_add_line(&ret->code,load_var(ret->reg, o->operand.variable));
                    hash_insert(&CURRENT_LOADED_REGS,o->operand.variable, ret->reg); // new register of variable
                }
        }
    }else if(e->type == E_CONDITIONAL){
        struct computed_expression* left = generate_code(e->conditional_expression.branch.e_left);
        struct computed_expression* right = generate_code(e->conditional_expression.branch.e_right);
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
        free(left);
        free(right);
    }else if(e->type == E_AFFECT){
        //register is no longer up to date.
        hash_delete(&CURRENT_LOADED_REGS, e->expression.operand.operand.variable);
        struct computed_expression* affected_value = generate_code(e->expression.cond_expression);
        ret->reg = affected_value->reg;
        ret->type = hash__get_item(&scope, e->expression.operand.operand.variable).declarator.variable.type;
        //check même type ?
        llvm__fusion_programs(&ret->code, &affected_value->code);
        switch (e->expression.assign_operator)
        {
            case OP_SIMPLE_ASSIGN:
                llvm__program_add_line(&ret->code, store_var(e->expression.operand.operand.variable, ret->reg, ret->type));
                break;
            case OP_MUL_ASSIGN:
            case OP_DIV_ASSIGN:
            case OP_REM_ASSIGN:
            case OP_SHL_ASSIGN:
            case OP_SHR_ASSIGN:
            case OP_ADD_ASSIGN:
            case OP_SUB_ASSIGN:
                printf("TODO\n");
                llvm__program_add_line(&ret->code, "TODO ASSIGN");
            default:
                printf("erreur. Enfin, je crois\n");
        }
        free(affected_value);
    }else{
        printf("erreur. Je suppose.\n");
    }
    return ret;
}

void llvm__print(struct llvm__program* program){
    for(int i = 0; i < program->line_number; ++i){
        printf("%s\n", program->code[i]);
    }
}

