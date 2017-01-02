#ifndef _ERRORS_H_
#define _ERRORS_H_

#include "type.h"

extern int ERR_COUNT;

int error_flag;

enum ERROR_TYPE{
    UNDEFINED_VAR,
    DEFINED_VAR,
    UNDEFINED_FUNC,
    DEFINED_FUNC,
    DEFINED_FUNC_VAR,
    NOT_ASSIGNABLE_EXPR,
    POSTF_OPERATOR_NOT_USABLE,
    PREF_OPERATOR_NOT_USABLE,
    VOID_UNAUTHORIZED,
    VOID_ASSIGN,
    FUNCTION_AS_VARIABLE,
    FUNCTION_AS_PARAMETER,
    UNARY_ON_FUNCTION,
    INVALID_FUNC_DECLARATION,
    SCOPE_MAX_LEVEL,
    SCOPE_MAX_IDENT,
    FUNCTION_EXTERNAL_REGISTERED,
    FUNCTION_INVALID_PARAM_COUNT,
    NOT_A_FUNCTION,
    MAIN_NOT_EXISTING
};

enum WARNING_TYPE{
    ASSIGN_DOUBLE_TO_INT,
    ASSIGN_INT_TO_DOUBLE,
    UNARY_ON_UNINIT,
    UNINTIALIZED_VAR,
    USELESS_CAST,
    FUNCTION_ARG_WRONG_TYPE
};

struct arg_wrong_type{
    char* position;
    enum TYPE given;
    enum TYPE expected;
    char* function_name;
};

extern void yyerror (char const*);

void report_error(enum ERROR_TYPE type, void* data);
void report_warning(enum WARNING_TYPE type, void* data);
int verify_no_error(char* file_name);

#endif