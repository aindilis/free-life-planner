/*********************************************************************
 * (C) Copyright 2002  Universita' degli Studi di Brescia
 *     Dipartimento di Elettronica per l'Automazione
 *     Via Branze 38, 25123 Brescia, Italy
 *
 * All rights reserved. Use of this software is permitted ONLY for
 * non-commercial research purposes, and it may be copied only
 * for that use only. All copies must include this copyright message.
 * This software is made available AS IS, and neither the authors
 * nor the University of Brescia make any warranty about the
 * software or its performance.
 *
 *********************************************************************/



/********************************************************************
 * File: main.c 
 * Description:  Main routines of LPG.
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/ 



#include <math.h>
#include <sys/time.h>
#include "lpg.h"
#include "parse.h"
#include "inst_easy.h"
#include "inst_hard.h"
#include "inst_pre.h"
#include "inst_final.h"
#include "inst_utils.h"
#include "check.h"
#include "utilities.h"
#include "numeric.h"
#include "LpgOutput.h"
#include "output.h"
#include "mutex.h"
#include "LocalSearch.h"
#include "ActionSubgraph.h"
#include "ComputeReachInf.h"
#include "search.h"
#include "relax.h"


/******************************************************************************
 *                             GLOBAL VARIABLES                               *
 ******************************************************************************/

/**
 * PARSING
 **/

/* used for pddl parsing, flex only allows global variables */
int gbracket_count;
char *gproblem_name;

/* The current input line number */
int lineno = 1;

/* The current input filename */
char *gact_filename;

/* The pddl domain name */
char *gdomain_name = NULL;

/* loaded, uninstantiated operators */
PlOperator *gloaded_ops = NULL;

PlOperator *gloaded_pl2ops = NULL;

/* stores initials as fact_list */
PlNode *gorig_initial_facts = NULL;

/* not yet preprocessed goal facts  */

PlNode *gorig_goal_facts = NULL;

/* metric for the plan*/
PlNode *gmetric_exp = NULL;

/* axioms as in UCPOP before being changed to ops */
PlOperator *gloaded_axioms = NULL;

/* the types, as defined in the domain file */
TypedList *gparse_types = NULL;

/* the constants, as defined in domain file */
TypedList *gparse_constants = NULL;

/* the predicates and their arg types, as defined in the domain file */
TypedListList *gparse_predicates = NULL;

/* PDDL2--*/
TypedListList *gparse_functions = NULL;

/* the objects, declared in the problem file */
TypedList *gparse_objects = NULL;

/* connection to instantiation ( except ops, goal, initial ) */

/* all typed objects  */
FactList *gorig_constant_list = NULL;

/* the predicates and their types */
FactList *gpredicates_and_types = NULL;

FactList *gfunctions_and_types = NULL;



/**
 * INSTANTIATING
 **/

/* global arrays of constant names,
 *               type names (with their constants),
 *               predicate names,
 *               predicate aritys,
 *               defined types of predicate args
 */
Token gconstants[MAX_CONSTANTS];
int gnum_constants = 0;
Token gtype_names[MAX_TYPES];
int gtype_consts[MAX_TYPES][MAX_TYPE];
Bool gis_member[MAX_CONSTANTS][MAX_TYPES];
int gtype_size[MAX_TYPES];
int gnum_types = 0;

Token gpredicates[MAX_PREDICATES];
int garity[MAX_PREDICATES];
int gpredicates_args_type[MAX_PREDICATES][MAX_ARITY];
int gnum_predicates = 0;

Token gfunctions[MAX_FUNCTIONS];
int gfunarity[MAX_FUNCTIONS];
int gfunctions_args_type[MAX_FUNCTIONS][MAX_ARITY];
int gnum_functions = 0;



/* the domain in integer (Fact) representation
 */
Operator_pointer goperators[MAX_OPERATORS];
int gnum_operators = 0;
Fact gfull_initial[MAX_INITIAL];
int gnum_full_initial = 0;


NumVar **gfullnum_initial = NULL;
int gnum_fullnum_initial = 0;
int gnum_fullnum_blocks;

int max_num_value = 30000; //MAX_NUM_INITIAL

int gnum_comp_var = 0;
int gnum_block_compvar;
int *gis_inertial = NULL;
int goptimization_exp = -1;
int *gis_not_appliable;



WffNode *ggoal = NULL;


/* stores inertial - information: is any occurence of the predicate
 * added / deleted in the uninstantiated ops ?
 */
Bool gis_added[MAX_PREDICATES];
Bool gis_deleted[MAX_PREDICATES];



/* splitted initial state:
 * initial non static facts,
 * initial static facts, divided into predicates
 * (will be two dimensional arrays, allocated directly before need)
 */
Facts *ginitial = NULL;
int gnum_initial = 0;
Fact **ginitial_predicate;
int *gnum_initial_predicate;



/* the type numbers corresponding to any unary inertia
 */
int gtype_to_predicate[MAX_PREDICATES];
int gpredicate_to_type[MAX_TYPES];

/* (ordered) numbers of types that new type is intersection of
 */
TypeArray gintersected_types[MAX_TYPES];
int gnum_intersected_types[MAX_TYPES];



/* splitted domain: hard n easy ops
 */
Operator_pointer *ghard_operators;
int gnum_hard_operators;
NormOperator_pointer *geasy_operators;
int gnum_easy_operators;



/* so called Templates for easy ops: possible inertia constrained
 * instantiation constants
 */
EasyTemplate *geasy_templates;
int gnum_easy_templates;



/* first step for hard ops: create mixed operators, with conjunctive
 * precondition and arbitrary effects
 */
MixedOperator *ghard_mixed_operators;
int gnum_hard_mixed_operators;



/* hard ''templates'' : pseudo actions
 */
PseudoAction_pointer *ghard_templates;
int gnum_hard_templates;



/* store the final "relevant facts"
 */
Fact grelevant_facts[MAX_RELEVANT_FACTS];
int gnum_relevant_facts;
int gnum_pp_facts;



/* the final actions and problem representation
 */
Action *gactions;
int gnum_actions;
State ginitial_state;
State ggoal_state;

int cvar_hash_table[HASH_SIZE];
int tot = 0, compl = 0;

char *lvar_names[MAX_VARS];
int lvar_types[MAX_VARS];
int **l_vals = NULL; 
int **lstar_vals = NULL;
int **r_vals = NULL;
int **tested_vars = NULL;

/* for facts and mutex
*/
int *F;			/*[MAX_RELEVANT_FACTS/32+1]; */


const char *goperator_table[] = {
  "MUL_OP",
  "DIV_OP",
  "MINUS_OP",
  "UMINUS_OP",
  "PLUS_OP",

  "FIX_NUMBER",
  "VARIABLE_OP",

  "INCREASE_OP",
  "DECREASE_OP",
  "SCALE_UP_OP",
  "SCALE_DOWN_OP",
  "ASSIGN_OP",

  "LESS_THAN_OP",
  "LESS_THAN_OR_EQUAL_OP",
  "EQUAL_OP",
  "GREATER_THAN_OP",
  "GREATER_OR_EQUAL_OP",

  "MINIMIZE_OP",
  "MAXIMIZE_OP"
};




/**
 * CONNECTIVITY GRAPH
 **/


/* one ops (actions) array ... */
OpConn *gop_conn;
int gnum_op_conn;



/* one effects array ... */
EfConn *gef_conn;
int gnum_ef_conn;



/* one facts array. */
FtConn *gft_conn;
int gnum_ft_conn;

FtConn *gnoop_conn;

int gnum_ft_block;
int gnum_ef_block;




/**
 * FF SEARCHING NEEDS
 **/


/* byproduct of fixpoint: applicable actions */
int *gA;
int gnum_A;



/* communication from extract 1.P. to search engines:
 * 1P action choice */
int *gH;
int gnum_H;



/* the effects that are considered true in relaxed plan */
int *gin_plan_E;
int gnum_in_plan_E;


/* always stores (current) serial plan */
int gplan_ops[MAX_PLAN_LENGTH];
int gnum_plan_ops = 0;
int gtot_plan_ops = 0;


/* stores the states that the current plan goes through
 * ( for knowing where new agenda entry starts from ) */
State gplan_states[MAX_PLAN_LENGTH + 1];

PlanAction *subplan_actions = NULL;



/**
 * LPG LOCAL SEARCH
 **/


int num_try;
int return_count;
unsigned int seed;


constraints_list treated_c_l[MAX_FALSE];
constraints_list unsup_fact[MAX_FALSE];
constraints_list unsup_num_fact[MAX_FALSE];

neighb_list neighb_vect[MAX_MAX_NODES];
int num_neighborhood;

/* final sort of actions in temp_vect */  
int *pos_temp_vect;//[MAX_MAX_NODES];

def_level * vectlevel[MAX_PLAN_LENGTH + 1];
def_level * temp_vectlevel[MAX_PLAN_LENGTH + 1];


inform_list *remove_act_chain; //[MAX_PLAN_LENGTH];
int ind_remove_act_chain;

/* Used for action <--> noop mutex 
 */
noop_not_in *noop_free_list; 

unsigned long tot_alloc_mem_size;

char fct_file[MAX_LENGTH];

/* Statistical data about Lagrange multipliers
*/
#ifdef __STATISTIC_LM__

 /* global variables used to compute average, total maximum value, minimum value of
   Lagrange multipliers for preconditions and mutex 
  */
 
 float average_prec_final = 0.0;
 float average_me_final = 0.0;
 float var_prec_final = 0.0;
 float var_me_final = 0.0;

 float lm_prec_min_final,lm_prec_max_final,lm_me_min_final,lm_me_max_final;
 
/*Vars used for files
 */

 FILE *file_average_prec;
 FILE *file_var_prec;
 FILE *file_average_me;
 FILE *file_var_me;

#endif // end __STATISTIC_LM__



/**
 * COMPUTE MUTEX
 **/


/* Number of set mutex and level
 */
int gnum_mutex;
int gmutex_level;
/* Total number of fact-action mutex, action-fact mutex, 
   action-action mutex, fact-fact mutex 
 */
int total_ft_ef_mutex = 0;
int total_ef_ft_mutex = 0;
int total_ef_ef_mutex = 0;
int total_ft_ft_mutex = 0;

/* fact-fact mutex matrix
 */
int **FT_FT_mutex = NULL;
/* fact-action mutex matrix
 */
int **FT_EF_mutex = NULL;
/* action-action mutex matrix
 */
int **EF_EF_mutex = NULL;
/* action-fact mutex matrix
 */
int **EF_FT_mutex = NULL;


/**
 * NUMERIC PLANNING
 **/

/* Structure for numeric vars
 */

CompositeNumVar *gcomp_var;
float  *gcomp_var_value;
float  *gcomp_var_value_before;


/**
 * TEMPORAL PLANNING
 **/

char **mat_ord;
inform_list *act_ord_vect;
int num_act_ord;
short *prop_level_index;


/**
 * CPU TIME MANAGEMENT
 **/

struct tms start_time;
struct tms glob_start_time;
struct tms glob_end_time;
float gtotal_time;
char gcomm_line[MAX_LENGTH * 2];
char gops_file[MAX_LENGTH];
char gfct_file[MAX_LENGTH];
char glpg_path[MAX_LENGTH];


/**
 * MISCELLANEUS
 **/

/* used to time the different stages of the planner
 */
float gtempl_time = 0, greach_time = 0, grelev_time = 0, gconn_time = 0, 
  gnum_time = 0, gmutex_total_time = 0, gmutex_ft_time = 0, 
  gmutex_ops_time = 0, gmutex_num_time = 0;
float gsearch_time = 0;

float build_ad_time, fixpoint_time;

/* the command line inputs
 */
struct _command_line gcmd_line;

/* number of states that got heuristically evaluated
 */
int gevaluated_states = 0;

/* maximal depth of breadth first search
 */
int gmax_search_depth = 0;

char temp_name[256];

node_cost *fact_costs; //[MAX_MAX_NODES];
/* Bitvector used by remove_temp_action to find facts that 
   become TRUE after it is removed
*/
int *new_true_facts;
/* Bitvector used by remove_temp_action to find facts that 
   become FALSE after it is removed
*/
int *new_false_facts;	

/* TRUE if termination condition is reached
 */
Bool is_terminated=FALSE;



/********************************************************************
 *                           HEADERS FOR PARSING                    *
 ********************************************************************/

void load_ops_file (char *filename);
void load_fct_file (char *filename);



/*****************************************************************
 *                          MAIN ROUTINE                         *
 *****************************************************************/


int main (int argc, char *argv[])
{

  /* resulting name for ops file */
  char ops_file[MAX_LENGTH] = "";
  /* same for fct file */
  char fct_file[MAX_LENGTH] = "";

  char sol_file[MAX_LENGTH] = "";

  struct tms start, end;

  struct timeval tv;
  struct timezone tz;

  State current_start, current_end;
  int i, j, k;
  Bool found_plan=0;


#ifdef __EFENCE__
  extern int EF_ALLOW_MALLOC_0;
  EF_ALLOW_MALLOC_0 = 1;
#endif


  so_signal_management();

  strcpy (gcomm_line, "");
  for (i = 0; i < argc; i++)
    {
      strcat (gcomm_line, argv[i]);
      strcat (gcomm_line, " ");
    }
  get_path (*argv, glpg_path);
  initialize_preset_values ();


#ifdef __STATISTIC_LM__
  init_statistic();
#endif 

  /*Reset  hash-table
   */ 
  reset_cvar_hash_table();

  /* Initialize random seed
   */
  gettimeofday (&tv, &tz);
  seed = ((tv.tv_sec & 0177) * 1000000) + tv.tv_usec;


  /* command line treatment
   */
  if (argc == 1 || (argc == 2 && *++argv[0] == '?'))
    {
      lpg_usage ();
      exit (1);
    }
  if (!process_command_line (argc, argv))
    {
      lpg_usage ();
      exit (1);
    }

  /* make file names
   */

  /* one input name missing
   */
  if (!gcmd_line.ops_file_name || !gcmd_line.fct_file_name)
    {
      fprintf (stdout, "\n%s: two input files needed\n\n", NAMEPRG);
      lpg_usage ();
      exit (1);
    }
  /* add path info, complete file names will be stored in
   * ops_file and fct_file 
   */
  sprintf (ops_file, "%s%s", gcmd_line.path, gcmd_line.ops_file_name);
  sprintf (fct_file, "%s%s", gcmd_line.path, gcmd_line.fct_file_name);

  strcpy (gops_file, ops_file);
  strcpy (gfct_file, fct_file);
  sprintf (sol_file, "%s%s", gcmd_line.path, gcmd_line.sol_file_name);


  /* parse the input files
   */

  /* start parse & instantiation timing
   */
  times (&glob_start_time);
  times (&start);
  /* domain file (ops)
   */

  printf ("\nParsing domain file: ");

  /* it is important for the pddl language to define the domain before 
   * reading the problem 
   */
  load_ops_file (ops_file);

  /*dirty trick to get another copy of gloaded_ops */
  gloaded_pl2ops = gloaded_ops;
  gloaded_ops = NULL;
  gdomain_name = NULL;
  gorig_initial_facts = NULL;
  gorig_goal_facts = NULL;
  gmetric_exp = NULL;
  gloaded_axioms = NULL;
  gparse_types = NULL;
  gparse_constants = NULL;
  gparse_predicates = NULL;
  gparse_functions = NULL;
  gparse_objects = NULL;
  gorig_constant_list = NULL;
  gpredicates_and_types = NULL;
  gfunctions_and_types = NULL;
  load_ops_file (ops_file);

  /*add dummy effect to operators without boolean effects */
  add_dummy_effects (gloaded_ops);
  add_dummy_effects (gloaded_pl2ops);
  /*counts numeric preconds and effects */
  count_num_preconds_and_effects ();
  GpG.gplan_actions = NULL;


  /* problem file (facts)
   */
  if (gcmd_line.display_info >= 1)
    {
      printf (" ... done.\nParsing problem file: ");
    }

  load_fct_file (fct_file);
  if (gcmd_line.display_info >= 1)
    printf (" ... done.\n\n");

  allocate_after_parser();

  /* now we have PlOperators and PlNodes */
  reduce_pddl2_to_pddl1 ();

  /* This is needed to get all types.
   */
  build_orig_constant_list ();

  /* last step of parsing: see if it's an ADL domain!
   */

  if (!make_adl_domain ())
    {
      printf ("\n%s: this is  an ADL problem!", NAMEPRG);
      printf ("\n    can't be handled by this version.\n\n");
      exit (1);
    }


  /* now instantiate operators;
   */


  /**************************
   * first do PREPROCESSING * 
   **************************/


  /* start by collecting all strings and thereby encoding 
   * the domain in integers.
   */
  encode_domain_in_integers ();

  /* inertia preprocessing, first step:
   *   - collect inertia information
   *   - split initial state into
   *        _ arrays for individual predicates
   *        - arrays for all static relations
   *        - array containing non - static relations
   */
  do_inertia_preprocessing_step_1 ();

  /* normalize all PL1 formulae in domain description:
   * (goal, preconds and effect conditions)
   *   - simplify formula
   *   - expand quantifiers
   *   - NOTs down
   */
  normalize_all_wffs ();

  /* translate negative preconds: introduce symmetric new predicate
   * NOT-p(..) (e.g., not-in(?ob) in briefcaseworld)
   */
  translate_negative_preconds ();

  /* split domain in easy (disjunction of conjunctive preconds)
   * and hard (non DNF preconds) part, to apply 
   * different instantiation algorithms
   */
  split_domain ();


  /***********************************************
   * PREPROCESSING FINISHED                      *
   *                                             *
   * NOW MULTIPLY PARAMETERS IN EFFECTIVE MANNER *
   ***********************************************/

  build_easy_action_templates ();

  build_hard_action_templates ();

  times (&end);
  TIME (gtempl_time);
  times (&start);


  check_time_and_length (0);

  srandom(seed);

#ifdef __MY_OUTPUT__
  printf ("\n Seed %d  \n", seed);
#endif


  /* perform reachability analysis in terms of relaxed 
   * fixpoint
   */

  perform_reachability_analysis ();

  times (&end);
  TIME (greach_time);
  times (&start);

  check_time_and_length (0);


  /* collect the relevant facts and build final domain
   * and problem representations.
   */

  collect_relevant_facts ();

  times (&end);
  TIME (grelev_time);
  times (&start);

  check_time_and_length (0);


  /* now build globally accessable connectivity graph
   */
  build_connectivity_graph ();
  times (&end);
  TIME (gconn_time);
  times (&start);

  check_time_and_length (0);

  /* association to gef_conn[i] a corresponding complet ploperator */
  associate_PlOperator_with_EfConn ();


  /* adding composed numeric quantities */
  add_composite_vars ();

  make_numgoal_state(GpG.numeric_goal_PlNode);

  /* make false the comparison between uninitialized numeric quantities */
  make_false_all_checks_on_not_init ();

  /* Semplification for inertial vars
   */
  propagate_inertias ();

  if (DEBUG0)
    if (GpG.non_strips_domain)
      {
	if (GpG.variable_duration)
	  printf ("\n\nAction durations have been computed\n");
	else
	  printf ("\n\nThere is no action duration to compute\n");
      }

  /* Set vars orig_weight_cost and orig_weight_time according with plan evaluation metric
   */
  if (goptimization_exp != -1)
    set_cost_and_time_coeffs ();

  if (DEBUG0)
    printf("\nEvaluation function weights:\n     Action duration %.2f; Action cost %.2f", GpG.orig_weight_time, GpG.orig_weight_cost);

  if (DEBUG0)
    printf ("\n\nTemporal flag: %s\n", GpG.temporal_plan ? "ON" : "OFF");
 
  /* Make numeric effects structure
   */
  create_descnumeff_of_efconns ();

  /* Sets flag is_numeric for each action (efconn)
   */
  set_numeric_flag ();

  assert (gnum_comp_var < MAX_NUM_VALUE);

  /* Copy initial state in  initial_state
   */
  for (i = 0; i < gnum_comp_var; i++)
    ginitial_state.V[i] = GCOMP_VAR_VALUE(i);


  times (&end);
  TIME (gnum_time);
  times (&start);

  /* Print information about action istantiation 
   */
  print_parser_info_for_debug();

  if (GpG.numrun > 0 && GpG.numtry > 0) {

    if (DEBUG0 && !DEBUG1) {
      printf ("\nComputing mutex... ");
      fflush (stdout);
    }
    if (DEBUG1)
      printf ("\n\n--- COMPUTE MUTEX BETWEEN FACTS ---\n");
    
    if (GpG.accurate_cost >= 1)
      allocate_reachability_information_data();
	
    /* Comute mutex between facts    
    */
    calc_mutex (&ginitial_state);
    
    if (!are_goal_reachable_and_non_mutex ()) {
      printf ("\nThe problem is unsolvable since at the fixpoint level the goals are mutex or not reachable\n\n");
      exit (0);
    }
  }
  

  times (&end);
  TIME (gmutex_ft_time);

  if (DEBUG2)
    printf ("\n");
  if (DEBUG1)
    printf ("\n   --> Compute mutex between facts TOTAL TIME: %12.2f",gmutex_ft_time);

  times (&start);
  if (GpG.numrun > 0 && GpG.numtry > 0) {
    if (DEBUG1)
      printf ("\n\n--- COMPUTE MUTEX BETWEEN ACTIONS ---\n");
    /*Compute action-action, action_fact, fact-action mutex
     */
    calc_mutex_ops ();
  }


  times (&end);
  TIME (gmutex_ops_time);

  if (DEBUG1)
    printf ("\n   --> Compute mutex between actions TOTAL TIME: %12.2f\n",gmutex_ops_time);


  
  times (&start);

  if (GpG.numrun > 0 && GpG.numtry > 0) {
    if (DEBUG1)
      printf ("\n\n--- COMPUTE MUTEX BETWEEN NUMERIC FACTS ---\n");
    /* Compute mutex between action with numeric effects
     */  
    if (!GpG.lowmemory)
      calc_mutex_num_efs ();
  }
  
  times (&end);
  TIME (gmutex_num_time);

  if (DEBUG1)
    printf("\n   --> Compute mutex between numeric facts TOTAL TIME: %12.2f\n",gmutex_num_time);
  if (DEBUG2)
    print_mutex_result ();
  if (DEBUG0 && !DEBUG1) {
    printf ("done");
    fflush (stdout);
  }

  times (&start);

  if (DEBUG6 && !GpG.lowmemory)
    print_matrs ();

  gmutex_total_time = gmutex_ft_time + gmutex_ops_time + gmutex_num_time;

  printf ("\n");

  if (strlen (gcmd_line.sol_file_name) > 0)
    load_pddl2_plan (sol_file, &GpG.gplan_actions, 0);


  GpG.max_num_actions = gnum_ef_conn;
  GpG.max_num_facts = gnum_ft_conn;
  GpG.max_num_ft_block = gnum_ft_block;



  /***********************************************************
   * we are finally through with preprocessing and can worry *
   * about finding a plan instead.                            *
   ***********************************************************/


  /* another quick preprocess: approximate goal orderings and split
   * goal set into sequence of smaller sets, the goal agenda
   */

  source_to_dest (&(gplan_states[0]), &ginitial_state);
  source_to_dest (&current_start, &ginitial_state);
  source_to_dest (&current_end, &ggoal_state);

  remove_unappliable_actions ();

  if (GpG.search_type == LOCAL && GpG.numrun > 0 && GpG.numtry > 0)
    {
      k = MAX (GpG.input_plan_lenght, gmutex_level);
      for (i = 0; i < k; i++)
	{
	  if (i < gmutex_level)
	    create_vectlevel (0);
	  else
	    create_vectlevel (1);
	}
      allocate_data_for_local_search();
      create_all_min_array ();

      GpG.fixpoint_plan_length = GpG.max_plan_length - 1;
      GpG.curr_goal_state =  &current_end;

    }


  if (DEBUG1) {
    printf ("\n\nTime spent for preprocessing:");
    printf ("\n Instantiating:     %7.2f seconds", gtempl_time + greach_time + grelev_time + gconn_time + gsearch_time);
    printf ("\n Mutex relations:   %7.2f seconds", gmutex_total_time);
    printf ("\n Numeric relations: %7.2f seconds", gnum_time);
  }
  if (DEBUG0) {   
    times (&glob_end_time);
    gtotal_time = (float) ((glob_end_time.tms_utime - glob_start_time.tms_utime + glob_end_time.tms_stime - glob_start_time.tms_stime) / 100.0);
    printf ("\nPreprocessing total time: %.2f seconds",gtotal_time);
  }
  

#ifdef __TEST__
  printf ("\n\ninitial state is:\n\n");
  for (i = 0; i < ginitial_state.num_F; i++)
    {
      print_ft_name (current_start.F[i]);
      printf ("\n");
    }
  printf ("\n\ngoal state is:\n\n");
  for (i = 0; i < current_end.num_F; i++)
    {
      print_ft_name (current_end.F[i]);
      printf ("\n");
    }
  
  for (i = 0; i < gnum_op_conn; i++)
  {
    print_op_name(i);
    printf(" -- %f \n", get_action_cost (i));
  }
#endif


  if (GpG.do_best_first == TRUE && GpG.numrun==0)  
    GpG.search_type=BEST_FIRST;
  /* Search untill it is not reached termination condition (given by the function 'is_term_condition_reached')
   */
  while(!is_terminated)
    {
      /* Different types of local search 
       */      
      switch(GpG.search_type)
	{
	  /* Local Search usually used in LPG
	   */
	case LOCAL:
	  /* Do Local Search
	   */
	  LocalSearch (&current_start, &current_end, &subplan_actions);
	  /* Store plan in GpG.gplan_actions
	   */
	  GpG.gplan_actions = subplan_actions;	 
	  subplan_actions = NULL;
	  /* Control if the termination condition is reached
	   */
	  is_terminated= is_term_condition_reached();
	  break;
	 
	  /* Best First Search implemented by J. Hoffmann (FF-v2.3)
	   */
	case BEST_FIRST:

	  if (DEBUG0)
	    printf("\n\nSwitching to Best-first Search ( code from J. Hoffmann's package FF-v2.3 ) \n");
	  check_time_and_length (0);	/* con zero non controlla la lunghezza */
	  /* Return solution if reached, FALSE otherwise
	   */
	  found_plan = do_best_first_search ();
	  
	  times (&end);
	  TIME (gsearch_time);
	  
	  times (&end);
	  times (&glob_end_time);
	  gtotal_time = (float) ((glob_end_time.tms_utime - glob_start_time.tms_utime + glob_end_time.tms_stime - glob_start_time.tms_stime) / 100.0);
	  /* If a solution was found in best first search print solution
	   */
	  if (found_plan)
	    {
	      
#ifdef __MY_OUTPUT__
	      printf ("\nFFGGHH::%.2f::%d\n", gtotal_time, gnum_plan_ops);
#endif
	      store_adapted_temporal_plan_ff (gcmd_line.fct_file_name);
	      
	      
	      printf ("\nTotal time:      %.2f\nSearch time:     %.2f\nActions:         %d\nExecution cost:  %.2f\nDuration:        %.3f\nPlan quality:    %.3f", gtotal_time, gsearch_time, GpG.num_actions, GpG.total_cost, GpG.total_time,GpG.total_cost * GpG.orig_weight_cost + GpG.total_time * GpG.orig_weight_time);

	      printf ("\n     Plan file:");
	      printf ("       plan_bestfirst_%s.SOL", gcmd_line.fct_file_name);
	  
	    }
	  
	  if (DEBUG1)
	    output_planner_info ();
	  /* Control if the termination condition is reached
	   */
	  is_terminated= is_term_condition_reached();
	  break;
	  
	  /* Hill Climbing Search
	   */
	case   HILL_CLIMBING:
	  
	  if (do_enforced_hill_climbing (&current_start, &current_end))
	    source_to_dest (&current_start, &(gplan_states[gnum_plan_ops]));
	  /* Control if the termination condition is reached
	   */
	  is_terminated= is_term_condition_reached();
	  break;
	  
	default:
	  /* Control if the termination condition is reached
	   */
	  is_terminated= is_term_condition_reached();
	  break;

	}  

	  
      if (DEBUG2)
	{
	  printf ("\n\nInitial state is:\n\n");
	  for (j = 0; j < ginitial_state.num_F; j++)
	    {
	      print_ft_name (current_start.F[j]);
	      printf ("\n");
	    }
	  printf ("\n\nGoal state is:\n\n");
	  for (j = 0; j < current_end.num_F; j++)
	    {
	      print_ft_name (current_end.F[j]);
	      printf ("\n");
	    }
	}
    }
    
  printf ("\n\n");
  exit (0);

}
