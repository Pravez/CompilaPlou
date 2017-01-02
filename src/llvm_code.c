#define _GNU_SOURCE
#include "llvm_code.h"
#include "type.h"
#include "external_function.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define TO_LLVM_STRING(type) type_of(llvm__convert(type))
#define GET_VAR_TYPE(ptr_scope, var_id) hash__get_item(ptr_scope, var_id).declarator.variable.type
#define IS_FUNC_PARAM(ptr_scope, var_id) hash__get_item(ptr_scope, var_id).declarator.variable.is_func_param

void llvm__init_program(struct llvm__program* program){
    program->line_number = 0;
    program->validity = 1;
    program->size = DEFAULT_SIZE;
    program->code = malloc(sizeof(char*) * DEFAULT_SIZE);
}

int llvm__program_add_line(struct llvm__program* program, char* line){
    program->code[program->line_number++] = line;

    if(program->line_number == program->size - 1){
        program->size*= 2;
        program->code = realloc(program->code, (size_t) program->size);
    }

    return 1;
}

char* llvm__create_constant(char* name, enum TYPE type, int size, void* value){
    char* constant;
    char* type_char = type_of(llvm__convert(type));
    asprintf(&constant, "@%s = constant [%d x %s] c\"%s\"", name, size, type_char, (char*) value);

    return constant;
}

void llvm__fusion_programs(struct llvm__program* main, const struct llvm__program* toappend){
    //debug("FUSION\n", GREEN);
    //printf("\t main:\n");
    //llvm__print(main);
    //printf("\t toappend:\n");
    //llvm__print(toappend);
    //printf("\n");
    for(int i=0;i<toappend->line_number;i++){
        llvm__program_add_line(main, toappend->code[i]);
    }
}

char** llvm___create_function_def(struct Function function){
    if(!is_registered_external(function.identifier)) {
        char *definition;

        char *args = malloc(sizeof(char) * 256);
        args[0] = '\0';

        char ** func_var_initializer = malloc(sizeof(char*) * (1+function.var_list_size));

        for (int i = 0; i < function.var_list_size; i++) {
            m_strcat(args, TO_LLVM_STRING(function.var_list[i].type));
            m_strcat(args, " %");
            m_strcat(args, function.var_list[i].identifier);
            func_var_initializer[i+1] = alloca_func_param(function.var_list[i]);
            if (i != function.var_list_size - 1)
                m_strcat(args, ", ");
        }

        asprintf(&definition, "define %s @%s(%s) {", TO_LLVM_STRING(function.return_type), function.identifier, args);
        func_var_initializer[0] = definition;

        return func_var_initializer;
    }else{
        report_error(FUNCTION_EXTERNAL_REGISTERED, function.identifier);
        return NULL;
    }
}

/**
 *
 * @param code
 * @param e1
 * @param e2
 * @return true if conversion occured, false if not
 */
short int convert_if_needed(struct llvm__program* code, struct computed_expression* e1, struct computed_expression* e2){
    if(e1->type != e2->type){
        if(e1->type == T_DOUBLE){
            debug("CONVERT !", GREEN);
            int reg = new_register();
            llvm__program_add_line(code, convert_reg(e2->reg, e2->type, reg, e1->type));
            e2->type = e1->type;
            e2->reg = reg;
            return true;
        }
        if(e2->type == T_DOUBLE){
            debug("CONVERT !", GREEN);
            int reg = new_register();
            llvm__program_add_line(code, convert_reg(e1->reg, e1->type, reg, e2->type));
            e1->type = e2->type;
            e1->reg = reg;
            return true;
        }
        // Should not happend
        report_error(VOID_TYPE_USED_AS_VALUE, "");
        return false;
    }
    return false;
}
short int convert_computed_expr_to_type_if_needed(struct llvm__program* code, struct computed_expression* e, enum TYPE t){
    if(e->type != t){
        debug("CONVERT", GREEN);
        int reg = new_register();
        llvm__program_add_line(code, convert_reg(e->reg, e->type, reg, t));
        e->type = t;
        e->reg = reg;
        return true;
    }
    return false;
}

struct computed_expression* generate_code(struct Expression* e){
    printf("generate_expression : "); print_tree(e); printf(" => ");

    struct computed_expression* ret = malloc(sizeof(struct computed_expression));
    ret->code = malloc(sizeof(struct llvm__program));
    llvm__init_program(ret->code);

    if(e->type == E_CONDITIONAL && e->conditional_expression.type == C_LEAF){ // Operand
        printf("operande %d var %d \n", e->conditional_expression.leaf.prefix,
               e->conditional_expression.leaf.postfix);
        struct expr_operand* o = &e->conditional_expression.leaf;

        int* args_regs;
        enum TYPE* args_types;

        if(o->type == O_FUNCCALL_ARGS) {
            args_regs = (int*) malloc(sizeof(int)*o->operand.function.parameters.expression_count);
            args_types = (enum TYPE*) malloc(sizeof(enum TYPE)*o->operand.function.parameters.expression_count);
        }

        char* var_name;
        switch(o->type){
            case O_INT:
                ret->reg = new_register();
                ret->type = T_INT;
                llvm__program_add_line(ret->code,load_int(ret->reg, o->operand.int_value));
                break;
            case O_DOUBLE:
                ret->reg = new_register();
                ret->type = T_DOUBLE;
                llvm__program_add_line(ret->code,load_double(ret->reg, o->operand.double_value));
                break;
            case O_VARIABLE:
                var_name = o->operand.variable;
                printf("cherche pour la variable %s...", var_name);
                if(IS_FUNC_PARAM(&scope, var_name)){
                    asprintf(&var_name, "%s.addr", var_name);
                    printf(" est un argument de fonction chercher %s à la place...", var_name);
                }
                printf("\n");
                ret->reg = hash_lookup(&CURRENT_LOADED_REGS, var_name);
                
                ret->type = GET_VAR_TYPE(&scope, var_name);
                if(ret->reg == HASH_FAIL) {
                    ret->reg = new_register();
                    llvm__program_add_line(ret->code,load_var(ret->reg, var_name));
                }
                //prefix and postfix modifications
                int new_reg = 0;
                while (o->prefix > 0){
                    new_reg = new_register();
                    llvm__program_add_line(ret->code, binary_op_on_reg_const(REG_ADD, new_reg, ret->reg, 1, ret->type));
                    ret->reg = new_reg;
                    o->prefix--;
                }
                while (o->prefix < 0){
                    new_reg = new_register();
                    llvm__program_add_line(ret->code, binary_op_on_reg_const(REG_SUB, new_reg, ret->reg, 1, ret->type));
                    ret->reg = new_reg;
                    o->prefix++;
                }
                int old_reg = 0;
                while (o->postfix > 0){
                    printf("a++\n");
                    old_reg = ret->reg;
                    new_reg = new_register();
                    llvm__program_add_line(ret->code, binary_op_on_reg_const(REG_ADD, new_reg, ret->reg, 1, ret->type));
                    ret->reg = new_reg;
                    o->postfix--;
                }
                while (o->postfix < 0){
                    old_reg = ret->reg;
                    new_reg = new_register();
                    llvm__program_add_line(ret->code, binary_op_on_reg_const(REG_SUB, new_reg, ret->reg, 1, ret->type));
                    ret->reg = new_reg;
                    o->postfix++;
                }
                // Post or Pre fix opperation used
                if(new_reg != 0) {
                    llvm__program_add_line(ret->code, store_var(var_name, ret->reg, ret->type));
                    //Postfix operator used
                    if(old_reg != 0)
                        ret->reg = old_reg;
                }else {
                    new_reg = ret->reg;
                }

                hash_delete(&CURRENT_LOADED_REGS, var_name);
                hash_insert(&CURRENT_LOADED_REGS, var_name, new_reg); // new register of variable

                break;
            case O_FUNCCALL_ARGS:
                ret->type = hash__get_item(&scope, o->operand.function.name).declarator.function.return_type;
                if(e->conditional_expression.is_alone)
                    ret->reg = -1;
                else
                    ret->reg = new_register();

                for(int i = 0;i < o->operand.function.parameters.expression_count; i++){
                    llvm__fusion_programs(ret->code, o->operand.function.computed_array[i].code);
                    args_regs[i] = o->operand.function.computed_array[i].reg;
                    args_types[i] = o->operand.function.computed_array[i].type;
                }

                llvm__program_add_line(ret->code, call_function(ret->reg, o->operand.function.name, ret->type, args_types,
                                                                args_regs, o->operand.function.parameters.expression_count));
                break;
        }

        if(o->type == O_FUNCCALL_ARGS) {
            free(args_regs);
            free(args_types);
        }

    }else if(e->type == E_CONDITIONAL){ // Operation
        printf("operatioon\n");
        struct computed_expression* left = generate_code(e->conditional_expression.branch.e_left);
        struct computed_expression* right = generate_code(e->conditional_expression.branch.e_right);
        enum COND_OPERATOR operator = e->conditional_expression.branch.operator;
        llvm__fusion_programs(ret->code, left->code);
        llvm__fusion_programs(ret->code, right->code);

        //CONVERSION IF NEEDED
        if(convert_if_needed(ret->code, right, left)){
            //TODO report warning ? ou on le fait plus haut, c'est plus logique ?
        }

        ret->reg = new_register();

        ret->type = left->type;

        char* operation_code = operation_on_regs(operator, ret->reg, left->reg, right->reg, ret->type);
        llvm__program_add_line(ret->code, operation_code);

        free(left->code);
        free(left);
        free(right->code);
        free(right);

    }else if(e->type == E_AFFECT){
        printf("affect\n");
        //register is no longer up to date.
        hash_delete(&CURRENT_LOADED_REGS, e->expression.operand.operand.variable);
        //return type is the affected variable type
        ret->type = GET_VAR_TYPE(&scope, e->expression.operand.operand.variable);

        //if next nested expression is an affectation, it has already been computed
        if(e->expression.cond_expression->type == E_AFFECT ||
                (is_already_computed(e->expression.cond_expression))){
            print_tree(e->expression.cond_expression);
            printf(" est une expression déjà calculée dans %%x%d.\n", e->expression.cond_expression->code->reg);

            convert_computed_expr_to_type_if_needed(ret->code, e->expression.cond_expression->code, ret->type);
            ret->reg = e->expression.cond_expression->code->reg;
        }else {
            struct computed_expression *affected_value = generate_code(e->expression.cond_expression);

            convert_computed_expr_to_type_if_needed(affected_value->code, affected_value, ret->type);

            ret->reg = affected_value->reg;
            llvm__fusion_programs(ret->code, affected_value->code);

            free(affected_value);
        }
        switch (e->expression.assign_operator) {
            case OP_SIMPLE_ASSIGN:
                llvm__program_add_line(ret->code,
                                       store_var(e->expression.operand.operand.variable, ret->reg, ret->type));
                break;
            case OP_MUL_ASSIGN:
            case OP_DIV_ASSIGN:
            case OP_REM_ASSIGN:
            case OP_SHL_ASSIGN:
            case OP_SHR_ASSIGN:
            case OP_ADD_ASSIGN:
            case OP_SUB_ASSIGN:
                printf("TODO\n");
                llvm__program_add_line(ret->code, "TODO ASSIGN");
            default:
                printf("erreur. Enfin, je crois\n");
        }
        // new register for affected variable
        hash_insert(&CURRENT_LOADED_REGS, e->expression.operand.operand.variable, ret->reg);
    }else{
        printf("erreur. Je suppose.\n");
    }
    return ret;
}

struct llvm__program* generate_multiple_var_declarations(struct DeclaratorList* list, short int are_globals){
    struct llvm__program* ret = malloc(sizeof(struct llvm__program));
    llvm__init_program(ret);
    for(int i = 0; i < list->size; ++i){
        llvm__program_add_line(ret, declare_var(
                list->declarator_list[i].declarator.variable.identifier,
                list->declarator_list[i].declarator.variable.type,
                are_globals));
    }
    return ret;
}
struct llvm__program* generate_var_declaration(struct Variable* v, short int is_global){
    struct llvm__program* ret = malloc(sizeof(struct llvm__program));
    llvm__init_program(ret);
    llvm__program_add_line(ret, declare_var(v->identifier, v->type, is_global));
    return ret;
}

void llvm__print(const struct llvm__program* program){
    for(int i = 0; i < program->line_number; ++i){
        printf("%s\n", program->code[i]);
    }
}

struct llvm__program* generate_while_code(struct Expression* condition, struct llvm__program* statement_code, int is_dowhile){
    int start = new_label();
    int loop = new_label();
    int end = new_label();
    union COMPARATOR comparator;
    comparator.icmp = ICOMP_NE;

    establish_expression_final_type(condition);
    struct computed_expression* computed_condition = generate_code(condition);
    struct llvm__program* while_program = malloc(sizeof(struct llvm__program));
    llvm__init_program(while_program);

    if(is_dowhile){
        struct llvm__program while_jump = do_jump(computed_condition->type == T_INT ? 0 : 1, computed_condition->reg,
                                                  comparator, start, end);

        //do{ statement
        llvm__program_add_line(while_program, label_to_string(start, 1, ";do while start"));
        llvm__fusion_programs(while_program, statement_code);

        //}While(
        llvm__program_add_line(while_program, "; while condition");
        llvm__fusion_programs(while_program, computed_condition->code);
        llvm__fusion_programs(while_program, &while_jump);

        //);
        llvm__program_add_line(while_program, "; end of loop");
        llvm__program_add_line(while_program, label_to_string(end, 0, ";while end"));

    }else{
        struct llvm__program while_jump = do_jump(computed_condition->type == T_INT ? 0 : 1, computed_condition->reg,
                                                     comparator, loop, end);

        //While(
        llvm__program_add_line(while_program, label_to_string(start, 1, ";while start"));

        //expression){
        llvm__program_add_line(while_program, "; while condition");
        llvm__fusion_programs(while_program, computed_condition->code);
        llvm__fusion_programs(while_program, &while_jump);

        //statement
        llvm__program_add_line(while_program, label_to_string(loop, 0, ";while loop"));
        llvm__program_add_line(while_program, "; statement");
        llvm__fusion_programs(while_program, statement_code);

        //}
        llvm__program_add_line(while_program, "; loop");
        llvm__program_add_line(while_program, jump_to(start));
        llvm__program_add_line(while_program, label_to_string(end, 0, ";while end"));
    }

    return while_program;
}

struct llvm__program* generate_if_code(struct Expression* condition, struct llvm__program* statement_code){
    int then = new_label();
    int end = new_label();

    union COMPARATOR comparator;
    comparator.icmp = ICOMP_NE;

    establish_expression_final_type(condition);
    struct computed_expression* computed_condition = generate_code(condition);
    struct llvm__program* if_program = malloc(sizeof(struct llvm__program));
    struct llvm__program if_jump = do_jump(computed_condition->type == T_INT ? 0 : 1, computed_condition->reg, comparator,
                                                                then, end);

    llvm__init_program(if_program);

    llvm__program_add_line(if_program, "; if starting condition");
    llvm__fusion_programs(if_program, computed_condition->code);
    llvm__program_add_line(if_program, "; jump");
    llvm__fusion_programs(if_program, &if_jump);
    llvm__program_add_line(if_program, label_to_string(then, 0, ";then"));
    llvm__fusion_programs(if_program, statement_code);

    llvm__program_add_line(if_program, label_to_string(end, 1, ";endif"));

    return if_program;
}

struct llvm__program* generate_ifelse_code(struct Expression* condition, struct llvm__program* statement_if, struct llvm__program* statement_else){
    int then = new_label();
    int l_else = new_label();
    int end = new_label();

    union COMPARATOR comparator;
    comparator.icmp = ICOMP_NE;

    establish_expression_final_type(condition);
    struct computed_expression* computed_condition = generate_code(condition);
    struct llvm__program* if_else_program = malloc(sizeof(struct llvm__program));
    struct llvm__program if_jump = do_jump(computed_condition->type == T_INT ? 0 : 1, computed_condition->reg, comparator,
                                           then, l_else);

    llvm__init_program(if_else_program);

    llvm__program_add_line(if_else_program, "; if starting condition");
    llvm__fusion_programs(if_else_program, computed_condition->code);
    llvm__program_add_line(if_else_program, "; jump");
    llvm__fusion_programs(if_else_program, &if_jump);

    llvm__program_add_line(if_else_program, label_to_string(then, 0, ";then"));
    llvm__fusion_programs(if_else_program, statement_if);
    llvm__program_add_line(if_else_program, jump_to(end));

    llvm__program_add_line(if_else_program, label_to_string(l_else, 0, ";else"));
    llvm__fusion_programs(if_else_program, statement_else);

    llvm__program_add_line(if_else_program, label_to_string(end, 1, ";endif"));


    return if_else_program;
}

struct llvm__program do_jump(int float_or_int, int condition, union COMPARATOR comparator, int labeltrue, int labelfalse){
    struct llvm__program jump;
    char* cmp_line;
    char* br_line;
    int cmp_register = new_register();


    llvm__init_program(&jump);

    //float = 1, int = 0
    asprintf(&cmp_line, "%%x%d = %s %s i32 %%x%d, 0", cmp_register, float_or_int ? "fcmp" : "icmp",
             comparator_to_string(comparator, float_or_int), condition);
    asprintf(&br_line, "br i1 %%x%d, label %%label%d, label %%label%d", cmp_register, labeltrue, labelfalse);
    llvm__program_add_line(&jump, cmp_line);
    llvm__program_add_line(&jump, br_line);

    return jump;
}

struct llvm__program add_external_functions_declaration(){
    struct llvm__program functions;
    llvm__init_program(&functions);

    for(int i = 0;i < external_functions.functions_number; i++){
        if(external_functions.extern_functions[i].to_add){
            char* parameters = "";
            for(int j = 0; j < external_functions.extern_functions[i].function.var_list_size;j++){
                if(j == external_functions.extern_functions[i].function.var_list_size - 1)
                    parameters = concatenate_strings(2, parameters,
                    type_of(llvm__convert(external_functions.extern_functions[i].function.var_list[j].type)));
                else
                    parameters = concatenate_strings(3, parameters,
                                    type_of(llvm__convert(external_functions.extern_functions[i].function.var_list[j].type)), ", ");
            }
            char* function_line;
            asprintf(&function_line, "declare %s @%s(%s)", type_of(llvm__convert(external_functions.extern_functions[i].function.return_type)),
                     external_functions.extern_functions[i].function.identifier, parameters);
            llvm__program_add_line(&functions, function_line);
        }
    }

    llvm__program_add_line(&functions, "\n");

    return functions;
}

void write_file(struct llvm__program* main_program, char* filename){
    FILE* file = fopen(filename, "w");

    if(file == NULL){
        fflush(stdout);
        fprintf(stderr, "Error when trying to create or open file");
        exit(1);
    }

    for(int i=0;i<main_program->line_number;i++){
        fprintf(file, "%s\n", main_program->code[i]);
    }

    fclose(file);
}