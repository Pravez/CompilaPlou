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

struct expr_operand init_operand_function(char* name, struct Expression_array *array){
    struct expr_operand operand;
    operand = init_operand(O_FUNCCALL_ARGS);
    operand.operand.function.name = name;


    if(array != NULL) {
        operand.operand.function.parameters = *array;
        operand.operand.function.computed_array = malloc(sizeof(struct computed_expression) * array->expression_count);

        for (int i = 0; i < operand.operand.function.parameters.expression_count; i++) {
            operand.operand.function.computed_array[i] = *generate_code(&operand.operand.function.parameters.array[i]);

        }
    }else{
        operand.operand.function.parameters.expression_count = 0;
    }

    return operand;
}

int is_corresponding_to_function(struct expr_operand* operand){
    struct Declarator function = hash__get_item(&scope, operand->operand.function.name);

    //Check it is a function
    if(function.decl_type == FUNCTION){
        //Check it has same parameters number
        if(function.declarator.function.var_list_size == operand->operand.function.parameters.expression_count){
            enum TYPE arg_type;
            enum TYPE expr_type;
            //Check parameters have same type
            for(int i = 0;i < operand->operand.function.parameters.expression_count;i++){
                arg_type = function.declarator.function.var_list[i].type;
                expr_type = operand->operand.function.computed_array[i].type;
                //Means that an arg is wrong type
                if(arg_type != expr_type){
                    struct arg_wrong_type* wrong = malloc(sizeof(struct arg_wrong_type));
                    wrong->expected = arg_type;
                    wrong->given = expr_type;
                    wrong->function_name = operand->operand.function.name;
                    char arg_pos[2];
                    arg_pos[0] = '\0';
                    sprintf(arg_pos, "%d", i+1);
                    wrong->position = arg_pos;
                    report_warning(FUNCTION_ARG_WRONG_TYPE, (void*)wrong);
                    free(wrong);
                }
            }
            //Means that different parameters count
        }else{
            struct arg_wrong_type* wrong = malloc(sizeof(struct arg_wrong_type));
            wrong->function_name = operand->operand.function.name;
            wrong->expected = -1;
            wrong->given = -1;
            char arg_pos[2];
            arg_pos[0] = '\0';
            sprintf(arg_pos, "%d", function.declarator.function.var_list_size);
            wrong->position = arg_pos;
            report_error(FUNCTION_INVALID_PARAM_COUNT, (void*)wrong);
            free(wrong);

            return 0;
        }
    }else{
        report_error(NOT_A_FUNCTION, operand->operand.function.name);
        return 0;
    }

    return 1;
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
    }else if(operand->type == O_FUNCCALL_ARGS){
        report_error(UNARY_ON_FUNCTION, operand->operand.function.name);
        return 0;
    }else if(operand->type != O_VARIABLE){
        report_error(POSTF_OPERATOR_NOT_USABLE, value > 0 ? "++" : "--");
        return 0;
    }else{
        struct Declarator declarator = hash__get_item(&scope, operand->operand.variable);
        if(declarator.decl_type != -1) {
            if (!declarator.declarator.variable.initialized) {
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
        case O_FUNCCALL_ARGS:
            printf("%s ", operand.operand.function.name);
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
                    printf("+");  break;
                case OP_SUB:
                    printf("-");  break;
                case OP_MUL:
                    printf("*");  break;
                case OP_DIV:
                    printf("/");  break;
                case OP_SSHR:
                    printf(">>"); break;
                case OP_SSHL:
                    printf("<<"); break;
                case OP_SHL:
                    printf("<");  break;
                case OP_SHR:
                    printf(">");  break;
                case OP_LE:
                    printf("<="); break;
                case OP_GE:
                    printf(">="); break;
                case OP_EQ:
                    printf("=="); break;
                case OP_NE:
                    printf("!="); break;
                case OP_AND:
                    printf("&&"); break;
                case OP_OR:
                    printf("||"); break;
                default:
                    printf(" <?> ");  break;
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
    expression.conditional_expression.is_alone = 0;

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
    expression.conditional_expression.is_alone = 0;

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
    if(expression_left.type == -1 || expression_right.type == -1){
        expression.type = -1;
    }else{
        expression.type = E_CONDITIONAL;
    }
    struct Expression* left = malloc(sizeof(struct Expression));
    struct Expression* right = malloc(sizeof(struct Expression));
    *left = expression_left;
    *right = expression_right;
    expression.code = NULL;


    expression.conditional_expression.type = C_BRANCH;
    expression.conditional_expression.branch.operator = operator;
    expression.conditional_expression.branch.e_right = right;
    expression.conditional_expression.branch.e_left = left;
    expression.conditional_expression.is_alone = 0;

    //print_tree(&expression);
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
    //If expression has already been calculated (affectation or cast for instance)
    if(is_already_computed(expression)){
        return expression->code->type;
    }else
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
    struct Declarator func_or_var;
    switch(operand.type){
        case O_INT:
            return T_INT;
        case O_DOUBLE:
            return T_DOUBLE;
        case O_VARIABLE:
            func_or_var = hash__get_item(&scope, operand.operand.variable);
            if(func_or_var.decl_type != -1) {
                if (func_or_var.decl_type == VARIABLE){
                    if(!func_or_var.declarator.variable.initialized)
                        report_warning(UNINTIALIZED_VAR, func_or_var.declarator.variable.identifier);
                    return func_or_var.declarator.variable.type;
                }
            }
            break;
        case O_FUNCCALL_ARGS:
            func_or_var = hash__get_item(&scope, operand.operand.function.name);
            if(func_or_var.decl_type != -1) {
                if (func_or_var.decl_type == FUNCTION) {
                    return func_or_var.declarator.function.return_type;
                }
            }
            break;
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

short int is_already_computed(struct Expression* e){
    return (e->code != NULL && e->code->code != NULL);
}


struct Expression_array create_expression_array(struct Expression expression){
    struct Expression_array array;
    array.size = DEFAULT_EXPR_ARRAY_SIZE;
    array.array = malloc(sizeof(struct Expression)* array.size);
    array.expression_count = 1;

    array.array[0] = expression;

    return array;
}

void add_expression_to_array(struct Expression_array* array, struct Expression expression){
    if(array->expression_count == array->size-1) {
        array->size *= 2;
        array->array = realloc(array->array, sizeof(struct Expression)* array->size);
    }

    array->array[array->expression_count++] = expression;
}