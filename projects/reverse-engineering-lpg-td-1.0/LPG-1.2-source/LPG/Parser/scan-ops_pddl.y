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
/*#define YYDEBUG 1*/
#ifdef YYDEBUG
  extern int yydebug=1;


#define YYPRINT(file, type, value)   yyprint (file, type, value)
#endif
   
#include <stdio.h>
#include <string.h> 
#include "ff.h"
#include "memory.h"
#include "parse.h"


#ifndef SCAN_ERR
#define SCAN_ERR
#define DOMDEF_EXPECTED            0
#define DOMAIN_EXPECTED            1
#define DOMNAME_EXPECTED           2
#define LBRACKET_EXPECTED          3
#define RBRACKET_EXPECTED          4
#define DOMDEFS_EXPECTED           5
#define REQUIREM_EXPECTED          6
#define TYPEDLIST_EXPECTED         7
#define LITERAL_EXPECTED           8
#define PRECONDDEF_UNCORRECT       9
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
#define ACTION                    20
#endif


#define NAME_STR "name\0"
#define VARIABLE_STR "variable\0"
#define STANDARD_TYPE "OBJECT\0"
 

static char *serrmsg[] = {
  "domain definition expected",
  "'domain' expected",
  "domain name expected",
  "'(' expected",
  "')' expected",
  "additional domain definitions expected",
  "requirements (e.g. ':STRIPS') expected",
  "typed list of <%s> expected",
  "literal expected",
  "uncorrect precondition definition",
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
  "action definition is not correct",
  NULL
};


//void opserr( int errno, char *par );


static int sact_err;
static char *sact_err_par = NULL;
static PlOperator *scur_op = NULL;
static Bool sis_negated = FALSE;


/* 
 * call	bison -pops -bscan-ops scan-ops.y
 */

void opserr( int errno, char *par )

{

  sact_err = errno;

  if ( sact_err_par ) {
    free(sact_err_par);
  }
  if ( par ) {
    sact_err_par = new_Token(strlen(par)+1);
    strcpy(sact_err_par, par);
  } else {
    sact_err_par = NULL;
  }

}
  
int supported( char *str )

{
  
  int i;
  
/*sositituito per pddl2 (negative-precontions oltre a negation*/
  /*  char * sup[] = { ":STRIPS", ":NEGATION", ":EQUALITY",":TYPING", 
		   ":CONDITIONAL-EFFECTS", ":DISJUNCTIVE-PRECONDITIONS", 
		   ":EXISTENTIAL-PRECONDITIONS", ":UNIVERSAL-PRECONDITIONS", 
		   ":QUANTIFIED-PRECONDITIONS", ":ADL",
		   NULL };     */
  char * sup[] = { ":STRIPS", ":NEGATIVE-PRECONDITIONS",":NEGATION",
		     ":EQUALITY",":TYPING", 
		   ":CONDITIONAL-EFFECTS", ":DISJUNCTIVE-PRECONDITIONS", 
		   ":EXISTENTIAL-PRECONDITIONS", ":UNIVERSAL-PRECONDITIONS", 
		   ":QUANTIFIED-PRECONDITIONS", ":ADL",
		   NULL };    

  return 1;
  for (i=0; NULL != sup[i]; i++) {
    if ( SAME == strcmp(sup[i], str) ) {
      return TRUE;
    }
  }
  
  return FALSE;

}


 
%}


%start file


%union {

  char string[MAX_LENGTH];
  char *pstring;
  PlNode *pPlNode;
  FactList *pFactList;
  TokenList *pTokenList;
  TypedList *pTypedList;

    int ival;
    float fval;
}

/* specifico il tipo dei simboli non terminali*/
%type <pPlNode> adl_effect
%type <pPlNode> adl_effect_star
%type <pPlNode> adl_goal_description
%type <pPlNode> adl_goal_description_star
%type <pPlNode> f_exp_da
%type <pTokenList> literal_term
%type <pTokenList> term_star
%type <pTypedList> typed_list_name
%type <pTypedList> typed_list_variable
%type <pstring> term
%type <pTokenList> atomic_formula_term
%type <pTokenList> name_plus
%type <pstring> predicate
/*PDDL2--*/
%type <pPlNode>  num_exp
%type <pTokenList>  number
%type <pPlNode> f_comp
%type <pPlNode> binary_comp
%type <pPlNode> d_op
%type <pPlNode> f_exp
%type <pTokenList> f_head
%type <pPlNode> da_adl_goal_description
%type <pPlNode> da_adl_effect
%type <pPlNode> timed_adl_goal_description
%type <pPlNode> timed_adl_goal_description_plus
%type <pstring> function_symbol
/*%type <pTokenList> name_star*/
%type <pTokenList> variable_star
%type <pPlNode> timed_adl_effect
%type <pPlNode> timed_adl_effect_plus
%type <pPlNode> da_adl_effect
%type <pPlNode> binary_op
%type <pPlNode> f_assign_da
%type <pPlNode> assign_op
%type <pPlNode> f_exp_t
%type <pPlNode> duration_constraint
%type <pPlNode> simple_duration_constraint
%type <pPlNode> simple_duration_constraint_plus
%type <pPlNode> d_value
%type <pstring> type
/*--PDDL2*/

%token DEFINE_TOK
%token DOMAIN_TOK
%token REQUIREMENTS_TOK
%token TYPES_TOK
%token EITHER_TOK
%token CONSTANTS_TOK
%token PREDICATES_TOK
/*PDDL2--*/
%token FUNCTIONS_TOK
%token DURATIVE_ACTION_TOK
%token CONDITION_TOK
%token DURATION_TOK
%token <pPlNode> DURATION_VAR_TOK
%token AT_START
%token AT_END
%token OVER_ALL
%token INCREASE_TOK
%token DECREASE_TOK
%token <pPlNode> TIME_TOK
%token GREATER_OR_EQUAL_TOK
%token LESS_THAN_OR_EQUAL_TOK
%token <string> INTVAL
%token <string> FLOATVAL
%token ASSIGN_TOK
%token SCALE_UP_TOK
%token SCALE_DOWN_TOK
/*%token ASK_TOK*/
%token PLUS_TOK
%token MINUS_TOK 
%token MUL_TOK
%token DIV_TOK
%token EQUAL_TOK
%token GREATER_TOK
%token LESS_THAN_TOK
/*--PDDL2*/
%token ACTION_TOK
%token VARS_TOK
/*%token CONTEXT_TOK*/
/*%token IMPLIES_TOK*/
%token PRECONDITION_TOK
%token PARAMETERS_TOK
%token EFFECT_TOK
%token AND_TOK
%token NOT_TOK
%token WHEN_TOK
%token FORALL_TOK
%token IMPLY_TOK
%token OR_TOK
%token EXISTS_TOK
%token EQUAL_TOK
%token <string> NAME
%token <string> VARIABLE
%token OPEN_PAREN
%token CLOSE_PAREN


%left MINUS_TOK PLUS_TOK	
%left MUL_TOK DIV_TOK
%left UMINUS

%%

/**********************************************************************/
file:
{ 
  opserr( DOMDEF_EXPECTED, NULL ); 
}
domain_definition 
;
/* can be extended to support 'addenda' and similar stuff */


/**********************************************************************/
domain_definition : 
OPEN_PAREN  DEFINE_TOK  domain_name       
{ 
}
optional_domain_defs 
{
  static int once=0;
  if ( gcmd_line.display_info >= 1 && once==0) {
    printf(" domain '%s' defined", gdomain_name);
    once=1;
  }
}
;


/**********************************************************************/
domain_name :
OPEN_PAREN  DOMAIN_TOK  NAME  CLOSE_PAREN 
{ 
  gdomain_name = new_Token( strlen($3)+1 );
  strcpy( gdomain_name, $3);
}
;


/**********************************************************************/
optional_domain_defs:
CLOSE_PAREN  /* end of domain */
|
require_def  optional_domain_defs
|
constants_def  optional_domain_defs
|
types_def  optional_domain_defs
|
action_def  optional_domain_defs
|
predicates_def  optional_domain_defs
/*PDDL2--*/
|
durative_action_def optional_domain_defs
|
functions_def optional_domain_defs
/*--PDDL2*/
;


/**********************************************************************/
predicates_def :
OPEN_PAREN PREDICATES_TOK  predicates_list 
{
}
CLOSE_PAREN
{ 
}
;
/**********************************************************************/
predicates_list :
/* empty = finished */
{}
|
OPEN_PAREN  NAME typed_list_variable  CLOSE_PAREN
{

  TypedListList *tll;

  if ( gparse_predicates ) {
    tll = gparse_predicates;
    while ( tll->next ) {
      tll = tll->next;
    }
    tll->next = new_TypedListList();
    tll = tll->next;
  } else {
    tll = new_TypedListList();
    gparse_predicates = tll;
  }

  tll->predicate = new_Token( strlen( $2 ) + 1);
  strcpy( tll->predicate, $2 );

  tll->args = $3;

}
predicates_list
;


/*PDDL2--*/
/**********************************************************************/
functions_def :
OPEN_PAREN FUNCTIONS_TOK  functions_list 
{
}
CLOSE_PAREN
{ 
}
;
/**********************************************************************/
functions_list :
/* empty = finished */
{}
|
OPEN_PAREN  NAME typed_list_variable  CLOSE_PAREN
{

  TypedListList *tll;

  if ( gparse_functions ) {
    tll = gparse_functions;
    while ( tll->next ) {
      tll = tll->next;
    }
    tll->next = new_TypedListList();
    tll = tll->next;
  } else {
    tll = new_TypedListList();
    gparse_functions = tll;
  }

  tll->predicate = new_Token( strlen( $2 ) + 1);
  strcpy( tll->predicate, $2 );

  tll->args = $3;

}
functions_list
;

/*--PDDL2*/
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


/**********************************************************************/
types_def:
OPEN_PAREN  TYPES_TOK
{ 
  opserr( TYPEDEF_EXPECTED, NULL ); 
}
typed_list_name  CLOSE_PAREN
{
  gparse_types = $4;
}
; 


/**********************************************************************/
constants_def:
OPEN_PAREN  CONSTANTS_TOK
{ 
  opserr( CONSTLIST_EXPECTED, NULL ); 
}
typed_list_name  CLOSE_PAREN
{
  gparse_constants = $4;
}
;


/**********************************************************************
 * actions and their optional definitions
 **********************************************************************/
action_def:
OPEN_PAREN  ACTION_TOK  
{ 
#if YYDEBUG != 0
  printf("\n\nin action_def rule\n\n\n"); 
#endif
  opserr( ACTION, NULL );
}  
NAME
{ 
  scur_op = new_PlOperator( $4 );
}
param_def  action_def_body  CLOSE_PAREN
{
  scur_op->next = gloaded_ops;
  gloaded_ops = scur_op; 
}
;


/**********************************************************************/
param_def:
/* empty */
{ 
  scur_op->params = NULL; 
}
|
PARAMETERS_TOK  OPEN_PAREN  typed_list_variable  CLOSE_PAREN
{
  TypedList *tl;
  scur_op->parse_params = $3;
  for (tl = scur_op->parse_params; tl; tl = tl->next) {
    /* to be able to distinguish params from :VARS 
     */
    scur_op->number_of_real_params++;
  }
}
;

/**********************************************************************/
action_def_body:
/* empty */
|
VARS_TOK  OPEN_PAREN  typed_list_variable  CLOSE_PAREN  action_def_body
{
  TypedList *tl = NULL;

  /* add vars as parameters 
   */
  if ( scur_op->parse_params ) {
    for( tl = scur_op->parse_params; tl->next; tl = tl->next ) {
      /* empty, get to the end of list 
       */
    }
    tl->next = $3;
    tl = tl->next;
  } else {
    scur_op->parse_params = $3;
  }
}
|
PRECONDITION_TOK  adl_goal_description
{ 
  scur_op->preconds = $2; 
}
action_def_body
|
EFFECT_TOK  adl_effect
{ 
  scur_op->effects = $2; 
}
action_def_body
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
/* AGGIUNTO LAZZA*/
|
number
{
    $$=new_PlNode(ATOM);
    $$->atom = $1;
}
/*ENDLAZZA*/
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

/*
variable:
ASK_TOK NAME
;
*/

assign_op:
ASSIGN_TOK
{
  $$=new_PlNode(ASSIGN_CONN);
}
|
SCALE_UP_TOK
{
  $$=new_PlNode(SCALE_UP_CONN);
}
|
SCALE_DOWN_TOK
{
  $$=new_PlNode(SCALE_DOWN_CONN);
}
|
INCREASE_TOK
{
  $$=new_PlNode(INCREASE_CONN);
}
|
DECREASE_TOK
{
  $$=new_PlNode(DECREASE_CONN);
}
;


/*--PDDL2*/
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
 * effects as allowed in pddl are saved in FF data structures
 * describes everything after the keyword :effect
 *********************************************************************/
adl_effect:
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
OPEN_PAREN  AND_TOK  adl_effect_star  CLOSE_PAREN
{ 
  $$ = new_PlNode(AND);
  $$->sons = $3;
}
|
OPEN_PAREN  FORALL_TOK 
OPEN_PAREN  typed_list_variable  CLOSE_PAREN 
adl_effect  CLOSE_PAREN
{ 

  PlNode *pln;

  pln = new_PlNode(ALL);
  pln->parse_vars = $4;

  $$ = pln;
  pln->sons = $6;

}
|
OPEN_PAREN  WHEN_TOK  adl_goal_description  adl_effect  CLOSE_PAREN
{
  /* This will be conditional effects in FF representation, but here
   * a formula like (WHEN p q) will be saved as:
   *  [WHEN]
   *  [sons]
   *   /  \
   * [p]  [q]
   * That means, the first son is p, and the second one is q. 
   */
  $$ = new_PlNode(WHEN);
  $3->next = $4;
  $$->sons = $3;
}
|
OPEN_PAREN assign_op f_head f_exp CLOSE_PAREN
{
  PlNode* pln;
  $$=$2;
  pln = new_PlNode(FN_HEAD);
  $$->sons = pln;
  $$->sons->atom = $3;
  $$->sons->next = $4;
}
;


/**********************************************************************/
adl_effect_star:
{ 
  $$ = NULL; 
}
|
adl_effect  adl_effect_star
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
{ $$ = NULL; }
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
  $$ = new_Token( strlen($1)+1 );
  strcpy( $$, $1 );
}
|
VARIABLE
{ 
  $$ = new_Token( strlen($1)+1 );
  strcpy( $$, $1 );
}
;


/**********************************************************************/
name_plus:
NAME
{
  $$ = new_TokenList();
  $$->item = new_Token( strlen($1)+1 );
  strcpy( $$->item, $1 );
}
|
NAME  name_plus
{
  $$ = new_TokenList();
  $$->item = new_Token( strlen($1)+1 );
  strcpy( $$->item, $1 );
  $$->next = $2;
}
;

/**********************************************************************/
predicate:
NAME
{ 
  $$ = new_Token( strlen($1)+1 );
  strcpy( $$, $1 );
}
|
EQUAL_TOK
{ 
  $$ = new_Token( strlen(EQ_STR)+1 );
  strcpy( $$, EQ_STR );
}
;


/**********************************************************************/
typed_list_name:     /* returns TypedList */
/* empty */
{ $$ = NULL; }
|
NAME  either  name_plus  CLOSE_PAREN  typed_list_name
/*
NAME  EITHER_TOK  name_plus  CLOSE_PAREN  typed_list_name
*/
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
/*
VARIABLE  EITHER_TOK  name_plus  CLOSE_PAREN  typed_list_variable
*/
{ 
  $$ = new_TypedList();
  $$->name = new_Token( strlen($1)+1 );
  strcpy( $$->name, $1 );
  $$->type = $3;
  $$->next = $5;
}
|
VARIABLE  type  typed_list_variable   /* end of list for one type */
/*
VARIABLE  TYPE  typed_list_variable    
*/
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
/*PDDL2--*/
/***********************************************/
durative_action_def: 
OPEN_PAREN  DURATIVE_ACTION_TOK  
{ 
  opserr( ACTION, NULL ); 
}  
NAME
{ 
  scur_op = new_PlOperator( $4 );
}
param_def  durative_action_def_body  CLOSE_PAREN
{
  scur_op->next = gloaded_ops;
  gloaded_ops = scur_op; 
}
;
/***********************************************/
durative_action_def_body:
/* empty */
|
VARS_TOK  OPEN_PAREN  typed_list_variable  CLOSE_PAREN  durative_action_def_body
{
  TypedList *tl = NULL;

  /* add vars as parameters 
   */
  if ( scur_op->parse_params ) {
    for( tl = scur_op->parse_params; tl->next; tl = tl->next ) {
      /* empty, get to the end of list 
       */
    }
    tl->next = $3;
    tl = tl->next;
  } else {
    scur_op->parse_params = $3;
  }
}
|
DURATION_TOK duration_constraint
{

scur_op->duration = $2
  
}
durative_action_def_body
|
CONDITION_TOK  da_adl_goal_description
{ 
  scur_op->preconds = $2; 
}
durative_action_def_body
|
EFFECT_TOK  da_adl_effect
{ 
  scur_op->effects = $2; 
}
durative_action_def_body
;

/**********************************************************************/

/*VEDI PAG 31 PDDL 2.1 MANUAL */

da_adl_goal_description:
timed_adl_goal_description
{
  $$ = new_PlNode(AND);
  $$->sons = $1;
}
|
OPEN_PAREN AND_TOK timed_adl_goal_description_plus CLOSE_PAREN
{
  $$ = new_PlNode(AND);
  $$->sons = $3;
}
;

timed_adl_goal_description_plus:
timed_adl_goal_description
{
  $$=$1;
}
|
timed_adl_goal_description timed_adl_goal_description_plus
{
  $$ = $1;
  $$->next = $2;
}
;

timed_adl_goal_description:
OPEN_PAREN AT_START adl_goal_description CLOSE_PAREN
{
  $$ = new_PlNode(AT_START_CONN);
  $$->sons = $3;
}
|
OPEN_PAREN AT_END adl_goal_description CLOSE_PAREN
{
  $$ = new_PlNode(AT_END_CONN);
  $$->sons = $3;
}
|
OPEN_PAREN OVER_ALL adl_goal_description CLOSE_PAREN
{
  $$ = new_PlNode(OVER_ALL_CONN);
  $$->sons = $3;
}
;


/**********************************************************************/
/*vedi pag 32*/

da_adl_effect:
OPEN_PAREN AND_TOK timed_adl_effect_plus CLOSE_PAREN
{
  $$ = new_PlNode(AND);
  $$->sons = $3;
}
|
timed_adl_effect
{
  $$ = new_PlNode(AND);
  $$->sons = $1;
}
|
/*OPEN_PAREN FORALL_TOK OPEN_PAREN variable_star CLOSE_PAREN da_adl_effect CLOSE_PAREN*/
OPEN_PAREN  FORALL_TOK 
OPEN_PAREN  typed_list_variable  CLOSE_PAREN 
da_adl_effect  CLOSE_PAREN
{
  PlNode *pln;

  pln = new_PlNode(ALL);
  pln->parse_vars = $4;

  $$ = pln;
  pln->sons = $6;
}
|
OPEN_PAREN  WHEN_TOK  da_adl_goal_description  timed_adl_effect  CLOSE_PAREN
{
  /* This will be conditional effects in FF representation, but here
   * a formula like (WHEN p q) will be saved as:
   *  [WHEN]
   *  [sons]
   *   /  \
   * [p]  [q]
   * That means, the first son is p, and the second one is q. 
   */
  $$ = new_PlNode(WHEN);
  $3->next = $4;
  $$->sons = $3;
}
|
OPEN_PAREN assign_op f_head f_exp_da CLOSE_PAREN
{
  PlNode* pln;
  $$=$2;
  pln = new_PlNode(FN_HEAD);
  $$->sons = pln;
  $$->sons->atom = $3;
  $$->sons->next = $4;
}
;

timed_adl_effect_plus:
timed_adl_effect
{
  $$ = $1;
}
|
timed_adl_effect timed_adl_effect_plus
{
  $$ = $1;
  $$->next = $2;
}
;

timed_adl_effect:
OPEN_PAREN AT_START adl_effect CLOSE_PAREN
{
  $$ = new_PlNode(AT_START_CONN);
  $$->sons = $3;
}
|
OPEN_PAREN AT_END adl_effect CLOSE_PAREN
{
  $$ = new_PlNode(AT_END_CONN);
  $$->sons = $3;
}
|
OPEN_PAREN AT_START f_assign_da CLOSE_PAREN
{
  $$ = new_PlNode(AT_START_CONN);
  $$->sons = $3;
}
|
OPEN_PAREN AT_END f_assign_da CLOSE_PAREN
{
  $$ = new_PlNode(AT_END_CONN);
  $$->sons = $3;
}
|
OPEN_PAREN INCREASE_TOK f_head f_exp_t CLOSE_PAREN
{
  $$ = new_PlNode(INCREASE_CONN);
  $$->sons = $3;
  $$->sons->next = $4;
}
|
OPEN_PAREN DECREASE_TOK f_head f_exp_t CLOSE_PAREN
{
  $$ = new_PlNode(DECREASE_CONN);
  $$->sons = $3;
  $$->sons->next = $4;
}
;

f_assign_da:
OPEN_PAREN assign_op f_head f_exp_da CLOSE_PAREN
{

  PlNode* pln;
  $$=$2;
  pln = new_PlNode(FN_HEAD);
  $$->sons = pln;
  $$->sons->atom = $3;
  $$->sons->next = $4;

}
;


f_exp_da:
OPEN_PAREN binary_op f_exp_da f_exp_da CLOSE_PAREN
{
  $$ = $2;
  $$->sons->sons = $3;
  $$->sons->sons->next = $4;
}
|
OPEN_PAREN MINUS_TOK f_exp_da CLOSE_PAREN %prec UMINUS
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(UMINUS_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
}
|
DURATION_VAR_TOK
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(DURATION_VAR_ATOM); 
  $$->sons = pln;
}
|
f_exp
;


binary_op:
PLUS_TOK
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(PLUS_CONN); 
  $$->sons = pln;

}
|
MINUS_TOK
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(MINUS_CONN); 
  $$->sons = pln;

}
|
MUL_TOK
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(MUL_CONN); 
  $$->sons = pln;

}
|
DIV_TOK
{
  PlNode *pln;

  $$=new_PlNode(F_EXP);
  pln=new_PlNode(DIV_CONN); 
  $$->sons = pln;

}
;


f_exp_t:
OPEN_PAREN '*' f_exp TIME_TOK CLOSE_PAREN
{
  PlNode *pln, *pln2;

  $$ = new_PlNode(F_EXP_T);
  pln2 = new_PlNode(TIME_VAR);
  pln=new_PlNode(MUL_CONN); 
  $$->sons = pln;
  $$->sons->sons = $3;
  $$->sons->sons->next = pln2;  

}
|
OPEN_PAREN '*' TIME_TOK f_exp CLOSE_PAREN
{
  PlNode *pln, *pln2;

  $$ = new_PlNode(F_EXP_T);
  pln2 = new_PlNode(TIME_VAR);
  pln=new_PlNode(MUL_CONN); 
  $$->sons = pln;
  $$->sons->sons = pln2;
  $$->sons->sons->next = $3;  

}
|
TIME_TOK
{
  PlNode *pln;

  $$ = new_PlNode(F_EXP_T);
  pln = new_PlNode(TIME_VAR);
  $$->sons = pln;
}
;


duration_constraint:
OPEN_PAREN AND_TOK simple_duration_constraint_plus CLOSE_PAREN
{
  $$ = new_PlNode(AND);
  $$->sons = $3;
}
|
simple_duration_constraint
{
  $$ = new_PlNode(AND);
  $$->sons = $1;
}
;

simple_duration_constraint_plus:
simple_duration_constraint
{
  $$ = $1;
}
|
simple_duration_constraint simple_duration_constraint_plus
{
  $$=$1;
  $$->next = $2;
}
;

simple_duration_constraint:
OPEN_PAREN d_op DURATION_VAR_TOK d_value CLOSE_PAREN
/*
{
  $$ = new_PlNode(AND);
}*/
{
  PlNode *pln;

  $$ = $2;
  pln = new_PlNode(DURATION_VAR_ATOM);
  $$->sons->sons = pln;
  $$->sons->sons->next = $4;
}

|
OPEN_PAREN AT_START simple_duration_constraint CLOSE_PAREN
{
  $$ = new_PlNode(AND);
}
|
OPEN_PAREN AT_END simple_duration_constraint CLOSE_PAREN
{
  $$ = new_PlNode(AND);
}
;

d_op:
LESS_THAN_OR_EQUAL_TOK
{
  PlNode *pln;

  $$=new_PlNode(DURATION_CONSTRAINT_CONN);
  pln=new_PlNode(LESS_THAN_OR_EQUAL_CONN); 
  $$->sons = pln;
}
|
GREATER_OR_EQUAL_TOK
{
  PlNode *pln;

  $$=new_PlNode(DURATION_CONSTRAINT_CONN);
  pln=new_PlNode(GREATER_OR_EQUAL_CONN); 
  $$->sons = pln;
}
|
EQUAL_TOK
{
  PlNode *pln;

  $$=new_PlNode(DURATION_CONSTRAINT_CONN);
  pln=new_PlNode(EQUAL_CONN); 
  $$->sons = pln;
}
;

d_value:
num_exp
|
f_exp
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
/*--PDDL2*/
%%
#include "lex.ops_pddl.c"


/**********************************************************************
 * Functions
 **********************************************************************/



int yyerror( char *msg )

{

  fflush(stdout);
  fprintf(stderr, "\n%s: syntax error in line %d, '%s':\n", 
	  gact_filename, lineno, yytext);

  if ( NULL != sact_err_par ) {
    fprintf(stderr, "%s %s\n", serrmsg[sact_err], sact_err_par);
  } else {
    fprintf(stderr, "%s\n", serrmsg[sact_err]);
  }

  exit( 1 );

}



void load_ops_file( char *filename )

{

  FILE * fp;/* pointer to input files */
  char tmp[MAX_LENGTH] = "";

  /* open operator file 
   */
  if( ( fp = fopen( filename, "r" ) ) == NULL ) {
    sprintf(tmp, "\n Can't find operator file: %s\n\n", filename );
    perror(tmp);
    exit( 1 );
  }

  gact_filename = filename;
  lineno = 1; 
  yyin = fp;

  yyparse();

  fclose( fp );/* and close file again */

}

#ifdef YYDEBUG
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
#endif
