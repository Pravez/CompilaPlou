#include <stdlib.h>
#include <stdio.h>
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

void print_operand(struct expr_operand operand){
    switch(operand.type){
        case O_VARIABLE:
            printf("operand: %s ", operand.operand.variable);
            break;
        case O_INT:
            printf("operand: %d ", operand.operand.int_value);
            break;
        case O_DOUBLE:
            printf("operand: %f ", operand.operand.double_value);
            break;
    }
}

struct cond_expression create_cond_expression(struct expr_operand operand){
    struct cond_expression cond;
    cond.next = NULL;
    cond.operand = operand;
    cond.operator = NONE;

    return cond;
}

struct cond_expression add_expression_to_cond(struct cond_expression expr, struct expr_operand operand, enum COND_OPERATOR operator){
    struct cond_expression* next = malloc(sizeof(struct cond_expression));
    next->operator = NONE;
    next->next = NULL;
    next->operand = operand;

    expr.next = next;
    expr.operator = operator;

    return expr;
}

struct cond_expression add_direct_expression_to_cond(struct cond_expression expr, struct cond_expression* next_expr, enum COND_OPERATOR operator){
    struct cond_expression* next = malloc(sizeof(struct cond_expression));
    next->operator = next_expr->operator;
    next->operand = next_expr->operand;
    next->next = next_expr->next;

    expr.next = next;
    expr.operator = operator;

    return expr;
}

//ADDED
struct Expression expression_from_cond(const struct cond_expression* e){
    struct Expression expression;

    expression.cond_expression = *e;
    expression.type = E_CONDITIONAL;
    expression.assign_operator = -1;

    return expression;
}

struct Expression expression_from_unary_cond(const struct expr_operand* operand, enum ASSIGN_OPERATOR assign_operator, const struct cond_expression* cond){
    struct Expression expression;

    expression.type = E_AFFECT;
    expression.assign_operator = assign_operator;
    expression.cond_expression = *cond;
    expression.operand = *operand;

    print_operand(*operand);
    printf(" = ");
    const struct cond_expression* next = cond;
    while(next != NULL){
        print_operand(next->operand);
        next = next->next;
    }

    printf("\n");

    return expression;
}