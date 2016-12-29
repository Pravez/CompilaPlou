#ifndef _REGOPERATION_H_
#define _REGOPERATION_H_

#include "type.h"
#include "scope.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

enum REG_BINARY_OP{
    REG_ADD,    /*  add or fadd will be used */
    REG_SUB,    /*  sub or fsub will be used */
    REG_MUL,    /*  mul or fmul will be used */
    REG_DIV,    /* sdiv or fdiv will be used */
    REG_REM,    /* srem of frem will be used */
//    REG_UKN     /* unknown operation (basically means something got wrong) */
};

enum REG_BITWISE_OP{
    REG_SHL,    /* << in c */
    REG_SHR,    /* >> in c    NB: the arithmetic one will be used (ashr in llvm) */
    REG_AND,    /*  & in c */
    REG_OR,     /*  | in c */
    REG_XOR     /*  ~ in c */
};


char* load_int(int reg, int value);
char* load_double(int reg, double value);
short int is_binary_op(enum COND_OPERATOR o);

char* binary_op_on_regs(enum REG_BINARY_OP op, int reg_dest, int reg1, int reg2, enum TYPE type);

char* declare_var(char* id, enum TYPE type, short int is_global);
char* load_var(int reg, char* id);
char* store_var(char* id, int reg, enum TYPE type);


#endif