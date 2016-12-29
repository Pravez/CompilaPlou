#ifndef _ERRORS_H_
#define _ERRORS_H_

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
    FUNCTION_AS_VARIABLE,
    FUNCTION_AS_PARAMETER,
    UNARY_ON_FUNCTION
};

enum WARNING_TYPE{
    CAST_INT_TO_DOUBLE,
    CAST_DOUBLE_TO_INT,
    UNARY_ON_UNINIT
};

extern void yyerror (char const*);

void report_error(enum ERROR_TYPE type, void* data);
void report_warning(enum WARNING_TYPE type, void* data);
void verify_no_error(char* file_name);

#endif