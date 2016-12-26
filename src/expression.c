#include "expression.h"

struct expr_operand init_operand(enum OPERAND_TYPE type){
    struct expr_operand operand;
    operand.type = type;
    operand.postfix = 0;
    operand.prefix = 0;
    return operand;
}

struct expr_operand init_operand_identifier(char* variable){
    struct expr_operand operand;
    operand = init_operand(O_VARIABLE);
    operand.operand.variable = variable;

    return operand;
}

struct expr_operand init_operand_integer(int int_value){
    struct expr_operand operand;
    operand = init_operand(O_INT);
    operand.operand.int_value = int_value;

    return operand;
}

struct expr_operand init_operand_double(double double_value){
    struct expr_operand operand;
    operand = init_operand(O_DOUBLE);
    operand.operand.double_value = double_value;

    return operand;
}

int operand_add_postfix(struct expr_operand* operand, int value){
    if(operand->prefix == 0){
        last_error = report_error(NOT_ASSIGNABLE_EXPR, 0);
        return 0;
    }

    operand->postfix += value;
    return 1;
}

int operand_add_prefix(struct expr_operand* operand, int value){
    if(operand->postfix == 0){
        last_error = report_error(NOT_ASSIGNABLE_EXPR, 0);
        return 0;
    }

    operand->prefix += value;
    return 1;
}