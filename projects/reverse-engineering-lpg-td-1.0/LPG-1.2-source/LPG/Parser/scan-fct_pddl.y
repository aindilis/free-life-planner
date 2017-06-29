/*********************************************************************
 * (C) Copyright 2000 Albert Ludwigs University Freiburg
 *     Institute of Computer Science
 *
 * All rights reserved. Use of this software is permitted for 
 * non-commercial research purposes, and it may be copied only 
 * for that use.  All copies must include this copyright message.
 * This software is made available AS IS, and neither the authors
 * nor the  Albert Ludwigs University Freiburg make any warranty
 * about the software or its performance. 
 *********************************************************************/
 




 

%{
#define YYDEBUG 1

#ifdef YYDEBUG
/*  extern int yydebug=1;*/
#endif

#define YYPRINT(file, type, value)   yyprint (file, type, value)

#include <stdio.h>
#include <string.h> 
#include "ff.h"
#include "memory.h"
#include "parse.h"


#ifndef SCAN_ERR
#define SCAN_ERR
#define DEFINE_EXPECTED            0
#define PROBLEM_EXPECTED           1
#define PROBNAME_EXPECTED          2
#define LBRACKET_EXPECTED          3
#define RBRACKET_EXPECTED          4
#define DOMDEFS_EXPECTED           5
#define REQUIREM_EXPECTED          6
#define TYPEDLIST_EXPECTED         7
#define DOMEXT_EXPECTED            8
#define DOMEXTNAME_EXPECTED        9
#define TYPEDEF_EXPECTED          10
#define CONSTLIST_EXPECTED        11
#define PREDDEF_EXPECTED          12 
#define NAME_EXPECTED             13
#define VARIABLE_EXPECTED         14
#define ACTIONFUNCTOR_EXPECTED    15
#define ATOM_FORMULA_EXPECTED     16
#define EFFECT_DEF_EXPECTED       17
#define NEG_FORMULA_EXPECTED      18
#define NOT_SUPPORTED             19
#define SITUATION_EXPECTED        20
#define SITNAME_EXPECTED          21
#define BDOMAIN_EXPECTED          22
#define BADDOMAIN                 23
#define INIFACTS                  24
#define GOALDEF                   25
#define ADLGOAL                   26
#endif


static char * serrmsg[] = {
  "'define' expected",
  "'problem' expected",
  "problem name expected",
  "'(' expected",
  "')' expected",
  "additional domain definitions expected",
  "requirements (e.g. ':strips') expected",
  "typed list of <%s> expected",
  "domain extension expected",
  "domain to be extented expected",
  "type definition expected",
  "list of constants expected",
  "predicate definition expected",
  "<name> expected",
  "<variable> expected",
  "action functor expected",
  "atomic formula expected",
  "effect definition expected",
  "negated atomic formula expected",
  "requirement %s not supported by this IPP version",  
  "'situation' expected",
  "situation name expected",
  "':domain' expected",
  "this problem needs another domain file",
  "initial facts definition expected",
  "goal definition expected",
  "first order logic expression expected",
  NULL
};


//void fcterr( int errno, char *par );


static int sact_err;
static char *sact_err_par = NULL;
static Bool sis_negated = FALSE;




/* 
 * call	bison -pfct -bscan-fct scan-fct.y
 */
void fcterr( int errno, char *par ) {

  sact_err = errno;

  if ( sact_err_par ) {
    free( sact_err_par );
  }
  if ( par ) {
    sact_err_par = new_Token( strlen(par)+1 );
    strcpy( sact_err_par, par);
  } else {
    sact_err_par = NULL;
  }

}


%}


%start file


%union {

  char string[MAX_LENGTH];
  char* pstring;
  PlNode* pPlNode;
  FactList* pFactList;
  TokenList* pTokenList;
  TypedList* pTypedList;

    int ival;
    float fval;
}


%type <pstring> problem_name
%type <pPlNode> adl_goal_description
%type <pPlNode> adl_goal_description_star
/*%type <pPlNode> literal_name_plus*/
%type <pPlNode> init_el 
%type <pPlNode> literal_name
%type <pTokenList> literal_term
%type <pTokenList> atomic_formula_term
%type <pTokenList> term_star
%type <pstring> term
%type <pTokenList> name_star
%type <pTokenList> atomic_formula_name
%type <pstring> predicate
%type <pTypedList> typed_list_name
%type <pTypedList> typed_list_variable
%type <pTokenList> name_plus
/*PDDL2--*/
%type <pPlNode>  num_exp
%type <pTokenList>  number
%type <pPlNode> binary_comp
%type <pPlNode> optimization

%type <pPlNode> f_comp
%type <pPlNode> f_exp
%type <pPlNode> ground_f_exp

/*%type <pPlNode> f_exp_da*/
%type <pTokenList> f_head
%type <pPlNode> init_el_plus
/*%type <pPlNode> da_adl_goal_description
%type <pPlNode> da_adl_effect
%type <pPlNode> timed_adl_goal_description
%type <pPlNode> timed_adl_goal_description_plus*/
%type <pstring> function_symbol
%type <pTokenList> name_star
/*LAZZA:TOLTO%type <pTokenList> variable_star*/
/*%type <pPlNode> timed_adl_effect*/
/*%type <pPlNode> da_adl_effect
  %type <pPlNode> da_adl_effect_star*/
/*%type <pPlNode> f_assign_da*/

/*%type <pPlNode> f_exp_t

%type <pPlNode> duration_constraint
%type <pPlNode> simple_duration_constraint
%type <pPlNode> simple_duration_constraint_plus
%type <pPlNode> d_value*/
%type <pstring> type
/*--PDDL2*/

%token DEFINE_TOK
%token PROBLEM_TOK
%token SITUATION_TOK
%token BSITUATION_TOK
%token OBJECTS_TOK
%token BDOMAIN_TOK
%token INIT_TOK
%token GOAL_TOK
%token AND_TOK
%token NOT_TOK
%token <string> NAME
%token <string> VARIABLE
%token EQUAL_TOK
%token FORALL_TOK
%token IMPLY_TOK
%token OR_TOK
%token EXISTS_TOK
%token EITHER_TOK
%token OPEN_PAREN
%token CLOSE_PAREN
/*PDDL2--*/
%token REQUIREMENTS_TOK
%token GREATER_OR_EQUAL_TOK
%token LESS_THAN_OR_EQUAL_TOK
%token <string> INTVAL
%token <string> FLOATVAL
%token PLUS_TOK
%token MINUS_TOK 
%token MUL_TOK
%token DIV_TOK
%token EQUAL_TOK
%token GREATER_TOK
%token LESS_THAN_TOK
%token METRIC_TOK
%token MINIMIZE_TOK
%token MAXIMIZE_TOK
%token LENGTH_TOK
%token SERIAL_TOK
%token PARALLEL_TOK
%token TOTAL_TIME_TOK
/*--PDDL2*/


%left MINUS_TOK PLUS_TOK	
%left MUL_TOK DIV_TOK
%left UMINUS

%%


/**********************************************************************/
file:
/* empty */
|
problem_definition  file
;


/**********************************************************************/
problem_definition : 
OPEN_PAREN DEFINE_TOK         
{ 
  fcterr( PROBNAME_EXPECTED, NULL ); 
}
problem_name  problem_defs  CLOSE_PAREN                 
{  
  gproblem_name = $4;
  if ( gcmd_line.display_info >= 1 ) {
    printf(" problem '%s' defined", gproblem_name);
  }
}
;


/**********************************************************************/
problem_name :
OPEN_PAREN  PROBLEM_TOK  NAME  CLOSE_PAREN        
{ 
  $$ = new_Token( strlen($3)+1 );
  strcpy($$, $3);
}
;


/**********************************************************************/
base_domain_name :
OPEN_PAREN  BDOMAIN_TOK  NAME  CLOSE_PAREN
{ 
  if ( SAME != strcmp($3, gdomain_name) ) {
    fcterr( BADDOMAIN, NULL );
    yyerror();
  }
}
;

/**********************************************************************/
problem_defs:
/* empty */
|
objects_def  problem_defs
|
init_def  problem_defs
|
goal_def  problem_defs
|
base_domain_name  problem_defs
/*PDDL2--*/
|
require_def
|
metric_spec
|
length_spec
/*--PDDL2*/
;


/**********************************************************************/
objects_def:
OPEN_PAREN  OBJECTS_TOK  typed_list_name  CLOSE_PAREN
{ 
  gparse_objects = $3;
}
;


/**********************************************************************/
/*PDDL1
init_def:
OPEN_PAREN  INIT_TOK
{
  fcterr( INIFACTS, NULL ); 
}
literal_name_plus  CLOSE_PAREN

{
  gorig_initial_facts = new_PlNode(AND);
  gorig_initial_facts->sons = $4;
}
;
PDDL1*/

/*PDDL2--*/
init_def:
OPEN_PAREN  INIT_TOK
{
  fcterr( INIFACTS, NULL ); 
}
init_el_plus  CLOSE_PAREN
{
  gorig_initial_facts = new_PlNode(AND);
  gorig_initial_facts->sons = $4; /*4 perche' una mid rule action conta 1*/
}
;

init_el_plus:
init_el

{
  $$=$1;
}

|
init_el init_el_plus

{
  $$=$1;
  $$->next = $2;
}

;

init_el:
literal_name
|
OPEN_PAREN EQUAL_TOK f_head num_exp CLOSE_PAREN
{
  PlNode* pln;
  $$=new_PlNode(EQUAL_CONN);
  pln = new_PlNode(FN_HEAD);
  $$->sons = pln;
  $$->sons->atom = $3;
  $$->sons->next = $4;
}
;
 
/*--PDDL2*/
/**********************************************************************/
goal_def:
OPEN_PAREN  GOAL_TOK
{ 
  fcterr( GOALDEF, NULL ); 
}
adl_goal_description  CLOSE_PAREN
{
  $4->next = gorig_goal_facts;
  gorig_goal_facts = $4;
}
;


/**********************************************************************
 * Goal description providing full ADL.
 * RETURNS a tree with the connectives in the nodes and the atomic 
 * predicates in the leafs.
 **********************************************************************/
adl_goal_description:
literal_term
{ 
  if ( sis_negated ) {
    $$ = new_PlNode(NOT);
    $$->sons = new_PlNode(ATOM);
    $$->sons->atom = $1;
    sis_negated = FALSE;
  } else {
    $$ = new_PlNode(ATOM);
    $$->atom = $1;
  }
}
|
OPEN_PAREN  AND_TOK  adl_goal_description_star  CLOSE_PAREN
{ 
  $$ = new_PlNode(AND);
  $$->sons = $3;
}
|
OPEN_PAREN  OR_TOK  adl_goal_description_star  CLOSE_PAREN
{ 
  $$ = new_PlNode(OR);
  $$->sons = $3;
}
|
OPEN_PAREN  NOT_TOK  adl_goal_description  CLOSE_PAREN
{ 
  $$ = new_PlNode(NOT);
  $$->sons = $3;
}
|
OPEN_PAREN  IMPLY_TOK  adl_goal_description  adl_goal_description  CLOSE_PAREN
{ 
  PlNode *np = new_PlNode(NOT);
  np->sons = $3;
  np->next = $4;

  $$ = new_PlNode(OR);
  $$->sons = np;
}
|
OPEN_PAREN  EXISTS_TOK 
OPEN_PAREN  typed_list_variable  CLOSE_PAREN 
adl_goal_description  CLOSE_PAREN
{ 

  PlNode *pln;

  pln = new_PlNode(EX);
  pln->parse_vars = $4;

  $$ = pln;
  pln->sons = $6;

}
|
OPEN_PAREN  FORALL_TOK 
OPEN_PAREN  typed_list_variable  CLOSE_PAREN 
adl_goal_description  CLOSE_PAREN
{ 

  PlNode *pln;

  pln = new_PlNode(ALL);
  pln->parse_vars = $4;

  $$ = pln;
  pln->sons = $6;

}
/*PDDL2--*/
|
f_comp
/*
{
  $$=$1;
}
*/
/*--PDDL2*/

;

/*PDDL2--*/
f_comp:
OPEN_PAREN binary_comp f_exp f_exp CLOSE_PAREN
{ 
//AGGIUNTALAZZA
  //    printf("\n\nNumeric goal still not allowed in the goal definition\n\n");
    // exit(1);
//FINEAGGIUNTALAZZA
  $$ = new_PlNode(BIN_COMP);
  $$->sons = $2;
  $$->sons->sons= $3;
  $$->sons->sons->next= $4;
}

;

f_exp:
num_exp
{
       $$=new_PlNode(NUM_EXP);
       $$->sons = $1;
}
|
OPEN_PAREN MINUS_TOK f_exp CLOSE_PAREN %prec UMINUS
{ 
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(UMINUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
}
|
OPEN_PAREN MINUS_TOK f_exp f_exp CLOSE_PAREN
{ 
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(MINUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;

}
|
OPEN_PAREN PLUS_TOK f_exp f_exp CLOSE_PAREN
{ 
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(PLUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;


}
|
OPEN_PAREN MUL_TOK f_exp f_exp CLOSE_PAREN
{ 
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(MUL_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;

}
|
OPEN_PAREN DIV_TOK f_exp f_exp CLOSE_PAREN
{ 
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(DIV_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;

}
|
f_head
{
  $$ = new_PlNode(FN_HEAD);
  $$->atom = $1;
}
;

f_head:
OPEN_PAREN function_symbol term_star CLOSE_PAREN
{ 
  $$ = new_TokenList();
  $$->item = $2;
  $$->next = $3;
}
|
function_symbol
{ 
  $$ = new_TokenList();
  $$->item = $1;
}

;



/*
binary_op:
PLUS_TOK
|
MINUS_TOK
|
MUL_TOK
|
DIV_TOK
;
*/
binary_comp:
GREATER_TOK
{
  $$=new_PlNode(GREATER_THAN_CONN);
}
|
LESS_THAN_TOK
{
  $$=new_PlNode(LESS_THAN_CONN);
}
|
EQUAL_TOK
{
  $$=new_PlNode(EQUAL_CONN);
}
|
GREATER_OR_EQUAL_TOK
{
  $$=new_PlNode(GREATER_OR_EQUAL_CONN);
}
|
LESS_THAN_OR_EQUAL_TOK
{
  $$=new_PlNode(LESS_THAN_OR_EQUAL_CONN);
}
; 


num_exp:
OPEN_PAREN MINUS_TOK num_exp CLOSE_PAREN %prec UMINUS
{
  $$=new_PlNode(UMINUS_CONN);
  $$->sons = $3;
   
}
|
OPEN_PAREN MINUS_TOK num_exp num_exp CLOSE_PAREN
{
  $$=new_PlNode(MINUS_CONN);
  $$->sons = $3;
  $$->sons->next = $4;
}
|
OPEN_PAREN PLUS_TOK num_exp num_exp CLOSE_PAREN 
{
  $$=new_PlNode(PLUS_CONN);
  $$->sons = $3;
  $$->sons->next = $4;
}
|
OPEN_PAREN MUL_TOK num_exp num_exp CLOSE_PAREN 
{
  $$=new_PlNode(MUL_CONN);
  $$->sons = $3;
  $$->sons->next = $4;
}
|
OPEN_PAREN DIV_TOK num_exp num_exp CLOSE_PAREN 
{
  $$=new_PlNode(DIV_CONN);
  $$->sons = $3;
  $$->sons->next = $4;
}
|
number
{
    $$=new_PlNode(ATOM);
    $$->atom = $1;
}
;

number:
INTVAL
{
  Token t;
  t = new_Token( strlen($1)+1 );
  strcpy (t, $1);
  $$ = new_TokenList();
  $$->item = t;

}
|
FLOATVAL
{
  Token t;
  t = new_Token( strlen($1)+1 );
  strcpy (t, $1);
  $$ = new_TokenList();
  $$->item = t;

}
;

function_symbol:
NAME
{ 
  $$ = new_Token( strlen($1)+1 );
  strcpy( $$, $1 );
}
;


/*
name_star:

{
  $$=NULL;
}

|
NAME name_star
{
  $$ = new_TokenList();
  $$->item = $1;
  $$->next = $2;
}
;
*/

/*LAZZA: tolto
variable_star:
{
  $$=NULL;
}
|
VARIABLE variable_star
{
  $$ = new_TokenList();
  $$->item = $1;
  $$->next = $2;
}
;
*/



/**********************************************************************/
adl_goal_description_star:
/* empty */
{
  $$ = NULL;
}
|
adl_goal_description  adl_goal_description_star
{
  $1->next = $2;
  $$ = $1;
}
;



/**********************************************************************
 * some expressions used in many different rules
 **********************************************************************/
literal_term:
OPEN_PAREN  NOT_TOK  atomic_formula_term  CLOSE_PAREN
{ 
  $$ = $3;
  sis_negated = TRUE;
}
|
atomic_formula_term
{
  $$ = $1;
}
;


/**********************************************************************/
atomic_formula_term:
OPEN_PAREN  predicate  term_star  CLOSE_PAREN
{ 
  $$ = new_TokenList();
  $$->item = $2;
  $$->next = $3;
}
;


/**********************************************************************/
term_star:
/* empty */
{
  $$ = NULL;
}
|
term  term_star
{
  $$ = new_TokenList();
  $$->item = $1;
  $$->next = $2;
}
;


/**********************************************************************/
term:
NAME
{ 
  $$ = new_Token(strlen($1) + 1);
  strcpy($$, $1);
}
|
VARIABLE
{ 
  $$ = new_Token(strlen($1) + 1);
  strcpy($$, $1);
}
;


/**********************************************************************/
name_plus:
NAME
{
  $$ = new_TokenList();
  $$->item = new_Token(strlen($1) + 1);
  strcpy($$->item, $1);
}
|
NAME  name_plus
{
  $$ = new_TokenList();
  $$->item = new_Token(strlen($1) + 1);
  strcpy($$->item, $1);
  $$->next = $2;
}
;

/**********************************************************************/
typed_list_name:     /* returns TypedList */
/* empty */
{ $$ = NULL; }
|
NAME  either  name_plus  CLOSE_PAREN  typed_list_name
{ 
  $$ = new_TypedList();
  $$->name = new_Token( strlen($1)+1 );
  strcpy( $$->name, $1 );
  $$->type = $3;
  $$->next = $5;
}
|
NAME  type  typed_list_name   /* end of list for one type */
{
  $$ = new_TypedList();
  $$->name = new_Token( strlen($1)+1 );
  strcpy( $$->name, $1 );
  $$->type = new_TokenList();
  $$->type->item = new_Token( strlen($2)+1 );
  strcpy( $$->type->item, $2 );
  $$->next = $3;
}
|
NAME  typed_list_name        /* a list element (gets type from next one) */
{
  $$ = new_TypedList();
  $$->name = new_Token( strlen($1)+1 );
  strcpy( $$->name, $1 );
  if ( $2 ) {/* another element (already typed) is following */
    $$->type = copy_TokenList( $2->type );
  } else {/* no further element - it must be an untyped list */
    $$->type = new_TokenList();
    $$->type->item = new_Token( strlen(STANDARD_TYPE)+1 );
    strcpy( $$->type->item, STANDARD_TYPE );
  }
  $$->next = $2;
}
;


/***********************************************/
typed_list_variable:     /* returns TypedList */
/* empty */
{ $$ = NULL; }
|
VARIABLE  either  name_plus  CLOSE_PAREN  typed_list_variable
{ 
  $$ = new_TypedList();
  $$->name = new_Token( strlen($1)+1 );
  strcpy( $$->name, $1 );
  $$->type = $3;
  $$->next = $5;
}
|
VARIABLE  type  typed_list_variable   /* end of list for one type */
{
  $$ = new_TypedList();
  $$->name = new_Token( strlen($1)+1 );
  strcpy( $$->name, $1 );
  $$->type = new_TokenList();
  $$->type->item = new_Token( strlen($2)+1 );
  strcpy( $$->type->item, $2 );
  $$->next = $3;
}
|
VARIABLE  typed_list_variable        /* a list element (gets type from next one) */
{
  $$ = new_TypedList();
  $$->name = new_Token( strlen($1)+1 );
  strcpy( $$->name, $1 );
  if ( $2 ) {/* another element (already typed) is following */
    $$->type = copy_TokenList( $2->type );
  } else {/* no further element - it must be an untyped list */
    $$->type = new_TokenList();
    $$->type->item = new_Token( strlen(STANDARD_TYPE)+1 );
    strcpy( $$->type->item, STANDARD_TYPE );
  }
  $$->next = $2;
}
;




/**********************************************************************/
predicate:
NAME
{ 
  $$ = new_Token(strlen($1) + 1);
  strcpy($$, $1);
}
|
EQUAL_TOK
{ 
  $$ = new_Token( strlen(EQ_STR)+1 );
  strcpy( $$, EQ_STR );
}
;


/**********************************************************************/
literal_name:
OPEN_PAREN  NOT_TOK  atomic_formula_name  CLOSE_PAREN
{ 
  PlNode *tmp;

  tmp = new_PlNode(ATOM);
  tmp->atom = $3;
  $$ = new_PlNode(NOT);
  $$->sons = tmp;
}
|
atomic_formula_name
{
  $$ = new_PlNode(ATOM);
  $$->atom = $1;
}
;


/**********************************************************************/
atomic_formula_name:
OPEN_PAREN  predicate  name_star  CLOSE_PAREN
{ 
  $$ = new_TokenList();
  $$->item = $2;
  $$->next = $3;
}
;


/**********************************************************************/
name_star:
/* empty */
{ $$ = NULL; }
|
NAME  name_star
{
  $$ = new_TokenList();
  $$->item = new_Token(strlen($1) + 1);
  strcpy($$->item, $1);
  $$->next = $2;
}
;


/*PDDL2--*/
metric_spec:
OPEN_PAREN METRIC_TOK optimization ground_f_exp CLOSE_PAREN
{
  gmetric_exp = new_PlNode (METRIC_CONN);
  gmetric_exp->sons = $3;
  gmetric_exp->sons->sons = $4;
}
;

optimization:
MINIMIZE_TOK
{
  $$ = new_PlNode ( MINIMIZE_CONN);
}
|
MAXIMIZE_TOK
{
  $$ = new_PlNode ( MAXIMIZE_CONN);
}
;

ground_f_exp:
OPEN_PAREN MINUS_TOK ground_f_exp CLOSE_PAREN %prec UMINUS
{ 
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(UMINUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
}
|
OPEN_PAREN MINUS_TOK ground_f_exp ground_f_exp CLOSE_PAREN
{ 
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(MINUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;

}
|
OPEN_PAREN PLUS_TOK ground_f_exp ground_f_exp CLOSE_PAREN 
{ 
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(PLUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;


}
|
OPEN_PAREN MUL_TOK ground_f_exp ground_f_exp CLOSE_PAREN 
{ 
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(MUL_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;

}
|
OPEN_PAREN DIV_TOK ground_f_exp ground_f_exp CLOSE_PAREN 
{ 
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(DIV_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;

}
|
num_exp
{
       $$=new_PlNode(NUM_EXP);
       $$->sons = $1;
}
|
f_head
{
  $$ = new_PlNode(FN_HEAD);
  $$->atom = $1;
}
|
TOTAL_TIME_TOK
{
  $$ = new_PlNode(TOTAL_TIME_CONN);
}
|
OPEN_PAREN ground_f_exp CLOSE_PAREN
{
  $$ = $2
}
/*LAZZAaggiunta: combinazione num_exp e ground_f_exp*/
|
OPEN_PAREN MINUS_TOK num_exp ground_f_exp CLOSE_PAREN
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(MINUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;
}
|
OPEN_PAREN MINUS_TOK ground_f_exp num_exp CLOSE_PAREN
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(MINUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;
}
|
OPEN_PAREN PLUS_TOK num_exp ground_f_exp CLOSE_PAREN
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(PLUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;
}
|
OPEN_PAREN PLUS_TOK ground_f_exp num_exp CLOSE_PAREN
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(PLUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;
}
|
OPEN_PAREN DIV_TOK num_exp ground_f_exp CLOSE_PAREN
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(DIV_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;
}
|
OPEN_PAREN DIV_TOK ground_f_exp num_exp CLOSE_PAREN
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(DIV_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;
}
|
OPEN_PAREN MUL_TOK num_exp ground_f_exp CLOSE_PAREN
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(MUL_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;
}
|
OPEN_PAREN MUL_TOK ground_f_exp num_exp CLOSE_PAREN
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(MUL_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;
}
/*ENDLAZZA*/
;

length_spec:
OPEN_PAREN LENGTH_TOK ser_par CLOSE_PAREN
;

ser_par:
|
OPEN_PAREN SERIAL_TOK INTVAL CLOSE_PAREN
|
OPEN_PAREN PARALLEL_TOK INTVAL CLOSE_PAREN
;

type:
MINUS_TOK NAME
{
  $$ = new_Token(strlen($2) + 1);
  strcpy($$, $2);
}
;

either:
MINUS_TOK OPEN_PAREN EITHER_TOK
;

/**********************************************************************/
require_def:
OPEN_PAREN  REQUIREMENTS_TOK 
{ 
  opserr( REQUIREM_EXPECTED, NULL ); 
}
NAME
{ 
  if ( !supported( $4 ) ) {
    opserr( NOT_SUPPORTED, $4 );
    yyerror();
  }
}
require_key_star  CLOSE_PAREN
;


/**********************************************************************/
require_key_star:
/* empty */
|
NAME
{ 
  if ( !supported( $1 ) ) {
    opserr( NOT_SUPPORTED, $1 );
    yyerror();
  }
}
require_key_star
;
/*--PDDL2*/


%%


#include "lex.fct_pddl.c"


/**********************************************************************
 * Functions
 **********************************************************************/



int yyerror( char *msg )

{
  fflush( stdout );
  fprintf(stderr,"\n%s: syntax error in line %d, '%s':\n", 
	  gact_filename, lineno, yytext );

  if ( sact_err_par ) {
    fprintf(stderr, "%s%s\n", serrmsg[sact_err], sact_err_par );
  } else {
    fprintf(stderr,"%s\n", serrmsg[sact_err] );
  }

  exit( 1 );

}



void load_fct_file( char *filename ) 

{

  FILE *fp;/* pointer to input files */
  char tmp[MAX_LENGTH] = "";

  /* open fact file 
   */
  if( ( fp = fopen( filename, "r" ) ) == NULL ) {
    sprintf(tmp, "\n Can't find fact file: %s\n\n", filename );
    perror(tmp);
    exit ( 1 );
  }

  gact_filename = filename;
  lineno = 1; 
  yyin = fp;

  yyparse();

  fclose( fp );/* and close file again */

}

     static void
     yyprint (thisfile, mytype, value)
         FILE *thisfile;
          int mytype;
          YYSTYPE value;
     {
         fprintf (thisfile, " %s", value.string);
	 /*
       if (type == VAR)
         fprintf (file, " %s", value.tptr->name);
       else if (type == NUM)
         fprintf (file, " %d", value.val);
	 */
     }
