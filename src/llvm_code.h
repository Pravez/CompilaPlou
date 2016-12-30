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

void llvm__init_program(struct llvm__program* program);
int llvm__program_add_line(struct llvm__program* program, char* line);
void llvm__fusion_programs(struct llvm__program* main, const struct llvm__program* toappend);
char* llvm__create_constant(char* name, enum TYPE type, int size, void* value);
char* llvm___create_function_def(struct Function function);

void llvm__print(struct llvm__program* program);
struct computed_expression* generate_code(struct Expression* e);

struct llvm__program* generate_var_declaration(struct Variable* v, short int is_global);
struct llvm__program* generate_multiple_var_declarations(struct DeclaratorList* list, short int are_globals);

struct llvm__program* generate_while_code(struct Expression* condition, struct llvm__program* statement_code, int is_dowhile);
struct llvm__program* generate_if_code(struct Expression* condition, struct llvm__program* statement_code);
struct llvm__program do_jump(int float_or_int, int condition, union COMPARATOR comparator, int labeltrue, int labelfalse);

#endif //TESTS_LLVM_CODE_H
