
#include "type.h"
#include <stdio.h>


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
                case T_FLOAT: printf("FLOAT "); break;
            }
            printf(" %s, ", list.declarator_list[i].declarator.variable.identifier);
        }
    }

    printf("\n");
}

struct DeclaratorList apply_type(enum TYPE type, struct DeclaratorList list){
    for(int i=0;i<list.size;i++){
        if(list.declarator_list[i].decl_type == VARIABLE)
            list.declarator_list[i].declarator.variable.type = type;
    }

    return list;
}