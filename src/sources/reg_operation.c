#define _GNU_SOURCE
#include "reg_operation.h"

#include <stdio.h>
#include "llvm_code.h"

#define TO_LLVM_STRING(type) type_of(llvm__convert(type))
#define IS_GLOBAL(ptr_scope, var_id) (scope__get_declarator(ptr_scope, var_id).declarator.variable.is_global)

char* load_int(int reg, int value, short int negative){
    char* type_name = type_of(llvm__convert(T_INT));
    char* code;
    asprintf(&code, "%%x%d = %s %s 0, %d", reg, negative ? "sub" : "add", type_name, value);
    return code;
}

char* load_double(int reg, double value, short int negative){
    char* type_name = type_of(llvm__convert(T_DOUBLE));
    char* code;
    asprintf(&code, "%%x%d = %s %s 0x0000000000000000, 0x%llx", reg, negative ? "fsub" : "fadd", type_name, *(unsigned long long *)&value);
    return code;
}

char* call_function(int reg, char* function_name, enum TYPE func_type, enum TYPE* args_types, int* args_regs, int args_qty){
    char* code;
    char* args;
    char reg_code[10];
    reg_code[0] = '\0';
    if(reg != -1){
        sprintf(reg_code, "%%x%d = ", reg);
    }

    if(args_qty == 0) {
        args = "";
    }else {
        args = "\0";
        for (int i = 0; i < args_qty; i++) {
            char char_reg[10];
            char_reg[0] = '\n';
            sprintf(char_reg, "%d", args_regs[i]);
            if (i == args_qty - 1)
                args = concatenate_strings(4, args, type_of(llvm__convert(args_types[i])), " %x", char_reg);
            else
                args = concatenate_strings(5, args, type_of(llvm__convert(args_types[i])), " %x", char_reg, ", ");
        }
    }

    asprintf(&code, "%scall %s @%s(%s)", reg_code, type_of(llvm__convert(func_type)), function_name, args);
    return code;
}

char* alloca_func_param(struct Variable variable){
    char* new_var;
    asprintf(&new_var, "%s.addr", variable.identifier);

    char* code;
    char* llvm_type_of = type_of(llvm__convert(variable.type));
    asprintf(&code, "%%%s = alloca %s\nstore %s %%%s, %s* %%%s", new_var, llvm_type_of, llvm_type_of, variable.identifier, llvm_type_of, new_var);

    variable.identifier = new_var;
    struct Declarator declarator = { .declarator.variable = variable, .decl_type = VARIABLE };
    //Supposed to always return true ... but we are not going to check
    scope__add_individual_function_item(&scope, declarator);

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

char* binary_op_on_reg_const(enum REG_BINARY_OP op, int reg_dest, int reg1, double value, enum TYPE type){
    char *llvm_op = binary_op_to_llvm_op(op, type);
    char *type_name = TO_LLVM_STRING(type);
    char *code;
    char* str_value;

    if(type == T_INT)
        asprintf(&str_value, "%d", (int) value);
    else
        asprintf(&str_value, "%f", value);

    asprintf(&code, "%%x%d = %s %s %%x%d, %s ; coucou", reg_dest, llvm_op, type_name, reg1, str_value);

    free(llvm_op);
    free(str_value);
    return code;
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
            return -1;
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
    asprintf(&code, "; comparison\n%%t%d = %s %s %s %%x%d, %%x%d\n%%x%d = select i1 %%t%d, %s %s, %s %s",
             /*                    %tx = icmp eq i32 %xy %xz      %xd  =            %%tx, i32 0, i32 0           */
             /*                                                                                                  */
             /*  %xx                icmp                    eq      i32      %xy  %xz                            */
             temp_reg, (type == T_INT)? "icmp" : "fcmp", llvm_op, type_name, reg1, reg2,
             /*  %xd      %xx      i32                  0                   i32                0                 */
             //reg_dest, temp_reg, type_name, (type == T_INT)? "1" : "1.0", type_name, (type == T_INT)? "0" : "0.0");
             reg_dest, temp_reg, type_name, (type == T_INT)? "1" : "0x3ff0000000000000", type_name, (type == T_INT)? "0" : "0x0000000000000000");

    return code;
}

/**** LOGICAL OP *****/
//PRIVATE FUNCTION
enum REG_LOGICAL_OP cond_op_to_logical_op(enum COND_OPERATOR o){
    switch (o){
        case OP_AND:
            return REG_LAND;
        case OP_OR:
            return REG_LOR;
        default:
            return -1; // Error. Should have call is_logical_op first.
    }
}

char* logical_op_to_llvm_op(enum REG_LOGICAL_OP op){
    switch (op){
        case REG_LAND:
            return "and";
        case REG_LOR:
            return "or";
        default:
            return "UNKNOWN_OPERATION";
    }
}

char* logical_op_on_regs(enum REG_LOGICAL_OP op, int reg_dest, int reg1, int reg2, enum TYPE type){
    char *llvm_op = logical_op_to_llvm_op(op);
    char *type_name = TO_LLVM_STRING(type);
    char *code;
    int temp_reg1 = new_register();
    int temp_reg2 = new_register();
    asprintf(&code, "; logical %s\n%%t%d = %s %s %%x%d, %%x%d\n%%t%d = icmp ne i32 0, %%t%d\n%%x%d = select i1 %%t%d, i32 1, i32 0",
             /*                    %xt1= and i32 %xy %xz        %xt2  =                %xt1  %dest              %xt2  */
             llvm_op,
             /* %xt1       and       i32     %xy   %xz                                                                */
             temp_reg1, llvm_op, type_name, reg1, reg2,
             /* %xt2       %xt1                                                                                       */
             temp_reg2, temp_reg1,
             /* %dest      %xt2                                                                                       */
             reg_dest, temp_reg2);

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
    if(is_binary_op(op)) {
        return binary_op_on_regs(cond_op_to_binary_op(op), reg_dest, reg1, reg2, type);
    }
    else if(is_bitwise_op(op))
        return bitwise_op_on_regs(cond_op_to_bitwise_op(op), reg_dest, reg1, reg2, type);
    else if(is_comparison_op(op)){
        return comparison_op_on_regs(cond_op_to_comparison_op(op, type), reg_dest, reg1, reg2, type);
    }else if(is_logical_op(op)){
        return logical_op_on_regs(cond_op_to_logical_op(op), reg_dest, reg1, reg2, type);
    }else{
        char* res;
        asprintf(&res, "ERREUR, UNKNOW OPERATION <> <>");
        return res;
    }
}

char* declare_var(char* id, enum TYPE type, struct global_declaration global){
    char* ret;
    if(global.is_global)
        if(type == T_INT)
            asprintf(&ret,"@%s = common global %s %d", id, type_of(llvm__convert(type)), global.int_value);
        else
            asprintf(&ret,"@%s = common global %s 0x%llx", id, type_of(llvm__convert(type)), *(unsigned long long *)&global.double_value);
    else
        asprintf(&ret,"%%%s = alloca %s", id, TO_LLVM_STRING(type));
    return ret;
}

char* load_var(int reg, char* id){
    enum TYPE type = scope__get_declarator(&scope, id).declarator.variable.type;
    char* type_name = type_of(llvm__convert(type));
    char* code;
    asprintf(&code, "%%x%d = load %s, %s* %s%s",
             reg, type_name, type_name, IS_GLOBAL(&scope, id)?"@":"%",id);
    return code;
}

char* store_var(char* id, int reg, enum TYPE type){
    char* type_name = type_of(llvm__convert(type));
    char* code;
    asprintf(&code, "store %s %%x%d, %s* %s%s", type_name, reg, type_name, IS_GLOBAL(&scope, id)?"@":"%",id);
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
    return "ERROR COMP TO STRING '245254244'";
}

char* label_to_string(int label, int br_precedes, char* comment){
    char* code;
    asprintf(&code, "%s%s%s%slabel%d:", comment != NULL ? comment : "", comment != NULL ? "\n" : "",
             br_precedes ? jump_to(label) : "", br_precedes ? "\n" : "", label);
    return code;
}

char* true_comp(int reg){
    char* cmp;
    asprintf(&cmp, "%%x%d = fcmp true double 0.0, 0.0", reg);
    return cmp;
}

char* jump_to(int label){
    char* jump;
    asprintf(&jump, "br label %%label%d", label);
    return jump;
}

char* convert_reg(int reg_src, enum TYPE ty_src, int reg_dest, int ty_dest){
    char* ret;
    asprintf(&ret, "%%x%d = %s %s %%x%d to %s",
             reg_dest, (ty_dest == T_INT)? "fptosi" : "sitofp",
             TO_LLVM_STRING(ty_src), reg_src, TO_LLVM_STRING(ty_dest));
    return ret;
}

char* return_expr(int reg, enum TYPE type){
    char* ret;
    asprintf(&ret, "ret %s %%x%d", TO_LLVM_STRING(type), reg);
    return ret;
}

char* invert_value(int reg_src, enum TYPE type, int reg_dest){
    char* ret;
    if(type == T_INT)
        asprintf(&ret, "%%x%d = sub i32 0, %%x%d", reg_dest, reg_src);
    else
        asprintf(&ret, "%%x%d = fsub double 0.0, %%x%d", reg_dest, reg_src);
    return ret;
}
