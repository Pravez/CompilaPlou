#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include "type.h"
#include "tools.h"
#include "errors.h"

enum EXPR_COMMAND{
    ADD, SUB, DIV, CONV_2DOUBLE, CONV_2INT, MUL, FMUL
};

enum OPERAND_TYPE{
    O_VARIABLE, O_INT, O_DOUBLE, O_UNARY_SUB
};

enum EXPR_TYPE{
    E_CONDITIONAL, E_AFFECT
};

enum COND_EXPR_TYPE{
    C_BRANCH, C_LEAF
};

struct expr_operand{
    enum OPERAND_TYPE type;
    union{
        char* variable;
        int int_value;
        double double_value;
        int unary_sub;
    }operand;

    int postfix;
    int prefix;
};

/*struct cond_expression{
    enum COND_EXPR_TYPE type;
    union{
        struct expr_operand leaf;
        struct{
            enum COND_OPERATOR operator;
            struct cond_expression* e_left;
            struct cond_expression* e_right;
        }branch;
    };
};

struct Expression{
    enum EXPR_TYPE type;
    enum ASSIGN_OPERATOR assign_operator;
    struct expr_operand operand;
    struct cond_expression cond_expression;
};
*/

struct Expression{
    enum EXPR_TYPE type;
    union{
        struct{
            enum ASSIGN_OPERATOR assign_operator;
            struct expr_operand operand;
            struct Expression* cond_expression;
        }expression;
        struct{
            enum COND_EXPR_TYPE type;
            union{
                struct expr_operand leaf;
                struct{
                    enum COND_OPERATOR operator;
                    struct Expression* e_left;
                    struct Expression* e_right;
                }branch;
            };
        }conditional_expression;
    };
};

struct expr_operand init_operand(enum OPERAND_TYPE type);
struct expr_operand init_operand_identifier(char* variable);
struct expr_operand init_operand_integer(int int_value);
struct expr_operand init_operand_double(double double_value);
int operand_add_postfix(struct expr_operand* operand, int value);
int operand_add_prefix(struct expr_operand* operand, int value);


struct Expression create_leaf(struct expr_operand operand);
struct Expression create_branch(enum COND_OPERATOR operator, struct Expression* expression_right, struct Expression* expression_left);
struct Expression create_branch_cpy(enum COND_OPERATOR operator, struct Expression expression_right, struct Expression expression_left);

struct Expression expression_from_cond(const struct Expression* e);
struct Expression expression_from_unary_cond(struct expr_operand* operand, enum ASSIGN_OPERATOR assign_operator, struct Expression* cond);


void print_tree(struct Expression* expr);
#endif //_EXPRESSION_H
