#ifndef _REGOPERATION_H_
#define _REGOPERATION_H_

#include "type.h"
#include "scope.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

char* load_int(int reg, int value);
char* load_double(int reg, double value);
char* add_regs(int reg_dest, int reg1, int reg2, enum TYPE type);
char* mul_regs(int reg_dest, int reg1, int reg2, enum TYPE type);
char* operate_on_regs(enum COND_OPERATOR op, int reg_dest, int reg1, int reg2, enum TYPE type);

char* declare_var(char* id, enum TYPE type, short int is_global);
char* load_var(int reg, char* id);
char* store_var(char* id, int reg, enum TYPE type);


#endif