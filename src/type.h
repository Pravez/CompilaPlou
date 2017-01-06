#ifndef COMPILAPIOU_TYPE_H
#define COMPILAPIOU_TYPE_H

#define MAX_ARGUMENTS 10

#include "tools.h"

enum TYPE{
    T_INT, T_DOUBLE, T_VOID
};

enum DECL_TYPE{
    VARIABLE, FUNCTION
};

/* Operator */
enum ASSIGN_OPERATOR{
    OP_SIMPLE_ASSIGN,  /* = */
    OP_MUL_ASSIGN,     /* *= */
    OP_DIV_ASSIGN,     /* /= */
    OP_REM_ASSIGN,     /* %= */
    OP_SHL_ASSIGN,     /* <<= */
    OP_SHR_ASSIGN,     /* >>= */
    OP_ADD_ASSIGN,     /* += */
    OP_SUB_ASSIGN      /* -= */
};

enum COND_OPERATOR{
    NONE,
    OP_MUL,     /*  * */
    OP_DIV,     /*  / */
    OP_REM,     /*  % */
    OP_SHL,     /*  < */
    OP_SSHL,    /* << */
    OP_SHR,     /*  > */
    OP_SSHR,    /* >> */
    OP_LE,      /* <= */
    OP_GE,      /* >= */
    OP_EQ,      /* == */
    OP_NE,      /* != */
    OP_ADD,     /*  + */
    OP_SUB,     /*  - */
    OP_OR,      /* || */
    OP_AND      /* && */
    /* NB: '|', '&', and '~' are not recognised and throw syntax error */
};

enum LLVM_TYPE{
    LLVM_I32, /* Integer */
    LLVM_DOUBLE, /* Float */
    LLVM_I8, /* Character */
    LLVM_I32_PTR, /* Integer pointer */
    LLVM_DOUBLE_PTR, /* Float pointer */
    LLVM_I8_PTR, /* Character pointer */
    LLVM_VOID
};

/* Variables */
struct Variable{
    enum TYPE type; /* Can't be VOID */
    short int initialized;
    short int is_func_param;
    short int is_global;
    char* identifier;
};

/* Functions */
struct Function{
    enum TYPE return_type; /* Can be VOID */
    struct Variable var_list[MAX_ARGUMENTS];
    int var_list_size;
    char* identifier;
};

/* Declaration */
struct Declarator{
    union { 
        struct Variable variable;
        struct Function function;
    }declarator;
    enum DECL_TYPE decl_type;
};

/* List of Declaration */
struct DeclaratorList{
    struct Declarator declarator_list[MAX_ARGUMENTS];
    int size;
};

/*Add a declarator in a DeclaratorList */
struct DeclaratorList add_declarator(struct DeclaratorList list, struct Declarator declarator);
struct DeclaratorList add_parameter(struct DeclaratorList list, struct Declarator declarator);

struct Declarator init_declarator_as_variable(char* identifier);
/*Apply a type on a declarator */
struct Declarator apply_decl_type(enum TYPE type, struct Declarator declarator);

/*Apply a type on every elements in DeclaratorList */
struct DeclaratorList apply_type(enum TYPE type, struct DeclaratorList list);

/* Declare a function using a parameter list (DeclaratorList and an id) */
struct Declarator declare_function(struct DeclaratorList list, char* identifier);

/* Display declaratorList's content (for debug) */
void print_declarator_list(struct DeclaratorList list);
void describe_declarator(struct Declarator decl);

char* type_to_str(enum TYPE type);
char* type_of(enum LLVM_TYPE type);
enum LLVM_TYPE llvm__convert(enum TYPE type);


#endif //COMPILAPIOU_TYPE_H
