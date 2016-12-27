#ifndef TESTS_LLVM_CODE_H
#define TESTS_LLVM_CODE_H

#include "type.h"
#include "tools.h"

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

void llvm__init_program(struct llvm__program* program);
int llvm__program_add_line(struct llvm__program* program, char* line);
void llvm__fusion_programs(const struct llvm__program* main, const struct llvm__program* toappend);
char* llvm__create_constant(char* name, enum TYPE type, int size, void* value);
char* llvm___create_function_def(struct Function function);
char* type_of(enum LLVM_TYPE type);
enum LLVM_TYPE llvm__convert(enum TYPE type);

#endif //TESTS_LLVM_CODE_H
