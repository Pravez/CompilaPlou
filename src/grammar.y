%{
#include <stdio.h>
#include <string.h>
#define _GNU_SOURCE
#include <search.h>

#include "type.h"
#include "tools.h"
extern int yylineno;
int yylex ();
void yyerror (char const*);
int level = 0; // ne peut pas être négatif
size_t HASH_SIZE = 100; // nb max de IDENTIFIER stockable par level
size_t HASH_NB = 50; // nb max de bloc
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
//%type <variable> type_name declarator_list declarator
//%type <function> parameter_list
//%type <typarg> parameter_declaration
//%type <expr> primary_expression postfix_expression unary_expression multiplicative_expression additive_expression
%type <plou_declarator_list> declarator_list
%type <plou_declarator> declarator
%type <plou_type> type_name
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

    //struct Function function;
    //struct Variable variable;
    /*
  struct variable t;
  struct FUNCTION arg;
  enum argtype typarg;

  char* code; // code généré
  struct expression expr;
  struct code_expr tce; // type expression contient type type
    */
}
%%

conditional_expression
: logical_or_expression
;

logical_or_expression
: logical_and_expression
| logical_or_expression OR logical_and_expression
;

logical_and_expression
: comparison_expression
| logical_and_expression AND comparison_expression
;


shift_expression
: additive_expression
| shift_expression SHL additive_expression
| shift_expression SHR additive_expression
;

primary_expression
: IDENTIFIER {printf("used %s\n",$1); /*$$.nom_temp = $1; char *c; asprintf(&c, "%%x%i", ++id);*/}
| CONSTANTI  {printf("TOKEN_CONSTANTI \t: %d\n",$1);}
| CONSTANTF  {printf("TOKEN_CONSTANTF \t: %f\n",$1);}
| '(' expression ')' {printf("expression\n");}
| IDENTIFIER '(' ')' {printf("appel fonction %s sans argument\n",$1);}
| IDENTIFIER '(' argument_expression_list ')' {printf("appel fonction %s avec des aguments\n",$1);}
;

postfix_expression
: primary_expression
| postfix_expression INC_OP
| postfix_expression DEC_OP
;

argument_expression_list
: expression
| argument_expression_list ',' expression
;

unary_expression
: postfix_expression
| INC_OP unary_expression {}
| DEC_OP unary_expression  {}
| unary_operator unary_expression {}
;

unary_operator
: '-'
;

multiplicative_expression
: unary_expression
| multiplicative_expression '*' unary_expression
| multiplicative_expression '/' unary_expression
| multiplicative_expression REM unary_expression
;

additive_expression
: multiplicative_expression
| additive_expression '+' multiplicative_expression
| additive_expression '-' multiplicative_expression
;

comparison_expression
: shift_expression
| comparison_expression '<' shift_expression
| comparison_expression '>' shift_expression
| comparison_expression LE_OP shift_expression
| comparison_expression GE_OP shift_expression
| comparison_expression EQ_OP shift_expression
| comparison_expression NE_OP shift_expression
;

expression
: unary_expression assignment_operator conditional_expression { printf("Assignement\n");/*printf("%s = load i32, i32* v.addr\n", c, ++id); */}
| conditional_expression
;

assignment_operator
: '='
| MUL_ASSIGN
| DIV_ASSIGN
| REM_ASSIGN
| SHL_ASSIGN
| SHR_ASSIGN
| ADD_ASSIGN
| SUB_ASSIGN
;

declaration
: type_name declarator_list ';' { $2 = apply_type($1, $2); print_declarator_list($2);/*$2 = $1;*/}
;

declarator_list
: declarator { $$ = add_declarator($$, $1); /*$<t>0.symbol = $1.symbol; $1 = $<t>0; printType($1);*/}
| declarator_list ',' declarator { $$ = $1; $$ = add_declarator($$, $3); /*$<t>0.symbol = $3.symbol; $1 = $<t>0; $3 = $<t>0; printType($3);*/}
;

type_name
: VOID { $$ = T_VOID;/*$$.id = ID_FUNCTION; $$.a.fonction.ret = RET_VOID; $$.a.fonction.n_arg=0;*/}
| INT { $$ = T_INT; /*$$.id = ID_INT;*/}
| DOUBLE { $$ = T_DOUBLE;/*$$.id = ID_DOUBLE;*/}
;

declarator
: IDENTIFIER { $$.declarator.variable.identifier = $1; $$.decl_type = VARIABLE; /*$<t>0.symbol = $1;*/}
| '(' declarator ')' { $$ = $2; /*$$.symbol = $2.symbol; $2 = $<t>0;*/}
| declarator '(' parameter_list ')'
{ $$.decl_type = FUNCTION;/*char * buff = $1.symbol;
  if ($<t>0.id != ID_FUNCTION)
    {$1= constructFunction($<t>0.id,$3.arg,$3.n_arg);
      $<t>0 = $1;}
  else
    {$<t>0.a.fonction.n_arg = $3.n_arg;
      $<t>0.a.fonction.arg = $3.arg;
      $1 = $<t>0;}
  $<t>0.symbol = buff;
  $$=$<t>0;*/}
| declarator '(' ')' {/*char * buff = $1.symbol; if ($<t>0.id != ID_FUNCTION) {$1= constructProcess($<t>0.id); $<t>0 = $1;} $<t>0.symbol = buff; $$=$<t>0;*/}
;

parameter_list
: parameter_declaration {/*$<arg>0.arg=addArg($<arg>0.arg,$<arg>0.n_arg,$1);$<arg>0.n_arg++;$$=$<arg>0;*/}
| parameter_list ',' parameter_declaration {/*$<arg>0.arg=addArg($1.arg,$1.n_arg,$3); $<arg>0.n_arg=$1.n_arg+1; $$=$<arg>0;*/}
;

parameter_declaration
: type_name declarator {/*$2 = $1; printType($2);
   if ($1.id == ID_INT) $<typarg>0 = ARG_INT;
   else if ($1.id == ID_DOUBLE) $<typarg>0 = ARG_DOUBLE;*/}
;

statement
: compound_statement
| expression_statement
| selection_statement
| iteration_statement
| jump_statement
;

LB
: '{' {level++ ; debugi("level", level, RED);}// pour le hash[i] il faut faire attention si on retourne à un même level, ce n'est pas forcément le même bloc ! il faudra sûrement utiliser deux var, une disant le dernier hash_nb atteint et le hash_nb actuel à utiliser
;

RB
: '}' {level--; debugi("level", level, RED); /*hash_nb--;*/} // normalement ici pas de soucis pour le hash_nb
;

compound_statement
: LB RB
| LB statement_list RB
| LB declaration_list statement_list RB
| LB declaration_list RB
;

declaration_list
: declaration
| declaration_list declaration
;

statement_list
: statement
| statement_list statement
;

expression_statement
: ';'
| expression ';'
;

selection_statement
: IF '(' expression ')' statement {printf("if\n");}
| IF '(' expression ')' statement ELSE statement
| FOR '(' expression ';' expression ';' expression ')' statement
| FOR '(' expression ';' expression ';'            ')' statement
| FOR '(' expression ';'            ';' expression ')' statement
| FOR '(' expression ';'            ';'            ')' statement
| FOR '('            ';' expression ';' expression ')' statement
| FOR '('            ';' expression ';'            ')' statement
| FOR '('            ';'            ';' expression ')' statement
| FOR '('            ';'            ';'            ')' statement
;

iteration_statement
: WHILE '(' expression ')' statement
| DO statement WHILE '(' expression ')'
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
: type_name declarator compound_statement {/*printType($2);*/}
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
    //fflush (stdout);
    fprintf (stderr, "%s:%d:%d: %s\n", file_name, yylineno, column, s);
    //return 0;
}


int main (int argc, char *argv[]) {
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
    free (file_name);
    return 0;
}
