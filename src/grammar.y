%{
#include <stdio.h>
#include <string.h>
#define _GNU_SOURCE
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
    enum RETTYPE plou_rettype;
    union VALUE plou_value;

    //Operators:
    enum ASSIGN_OPERATOR assign_operator;

    //Expressions
    struct Expression plou_expression;
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
    is_declared(&scope, $1, VARIABLE);
    $$ = create_leaf(init_operand_identifier($1));
    }
| CONSTANTI  {
    $$ = create_leaf(init_operand_integer($1));
    }
| CONSTANTF  {
    $$ = create_leaf(init_operand_double($1));
    }
| '(' expression ')' { $$ = $2; }
| IDENTIFIER '(' ')' {
    is_declared(&scope, $1, FUNCTION);
    $$ = create_leaf(init_operand_identifier($1));
    }
| IDENTIFIER '(' argument_expression_list ')' { // A MODIFIER
    is_declared(&scope, $1, FUNCTION);
    $$ = create_leaf(init_operand_identifier($1));
    }
;

postfix_expression
: primary_expression { $$ = $1; }
| postfix_expression INC_OP { operand_add_postfix(&($1.conditional_expression.leaf), 1); }
| postfix_expression DEC_OP { operand_add_postfix(&($1.conditional_expression.leaf), -1); }
;

unary_expression
: postfix_expression { $$ = $1; }
| INC_OP unary_expression { operand_add_prefix(&($2.conditional_expression.leaf), 1); }
| DEC_OP unary_expression { operand_add_prefix(&($2.conditional_expression.leaf), -1); }
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
        printf("DEBUG assigment de %s\n", $1.conditional_expression.leaf.operand.variable);
        $$ = expression_from_unary_cond(&($1.conditional_expression.leaf), $2, &$3);
        //TODO clean le magnifique débug <3
        struct computed_expression* e = generate_code(&$$);
        printf("\n\tcode:\n");
        llvm__print(&e->code);
        printf("reg: %%x%d\n", e->reg);

}
| conditional_expression { $$ = $1; }
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
: type_name declarator_list ';' { $2 = apply_type($1, $2); hash__add_items(&scope, $2); }
| type_name declarator '=' expression ';' {
    if($2.decl_type == VARIABLE){
        struct computed_expression* e = generate_code(&$4);
        printf("\n\tcode:\n");
        llvm__print(&e->code);
        printf("reg: %%x%d\n", e->reg);

        $2.declarator.variable.type = $1;
        hash__add_item(&scope, $2.declarator.variable.identifier, $2);

        //TODO affecter la valeur du registre de expression à la variable

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
: IDENTIFIER { $$.declarator.variable.identifier = $1; $$.decl_type = VARIABLE; /*PAR DEFAUT UNE VARIABLE, SINON ON RECUPERE JUSTE LA VALEUR PUIS ON ECRASE (plus haut)*/}
| '(' declarator ')' { $$ = $2; } //TODO SUREMENT PAS CA
| declarator '(' parameter_list ')' { $$ = declare_function($3, $1.declarator.variable.identifier); /*MOCHE MAIS SOLUTION LA PLUS SIMPLE*/}
| declarator '(' ')' { struct DeclaratorList empty; empty.size = 0; $$ = declare_function(empty, $1.declarator.variable.identifier); /*PAREIL*/}
;

parameter_list
: parameter_declaration { $$ = ADD_PARAMETER($$, $1);}
| parameter_list ',' parameter_declaration { $$ = $1; $$ = ADD_PARAMETER($$, $3);}
;

parameter_declaration
: type_name declarator { $$ = apply_decl_type($1, $2); }
;

statement
: declaration
| compound_statement
| expression_statement
| selection_statement
| iteration_statement
| jump_statement
;

LB
: '{' {level++ ; hash__upper_level(&scope); llvm__program_add_line(&program, "{");}// pour le hash[i] il faut faire attention si on retourne à un même level, ce n'est pas forcément le même bloc ! il faudra sûrement utiliser deux var, une disant le dernier hash_nb atteint et le hash_nb actuel à utiliser
;

RB
: '}' {level--; hash__lower_level(&scope); llvm__program_add_line(&program, "}");} // normalement ici pas de soucis pour le hash_nb
;

compound_statement
: LB RB
| LB statement_list RB
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
: statement                 {}
| statement_list statement  {}
;

expression_statement
: ';'
| expression ';'
;

selection_statement
: IF '(' expression ')' statement {
    printf("-- code if --\n");
    int true_label = new_label();
    int false_label = new_label();
    printf("\texpression.code\n");
    printf("\tbr i1 %%<expression.reg> label %%label%d, label %%label%d\n", true_label, false_label);
    printf("\tlabel%d:\n", true_label);
    printf("\tstatement.code\n");
    printf("\tlabel%d:\n", false_label);
    printf("-- /if --");
}
| IF '(' expression ')' statement ELSE statement {
    printf("-- code if else --\n");
    int true_label = new_label();
    int false_label = new_label();
    printf("\texpression.code\n");
    printf("\tbr i1 %%<expression.reg> label %%label%d, label %%label%d\n", true_label, false_label);
    printf("\tlabel%d:\n", true_label);
    printf("\ts1.code\n");
    printf("\tlabel%d:\n", false_label);
    printf("\ts2.code\n");
    printf("\t-- /if --\n");
}
| FOR '(' expression ';' expression ';' expression ')' statement {
    printf("-- code for(e1;e2;e3) s --\n");
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
    printf("-- /for --\n");
}
| FOR '(' expression ';' expression ';'            ')' statement
| FOR '(' expression ';'            ';' expression ')' statement
| FOR '(' expression ';'            ';'            ')' statement
| FOR '('            ';' expression ';' expression ')' statement
| FOR '('            ';' expression ';'            ')' statement
| FOR '('            ';'            ';' expression ')' statement
| FOR '('            ';'            ';'            ')' statement
;

iteration_statement
: WHILE '(' expression ')' statement {
    printf("-- code while(e) s --\n");
    int start = new_label();
    int loop = new_label();
    int end = new_label();
    printf("\tlabel%d:", start);
    printf("\te.code\n");
    printf("\tbr i1 %%<e.reg> label %%label%d, label %%label%d\n", loop, end);
    printf("\tlabel%d:\n", loop);
    printf("\ts.code\n");
    printf("\tbr label %%label%d\n", start);
    printf("\tlabel%d:\n", end);
    printf("-- /while --\n");
};
| DO statement WHILE '(' expression ')' ';' {
    printf("-- code do s while(e); --\n");
    int loop = new_label();
    int end = new_label();
    printf("\tlabel%d:\n", loop);
    printf("\ts.code\n");
    printf("\te.code\n");
    printf("\tbr i1 %%<e.reg> label %%label%d, label %%label%d\n", loop, end);
    printf("\tlabel%d:\n", end);
    printf("-- /dowhile --\n");
}
;

jump_statement
: RETURN ';'
| RETURN expression ';'
;

program
: external_declaration
| program external_declaration
;

external_declaration
: function_definition
| declaration
;

function_definition
: function_declaration compound_statement
;

function_declaration
: type_name declarator {$2.declarator.function.return_type = $1;
    if(hash__add_item(&scope, $2.declarator.function.identifier, $2)){
        printf("%s\n", llvm___create_function_def(hash__get_item(&scope, $2.declarator.function.identifier).declarator.function));
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
    if (argc==2) {
        input = fopen (argv[1], "r");
        file_name = strdup(argv[1]);
        if (input) {
            yyin = input;
        }
        else {
            fprintf (stderr, "%s: Could not open %s\n", *argv, argv[1]);
            return 1;
        }
    }
    else {
        fprintf (stderr, "%s: error: no input file\n", *argv);
        return 1;
    }
    yyparse ();

    //First we verify errors
    verify_no_error(file_name);


    //Then we create code

    return 0;
}
