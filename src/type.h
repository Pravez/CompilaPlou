#ifndef COMPILAPIOU_TYPE_H
#define COMPILAPIOU_TYPE_H

#define MAX_ARGUMENTS 10

#include "tools.h"

enum TYPE{
    T_INT, T_DOUBLE, T_VOID
};

enum RETTYPE{ //USELESS ?
    R_INT, R_DOUBLE, R_VOID
};

enum DECL_TYPE{
    VARIABLE, FUNCTION
};

union VALUE{
    int value_int;
    double value_double;
};

//////Operator
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
    OP_MUL,     /* * */
    OP_DIV,     /* / */
    OP_REM,     /* % */
    OP_SHL,     /* < */
    OP_SSHL,     /* << */
    OP_SHR,     /* > */
    OP_SSHR,     /* >> */
    OP_LE,     /* <= */
    OP_GE,     /* >= */
    OP_EQ,      /* == */
    OP_NE,      /* != */
    OP_ADD,     /* + */
    OP_SUB,      /* - */
    OP_OR,
    OP_AND
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
    struct Variable var_list[MAX_ARGUMENTS];
    int var_list_size;
    char* identifier;
};

//////Declaration
struct Declarator{
    union { //A AMELIORER
        struct Variable variable;
        struct Function function;
    }declarator;
    enum DECL_TYPE decl_type;
};

//A AMELIORER
struct DeclaratorList{
    struct Declarator declarator_list[MAX_ARGUMENTS];
    int size;
};

//Fonction permettant d'ajouter un declarator à une DeclaratorList
struct DeclaratorList add_declarator(struct DeclaratorList list, struct Declarator declarator);
#define ADD_PARAMETER(list, declarator) add_declarator(list, declarator)

//Fonction appliquant un type sur un declarator
struct Declarator apply_decl_type(enum TYPE type, struct Declarator declarator);
//Fonction appliquant un type à tous les éléments d'une DeclaratorList
struct DeclaratorList apply_type(enum TYPE type, struct DeclaratorList list);
//Fonction de déclaration d'une fonction utilisant une liste de paramètres (DeclaratorList et un identifiant)
struct Declarator declare_function(struct DeclaratorList list, char* identifier);

//Fonction affichant (debug) le contenu d'une declaratorList
void print_declarator_list(struct DeclaratorList list);
void describe_declarator(struct Declarator decl);

#endif //COMPILAPIOU_TYPE_H
