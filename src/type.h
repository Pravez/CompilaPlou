#ifndef COMPILAPIOU_TYPE_H
#define COMPILAPIOU_TYPE_H


enum TYPE{
    T_INT, T_DOUBLE, T_FLOAT, T_VOID
};

enum RETTYPE{
    R_INT, R_DOUBLE, R_FLOAT, R_VOID
};

union VALUE{
    int value_int;
    double value_double;
};

//////Variables
struct Variable{
    enum TYPE type; // Ne peut pas être VOID
    union VALUE value;
    char* identifier;
};

//////Fonctions
struct Function{
    enum TYPE return_type; // Peut être VOID
    char* identifier;
    struct Variable* var_list;
    int var_list_size;
};

//////Declaration
union Declarator{
    struct Variable variable;
    struct Function function;
};

#endif //COMPILAPIOU_TYPE_H
