%{
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <search.h>
#include <stdlib.h>

#include "type.h"
#include "scope.h"
#include "tools.h"
#include "llvm_code.h"
#include "hash.h"
#include "expression.h"
#include "errors.h"

extern int yylineno;
int yylex ();
void yyerror (char const*);
int level = 0; // ne peut pas être négatif

struct llvm__program program;
%}

%token <string> IDENTIFIER // id d'un objet
%token <constf> CONSTANTF // un flottant (double)
%token <consti> CONSTANTI // un int
%token INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP
%token SUB_ASSIGN MUL_ASSIGN ADD_ASSIGN DIV_ASSIGN
%token SHL_ASSIGN SHR_ASSIGN
%token REM_ASSIGN
%token REM SHL SHR
%token AND OR
%token TYPE_NAME
%token INT DOUBLE VOID
%token IF ELSE DO WHILE RETURN FOR

//Declarations
%type <plou_declarator_list> declarator_list parameter_list
%type <plou_declarator> declarator parameter_declaration
%type <plou_type> type_name
%type <assign_operator> assignment_operator

//Expressions
%type <plou_expression> expression
%type <plou_expression> conditional_expression logical_or_expression logical_and_expression multiplicative_expression additive_expression comparison_expression
%type <plou_expression> primary_expression postfix_expression shift_expression unary_expression

//Statement
%type <code> expression_statement declaration statement compound_statement iteration_statement function_definition
%type <code> jump_statement selection_statement statement_list function_declaration program_parts external_declaration

%start program
%union {
    char *string;
    int consti;
    double constf;

    //Declaration
    struct DeclaratorList plou_declarator_list;
    struct Declarator plou_declarator;

    //Variables and functions
    struct Variable plou_variable;
    struct Function plou_function;

    //Basics : values and types
    enum TYPE plou_type;

    //Operators:
    enum ASSIGN_OPERATOR assign_operator;

    //Expressions
    struct Expression plou_expression;

    //Code
    struct llvm__program code;
}
%%

conditional_expression
: logical_or_expression { $$ = $1; }
;

logical_or_expression
: logical_and_expression { $$ = $1; }
| logical_or_expression OR logical_and_expression { $$ = create_branch_cpy(OP_OR, $3, $1); }
;

logical_and_expression
: comparison_expression { $$ = $1; }
| logical_and_expression AND comparison_expression { $$ = create_branch_cpy(OP_AND, $3, $1); }
;


shift_expression
: additive_expression { $$ = $1; }
| shift_expression SHL additive_expression { $$ = create_branch_cpy(OP_SSHL, $3, $1); }
| shift_expression SHR additive_expression { $$ = create_branch_cpy(OP_SSHR, $3, $1); }
;

primary_expression
: IDENTIFIER {
    $$ = create_leaf(init_operand_identifier($1));
    if(!is_declared(&scope, $1, VARIABLE)){
        $$.type = -1;
    }
    }
| CONSTANTI  {
    $$ = create_leaf(init_operand_integer($1));
    }
| CONSTANTF  {
    $$ = create_leaf(init_operand_double($1));
    }
| '(' expression ')' { $$ = $2; }
| IDENTIFIER '(' ')' {
    $$ = create_leaf(init_operand_identifier($1));
    if(!is_declared(&scope, $1, FUNCTION)){
        $$.type = -1;
    }
    }
| IDENTIFIER '(' argument_expression_list ')' { // A MODIFIER
    $$ = create_leaf(init_operand_identifier($1));
    if(!is_declared(&scope, $1, FUNCTION)){
        $$.type = -1;
    }
    }
;

postfix_expression
: primary_expression { $$ = $1; }
| postfix_expression INC_OP { operand_add_postfix(&($1.conditional_expression.leaf), 1); $$ = $1; }
| postfix_expression DEC_OP { operand_add_postfix(&($1.conditional_expression.leaf), -1); $$ = $1; }
;

unary_expression
: postfix_expression { $$ = $1; }
| INC_OP unary_expression { operand_add_prefix(&($2.conditional_expression.leaf), 1); $$ = $2; }
| DEC_OP unary_expression { operand_add_prefix(&($2.conditional_expression.leaf), -1); $$ = $2; }
//| unary_operator unary_expression {printf("negation de l'espace\n");}
| '-' unary_expression {printf("negation de l'espace\n");}
;

argument_expression_list
: expression
| argument_expression_list ',' expression
;

multiplicative_expression
: unary_expression { $$ = $1; }
| multiplicative_expression '*' unary_expression { $$ = create_branch_cpy(OP_MUL, $3, $1); }
| multiplicative_expression '/' unary_expression { $$ = create_branch_cpy(OP_DIV, $3, $1); }
| multiplicative_expression REM unary_expression { $$ = create_branch_cpy(OP_REM, $3, $1); }
;

additive_expression
: multiplicative_expression { $$ = $1; }
| additive_expression '+' multiplicative_expression { $$ = create_branch_cpy(OP_ADD, $3, $1); }
| additive_expression '-' multiplicative_expression { $$ = create_branch_cpy(OP_SUB, $3, $1); }
;

comparison_expression
: shift_expression { $$ = $1; }
| comparison_expression '<' shift_expression { $$ = create_branch_cpy(OP_SHL, $3, $1); }
| comparison_expression '>' shift_expression { $$ = create_branch_cpy(OP_SHR, $3, $1); }
| comparison_expression LE_OP shift_expression { $$ = create_branch_cpy(OP_LE, $3, $1); }
| comparison_expression GE_OP shift_expression { $$ = create_branch_cpy(OP_GE, $3, $1); }
| comparison_expression EQ_OP shift_expression { $$ = create_branch_cpy(OP_EQ, $3, $1); }
| comparison_expression NE_OP shift_expression { $$ = create_branch_cpy(OP_NE, $3, $1); }
;

expression
: unary_expression assignment_operator expression {
        if($1.type == E_CONDITIONAL && $1.conditional_expression.type == C_LEAF && $1.conditional_expression.leaf.type != O_VARIABLE){
            report_error(NOT_ASSIGNABLE_EXPR, "");
            struct llvm__program empty;
            llvm__init_program(&empty);
            $$.code->code = &empty;
            $$.code->type = T_VOID;
            $$.code->reg = -1;
        }else if($1.type != -1){
            char* msg;
            asprintf(&msg, "Assigment de %s:", $1.conditional_expression.leaf.operand.variable);
            debug(msg, GREEN);
            free(msg);
            if(expression_from_unary_cond(&($1.conditional_expression.leaf), $2, &$3, &$$)){
                //TODO clean le magnifique débug <3
                set_initialized(&scope, $1.conditional_expression.leaf.operand.variable);

                struct computed_expression* e = generate_code(&$$);
                //printf("\n\tcode:\n");
                //llvm__print(e->code);
                //printf("reg: %%x%d\n", e->reg);

                //debug("fin 1?", GREEN);
                // If next expression has already been calculated (it's an affectation)
                if($3.type == E_AFFECT){
                    //TODO je sais pas pourquoi $3.code peut être null... mais bon XD
                    if($3.code == NULL || $3.code->code == NULL){
                        debug("NE DEVRAIT PAS ARRIVER.... T.T", RED);
                    }else{
                        //debug("pas null, la suite est une affecation", GREEN);
                        llvm__fusion_programs($3.code->code, e->code);
                        free(e->code);
                        e->code = $3.code->code;

                        /*debug("dfevrait pas arriver, je crois", RED);
                        $3.code = malloc(sizeof(struct computed_expression));
                        llvm__init_program($3.code->code);*/
                    }
                }
                //printf("\t fusionné: \n");
                //llvm__print(e->code);

                $$.code = e;
                //debug("fin 2?\n", GREEN);
                //free(e);
            }else{
                report_error(NOT_ASSIGNABLE_EXPR, "");
            }
        }else{
            // Undeclared variable... Keep going to look for more errors.
            struct llvm__program empty;
            llvm__init_program(&empty);
            $$.code->code = &empty;
            $$.code->type = T_VOID;
            $$.code->reg = -1;
        }
    }
| conditional_expression { 
    //TODO implementer les operateurs unaires ici
    if($1.type != -1){
        $$ = $1;
    }else{
        // Undeclared variable... Keep going to look for more errors.
        struct llvm__program empty;
        llvm__init_program(&empty);
        $$.code->code = &empty;
        $$.code->type = T_VOID;
        $$.code->reg = -1;
    }
}
| '(' type_name ')' conditional_expression {
    if($2 == T_VOID){
        report_error(VOID_ASSIGN, "");
    }else{
        if($4.type != -1){
            if($2 == establish_expression_final_type(&$4)){
                report_warning(USELESS_CAST, "");
            }else{
                //TODO faire le cast.. mais comme $4 a déjà été évalué... XD
                debug("CAST", BLUE);
            }
            $$ = $4;
        }else{
            // Undeclared variable... Keep going to look for more errors.
            struct llvm__program empty;
            llvm__init_program(&empty);
            $$.code->code = &empty;
            $$.code->type = T_VOID;
            $$.code->reg = -1;
        }
    }
}
;

assignment_operator
: '='           {$$ = OP_SIMPLE_ASSIGN;}
| MUL_ASSIGN    {$$ = OP_MUL_ASSIGN;}
| DIV_ASSIGN    {$$ = OP_DIV_ASSIGN;}
| REM_ASSIGN    {$$ = OP_REM_ASSIGN;}
| SHL_ASSIGN    {$$ = OP_SHL_ASSIGN;}
| SHR_ASSIGN    {$$ = OP_SHR_ASSIGN;}
| ADD_ASSIGN    {$$ = OP_ADD_ASSIGN;}
| SUB_ASSIGN    {$$ = OP_SUB_ASSIGN;}
;

declaration
: type_name declarator_list ';' {
    $2 = apply_type($1, $2);
    if(hash__add_items(&scope, $2)){
        $$ = *generate_multiple_var_declarations(&$2, 0); //TODO gérer le cas où c'est global
    }
}
| type_name declarator '=' expression ';' {
    if($2.decl_type == VARIABLE){
        char* msg;
        asprintf(&msg, "Declaration + assigment de %s:", $2.declarator.variable.identifier);
        debug(msg, GREEN);
        free(msg);

        $2.declarator.variable.type = $1;
        if(verify_expression_type($2, &$4)){
            $2.declarator.variable.initialized = 1;
            if(hash__add_item(&scope, $2.declarator.variable.identifier, $2)){
                struct llvm__program* decl = generate_var_declaration(&$2.declarator.variable, false); //TODO gérer le cas où c'est global

                struct Expression affected_value;
                struct expr_operand declarated_operand = variable_to_expr_operand(&$2.declarator.variable);

                if(expression_from_unary_cond(&declarated_operand, OP_SIMPLE_ASSIGN, &$4, &affected_value)){
                    //TODO clean le magnifique débug <3
                    set_initialized(&scope, $2.declarator.variable.identifier);

                    struct computed_expression* e = generate_code(&affected_value);

                    // If next expression has already been calculated (it's an affectation)
                    if($4.type == E_AFFECT){
                        //TODO je sais pas pourquoi $4.code peut être null... mais bon XD
                        if($4.code == NULL || $4.code->code == NULL){
                            debug("NE DEVRAIT PAS ARRIVER.... T.T", RED);
                        }else{
                            llvm__fusion_programs($4.code->code, e->code);
                            free(e->code);
                            e->code = $4.code->code;
                        }
                    }

                    llvm__fusion_programs(decl, e->code);
                    $$ = *decl;
                    free(e);
                }else{
                    report_error(NOT_ASSIGNABLE_EXPR, "");
                }
            }else{
                report_error(DEFINED_VAR, $2.declarator.variable.identifier);
            }
        }else{
            // TODO ?
            debug("ERROR, OPERAND IS NOT A VARIABLE !!", RED);
        }
    } else{
        report_error(FUNCTION_AS_VARIABLE, $2.declarator.function.identifier);
    }
}
;

declarator_list
: declarator { $$ = add_declarator($$, $1); }
| declarator_list ',' declarator { $$ = $1; $$ = add_declarator($$, $3); }
;

type_name
: VOID { $$ = T_VOID;}
| INT { $$ = T_INT;}
| DOUBLE { $$ = T_DOUBLE;}
;

declarator
: IDENTIFIER { $$ = init_declarator_as_variable($1); /*PAR DEFAUT UNE VARIABLE, SINON ON RECUPERE JUSTE LA VALEUR PUIS ON ECRASE (plus haut)*/}
| '(' declarator ')' { $$ = $2; } //TODO SUREMENT PAS CA
| declarator '(' parameter_list ')' { $$ = declare_function($3, $1.declarator.variable.identifier); /*MOCHE MAIS SOLUTION LA PLUS SIMPLE*/}
| declarator '(' ')' { struct DeclaratorList empty; empty.size = 0; $$ = declare_function(empty, $1.declarator.variable.identifier); /*PAREIL*/}
;

parameter_list
: parameter_declaration { $$ = add_parameter($$, $1);}
| parameter_list ',' parameter_declaration { $$ = $1; $$ = add_parameter($$, $3);}
;

parameter_declaration
: type_name declarator { $$ = apply_decl_type($1, $2); }
;

statement
: declaration               {printf("--- statement DECL ---\n"); $$ = $1; llvm__print(&$1); printf("--- END  statement ---\n");}
| compound_statement        { $$ = $1; }
| expression_statement      {printf("--- statement CODE ---\n"); $$ = $1; llvm__print(&$1); printf("--- END  statement ---\n");}
| selection_statement       {printf("--- statement IF OR FOR ---\n");$$ = $1; llvm__print(&$1); printf("--- END  statement ---\n");}
| iteration_statement       {printf("--- statement WHILE ---\n"); $$ = $1; llvm__print(&$1);printf("--- END  statement ---\n");}
| jump_statement            {printf("\n\t\t\treturn. FAUT SORTIR !\n\n");}
;

LB
: '{' {level++ ; if(!hash__upper_level(&scope)) YYABORT; }// pour le hash[i] il faut faire attention si on retourne à un même level, ce n'est pas forcément le même bloc ! il faudra sûrement utiliser deux var, une disant le dernier hash_nb atteint et le hash_nb actuel à utiliser
;

RB
: '}' {level--; hash__lower_level(&scope);} // normalement ici pas de soucis pour le hash_nb
;

compound_statement
: LB RB { }
| LB statement_list RB { $$ = $2; }
/*| LB declaration_list statement_list RB
| LB declaration_list RB*/
;
/*
declaration_list
: declaration
| declaration_list declaration
;
*/

statement_list
: statement                 { $$ = $1; }
| statement_list statement  { llvm__fusion_programs(&$1, &$2); $$ = $1; }
;

expression_statement
: ';'            {struct llvm__program empty; llvm__init_program(&empty); $$ = empty;};
| expression ';' {$$ = *$1.code->code;}
;

selection_statement
: IF '(' expression ')' statement { $$ = *generate_if_code(&$3, &$5); }
| IF '(' expression ')' statement ELSE statement { $$ = *generate_ifelse_code(&$3, &$5, &$7); }
| FOR '(' expression ';' expression ';' expression ')' statement {
    /*printf("-- code for(e1;e2;e3) s --\n");
    int loop = new_label();
    int end = new_label();
    printf("\te1.code\n");
    printf("\te2.code\n");
    printf("\tbr i1 %%<e2.reg> label %%label%d, label %%label%d\n", loop, end);
    printf("\tlabel%d:\n", loop);
    printf("\tstatement.code\n");
    printf("\te3.code\n");
    printf("\te2.code\n");
    printf("\tbr i1 %%<e2.reg> label %%label%d, label %%label%d\n", loop, end);
    printf("\tlabel%d:\n", end);
    printf("-- /for --\n");*/
}
| FOR '(' expression ';' expression ';'            ')' statement {}
| FOR '(' expression ';'            ';' expression ')' statement {}
| FOR '(' expression ';'            ';'            ')' statement {}
| FOR '('            ';' expression ';' expression ')' statement {}
| FOR '('            ';' expression ';'            ')' statement {}
| FOR '('            ';'            ';' expression ')' statement {}
| FOR '('            ';'            ';'            ')' statement {}
;

iteration_statement
: WHILE '(' expression ')' statement { $$ = *generate_while_code(&$3, &$5, 0); }
| DO statement WHILE '(' expression ')' ';' { $$ = *generate_while_code(&$5, &$2, 1); }
;

jump_statement
: RETURN ';' {}
| RETURN expression ';' {}
;

program
: program_parts { llvm__fusion_programs(&program, &$1); }
;

program_parts
: external_declaration { $$ = $1; }
| program_parts external_declaration { llvm__fusion_programs(&$1, &$2); $$ = $1; }
;

external_declaration
: function_definition { $$ = $1; }
| declaration { $$ = $1; }
;

function_definition
: function_declaration compound_statement { llvm__fusion_programs(&$1, &$2);llvm__program_add_line(&$1, "}"); $$ = $1; }
;

function_declaration
: type_name declarator {$2.declarator.function.return_type = $1;
    if(hash__add_item(&scope, $2.declarator.function.identifier, $2)){
        struct llvm__program temp;
        llvm__init_program(&temp);
        llvm__program_add_line(&temp, llvm___create_function_def(hash__get_item(&scope, $2.declarator.function.identifier).declarator.function));
        $$ = temp;
    }}
;

%%
#include <stdio.h>
#include <string.h>

extern char yytext[];
extern int column;
extern int yylineno;
extern FILE *yyin;

char *file_name = NULL;

//Voir le error handling de gnu bison et le location-type
void yyerror (char const *s) {
    fflush (stdout);
    fprintf (stderr, "%s:%d:%d: %s\n", file_name, yylineno, column, s);

    //return 0;
}


int main (int argc, char *argv[]) {
    hash__init(&scope);
    llvm__init_program(&program);
    error_flag = 0;
    FILE *input = NULL;
    char* file_name_output;
    if (argc>=2) {
        input = fopen (argv[1], "r");
        file_name = strdup(argv[1]);
        if (input) {
            yyin = input;
            if(argc == 3){
                file_name_output = argv[2];
                //If no extension we add it
                if(strcmp(".ll", (file_name_output+(strlen(file_name_output)-4))) != 0){
                    asprintf(&file_name_output, "%s.lli", file_name_output);
                }
            }else{
                file_name_output = "output.ll";
            }
        }
        else {
            fprintf (stderr, "%s: Could not open %s\n", *argv, argv[1]);
            fprintf (stderr, "Usage : parse C_FILE [OUTPUT_FILE]\n");
            return 1;
        }
    }
    else {
        fprintf (stderr, "%s: error: no input file\n", *argv);
        fprintf (stderr, "Usage : parse C_FILE [OUTPUT_FILE]\n");
        return 1;
    }

    yyparse ();
    fclose(input);

    //First we verify errors
    if(verify_no_error(file_name)){
        //Then we create code
        printf("Writing program ...\n");
        write_file(&program, file_name_output);
    }



    return 0;
}
