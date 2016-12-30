#include "errors.h"
#include "tools.h"

#include <stdlib.h>
#include <stdio.h>

void report_error(enum ERROR_TYPE type, void* data){
    char* error;
    int allocated = 1;
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
            allocated = 0;
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
        case VOID_ASSIGN:
            allocated = 0;
            error = concatenate_strings(3, "Impossible to assign void value to an expression");
            break;
        case FUNCTION_AS_VARIABLE:
            identifier = (char*) data;
            error = concatenate_strings(3, "Function \033[31;1m", identifier, 
                                            "\033[0m is initialized like a variable");
            break;
        case FUNCTION_AS_PARAMETER:
            identifier = (char*) data;
            error = concatenate_strings(3, "Unable to declare function \033[31;1m", identifier, 
                                            "\033[0m as a function parameter");
            break;
        case UNARY_ON_FUNCTION:
            identifier = (char*) data;
            error = concatenate_strings(3, "Using unary operator on \033[31;1m", identifier, 
                                            "\033[0m which is a function");
            break;
        case INVALID_FUNC_DECLARATION:
            identifier = (char*) data;
            error = concatenate_strings(3, "Invalid declaration of function \033[31;1m", identifier,
                                            "\033[0m, definition must be followed by its implementation");
    }

    ERR_COUNT ++;
    error_flag = 1;

    char* result = concatenate_strings(2, "\033[31;1mERROR\033[0m : ", error);
    yyerror(result);
    if(allocated)
        free(error);
}

void report_warning(enum WARNING_TYPE type, void* data){
    char* warning;
    int allocated = 1;
    char* identifier;
    switch(type){
        case ASSIGN_DOUBLE_TO_INT:
            identifier = (char*) data;
            warning = concatenate_strings(3, "Assigning double to int variable \033[35;1m", identifier, 
                                            "\033[0m will require a cast");
            break;
        case ASSIGN_INT_TO_DOUBLE:
            identifier = (char*) data;
            warning = concatenate_strings(3, "Assigning int to double variable \033[35;1m", identifier, 
                                            "\033[0m will require a cast");
            break;
        case UNARY_ON_UNINIT:
            identifier = (char*) data;
            warning = concatenate_strings(3, "Using unary operator on \033[35;1m", identifier, 
                                            "\033[0m which has never been initialized");
            break;
        case UNINTIALIZED_VAR:
            identifier = (char*) data;
            warning = concatenate_strings(3, "Using variable \033[35;1m", identifier,
                                          "\033[0m which has never been initialized");
            break;
    }

    char* result = concatenate_strings(2, "\033[35;1mWARNING\033[0m : ", warning);
    yyerror(result);
    if(allocated)
        free(warning);
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