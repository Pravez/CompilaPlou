#ifndef COMPLL_EXPRESSION_H
#define COMPLL_EXPRESSION_H

#include "type.h"
#include "tools.h"
#include "errors.h"
#include "llvm_code.h"

#define DEFAULT_EXPR_ARRAY_SIZE 5

enum OPERAND_TYPE{
    O_VARIABLE, O_INT, O_DOUBLE, O_FUNCCALL_ARGS
};

enum EXPR_TYPE{
    E_CONDITIONAL, E_AFFECT
};

enum COND_EXPR_TYPE{
    C_BRANCH, C_LEAF
};

struct Expression_array{
    int expression_count;
    int size;
    struct Expression* array;
};


struct expr_operand{
    enum OPERAND_TYPE type;
    union{
        char* variable;
        int int_value;
        double double_value;
        int unary_sub;
        struct{
            char* name;
            struct Expression_array parameters;
            struct computed_expression* computed_array;
        }function;
    }operand;

    short int postfix;
    short int prefix;
};

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
            int is_alone;
            short int is_negative;
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
    struct computed_expression* code; //Declared in llvm_code.h
};

struct Expression_array create_expression_array(struct Expression expression);
void add_expression_to_array(struct Expression_array* array, struct Expression expression);

struct expr_operand init_operand(enum OPERAND_TYPE type);
struct expr_operand init_operand_identifier(char* variable);
struct expr_operand init_operand_integer(int int_value);
struct expr_operand init_operand_double(double double_value);
struct expr_operand init_operand_function(char* name, struct Expression_array* array);
int is_corresponding_to_function(struct expr_operand* operand);
int operand_add_postfix(struct expr_operand* operand, int value);
int operand_add_prefix(struct expr_operand* operand, int value);


struct Expression create_leaf(struct expr_operand operand);
struct Expression create_branch(enum COND_OPERATOR operator, struct Expression* expression_right, struct Expression* expression_left);
struct Expression create_branch_cpy(enum COND_OPERATOR operator, struct Expression expression_right, struct Expression expression_left);

struct Expression expression_from_cond(const struct Expression* e);
int expression_from_unary_cond(struct expr_operand* operand, enum ASSIGN_OPERATOR assign_operator, struct Expression* cond, struct Expression* final_expression);

int verify_expression_type(struct Declarator item, struct Expression* expression);
enum TYPE establish_expression_final_type(struct Expression* expression);
enum TYPE get_operand_type(struct expr_operand operand);

void print_tree(struct Expression* expr);

struct expr_operand variable_to_expr_operand(struct Variable* var);

short int is_already_computed(struct Expression* e);
#endif //_EXPRESSION_H
