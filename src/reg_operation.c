#include "reg_operation.h"
#include <stdio.h>

#define TO_LLVM_STRING(type) type_of(llvm__convert(type))

char* load_int(int reg, int value){
    char* type_name = type_of(llvm__convert(T_INT));
    char* code = malloc(strlen(type_name) + 12 + 20);
    //"%<reg> = add i32 0, <value>";
    sprintf(code, "%%x%d = add %s 0, %d", reg, type_name, value);
    return code;
}

char* load_double(int reg, double value){
    char* type_name = type_of(llvm__convert(T_DOUBLE));
    char* code;
    asprintf(&code, "%%x%d = fadd %s 0x0000000000000000, 0x%llx", reg, type_name, *(unsigned long long *)&value);
    return code;
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

char* declare_var(char* id, enum TYPE type, short int is_global){
    char* ret;
    if(is_global)
        asprintf(&ret,"TODO déclaration globale de %s", id); //TODO declaration globale
    else
        asprintf(&ret,"%%%s = alloca %s", id, TO_LLVM_STRING(type));
    return ret;
}
char* load_var(int reg, char* id){
    enum TYPE type = hash__get_item(&scope, id).declarator.variable.type;
    char* type_name = type_of(llvm__convert(type));
    char* code = malloc(strlen(id) + strlen(type_name)*2 + 16 + 10);
    sprintf(code, "%%x%d = load %s, %s* %%%s", reg, type_name, type_name, id);
    return code;
}

char* store_var(char* id, int reg, enum TYPE type){
    char* type_name = type_of(llvm__convert(type));
    char* code = malloc(sizeof(type_name)*2 + 15 + 10);
    sprintf(code, "store %s %%x%d, %s* %%%s", type_name, reg, type_name, id);
    return code;
}