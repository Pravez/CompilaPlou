#ifndef _REGOPERATION_H_
#define _REGOPERATION_H_

#include "type.h"
#include "scope.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

enum ICOMPARATOR{
    ICOMP_EQ, //Equal
    ICOMP_NE, //Not equal
    ICOMP_UGT, //Unsigned greater than
    ICOMP_UGE, //Unsigned greater or equal
    ICOMP_ULT, //unsigned less than
    ICOMP_ULE, //unsigned less or equal
    ICOMP_SGT, //signed greater than
    ICOMP_SGE, //signed greater or equal
    ICOMP_SLT, //signed less than
    ICOMP_SLE //signed less or equal
};

enum FCOMPARATOR{
    FCOMP_FALSE, //false
    FCOMP_OEQ,  //ordered and equal
    FCOMP_OGT,  //ordered and greater than
    FCOMP_OGE,  //ordered and grater than or equal
    FCOMP_OLT,  //ordered and less than
    FCOMP_OLE,  //ordered and less than or equal
    FCOMP_ONE,  //ordered and not equal
    FCOMP_ORD,  //ordered (no nans)
    FCOMP_UEQ,  //unordered or equal
    FCOMP_ULT,  //unordered or greater than
    FCOMP_ULE,  //unordered or or greater than or equal
    FCOMP_UNE,  //unordered or less than
    FCOMP_UNO,  //unordered or less than or equal
    FCOMP_TRUE  //true
};

union COMPARATOR{
    enum ICOMPARATOR icmp;
    enum FCOMPARATOR fcmp;
};


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

    // not suported by the grammar
    REG_AND,    /*  & in c */
    REG_OR,     /*  | in c */
    REG_XOR     /*  ~ in c */
};


char* load_int(int reg, int value);
char* load_double(int reg, double value);

short int is_binary_op(enum COND_OPERATOR o);
short int is_bitwise_op(enum COND_OPERATOR o);

char* binary_op_on_regs(enum REG_BINARY_OP op, int reg_dest, int reg1, int reg2, enum TYPE type);
char* bitwise_op_on_regs(enum REG_BITWISE_OP op, int reg_dest, int reg1, int reg2, enum TYPE type);


char* operation_on_regs(enum COND_OPERATOR op, int reg_dest, int reg1, int reg2, enum TYPE type);
char* declare_var(char* id, enum TYPE type, short int is_global);
char* load_var(int reg, char* id);
char* store_var(char* id, int reg, enum TYPE type);

char* comparator_to_string(union COMPARATOR comparator, int float_or_int);
char* label_to_string(int label);
char* true_comp(int reg);
char* jump_to(int label);

#endif