#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include "type.h"

enum EXPR_COMMAND{
    ADD, SUB, DIV, CONV_2DOUBLE, CONV_2INT, MUL, FMUL
};

enum OPERAND_TYPE{
    VARIABLE, INT, DOUBLE
};

struct Expression{
    char* register_name;
    enum TYPE type;
    union expr_operand operands[2];
};

struct expr_operand{
    enum OPERAND_TYPE type;
    union operand{
        char* variable;
        int int_value;
        double double_value;
    };
};

#endif //_EXPRESSION_H
