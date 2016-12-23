#ifndef COMPILAPIOU_TYPE_H
#define COMPILAPIOU_TYPE_H


enum TYPE{
    T_INT, T_DOUBLE, T_FLOAT, T_VOID
};

enum RETTYPE{ //USELESS ?
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
    struct Variable var_list[50];
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
    struct Declarator declarator_list[50];
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

#endif //COMPILAPIOU_TYPE_H
