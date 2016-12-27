#ifndef TESTS_LLVM_CODE_H
#define TESTS_LLVM_CODE_H

#include "type.h"
#include "tools.h"
#include "hash.h"
#include "expression.h"

#define DEFAULT_SIZE 100
#define DEFAULT_CONSTANT_SIZE 50
#define DEFAULT_FUNCTION_SIZE 100

enum LLVM_TYPE{
    LLVM_I32, //Integer
    LLVM_DOUBLE, //Float
    LLVM_I8, //Character
    LLVM_I32_PTR,
    LLVM_DOUBLE_PTR,
    LLVM_I8_PTR
};

struct llvm__program{
    int validity;
    int line_number;
    int size;
    char** code;
};

struct computed_expression{
    int reg;
    enum TYPE type;
    struct llvm__program code;
};

void llvm__init_program(struct llvm__program* program);
int llvm__program_add_line(struct llvm__program* program, char* line);
char* llvm__create_constant(char* name, enum TYPE type, int size, void* value);
char* llvm___create_function_def(struct Function function);
char* type_of(enum LLVM_TYPE type);
enum LLVM_TYPE llvm__convert(enum TYPE type);


void llvm__print(struct llvm__program* program);
struct computed_expression* generate_code(struct Expression* e, hash_t* loaded);


#endif //TESTS_LLVM_CODE_H
