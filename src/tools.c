//
// Created by kwetzakwak on 22/12/16.
//
#include <stdio.h>
#include "tools.h"
int current_regi = 0;
int current_lbli = 0;
char current_reg[10] = "x0";
char current_lbl[10] = "l0";

void debug(char* s, enum color c){
    switch(c){
        case RED:
            printf("\033[31m%s\033[0m\n",s);
            break;
        case BLUE:
            printf("\033[34m%s\033[0m\n",s);
            break;
        case GREEN:
            printf("\033[32m%s\033[0m\n",s);
            break;
    }
}

void debugi(char* s, int i, enum color c){
    switch(c){
        case RED:
            printf("\033[31m%s: %d\033[0m\n",s, i);
            break;
        case BLUE:
            printf("\033[34m%s: %d\033[0m\n",s, i);
            break;
        case GREEN:
            printf("\033[32m%s: %d\033[0m\n",s, i);
            break;
    }
}

void debugs(char* s, char* m, enum color c){
    switch(c){
        case RED:
            printf("\033[31m%s: %s\033[0m\n",s, m);
            break;
        case BLUE:
            printf("\033[34m%s: %s\033[0m\n",s, m);
            break;
        case GREEN:
            printf("\033[32m%s: %s\033[0m\n",s, m);
            break;
    }
}

int increment_register(){
    int tmp = ++current_regi;
    sprintf(current_reg, "x%i", tmp);
    return tmp;
}
int increment_label(){
    int tmp = ++current_lbli;
    sprintf(current_reg, "l%i", ++current_lbli);
    return tmp;
}