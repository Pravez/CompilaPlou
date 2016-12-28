#include "errors.h"
#include "tools.h"

#include <stdlib.h>
#include <stdio.h>

void report_error(enum ERROR_TYPE type, void* data){
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
            error = concatenate_strings(3, "Identifier \033[31;1m", identifier,
                                             "\033[0m has already been defined in scope as a function");
            break;
        case DEFINED_FUNC_VAR:
            identifier = (char*) data;
            error = concatenate_strings(3, "Function \033[31;1m", identifier,
                                        "\033[0m has already defined variable(s) as parameter(s) in its scope");
            break;
        case DEFINED_VAR:
            identifier = (char*) data;
            error = concatenate_strings(3, "Identifier \033[31;1m", identifier,
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
        case FUNCTION_AS_VARIABLE:
            identifier = (char*) data;
            error = concatenate_strings(3, "Function \033[31;1m", identifier, 
                                            "\033[0m is initialized like a variable");
            break;
    }

    ERR_COUNT ++;
    error_flag = 1;

    char* result = concatenate_strings(2, "\033[31;1mERROR\033[0m : ", error);
    yyerror(result);
    free(error);
}

void report_warning(){
    
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