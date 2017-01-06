#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "tools.h"

int CURRENT_REGI =      0;
int CURRENT_LBLI =      0;
char CURRENT_REG[10] =  "x0";
char CURRENT_LBL[10] =  "l0";

int ERR_COUNT = 0;
int WARN_COUNT = 0;

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

int new_register(){
    int tmp = ++CURRENT_REGI;
    sprintf(CURRENT_REG, "x%i", tmp);
    return tmp;
}
int new_label(){
    int tmp = ++CURRENT_LBLI;
    sprintf(CURRENT_LBL, "l%i", tmp);
    return tmp;
}

char* concatenate_strings(int qty, ...){
    va_list list;
    va_start(list, qty);

    int total_length = 1;
    for(int i=0;i<qty;i++){
        total_length += strlen(va_arg(list, char*));
    }
    va_end(list);
    va_start(list, qty);

    char* final = (char*) malloc(sizeof(char) * total_length);
    final[0] = '\0';

    for(int i=0;i<qty;i++){
        m_strcat(final, va_arg(list, char*));
    }

    va_end(list);
    return final;
}

char* m_strcat(char* dest, char* src){
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return --dest;
}