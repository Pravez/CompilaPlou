#include <stdlib.h>
#include <stdio.h>
#include "expression.h"
#include "scope.h"
#include "hash.h"

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
    if(operand->prefix != 0){
        report_error(NOT_ASSIGNABLE_EXPR, "");
        return 0;
    }else if(operand->type != O_VARIABLE){
        report_error(POSTF_OPERATOR_NOT_USABLE, value > 0 ? "++" : "--");
        return 0;
    }else{
        struct Declarator declarator = hash__get_item(&scope, operand->operand.variable);
        if(declarator.decl_type == FUNCTION){ //really ?
            report_error(UNARY_ON_FUNCTION, operand->operand.variable);
            return 0;
        }else{
            if(!declarator.declarator.variable.initialized){
                report_warning(UNARY_ON_UNINIT, operand->operand.variable);
                return 0;
            }
        }

        operand->postfix += value;
        return 1;
    }
}

int operand_add_prefix(struct expr_operand* operand, int value){
    if(operand->postfix != 0){
        report_error(NOT_ASSIGNABLE_EXPR, "");
        return 0;
    }else if(operand->type != O_VARIABLE){
        report_error(PREF_OPERATOR_NOT_USABLE, value > 0 ? "++" : "--");
        return 0;
    }else{
        struct Declarator declarator = hash__get_item(&scope, operand->operand.variable);
        if(declarator.decl_type == FUNCTION){ //really ?
            report_error(UNARY_ON_FUNCTION, operand->operand.variable);
            return 0;
        }else{
            if(!declarator.declarator.variable.initialized){
                report_warning(UNARY_ON_UNINIT, operand->operand.variable);
                return 0;
            }
        }

        operand->prefix += value;
        return 1;
    }
}

void print_operand(struct expr_operand operand){
    switch(operand.type){
        case O_VARIABLE:
            printf("%s ", operand.operand.variable);
            break;
        case O_INT:
            printf("%d ", operand.operand.int_value);
            break;
        case O_DOUBLE:
            printf("%f ", operand.operand.double_value);
            break;
    }
}

void print_tree(struct Expression* expr){
    printf("(");
    if(expr->conditional_expression.type == C_LEAF){
        print_operand(expr->conditional_expression.leaf);
    }else{
            print_tree(expr->conditional_expression.branch.e_left);
            switch(expr->conditional_expression.branch.operator){
                case OP_ADD:
                    printf ("+"); break;
                case OP_SUB:
                    printf ("-"); break;
                case OP_MUL:
                    printf ("*"); break;
                case OP_DIV:
                    printf ("/"); break;
                default:
                    printf ("?"); break;
            }
            print_tree(expr->conditional_expression.branch.e_right);
    }
    printf(")");

}

struct Expression create_leaf(struct expr_operand operand){
    struct Expression expression;
    expression.type = E_CONDITIONAL;
    expression.conditional_expression.type = C_LEAF;
    expression.conditional_expression.leaf = operand;
    expression.code = NULL;
    printf("CREATED LEAF "); print_operand(operand); printf(", type= %d\n", operand.type);

    return expression;
}

//////////////////////USELESS////////////////////////////
struct Expression create_branch(enum COND_OPERATOR operator, struct Expression* expression_right, struct Expression* expression_left){
    struct Expression expression;

    expression.type = E_CONDITIONAL;
    expression.conditional_expression.type = C_BRANCH;
    expression.conditional_expression.branch.operator = operator;
    expression.conditional_expression.branch.e_right = expression_left;
    expression.conditional_expression.branch.e_left = expression_right;
    expression.code = NULL;

    print_tree(&expression);
    printf("\n");

    return expression;
}
///////////////////////////////////////////////////////

struct Expression create_branch_cpy(enum COND_OPERATOR operator, struct Expression expression_right, struct Expression expression_left){
    struct Expression expression;
    expression.type = E_CONDITIONAL;
    struct Expression* left = malloc(sizeof(struct Expression));
    struct Expression* right = malloc(sizeof(struct Expression));
    *left = expression_left;
    *right = expression_right;
    expression.code = NULL;

    expression.conditional_expression.type = C_BRANCH;
    expression.conditional_expression.branch.operator = operator;
    expression.conditional_expression.branch.e_right = right;
    expression.conditional_expression.branch.e_left = left;

    print_tree(&expression);
    printf("\n");

    return expression;
}

/////////////////////////USELESS///////////////////////////
/*struct Expression expression_from_cond(const struct Expression* e){
    struct Expression expression;

    expression.cond_expression = *e;
    expression.type = E_CONDITIONAL;
    expression.assign_operator = -1;

    return expression;
}*/
////////////////////////////////////////////////////////////

int expression_from_unary_cond(struct expr_operand* operand, enum ASSIGN_OPERATOR assign_operator, struct Expression* cond, struct Expression* final_expression){
    
    struct Declarator item = hash__get_item(&scope, operand->operand.variable);
    if(item.decl_type == FUNCTION){
        return 0;
    }

    final_expression->type = E_AFFECT;

    final_expression->expression.assign_operator = assign_operator;
    final_expression->expression.cond_expression = cond;
    final_expression->expression.operand = *operand;
    final_expression->code = NULL;

    //Says that now it is initialized
    item.declarator.variable.initialized = 1;

    return 1;
}