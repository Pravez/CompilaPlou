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
            error = concatenate_strings(3, "Not existing function : Function \033[31;1m", identifier, "\033[0m");
            break;
        case UNDEFINED_VAR:
            identifier = (char*) data;
            error = concatenate_strings(3, "Not declared variable : Variable \033[31;1m", identifier, "\033[0m");
            break;
        case DEFINED_FUNC:
            identifier = (char*) data;
            error = concatenate_strings(3, "Identifier already defined as a function : Identifier \033[31;1m", identifier, "\033[0m");
            break;
        case DEFINED_FUNC_VAR:
            identifier = (char*) data;
            error = concatenate_strings(3, "Variable(s) already defined as parameter(s) in function : Function \033[31;1m", identifier, "\033[0m");
            break;
        case DEFINED_VAR:
            identifier = (char*) data;
            error = concatenate_strings(3, "Identifier already defined as a variable : Identifier \033[31;1m", identifier, "\033[0m");
            break;
        case NOT_ASSIGNABLE_EXPR:
            allocated = 0;
            error = "Not assignable expression";
            break;
        case POSTF_OPERATOR_NOT_USABLE:
            identifier = (char*) data;
            error = concatenate_strings(3, "Cannot use postfix operator on something else than a variable : Operator \033[31;1m", identifier, "\033[0m");
            break;
        case PREF_OPERATOR_NOT_USABLE:
            identifier = (char*) data;
            error = concatenate_strings(3, "Cannot use prefix operator on something else than a variable : Operator \033[31;1m", identifier, "\033[0m");
            break;
        case VOID_UNAUTHORIZED:
            identifier = (char*) data;
            error = concatenate_strings(3, "Impossible declaration, \033[34;1mvoid\033[0m type is forbidden for variables : Variable \033[31;1m", identifier, "\033[0m");
            break;
        case VOID_ASSIGN:
            allocated = 0;
            error = concatenate_strings(3, "Impossible to assign void value to an expression");
            break;
        case FUNCTION_AS_VARIABLE:
            identifier = (char*) data;
            error = concatenate_strings(3, "Function used like a variable : Function \033[31;1m", identifier, "\033[0m");
            break;
        case FUNCTION_AS_PARAMETER:
            identifier = (char*) data;
            error = concatenate_strings(3, "Function can't be declared as a function parameter : Function \033[31;1m", identifier, "\033[0m");
            break;
        case UNARY_ON_FUNCTION:
            identifier = (char*) data;
            error = concatenate_strings(3, "Unary operator used on a function : Function \033[31;1m", identifier, "\033[0m");
            break;
        case INVALID_FUNC_DECLARATION:
            identifier = (char*) data;
            error = concatenate_strings(3, "Invalid declaration of function : implementation missing after declaration \033[31;1m", identifier, "\033[0m");
            break;
        case SCOPE_MAX_LEVEL:
            allocated = 0;
            error = "(\033[31;1mFATAL\033[0m) Maximum level reached : Impossible to add a new level";
            break;
        case SCOPE_MAX_IDENT:
            allocated = 0;
            error = "(\033[31;1mFATAL\033[0m) Maximum quantity reached : Impossible to create another identifier";
            break;
        case FUNCTION_EXTERNAL_REGISTERED:
            identifier = (char*) data;
            error = concatenate_strings(3, "Function already declared as an external function : Function \033[31;1m", identifier, "\033[0m");
            break;
        case FUNCTION_INVALID_PARAM_COUNT:
            wrong_type = *(struct arg_wrong_type*) data;
            error = concatenate_strings(5, "Invalid count of parameter in function : Function \033[31;1m", wrong_type.function_name, "\033[0m requires \033[31;1m", wrong_type.position, "\033[0m parameters");
            break;
        case NOT_A_FUNCTION:
            identifier = (char*) data;
            error = concatenate_strings(3, "Identifier is not a function : Identifier \033[31;1m", identifier, "\033[0m");
            break;
        case VOID_FUNCTION_RETURNING:
            identifier = (char*) data;
            error = concatenate_strings(3, "Void function can't return : Function \033[33;1m", identifier, "\033[0m");
            break;
        case VOID_TYPE_USED_AS_VALUE:
            allocated = 0;
            error = concatenate_strings(1, "Void used as value");
            break;
        case UNKNOWN_ERROR:
            identifier = (char*) data;
            error = concatenate_strings(3, "Unknown error. Should disappear after correction of previous error(s) : Identifier \033[33;1m", identifier, "\033[0m");
            break;
        case GLOBAL_NEED_DOUBLE_INT:
            identifier = (char*) data;
            error = concatenate_strings(3, "Global initialization need to be an int or a double : Identifier \033[33;1m", identifier, "\033[0m");
            break;
        case GLOBAL_NEED_SINGLE_VALUE:
            identifier = (char*) data;
            error = concatenate_strings(3, "Global definition have to be initialized with a direct value : Identifier \033[33;1m", identifier, "\033[0m");
            break;
        case GLOBAL_NEED_SAME_TYPE:
            identifier = (char*) data;
            error = concatenate_strings(3, "Global variable affected with a wrong type (need the same type than its definition) : Variable \033[33;1m", identifier, "\033[0m ");
            break;
        case MISSING_A_CONDITION:
            allocated = 0;
            error = concatenate_strings(1, "Condition in for declaration missing");
            break;
        case APPLY_MINUS_ON_AFFECT:
            allocated = 0;
            error = concatenate_strings(1, "'\033[33;1m-\033[0m' cannot be applied on an affectation expression");
            break;
        case FUNCTION_ARG_WRONG_TYPE:
            wrong_type = *(struct arg_wrong_type*)data;
            error = concatenate_strings(9, "Wrong type parameter : Expected parameter \033[33;1m", wrong_type.position, "\033[0m of \033[33;1m", wrong_type.function_name,
                                          "\033[0m to be \033[34;1m", type_to_str(wrong_type.expected), "\033[0m, got \033[31;1m",
                                          type_to_str(wrong_type.given), "\033[0m");
            break;
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
            warning = concatenate_strings(3, "Assigning double to an int variable : Variable \033[35;1m", identifier, "\033[0m (cast by default) ");
            break;
        case ASSIGN_INT_TO_DOUBLE:
            identifier = (char*) data;
            warning = concatenate_strings(3, "Assigning int to a double variable : Variable \033[35;1m", identifier, "\033[0m (cast by default)");
            break;
        case UNARY_ON_UNINIT:
            identifier = (char*) data;
            warning = concatenate_strings(3, "Unary operator used on an initialized variable : Variable \033[35;1m", identifier, "\033[0m");
            break;
        case UNINTIALIZED_VAR:
            identifier = (char*) data;
            warning = concatenate_strings(3, "Uninitialized variable used : Variable \033[35;1m", identifier,"\033[0m ");
            break;
        case USELESS_CAST:
            warning = "Useless cast"; allocated = 0;
            break;
        case FUNCTION_WRONG_RETURN_TYPE:
            wrong_type = *(struct arg_wrong_type*)data;
            warning = concatenate_strings(7, "Wrong type return : Expected return type of \033[33;1m", wrong_type.function_name,
                                          "\033[0m to be \033[35;1m", type_to_str(wrong_type.expected), "\033[0m, got \033[35;1m",
                                          type_to_str(wrong_type.given), "\033[0m");
            break;
        case GLOBAL_NO_INIT:
            identifier = (char*) data;
            warning = concatenate_strings(3, "Not initialized global variable : Variable \033[35;1m", identifier, "\033[0m (Initialized by default)");
            break;
        case MISSING_AN_INITIALISATION:
            allocated = 0;
            warning = concatenate_strings(1, "Initialisation missing in for declaration. (Allowed by default.)");
            break;
        case MISSING_A_MOVING:
            allocated = 0;
            warning = concatenate_strings(1, "Moving missing in for declaration. (Allowed by default.)");
            break;
        case MAIN_NOT_EXISTING:
            allocated = 0;
            warning = concatenate_strings(1, "Main not existing : \033[35;1mmain\033[0m function is required to start the program");
            break;
    }

    WARN_COUNT++;

    char* result = concatenate_strings(2, "\033[35;1mWARNING\033[0m : ", warning);
    yyerror(result);
    if(allocated)
        free(warning);
}

int verify_no_error(char* file_name){
    if(WARN_COUNT > 0){
        printf("%s: \033[35;1m%d\033[0m warning(s) appeared. \n", file_name, WARN_COUNT);
        printf("%s: Look out, Warnings can induce to errors when trying to compile LLVM code. \n", file_name);
    }

    if(ERR_COUNT > 0){
        printf("%s: \033[31;1m%d\033[0m error(s) occured. \n", file_name, ERR_COUNT);
        printf("%s: exitting ...\n", file_name);
        free (file_name);
        return 0;
    }

    free (file_name);
    return 1;
}
