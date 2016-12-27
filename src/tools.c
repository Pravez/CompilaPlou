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

char* report_error(enum ERROR_TYPE type, void* data){
    char* error;
    char* identifier;
    switch(type){
        case UNDEFINED_FUNC:
            identifier = (char*) data;
            error = concatenate_strings(3, "Function \033[31;1m", identifier,
                                             "\033[0m does not exist");
            break;
        case UNDEFINED_VAR:
            identifier = (char*) data;
            error = concatenate_strings(3, "Variable \033[31;1m", identifier,
                                             "\033[0m has not yet been declared");
            break;
        case DEFINED_FUNC:
            identifier = (char*) data;
            error = concatenate_strings(3, "Function \033[31;1m", identifier,
                                             "\033[0m has already been defined in scope as a function");
            break;
        case DEFINED_FUNC_VAR:
            identifier = (char*) data;
            error = concatenate_strings(3, "Function \033[31;1m", identifier,
                                        "\033[0m has already defined variable(s) as parameter(s) in its scope");
            break;
        case DEFINED_VAR:
            identifier = (char*) data;
            error = concatenate_strings(3, "Variable \033[31;1m", identifier,
                                             "\033[0m has already been defined in scope as a variable");
            break;
        case NOT_ASSIGNABLE_EXPR:
            error = "Expression is not assignable";
            break;
        case POSTF_OPERATOR_NOT_USABLE:
            identifier = (char*) data;
            error = concatenate_strings(3, "Cannot use postfix operator \033[31;1m", identifier, 
                                            "\033[0m on other thing than variable");
            break;
        case PREF_OPERATOR_NOT_USABLE:
            identifier = (char*) data;
            error = concatenate_strings(3, "Cannot use prefix operator \033[31;1m", identifier, 
                                            "\033[0m on other thing than variable");
            break;
        case VOID_UNAUTHORIZED:
            identifier = (char*) data;
            error = concatenate_strings(3, "Variable \033[31;1m", identifier, 
                                            "\033[0m can't be declared : \033[34;1mvoid\033[0m type is forbidden for variables");
            break;
    }

    ERR_COUNT ++;
    error_flag = 1;

    char* errtype = "\033[31;1mERROR\033[0m : ";
    return concatenate_strings(2, errtype, error);
}

void verify_no_error(char* file_name){
    if(ERR_COUNT > 0){
        printf("%s: \033[31;1m%d\033[0m error(s) occured. \n", file_name, ERR_COUNT);
        printf("%s: exitting ...\n", file_name);
        free (file_name);
        exit(0);
    }

    free (file_name);
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
        strcat(final, va_arg(list, char*));
    }
    final[total_length-1] = '\0'; // theoretically, no use of that line, but anyway...

    va_end(list);
    return final;
}