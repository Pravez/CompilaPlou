#define _GNU_SOURCE
#include "llvm_code.h"
#include "type.h"
#include "external_function.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

short int no_optimization = 0;

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
    if(main->validity != -1 && toappend->validity != -1) {
        for (int i = 0; i < toappend->line_number; i++) {
            llvm__program_add_line(main, toappend->code[i]);
        }
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
short int convert_if_needed(struct llvm__program* code, struct computed_expression* e1, struct computed_expression* e2, enum TYPE convert_desired){
    //Probably not the best solution
    if(convert_desired != -1){
        if(e1->type != convert_desired){
            int reg = new_register();
            llvm__program_add_line(code, convert_reg(e1->reg, e1->type, reg, convert_desired));
            e1->type = convert_desired;
            e1->reg = reg;
        }
        if(e2->type != convert_desired){
            int reg = new_register();
            llvm__program_add_line(code, convert_reg(e2->reg, e2->type, reg, convert_desired));
            e2->type = convert_desired;
            e2->reg = reg;
        }
        return true;
    }else if(e1->type != e2->type){
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
    if(e->type == T_VOID && t != T_VOID){
        report_error(VOID_TYPE_USED_AS_VALUE, "");
    }else
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
        printf("operande %d %svar %d \n",
               e->conditional_expression.leaf.prefix,
               e->conditional_expression.is_negative ? "-" : "",
               e->conditional_expression.leaf.postfix);
        struct expr_operand* o = &e->conditional_expression.leaf;

        int* args_regs;
        enum TYPE* args_types;
        short int invert_applied = 0;

        if(o->type == O_FUNCCALL_ARGS) {
            args_regs = (int*) malloc(sizeof(int)*o->operand.function.parameters.expression_count);
            args_types = (enum TYPE*) malloc(sizeof(enum TYPE)*o->operand.function.parameters.expression_count);
        }

        char* var_name;
        switch(o->type){
            case O_INT:
                ret->reg = new_register();
                ret->type = T_INT;
                invert_applied = e->conditional_expression.is_negative;
                llvm__program_add_line(ret->code,load_int(ret->reg, o->operand.int_value, invert_applied));
                break;
            case O_DOUBLE:
                ret->reg = new_register();
                ret->type = T_DOUBLE;
                invert_applied = e->conditional_expression.is_negative;
                llvm__program_add_line(ret->code,load_double(ret->reg, o->operand.double_value, invert_applied));
                break;
            case O_VARIABLE:
                var_name = o->operand.variable;
                printf("cherche pour la variable %s...", var_name);
                if(IS_FUNC_PARAM(&scope, var_name)){
                    asprintf(&var_name, "%s.addr", var_name);
                    printf(" est un argument de fonction chercher %s à la place...", var_name);
                }
                printf("\n");

                if(!no_optimization)
                    ret->reg = hash_lookup(&CURRENT_LOADED_REGS, var_name);

                ret->type = GET_VAR_TYPE(&scope, var_name);
                if(no_optimization || ret->reg == HASH_FAIL) {
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

        if(e->conditional_expression.is_negative && !invert_applied){
            int temp_reg = new_register();
            llvm__program_add_line(ret->code, invert_value(ret->reg, ret->type, temp_reg));
            ret->reg = temp_reg;
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
        if(convert_if_needed(ret->code, right, left, operator == OP_AND ? T_INT : -1)){
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
        char* affected_var_name = e->expression.operand.operand.variable;
        int old_var_reg = hash_lookup(&CURRENT_LOADED_REGS, affected_var_name);
        hash_delete(&CURRENT_LOADED_REGS, affected_var_name);
        //return type is the affected variable type
        ret->type = GET_VAR_TYPE(&scope, affected_var_name);
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
        enum REG_BINARY_OP operation = -1;  //potantial operation before affecation
        switch (e->expression.assign_operator) {
            case OP_SIMPLE_ASSIGN:
                // no operation to do before assign
                break;
            case OP_MUL_ASSIGN:
                operation = REG_MUL; break;
            case OP_DIV_ASSIGN:
                operation = REG_DIV; break;
            case OP_REM_ASSIGN:
                operation = REG_REM; break;
            case OP_SHL_ASSIGN:
                operation = REG_SHL; break;
            case OP_SHR_ASSIGN:
                operation = REG_SHR; break;
            case OP_ADD_ASSIGN:
                operation = REG_ADD; break;
            case OP_SUB_ASSIGN:
                operation = REG_SUB; break;
            default:
                printf("erreur. Enfin, je crois\n");
        }
        if(operation != -1){ //operation need to be performed before affectation
            if(old_var_reg == HASH_FAIL){ // loading var if needed
                old_var_reg = new_register();
                llvm__program_add_line(ret->code, load_var(old_var_reg, affected_var_name));
            }
            int new_var_reg = new_register();// adding var to the result of the given expression (stored in ret->reg atm)
            llvm__program_add_line(ret->code, binary_op_on_regs(operation, new_var_reg, old_var_reg, ret->reg, ret->type));
            ret->reg = new_var_reg;
        }

        llvm__program_add_line(ret->code, store_var(affected_var_name, ret->reg, ret->type));

        // new register for affected variable
        hash_insert(&CURRENT_LOADED_REGS, affected_var_name, ret->reg);
    }else{
        printf("erreur. Je suppose.\n");
    }
    return ret;
}

struct llvm__program* generate_multiple_var_declarations(struct DeclaratorList* list, short int is_global){
    struct llvm__program* ret = malloc(sizeof(struct llvm__program));
    llvm__init_program(ret);
    for(int i = 0; i < list->size; ++i){
        llvm__program_add_line(ret, declare_var(
                list->declarator_list[i].declarator.variable.identifier,
                list->declarator_list[i].declarator.variable.type, (struct global_declaration){ .is_global = is_global,
                        .double_value = 0.0, .int_value = 0 }));
        if(is_global){
            report_warning(GLOBAL_NO_INIT, list->declarator_list[i].declarator.variable.identifier);
        }
    }
    return ret;
}

struct llvm__program* generate_var_declaration(struct Variable* v, struct global_declaration global){
    struct llvm__program* ret = malloc(sizeof(struct llvm__program));
    llvm__init_program(ret);
    llvm__program_add_line(ret, declare_var(v->identifier, v->type, global));
    return ret;
}

struct llvm__program* generate_global_decl_and_affect(struct expr_operand* operand, struct Variable *var){
    if(operand->type == O_DOUBLE){
        return generate_var_declaration(var, (struct global_declaration){ .double_value = operand->operand.double_value, .is_global = 1});
    }else if(operand->type == O_INT){
        return generate_var_declaration(var, (struct global_declaration){ .int_value = operand->operand.int_value, .is_global = 1});
    }else{
        report_error(GLOBAL_NEED_DOUBLE_INT, var->identifier);
        return NULL;
    }
}

void llvm__print(const struct llvm__program* program){
    for(int i = 0; i < program->line_number; ++i){
        printf("%s\n", program->code[i]);
    }
}
struct llvm__program* generate_for_code(struct Expression* initial, struct Expression* condition, struct Expression* moving, struct llvm__program* statement_code)
{
    /*
    printf("\nFOR !!!\n\n");
    printf("initial: ");
    print_tree(initial);
    printf("\ncode:\n");
    llvm__print(initial->code->code);
    printf("\ncondition: ");
    print_tree(condition);
    printf("\n moving: ");
    print_tree(moving);
    printf("\n")
            */

    //initialisation
    int start = new_label();
    int loop = new_label();
    int condition_label = new_label();
    int end = new_label();
    union COMPARATOR comparator;
    comparator.icmp = ICOMP_NE;

    struct computed_expression *computed_condition;
    struct computed_expression *computed_initialisation;
    struct computed_expression *computed_moving;

    if (initial == NULL) {
        report_warning(MISSING_AN_INITIALISATION, "");
        computed_initialisation = malloc(sizeof(struct computed_expression));
        computed_initialisation->code = malloc(sizeof(struct llvm__program));
        llvm__init_program(computed_initialisation->code);
        }
    else {
        if(is_already_computed(initial))
            computed_initialisation = initial->code;
        else
            computed_initialisation = generate_code(initial);
    }

    if (condition == NULL) {
        report_error(MISSING_A_CONDITION, "");
        return NULL;
    }
    else {
        no_optimization = 1;
        //establish_expression_final_type(condition);
        if(is_already_computed(condition))
            computed_condition = condition->code;
        else
            computed_condition = generate_code(condition);
        no_optimization = 0;
    }

    if (moving == NULL) {
        report_warning(MISSING_A_MOVING, "");
        computed_moving = malloc(sizeof(struct computed_expression));
        computed_moving->code = malloc(sizeof(struct llvm__program));
        llvm__init_program(computed_moving->code);
    }
    else {
        if(is_already_computed(moving))
            computed_moving = moving->code;
        else
            computed_moving = generate_code(moving);
    }

    struct llvm__program* for_program = malloc(sizeof(struct llvm__program));
    llvm__init_program(for_program);
    struct llvm__program for_jump = do_jump(computed_condition->type == T_INT ? 0 : 1, computed_condition->reg, comparator, loop, end);


    //for ( initialisation,
    llvm__program_add_line(for_program, label_to_string(start, 1, ";for start"));
    llvm__program_add_line(for_program, "; for initialisation");
    llvm__fusion_programs(for_program, computed_initialisation->code);

    //( .. , condition, .., ) {
    llvm__program_add_line(for_program, label_to_string(condition_label, 1, ";for loop (condition)"));
    llvm__fusion_programs(for_program, computed_condition->code);
    llvm__fusion_programs(for_program, &for_jump);

    //statement
    llvm__program_add_line(for_program, label_to_string(loop, 0, ";for content"));
    llvm__program_add_line(for_program, "; statement");
    llvm__fusion_programs(for_program, statement_code);

    //moving )
    llvm__program_add_line(for_program, "; for moving");
    llvm__fusion_programs(for_program, computed_moving->code);

    //}
    llvm__program_add_line(for_program, "; loop");
    llvm__program_add_line(for_program, jump_to(condition_label));
    llvm__program_add_line(for_program, label_to_string(end, 0, ";for end"));


    free(computed_initialisation);
    free(computed_condition);
    free(computed_moving);
    return for_program;
}

struct llvm__program* generate_while_code(struct Expression* condition, struct llvm__program* statement_code, int is_dowhile){
    int start = new_label();
    int loop = new_label();
    int end = new_label();
    union COMPARATOR comparator;
    comparator.icmp = ICOMP_NE;

    establish_expression_final_type(condition);
    no_optimization = 1;
    struct computed_expression* computed_condition = generate_code(condition);
    no_optimization = 0;
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
    no_optimization = 1;
    struct computed_expression* computed_condition = generate_code(condition);
    no_optimization = 0;
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
    no_optimization = 1;
    struct computed_expression* computed_condition = generate_code(condition);
    no_optimization = 0;
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
    asprintf(&cmp_line, "%%x%d = %s %s %s %%x%d, %s", cmp_register, float_or_int ? "fcmp" : "icmp",
             comparator_to_string(comparator, float_or_int), float_or_int ? "double" : "i32", condition, float_or_int ? "0.0" : "0");
    asprintf(&br_line, "br i1 %%x%d, label %%label%d, label %%label%d", cmp_register, labeltrue, labelfalse);
    llvm__program_add_line(&jump, cmp_line);
    llvm__program_add_line(&jump, br_line);

    return jump;
}

struct llvm__program add_external_functions_declaration(){
    struct llvm__program functions;
    llvm__init_program(&functions);

    char* functions_lines[external_functions.functions_number];

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
            asprintf(&functions_lines[i], "declare %s @%s(%s)", type_of(llvm__convert(external_functions.extern_functions[i].function.return_type)),
                     external_functions.extern_functions[i].function.identifier, parameters);
            llvm__program_add_line(&functions, functions_lines[i]);
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

    printf("lignes : %d\n", main_program->line_number);
    for(int i=0;i<main_program->line_number;i++){
        fprintf(file, "%s\n", main_program->code[i]);
    }

    fclose(file);
}
