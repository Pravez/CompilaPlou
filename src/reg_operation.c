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

/**** BINARY OP *****/
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

/**** BITWISE OP *****/
//PRIVATE FUNCTION
enum REG_BITWISE_OP cond_op_to_bitwise_op(enum COND_OPERATOR o){
    switch (o){
        case OP_SSHL:
            return REG_SHL;
        case OP_SSHR:
            return REG_SHR;
        default:
            return -1; // Error. Should have call is_bitwise_op first.
    }
}

char* bitwise_op_to_llvm_op(enum REG_BITWISE_OP op){
    char* llvm_op;
    switch (op){
        case REG_SHL:
            asprintf(&llvm_op, "%s",  "shl"); break;
        case REG_SHR:
            asprintf(&llvm_op, "%s", "ashr"); break;
        case REG_AND:
            asprintf(&llvm_op, "%s",  "and"); break;
        case REG_OR:
            asprintf(&llvm_op, "%s",   "or"); break;
        case REG_XOR:
            asprintf(&llvm_op, "%s",  "xor"); break;
        default:
            asprintf(&llvm_op, "%s", "UNKNOWN_OPERATION");
    }
    return llvm_op;
}

char* bitwise_op_on_regs(enum REG_BITWISE_OP op, int reg_dest, int reg1, int reg2, enum TYPE type){
    char *llvm_op = bitwise_op_to_llvm_op(op);
    char *type_name = TO_LLVM_STRING(type);
    char *code;
    asprintf(&code, "%%x%d = %s %s %%x%d, %%x%d", reg_dest, llvm_op, type_name, reg1, reg2);
    free(llvm_op);
    return code;
}

/**** COMPARISON OP *****/
//PRIVATE FUNCTION
union COMPARATOR cond_op_to_comparison_op(enum COND_OPERATOR o, enum TYPE type){
    switch (o){
        case OP_LE:
            return (type == T_INT) ?
                   (union COMPARATOR){.icmp = ICOMP_SLE}:
                   (union COMPARATOR){.fcmp = FCOMP_OLE};
        case OP_GE:
            return (type == T_INT) ?
                   (union COMPARATOR){.icmp = ICOMP_SGE}:
                   (union COMPARATOR){.fcmp = FCOMP_OGE};
        case OP_EQ:
            return (type == T_INT) ?
                   (union COMPARATOR){.icmp = ICOMP_EQ}:
                   (union COMPARATOR){.fcmp = FCOMP_OEQ};
        case OP_NE:
            return (type == T_INT) ?
                   (union COMPARATOR){.icmp = ICOMP_NE}:
                   (union COMPARATOR){.fcmp = FCOMP_ONE};
        case OP_SHL:
            return (type == T_INT) ?
                   (union COMPARATOR){.icmp = ICOMP_SLT}:
                   (union COMPARATOR){.fcmp = FCOMP_OLT};
        case OP_SHR:
            return (type == T_INT) ?
                   (union COMPARATOR){.icmp = ICOMP_SGT}:
                   (union COMPARATOR){.fcmp = FCOMP_OGT};
        default:
            return (union COMPARATOR){.icmp = -1}; // Error. Should have call is_comparison_op first.
    }
}

char* comparison_op_on_regs(union COMPARATOR op, int reg_dest, int reg1, int reg2, enum TYPE type){
    char *llvm_op = comparator_to_string(op, (type == T_DOUBLE));
    char *type_name = TO_LLVM_STRING(type);
    char *code;
    int temp_reg = new_register();
    asprintf(&code, "%%x%d = %s %s %s %%x%d, %%x%d\n%%x%d = select i1 %%x%d, %s %s, %s %s",
             /*       %xx = icmp eq i32 %xy %xz      %xd  =            %%xx, i32 0, i32 0           */
             /*                                                                                     */
             /*  %xx                icmp                    eq      i32      %xy  %xz               */
             temp_reg, (type == T_INT)? "icmp" : "fcmp", llvm_op, type_name, reg1, reg2,
             /*  %xd      %xx      i32                  0                   i32                0    */
             //reg_dest, temp_reg, type_name, (type == T_INT)? "1" : "1.0", type_name, (type == T_INT)? "0" : "0.0");
             reg_dest, temp_reg, type_name, (type == T_INT)? "1" : "0x3ff0000000000000", type_name, (type == T_INT)? "0" : "0x0000000000000000");

    return code;
}

/**** CHECK REG OPERATIONS *****/

short int is_binary_op(enum COND_OPERATOR o){
    return (o == OP_ADD  ||
            o == OP_SUB  ||
            o == OP_MUL  ||
            o == OP_DIV  );
}

short int is_bitwise_op(enum COND_OPERATOR o){
    return (o == OP_SSHL  ||
            o == OP_SSHR  );
}
short int is_comparison_op(enum COND_OPERATOR o){
    return (o == OP_LE  ||
            o == OP_GE  ||
            o == OP_EQ  ||
            o == OP_NE  ||
            o == OP_SHL ||
            o == OP_SHR );
}
short int is_logical_op(enum COND_OPERATOR o){
    return (o == OP_AND ||
            o == OP_OR  );
}
/****  OPERATION ON REG ****/
char* operation_on_regs(enum COND_OPERATOR op, int reg_dest, int reg1, int reg2, enum TYPE type){
    printf("OPERATOR: %d\n", op);
    if(is_binary_op(op)) {
        return binary_op_on_regs(cond_op_to_binary_op(op), reg_dest, reg1, reg2, type);
    }
    else if(is_bitwise_op(op))
        return bitwise_op_on_regs(cond_op_to_bitwise_op(op), reg_dest, reg1, reg2, type);
    else if(is_comparison_op(op)){
        return comparison_op_on_regs(cond_op_to_comparison_op(op, type), reg_dest, reg1, reg2, type);
    }else if(is_logical_op(op)){
        char* res;
        asprintf(&res, "TODO LOGICAL OP <> <>");
        return res;
    }else{
        char* res;
        asprintf(&res, "ERREUR, UNKNOW OPERATION <> <>");
        return res;
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

char* comparator_to_string(union COMPARATOR comparator, int is_float){
    //float 1 int 0
    if(is_float) {
        switch (comparator.fcmp) {
            case FCOMP_FALSE:
                return "false";
            case FCOMP_OEQ:
                return "oeq";
            case FCOMP_OGT:
                return "ogt";
            case FCOMP_OGE:
                return "oge";
            case FCOMP_OLT:
                return "olt";
            case FCOMP_OLE:
                return "ole";
            case FCOMP_ONE:
                return "one";
            case FCOMP_ORD:
                return "ord";
            case FCOMP_UEQ:
                return "ueq";
            case FCOMP_ULT:
                return "ult";
            case FCOMP_ULE:
                return "ule";
            case FCOMP_UNE:
                return "une";
            case FCOMP_UNO:
                return "uno";
            case FCOMP_TRUE:
                return "true";
        }
    }else{
        switch (comparator.icmp){
            case ICOMP_EQ:
                return "eq";
            case ICOMP_NE:
                return "ne";
            case ICOMP_UGT:
                return "ugt";
            case ICOMP_UGE:
                return "uge";
            case ICOMP_ULT:
                return "ult";
            case ICOMP_ULE:
                return "ule";
            case ICOMP_SGT:
                return "sgt";
            case ICOMP_SGE:
                return "sge";
            case ICOMP_SLT:
                return "slt";
            case ICOMP_SLE:
                return "sle";
        }
    }
    debug("ERROR COMP TO STRING '245254244'", RED);
    return "ERROR COMP TO STRING '245254244'";
}

char* label_to_string(int label){
    char* code;
    asprintf(&code, "label%d:", label);
    return code;
}

char* true_comp(int reg){
    char* cmp;
    asprintf(&cmp, "%%x%d = fcmp true double 0.0, 0.0");
    return cmp;
}

char* jump_to(int label){
    char* jump;
    asprintf(&jump, "br label %%label%d", label);
    return jump;
}