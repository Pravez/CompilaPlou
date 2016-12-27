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
/*
(int, int, char*) generateCode(conditional_expression e, hashmap<char*, int>* loaded){
    if(e.type == OPERAND){
        operand o = e.union.operand;
        int reg;
        char* code;
        switch(o.type){
            case INT:
                reg = new_register();
                code = load_int(reg, o.operand.int_value);
                return(reg, o.type, code);
            case DOUBLE:
                //mm chose avec load_double
            case VARIABLE:
                if(is_var_loaded(h, o.operand.variable))
                    return(reg_loaded(h, o.operand.variable), type?, "");

reg = new_register();
ccode = load_var(reg, o.operand.variable);
loaded.add(o.operand.variable, reg);
return (reg, ?type?, code);
}
}else{
conditional_expression left = generateCode(e.union.composed.left, loaded);
conditional_expression right = generateCode(e.union.composed.right, loaded);
operator = e.union.composed.operator;
if(left.type != right.type){
AHAHAHHAHAHAHAH
}else{
int reg = new_register();
type = left.type;
code = left.code + right.code + "<reg> =  <op> <left.reg>, <right.reg>";
return (reg, type, code);
}
}
}

char* load_int(int reg, int value){
    return "%<reg> = add i32 0, <value>";
}

char* load_double(int reg, double value){
    return "%<reg> = fadd 0x0000000000000000, <value en je sais pas quoi>";
}

char* load_var(int reg, char* id){
    type = // récupérer le type je sais pas comment
    return "%<reg> = load <type>, <type>* %<id>";
}
 */
char* load_int(int reg, int value){
    char* type_name = type_of(llvm__convert(T_INT));
    char* code = malloc(strlen(type_name) + 12 + 20);
    //"%<reg> = add i32 0, <value>";
    sprintf(code, "%%x%d = add %s 0 %d", reg, type_name, value);
    return code;
}

char* load_double(int reg, double value){
    return ""; //TODO
    //return "%<reg> = fadd 0x0000000000000000, <value en je sais pas quoi>";
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
                ret->type = o->type;
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
        if(left->type != right->type){
            printf("ahahahah. mdr.\n");
        }else{
            ret->reg = new_register();
            ret->type = left->type;
            //TODO check les types (sauf si c'est fait plus tôt ?)
            switch(operator){
                case OP_ADD:
                    break;
                default:
                    break;
            }
            llvm__program_add_line(&ret->code, "<reg> =  <op> <left.reg>, <right.reg>");
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

