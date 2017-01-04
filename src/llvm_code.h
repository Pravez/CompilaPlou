#ifndef TESTS_LLVM_CODE_H
#define TESTS_LLVM_CODE_H

#include "type.h"
#include "tools.h"
#include "hash.h"
#include "expression.h"
#include "reg_operation.h"

#define DEFAULT_SIZE 100
#define DEFAULT_CONSTANT_SIZE 50
#define DEFAULT_FUNCTION_SIZE 100

struct llvm__program{
    int validity;
    int line_number;
    int size;
    char** code;
};

struct computed_expression{
    int reg;
    enum TYPE type;
    struct llvm__program* code;
};

struct Expression;
struct expr_operand;

void llvm__init_program(struct llvm__program* program);
int llvm__program_add_line(struct llvm__program* program, char* line);
void llvm__fusion_programs(struct llvm__program* main, const struct llvm__program* toappend);
char* llvm__create_constant(char* name, enum TYPE type, int size, void* value);
char** llvm___create_function_def(struct Function function);

void llvm__print(const struct llvm__program* program);
struct computed_expression* generate_code(struct Expression* e);

struct llvm__program* generate_var_declaration(struct Variable* v, struct global_declaration global);
struct llvm__program* generate_multiple_var_declarations(struct DeclaratorList* list, short int is_global);
struct llvm__program* generate_global_decl_and_affect(struct expr_operand* operand, struct Variable *var);

struct llvm__program* generate_for_code(struct Expression* initial, struct Expression* condition, struct Expression* moving, struct llvm__program* statement_code);
struct llvm__program* generate_while_code(struct Expression* condition, struct llvm__program* statement_code, int is_dowhile);
struct llvm__program* generate_if_code(struct Expression* condition, struct llvm__program* statement_code);
struct llvm__program* generate_ifelse_code(struct Expression* condition, struct llvm__program* statement_if, struct llvm__program* statement_else);
struct llvm__program do_jump(int float_or_int, int condition, union COMPARATOR comparator, int labeltrue, int labelfalse);

struct llvm__program add_external_functions_declaration();

short int convert_computed_expr_to_type_if_needed(struct llvm__program* code, struct computed_expression* e, enum TYPE t);

void write_file(struct llvm__program* main_program, char* filename);

#endif //TESTS_LLVM_CODE_H
