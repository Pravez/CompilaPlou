#ifndef COMPILAPIOU_TYPE_H
#define COMPILAPIOU_TYPE_H

/*
#include "hash.h"
enum TYPE{
    INT, DOUBLE, FLOAT, VOID
};
*/
/*
enum RETTYPE{
    INT, DOUBLE, FLOAT, VOID
};
*/
/*
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
    struct plou_var* var_list;
    int var_list_size;
};

//////Declaration
union Declarator{
    struct Variable variable;
    struct Function function;
};

//////Scope
struct Scope{
    symbol_t* scope_maps[SIZE];
    int current_level; //Pas necessaire
};
*/
#endif //COMPILAPIOU_TYPE_H
