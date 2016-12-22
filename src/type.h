#ifndef COMPILAPIOU_TYPE_H
#define COMPILAPIOU_TYPE_H


enum TYPE{
    T_INT, T_DOUBLE, T_FLOAT, T_VOID
};

enum RETTYPE{
    R_INT, R_DOUBLE, R_FLOAT, R_VOID
};

enum DECL_TYPE{
    VARIABLE, FUNCTION
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
    struct Variable* var_list;
    int var_list_size;
    char* identifier;
};

//////Declaration
struct Declarator{
    union {
        struct Variable variable;
        struct Function function;
    }declarator;
    enum DECL_TYPE decl_type;
};

struct DeclaratorList{
    struct Declarator declarator_list[100];
    int size;
};


struct DeclaratorList add_declarator(struct DeclaratorList list, struct Declarator declarator);
void print_declarator_list(struct DeclaratorList list);
struct DeclaratorList apply_type(enum TYPE type, struct DeclaratorList list);

#endif //COMPILAPIOU_TYPE_H
