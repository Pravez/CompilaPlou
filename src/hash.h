#ifndef COMPILAPIOU_HASH_H
#define COMPILAPIOU_HASH_H

#define SIZE 1013

#include "type.h"



typedef struct {
    char *name;
    int type;
    char *code;
    char *var;
} symbol_t;

//////Scope
struct Scope{
    symbol_t* scope_maps[SIZE];
    int current_level; //Pas necessaire
};
////////////

int hachage(char *s);
symbol_t findtab(char *s);
void addtab(char *s,int type);
void init();

/*

int hachage(symbol_t &table, char *s);
symbol_t findtab(const symbol_t &table, char *s);
void addtab(symbol_t &table, char *s,int type);
void init(symbol_t &table);

 */
#endif //COMPILAPIOU_HASH_H
