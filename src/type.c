
#include "type.h"
#include <stdio.h>
#include "tools.h"

struct DeclaratorList add_declarator(struct DeclaratorList list, struct Declarator declarator){
    list.declarator_list[list.size++] = declarator;
    return list;
}

void print_declarator_list(struct DeclaratorList list){
    for(int i=0;i<list.size;i++){
        if(list.declarator_list[i].decl_type == VARIABLE){
            switch(list.declarator_list[i].declarator.variable.type){
                case T_INT: printf("INT ");break;
                case T_DOUBLE: printf("DOUBLE "); break;
                default:
                    debugi("Erreur: une variable doit être INT ou DOUBLE",
                           list.declarator_list[i].declarator.variable.type,
                           RED);
            }
            printf(" %s, ", list.declarator_list[i].declarator.variable.identifier);
        }else{
            switch(list.declarator_list[i].declarator.function.return_type){
                case T_INT: printf("INT ");break;
                case T_DOUBLE: printf("DOUBLE "); break;
                case T_VOID: printf("VOID "); break;
                default:
                    debugi("Erreur: une variable doit être INT, DOUBLE ou VOID",
                           list.declarator_list[i].declarator.function.return_type,
                           RED);
            }
            printf(" %s(", list.declarator_list[i].declarator.function.identifier);
            for(int j=0;j<list.declarator_list[i].declarator.function.var_list_size;j++){
                switch(list.declarator_list[i].declarator.function.var_list[j].type){
                    case T_INT: printf("INT ");break;
                    case T_DOUBLE: printf("DOUBLE "); break;
                    default:
                        debugi("Erreur: une variable doit être INT ou DOUBLE",
                               list.declarator_list[i].declarator.function.var_list[j].type,
                               RED);
                }
                printf(" %s, ", list.declarator_list[i].declarator.function.var_list[j].identifier);
            }
            printf(")");
        }
    }

    printf("\n");
}

struct DeclaratorList apply_type(enum TYPE type, struct DeclaratorList list){
    for(int i=0;i<list.size;i++){
        if(list.declarator_list[i].decl_type == VARIABLE)
            list.declarator_list[i].declarator.variable.type = type;
        else
            list.declarator_list[i].declarator.function.return_type = type;
    }

    return list;
}

//TODO Causes segfault if type == void
struct Declarator apply_decl_type(enum TYPE type, struct Declarator declarator){
    if(declarator.decl_type == VARIABLE){
        declarator.declarator.variable.type = type;
    }else{
        declarator.declarator.function.return_type = type;
    }

    return declarator;
}

struct Declarator declare_function(struct DeclaratorList list, char* identifier){
    struct Declarator function;
    function.decl_type = FUNCTION;
    function.declarator.function.identifier = identifier;
    function.declarator.function.var_list_size = 0;
    if(list.size != 0){
        for(int i=0;i<list.size;i++){
            if(list.declarator_list[i].decl_type == VARIABLE){
                function.declarator.function.var_list[function.declarator.function.var_list_size++] = list.declarator_list[i].declarator.variable;
            }
        }
    }

    return function;
}

void describe_declarator(struct Declarator decl){
    char* type;
    char* identifier;
    char* return_type;

    if(decl.decl_type == FUNCTION){
        type = "FUNCTION";
        identifier = decl.declarator.function.identifier;
        switch(decl.declarator.function.return_type){
            case T_INT: return_type = "INT";break;
            case T_DOUBLE: return_type = "DOUBLE";break;
            case T_VOID: return_type = "VOID";break;
        }
    }else{
        type = "VAR";
        identifier = decl.declarator.variable.identifier;
        switch(decl.declarator.variable.type){
            case T_INT: return_type = "INT";break;
            case T_DOUBLE: return_type = "DOUBLE";break;
            case T_VOID: return_type = "VOID";break;
        }

    }

    printf("%s %s %s", type, identifier, return_type);
}

char* type_of(enum LLVM_TYPE type){
    switch(type){
        case LLVM_I32:
            return "i32";
        case LLVM_DOUBLE:
            return "double";
        case LLVM_I8:
            return "i8";
        default:
            break;
    }

    return NULL;
}

enum LLVM_TYPE llvm__convert(enum TYPE type){
    switch(type){
        case T_INT:
            return LLVM_I32;
        case T_DOUBLE:
            return LLVM_DOUBLE;
        case T_VOID:
            break;
    }

    return -1;
}
