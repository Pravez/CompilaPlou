#include "hash.h"

#include <stdio.h>
#include <string.h>

int hachage(char *s) {
    unsigned int hash = 0;
    while (*s!='\0') hash = hash*31 + *s++;
    return hash%SIZE;
}
symbol_t findtab(char *s) {
    if (strcmp(hachtab[hachage(s)].name,s)) return hachtab[hachage(s)];
    return EMPTY;
}
void addtab(char *s,int type) {
    symbol_t *h=&hachtab[hachage(s)];
    h->name=s; h->type=type; h->code=NULL; h->var=NULL;
}
void init() {
    int i;
    for (i=0; i<SIZE; i++) hachtab[i]=EMPTY;
}