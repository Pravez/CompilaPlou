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

short int is_binary_op(enum COND_OPERATOR o){
    return (o == OP_ADD  ||
            o == OP_SUB  ||
            o == OP_MUL  ||
            o == OP_DIV  ||
            o == OP_SSHL ||
            o == OP_SSHR );
}

//PRIVATE FUNCTION
enum REG_BINARY_OP cond_op_to_binary_op(enum COND_OPERATOR o){
    switch (o){
        case OP_ADD:
            return REG_ADD;
        case OP_SUB:
            return REG_SUB;
        case OP_MUL:
            return REG_MUL;
        case OP_DIV:
            return REG_DIV;
        case OP_SSHL:
            return REG_SHL;
        case OP_SSHR:
            return REG_SHR;
        default:
            return -1; // Error. Should have call is_binary_op first.
    }
}

char* binary_op_to_llvm_op(enum REG_BINARY_OP op, enum TYPE type){
    char* llvm_op;
    switch (op){
        case REG_ADD:
            asprintf(&llvm_op, "%s", (type==T_INT)?  "add" : "fadd"); break;
        case REG_SUB:
            asprintf(&llvm_op, "%s", (type==T_INT)?  "sub" : "fsub"); break;
        case REG_MUL:
            asprintf(&llvm_op, "%s", (type==T_INT)?  "mul" : "fmul"); break;
        case REG_DIV:
            asprintf(&llvm_op, "%s", (type==T_INT)? "sdiv" : "fdiv"); break;
        case REG_REM:
            asprintf(&llvm_op, "%s", (type==T_INT)? "srem" : "frem"); break;
        default:
            asprintf(&llvm_op, "%s", "UNKNOWN_OPERATION");
    }
    return llvm_op;
}
char* binary_op_on_regs(enum REG_BINARY_OP op, int reg_dest, int reg1, int reg2, enum TYPE type){
    char *llvm_op = binary_op_to_llvm_op(op, type);
    char *type_name = TO_LLVM_STRING(type);
    char *code;
    asprintf(&code, "%%x%d = %s %s %%x%d, %%x%d", reg_dest, llvm_op, type_name, reg1, reg2);
    free(llvm_op);
    return code;
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