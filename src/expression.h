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
    enum COND_OPERATOR operator;
    struct cond_expression* next;
};

struct Expression{
    enum EXPR_TYPE type;
    enum ASSIGN_OPERATOR assign_operator;
    struct expr_operand operand;
    struct cond_expression cond_expression;
};

struct expr_operand init_operand(enum OPERAND_TYPE type);
struct expr_operand init_operand_identifier(char* variable);
struct expr_operand init_operand_integer(int int_value);
struct expr_operand init_operand_double(double double_value);
int operand_add_postfix(struct expr_operand* operand, int value);
int operand_add_prefix(struct expr_operand* operand, int value);

struct cond_expression create_cond_expression(struct expr_operand operand);
struct cond_expression add_expression_to_cond(struct cond_expression expr, struct expr_operand operand, enum COND_OPERATOR operator);
struct cond_expression add_direct_expression_to_cond(struct cond_expression expr, struct cond_expression* next_expr, enum COND_OPERATOR operator);

struct Expression expression_from_cond(const struct cond_expression* e);
struct Expression expression_from_unary_cond(const struct expr_operand* operand, enum ASSIGN_OPERATOR assign_operator, const struct cond_expression* cond);

#endif //_EXPRESSION_H
