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
        if(declarator.decl_type != -1) {
            if (declarator.decl_type == FUNCTION) { //really ?
                report_error(UNARY_ON_FUNCTION, operand->operand.variable);
                return 0;
            } else {
                if (!declarator.declarator.variable.initialized) {
                    report_warning(UNARY_ON_UNINIT, operand->operand.variable);
                    return 0;
                }
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
        if(declarator.decl_type != -1) {
            if (declarator.decl_type == FUNCTION) { //really ?
                report_error(UNARY_ON_FUNCTION, operand->operand.variable);
                return 0;
            } else {
                if (!declarator.declarator.variable.initialized) {
                    report_warning(UNARY_ON_UNINIT, operand->operand.variable);
                    return 0;
                }
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
    if(expr->type == E_CONDITIONAL) {
        if (expr->conditional_expression.type == C_LEAF) {
            print_operand(expr->conditional_expression.leaf);
        } else {
            print_tree(expr->conditional_expression.branch.e_left);
            switch (expr->conditional_expression.branch.operator) {
                case OP_ADD:
                    printf("+");
                    break;
                case OP_SUB:
                    printf("-");
                    break;
                case OP_MUL:
                    printf("*");
                    break;
                case OP_DIV:
                    printf("/");
                    break;
                case OP_SSHR:
                    printf(">>");
                    break;
                case OP_SSHL:
                    printf("<<");
                    break;
                default:
                    printf("?");
                    break;
            }
            printf("[%d]", expr->conditional_expression.branch.operator);
            print_tree(expr->conditional_expression.branch.e_right);
        }
        printf(")");
    }else{ //E_AFFECT
        printf("(%s) = ", expr->expression.operand.operand.variable);
        print_tree(expr->expression.cond_expression);
    }
}

struct Expression create_leaf(struct expr_operand operand){
    struct Expression expression;
    expression.type = E_CONDITIONAL;
    expression.conditional_expression.type = C_LEAF;
    expression.conditional_expression.leaf = operand;

    expression.code = malloc(sizeof(struct computed_expression));
    expression.code->code = NULL;

    //printf("CREATED LEAF "); print_operand(operand); printf(", type= %d\n", operand.type);

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

    expression.code = malloc(sizeof(struct computed_expression));
    expression.code->code = NULL;

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

    if(!verify_expression_type(item, cond)){
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

int verify_expression_type(struct Declarator item, struct Expression* expression){
    //We verify if the assigned type is good
    enum TYPE operand_type = item.declarator.variable.type;
    if(operand_type != T_VOID){
        enum TYPE final_type = establish_expression_final_type(expression);
        if(operand_type == T_INT && final_type == T_DOUBLE)
            report_warning(ASSIGN_DOUBLE_TO_INT, item.declarator.variable.identifier);
        else if(operand_type == T_DOUBLE && final_type == T_INT)
            report_warning(ASSIGN_INT_TO_DOUBLE, item.declarator.variable.identifier);
        return 1;
    }else{
        report_error(VOID_ASSIGN, 0);
        return 0;
    }
}

enum TYPE establish_expression_final_type(struct Expression* expression){

    if(expression->type == E_CONDITIONAL){
        if(expression->conditional_expression.type == C_LEAF){
            return get_operand_type(expression->conditional_expression.leaf);
        }else{
            enum TYPE left = establish_expression_final_type(expression->conditional_expression.branch.e_left);
            enum TYPE right = establish_expression_final_type(expression->conditional_expression.branch.e_right);

            //So if they are different (int or double), double always wins
            if(left != right){
                return T_DOUBLE;
            }else{
                //Else they are exactly the same so we return just one
                return left;
            }
        }
    }else /*if(expression->type == E_AFFECT)*/{
        return get_operand_type(expression->expression.operand);
    }
}

enum TYPE get_operand_type(struct expr_operand operand){
    struct Declarator variable;
    switch(operand.type){
        case O_INT:
            return T_INT;
        case O_DOUBLE:
            return T_DOUBLE;
        case O_VARIABLE:
            variable = hash__get_item(&scope, operand.operand.variable);
            if(variable.decl_type != -1) {
                if (variable.decl_type == FUNCTION) {
                    return variable.declarator.function.return_type;
                }else {
                    if(!variable.declarator.variable.initialized)
                        report_warning(UNINTIALIZED_VAR, variable.declarator.variable.identifier);
                    return variable.declarator.variable.type;
                }
            }

    }

    return -1;
}

struct expr_operand variable_to_expr_operand(struct Variable* var){
    struct expr_operand ret;
    ret.type = O_VARIABLE;
    ret.operand.variable = var->identifier;
    ret.postfix = false;
    ret.prefix = false;
    return ret;
}

/*
enum WARNING_TYPE verify_leaf_type(struct expr_operand operand, enum OPERAND_TYPE main_type){
    enum OPERAND_TYPE temp_type = operand.type;
    
    
    if(temp_type != main_type){
        if(main_type == O_INT){
            return ASSIGN_DOUBLE_TO_INT;
        }else if(main_type == O_DOUBLE){
            return ASSIGN_INT_TO_DOUBLE; // On s'en fout normalement non ?
        }
    }

    return -1;
}

enum OPERAND_TYPE type_to_optype(enum TYPE type){
    switch(type){
        case T_INT:
            return O_INT;
        case T_DOUBLE:
            return O_DOUBLE;
        default:
            return -1;
    }
}
*/

/*
int is_conditional_expr_leaf(struct Expression* expression){
    if(expression->type == E_CONDITIONAL){
        if(expression->conditional_expression.type == C_LEAF)
            return 1;
    }

    return 0;
}*/

/*
enum WARNING_TYPE verify_expression_type(enum OPERAND_TYPE main_type, struct Expression* expression){
    //No cast needed first (ehehe, we didn't analyze anything)
    expression->needed_cast = NO_CAST;
    printf("EXPRESSION TYPE = %d", expression->conditional_expression.type);
    //If the expression is just conditional and is a leaf
    if(is_conditional_expr_leaf(expression)){
            //Then we verify the type of the leaf accord to the main assignation type.
            printf("VERIFYING LEAF leaf");
            return verify_leaf_type(expression->conditional_expression.leaf, main_type);
    }else if(expression->type == E_CONDITIONAL){
        //Recursive, we call it until we have a leaf
        enum WARNING_TYPE returned = -1;
        if(is_conditional_expr_leaf(expression->conditional_expression.branch.e_left)){
            printf("NEXT IS LEAF");
            returned = verify_expression_type(main_type, expression->conditional_expression.branch.e_left);
            //If after verification types are different
            if(returned != -1){
                //We do the cast accordingly to what warning error the verification returned
                modify_expression_cast_type(expression, returned);
            }
        }else{
            //If it's not a leaf we do not care of the returned value, and we simply just go on
            //(but we return the value to know what error it was)
            returned = verify_expression_type(main_type, expression->conditional_expression.branch.e_left);
        }

        //We do the same for right branch
        if(is_conditional_expr_leaf(expression->conditional_expression.branch.e_right)){
            returned = verify_expression_type(main_type, expression->conditional_expression.branch.e_right);
            //If after verification types are different
            if(returned != -1){
                //We do the cast accordingly to what warning error the verification returned
                modify_expression_cast_type(expression, returned);
            }
        }else{
            returned = verify_expression_type(main_type, expression->conditional_expression.branch.e_right);
        }

        //If one of the returned values is not -1, or if the two are not -1 it doesn't change anything
        //because there is only one main type for this expresison so only one type cannot be the good 
        //one (because we have only 2 main types) , so if there's at least one cast needed it's ok.

        return returned;
    }else if(expression->type == E_AFFECT){

    }
}*/
