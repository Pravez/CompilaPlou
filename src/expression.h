#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include "type.h"
#include "tools.h"

enum EXPR_COMMAND{
    ADD, SUB, DIV, CONV_2DOUBLE, CONV_2INT, MUL, FMUL
};

enum OPERAND_TYPE{
    O_VARIABLE, O_INT, O_DOUBLE
};

enum EXPR_TYPE{
    E_CONDITIONAL, E_AFFECT
};

struct expr_operand{
    enum OPERAND_TYPE type;
    union{
        char* variable;
        int int_value;
        double double_value;
    }operand;

    int postfix;
    int prefix;
};

struct cond_expression{
    struct expr_operand operand;
    struct cond_expression* next;
};

struct affect_expression{
    char* register_name;
    enum TYPE type;
    struct expr_operand operands[2];
};


struct Expression{
    enum EXPR_TYPE type;
    enum ASSIGN_OPERATOR ass_op;
    struct cond_expression condition;
    struct affect_expression affectation;
};

struct expr_operand init_operand(enum OPERAND_TYPE type);
struct expr_operand init_operand_identifier(char* variable);
struct expr_operand init_operand_integer(int int_value);
struct expr_operand init_operand_double(double double_value);
int operand_add_postfix(struct expr_operand* operand, int value);
int operand_add_prefix(struct expr_operand* operand, int value);

struct Expression create_expression();

#endif //_EXPRESSION_H
