#include "errors.h"
#include "tools.h"

#include <stdlib.h>
#include <stdio.h>

void report_error(enum ERROR_TYPE type, void* data){
    char* error;
    int allocated = 1;
    char* identifier;
    struct arg_wrong_type wrong_type;

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
                                            "\033[0m on something that is not a variable");
            break;
        case PREF_OPERATOR_NOT_USABLE:
            identifier = (char*) data;
            error = concatenate_strings(3, "Cannot use prefix operator \033[31;1m", identifier, 
                                            "\033[0m on something that is not a variable");
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
            break;
        case SCOPE_MAX_LEVEL:
            allocated = 0;
            error = "(\033[31;1mFATAL\033[0m) Impossible to add a new level, maximum quantity reached";
            break;
        case SCOPE_MAX_IDENT:
            allocated = 0;
            error = "(\033[31;1mFATAL\033[0m) Unable to create another identifier, maximum quantity reached";
            break;
        case FUNCTION_EXTERNAL_REGISTERED:
            identifier = (char*) data;
            error = concatenate_strings(3, "Impossible to declare function \033[31;1m", identifier,
                                        "\033[0m, it has already been declared as external (probably part of p5 functions)");
            break;
        case FUNCTION_INVALID_PARAM_COUNT:
            wrong_type = *(struct arg_wrong_type*) data;
            error = concatenate_strings(5, "Function \033[31;1m", wrong_type.function_name, "\033[0m requires \033[31;1m", wrong_type.position,
                                        "\033[0m parameters");
            break;
        case NOT_A_FUNCTION:
            identifier = (char*) data;
            error = concatenate_strings(3, "\033[31;1m", identifier, "\033[0m is not a function");
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
    struct arg_wrong_type wrong_type;
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
        case USELESS_CAST:
            warning = "Cast has no effect"; allocated = 0;
            break;
        case FUNCTION_ARG_WRONG_TYPE:
            wrong_type = *(struct arg_wrong_type*)data;
            warning = concatenate_strings(9, "Expected parameter \033[33;1m", wrong_type.position, "\033[0m of \033[33;1m", wrong_type.function_name,
                                          "\033[0m to be \033[35;1m", type_to_str(wrong_type.expected), "\033[0m, got \033[35;1m",
                                            type_to_str(wrong_type.given), "\033[0m instead");
            break;

    }

    char* result = concatenate_strings(2, "\033[35;1mWARNING\033[0m : ", warning);
    yyerror(result);
    if(allocated)
        free(warning);
}

int verify_no_error(char* file_name){
    if(ERR_COUNT > 0){
        printf("%s: \033[31;1m%d\033[0m error(s) occured. \n", file_name, ERR_COUNT);
        printf("%s: exitting ...\n", file_name);
        free (file_name);
        return 0;
    }

    free (file_name);
    return 1;
}