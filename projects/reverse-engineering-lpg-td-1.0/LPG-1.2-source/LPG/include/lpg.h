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
 * File: lpg.h
 * Description: Types and data structures of LPG 
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/ 




#ifndef __LPG_H
#define __LPG_H
#include "ff.h"


/**
 *  -------------------------- DEFINES --------------------------------
 **/


#define DUMMY_PRED_STRING "DUMMYPRED"
#define DUMMY_VAR_STRING "DUMMY_VAR"
#define DUMMY_VAR_INDEX 0

#define TOTAL_TIME_STRING "TOTAL-TIME"
#define TOTAL_TIME_FUNCTION_INDEX 1

#define INTERNAL_TOTAL_COST_STRING "INTERNAL-TOTAL-COST"
#define INTERNAL_TOTAL_COST_FUNCTION_INDEX 2

#define MAX_FUNCTIONS 50
#define MAX_NUM_ACTIONS 2000

#define BIGPRIME 8000977
#define HASH_SIZE 8192

#define MIN_VALUE 1e-6
#define MAX_APPROX 1e-2

#define MIN_ACTION_COST     0.1
#define MIN_ACTION_DURATION 0.01
#define STRIPS_ACTIONS_DURATION 1.0
#define STRIPS_ACTIONS_COST 1.0


/**
 * Used for the start time of an action in the output plan
 **/
#define ROUND_TO_1_1000(f) ((float)((int)(f*1000.0 + 0.5 ))/1000.0)


/**
 * Used for compute action cost
 **/
#define GCOMP_VAR_VALUE(i) (gcomp_var_value[i])
#define GCOMP_VAR_VALUE_BEFORE(i) (gcomp_var_value_before[i])


/** 
 * Mutex macro: TRUE if actions v and w are  mutex 
 **/
#define ARE_MUTEX_EF( v, w ) ( ( ( gef_conn[v].ef_exclusive_vect[GUID_BLOCK(w)] ) & ( GUID_MASK(w) ) ) > 0 )


/** 
 * Mutex macro: TRUE if facts  v and w are  mutex 
 **/
#define ARE_MUTEX_FT( v, w ) ( ( ( gft_conn[v].ft_exclusive_vect[GUID_BLOCK(w)] ) & (GUID_MASK(w)) ) > 0 )


/**
 * Used for the action names
 **/ 
#define CONN_PLAN " "
#define INIT_CONN "( "
#define END_CONN " )"



#define TIMEOUT         4
#define UNS_VECT  	25 


/**
 * Used with Lagrange Multipliers
 **/
#define MIN_POS_S_S  	1.0
#define MAX_POS_S_S  	10.0
 

/**
 * Different types of inconsistence choices
 **/
#define RANDOM_INC 1 /* Random */
#define MIN_LEVEL_INC 2  /* Lower level */
#define MAX_LEVEL_INC 3  
#define MIN_COST_INC  4  /* Minimum cost */
#define MAX_COST_INC  5 
#define MIN_LEVEL_COST_INC 21  
#define MIN_LEVEL_CONSTR_INC 22 
#define MIN_LEVEL_MIN_CONSTR_INC 23 


/**
 * used to debug information
 **/
#define DEBUG0 (GpG.info_search==0 && GpG.verbose ) 
#define DEBUG1 (GpG.info_search>0 && GpG.verbose ) 
#define DEBUG2 (GpG.info_search>1 && GpG.verbose ) 
#define DEBUG3 (GpG.info_search>2 && GpG.verbose ) 
#define DEBUG4 (GpG.info_search>3 && GpG.verbose ) 
#define DEBUG5 (GpG.info_search>4 && GpG.verbose ) 
#define DEBUG6 (GpG.info_search>5 && GpG.verbose ) 


/**
 * Different types of constraints
 **/
#define C_T_INSERT_ACTION      1
#define C_T_REMOVE_ACTION      2
#define C_T_TREATED_CL         3
#define C_T_UNSUP_FACT         4
#define C_T_UNSUP_NUM_FACT     5


/**
 * used to obtain the data structure of action vertex_pos at level level
 **/
#define CONVERT_ACTION_TO_INFORM(vertex_pos, level)	( (vertex_pos!=vectlevel[level]->action.position) ? (NULL) : (&vectlevel[level]->action)) 


/**
 * used to obtain the data structure of fact vertex_pos at level level
 **/
#define CONVERT_FACT_TO_INFORM(vertex_pos, level)	(&vectlevel[level]->fact[vertex_pos]) 


/**
 * used to obtain the data structure of noop vertex_pos at level level
 **/
#define CONVERT_NOOP_TO_INFORM(vertex_pos, level)	(&vectlevel[level]->noop_act[vertex_pos]) 

  
/**
 * Position of action "position" in the connectivity graph
 **/
#define CONVERT_ACTION_TO_VERTEX(position)	(&gef_conn[position]) 

  
/**
 * Position of fact "position" in the connectivity graph
 **/
#define CONVERT_FACT_TO_VERTEX(position)	(&gft_conn[position]) 


/**
 * Position of noop "position" in the connectivity graph 
 * (the same of the corresponding fact)
 **/
#define CONVERT_NOOP_TO_VERTEX(position)	(&gft_conn[position]) 





/********************
 * Used for  ActionSubgraphs
 ********************/


/**
 * Action node type
 **/
#define IS_FACT 	-1 
#define IS_NOOP 	1 
#define IS_ACTION 	2 


/**
 * Local search variants
 **/
#define COMPUTE_FAST_COST      0 /* AAAI99 heuristic */
#define COMPUTE_MAX_COST       1 /* AIPS02 */
#define COMPUTE_ADD_COST       3 
#define COMPUTE_DG_SUM_COST    4 
#define COMPUTE_DG_MAX_COST    5 
#define COMPUTE_DG_LIST_COST   6 /* ICAPS03 */ 


/**
 * Initialization type
 **/
#define INIT_GOAL	0 
#define INIT_MIN_GOAL	1 
#define INIT_BEST_GOAL	2 
#define INIT_EMPTY_PLAN 3 
#define INIT_RANDOM	4 
#define PLAN_ADAPT	5 

#define ADD_NDEL        1 
#define ADD_DEL         2 
#define DEL_NADD        3 
#define DEL_ADD         4 
#define NADD_DEL        5 
#define PREC_DEL        10 

#define INITIALIZE_STEP -2000 
#define TEMP_INSERT 	-1 


#define MAX_GOALS 150		/* max number of goals in an array; just technical */
#define MAX_FALSE 	10000 	/* Max number of false actions in current plan */ 
#define MAX_COST 	10000000.0  // Max cost of a fact 


/**
 * Used in H_relaxed
 **/
#define MAX_LENGTH_H 2048
#define INITIAL_ACTION -2

/** 
 * Initial dimension of the structure for the reachability 
 * informations of the facts dg_inform_array 
 **/
#define DG_ARRAY_DIM 500   


/*********************
 * Used in utility functions
 ********************/

/**
 * Output name of the planner
 **/
#define NAMEPRG "LPG"

#define BOOLEAN unsigned short int	/* you should know about that one */


/**
 * Current version
 **/
#define VERSION "LPG-v1.2"


/* 
 * First bit in an integer 
 */
#define FIRST_1 	0x80000000	

/** 
 * upper limit for specified constant max_nodes 
 **/
#define MAX_MAX_NODES 32768	

/** 
 * MAX_MAX_NODES / 32; length of bitstring in vertex 
 **/
#define NUMINTS 1024	        

#define POSITIVE(value) value>0 ? value: 0.0 
#define MAX(a,b) a>b ? a : b
#define MIN(a,b) a<b?a:b

/**
 * function for randomize
 **/
#define MY_RANDOM random()


/**
 * Minimal tolerance in temporal domains
 **/
#define TIME_TOLERANCE 0.001


/**
 * Start time of the first action
 **/
#define START_TIME 0.001


#define LOCAL      	1 
#define STORED 		20
#define BEST_FIRST      30
#define HILL_CLIMBING   40

#define OPT_ACT_COST 		1 
#define OPT_PLAN_SIMILARITY 	2 



/**
 * Output  WARNINGS
 **/
#define WAR_MAX_FALSE \
"\n\nWarning:  Problem size too large. \
\n   Size of the array for the false facts exceeded.\
\n   LPG should be recompiled with a higher value for the parameter MAX_FALSE.\
\n   If the source code is not available, please contact the authors of LPG.\n"

#define WAR_MAX_LENGTH_H \
"\n\nWarning:  Problem size too large. \
\n   Size of the array for the DG heuristic exceeded.\
\n   LPG should be recompiled with a higher value for the parameter MAX_LENGTH_H.\
\n   If the source code is not available, please contact the authors of LPG.\n"

#define WAR_MAX_PLAN_LENGTH \
"\n\nWarning:  Problem size too large. \
\n   Size of the array for the levels exceeded.\
\n   LPG should be recompiled with a higher value for the parameter MAX_PLAN_LENGTH.\
\n   If the source code is not available, please contact the authors of LPG.\n"

#define WAR_MAX_NUM_VALUE \
"\n\nWarning:  Problem size too large. \
\n   Size of the array for the numeric quantities exceeded.\
\n   LPG should be recompiled with a higher value for the parameter MAX_NUM_VALUE.\
\n   If the source code is not available, please contact the authors of LPG.\n"

#define WAR_MAX_NUM_ACTIONS \
"\n\nWarning:  Problem size too large. \
\n   Size of the array for the temporal actions exceeded.\
\n   LPG should be recompiled with a higher value for the parameter MAX_NUM_ACTIONS.\
\n   If the source code is not available, please contact the authors of LPG.\n"

#define WAR_MAX_MAX_NODES \
"\n\nWarning:  Problem size too large. \
\n   Size of the array for the actions exceeded.\
\n   LPG should be recompiled with a higher value for the parameter MAX_MAX_NODES.\
\n   If the source code is not available, please contact the authors of LPG.\n"

#define WAR_NUMINTS \
"\n\nWarning:  Problem size too large. \
\n   Size of the array for the facts exceeded.\
\n   LPG should be recompiled with a higher value for the parameter NUMINTS.\
\n   If the source code is not available, please contact the authors of LPG.\n"

#define WAR_BUG \
"\n\nWarning:  The code contains a bug. \
\n   If the source code is not available, please contact the authors of LPG.\n"

#define WAR_NO_MEMORY "\nLPG:  sorry, I ran out of memory!\n"

#define WAR_OPEN_FILE "\nCannot open file! \n Please check the write permission\n"

#define WAR_MAX_VARS \
"\n\nWarning:  Problem size too large. \
\n   Size of the array of the actions exceeded.\
\n   LPG should be recompiled with a higher value for the parameter MAX_VARS.\
\n   If the source code is not available, please contact the authors of LPG.\n"

#define MSG_ERROR(str) printf("\n\aERROR in file %s:%d ; %s \n\n", __FILE__, __LINE__, str) 

#define GUID_BLOCK(pos)    (pos>>5)
#define GUID_MASK( pos )    (1<<(pos&31))
#define GET_BIT(bit_vect,nbit) (bit_vect[nbit>>5]  &   (1<<(nbit&31)))
#define SET_BIT(bit_vect,nbit) (bit_vect[nbit>>5]  |=  (1<<(nbit&31)))
#define RESET_BIT(bit_vect,nbit) (bit_vect[nbit>>5]  &= ~(1<<(nbit&31)))

#define CHECK_ACTION_POS( pos,  c_level)  (  ( gef_conn[pos].level>=0 &&   gef_conn[pos].level <= c_level) ? TRUE : FALSE)
#define CHECK_FACT_POS( pos,  c_level)    ( (pos<0 || (gft_conn[pos].level <= c_level) )  ? TRUE : FALSE)
#define CHECK_NOOP_POS( pos,  c_level)    ( (gft_conn[pos].level <= c_level) ? TRUE : FALSE)
#define CHECK_ACTION_OF_LEVEL(level)    ((vectlevel[level]->action.position>=0) ? TRUE : FALSE )
#define CHECK_ACTION_POSTION_OF_LEVEL(act_pos, level)    ((vectlevel[level]->action.position==act_pos) ? TRUE : FALSE )

#define GET_ACTION_OF_LEVEL(level)  (&vectlevel[level]->action)
#define GET_ACTION_POSITION_OF_LEVEL(level)  (vectlevel[level]->action.position)


/**
 * Constraint types in temporal domains
 **/

#define EA_SB        1
#define EA_EB__SA_SB 2
#define EA_EB        3
#define SA_SB        4
#define SA_EB        5


/**
 * Used in numeric domains
 **/

#define MAX_NUM_INC 10000

#define MAX_NUM_EFFS 64

#define MIN_DELTA_TIME 0.001

#define TOTAL_TIME_FUNCTION_INDEX 1



/**
 * Validator command
 **/

#define VALIDATOR "$HOME/Validator/validate -v " 
#define VALIDATOR_T "$HOME/Validator/validate -v -t 0.002 " 



/*
 *  ------------------------------ DATA STRUCTURES ----------------------------
 */

/**
 * PARSING 
 **/

extern Token gfunctions[MAX_FUNCTIONS];
extern int gfunarity[MAX_FUNCTIONS];
extern int gfunctions_args_type[MAX_FUNCTIONS][MAX_ARITY];
extern int gnum_functions;
extern const char *goperator_table[];
extern FactList *gfunctions_and_types;
extern PlOperator *gloaded_pl2ops;



typedef enum _TimeSpec
{
  AT_START_TIME,
  AT_END_TIME,
  OVER_ALL_TIME,
}
TimeSpec;


typedef enum _OPERATOR_TYPE
{
  MUL_OP,			/* 0 */
  DIV_OP,
  MINUS_OP,
  UMINUS_OP,
  PLUS_OP,

  FIX_NUMBER,			/* 5 */
  VARIABLE_OP,

  INCREASE_OP,
  DECREASE_OP,
  SCALE_UP_OP,
  SCALE_DOWN_OP,		/* 10 */
  ASSIGN_OP,

  LESS_THAN_OP,
  LESS_THAN_OR_EQUAL_OP,
  EQUAL_OP,
  GREATER_THAN_OP,		/* 15 */
  GREATER_OR_EQUAL_OP,

  MINIMIZE_OP,
  MAXIMIZE_OP
}
OPERATOR_TYPE;



typedef struct _IntList
{
  int item;
  struct _IntList *next;
}
IntList;



typedef struct _CompositeNumVar
{
  OPERATOR_TYPE operator;
  int first_op;
  int second_op;

  IntList *affects;
  IntList *increased_by;
  IntList *decreased_by;

  IntList *changed_by;

  struct _CompositeNumVar *next;
  int position;
}
CompositeNumVar;



typedef struct _action_set
{
  int A[MAX_NUM_ACTIONS];
  int num_A;
}
action_set;



typedef struct NODE_COST
{
  float weight;
  float act_cost;
  float act_time;
  int num_actions;
}
node_cost, *node_cost_list;



typedef char *token;



typedef struct TOKENLIST
{

  token item;
  int go_pointer;
  struct TOKENLIST *next;

}
 *token_list, token_list_elt;



typedef struct _PlanAction
{
  int act_pos;
  float start_time;
  float duration;
  float cost;
  struct _PlanAction *next;
  struct _PlanAction *previous;
}
PlanAction;



/**
 *   Data structure used to determine the position of the noops not
 *   in the actions subgraph in a level l because they are mutex
 *   with the action at level l 
 **/
typedef struct NOOP_NOT_IN
{
  int position;
  struct NOOP_NOT_IN *next;
}
noop_not_in;


/**
 * Data structure used for the numeric fact nodes
 **/
typedef struct _NumInfo
{
  float *values;		
  float *values_after_start;	
  int *modified_vars_start;	
  int *modified_vars_end;	
  int *used_vars;		
  short *w_is_goal;		
  short *w_is_used;		
  int *false_position;		
}
NumInfo;



/**
 * Data structure for fact and action nodes.
 **/
typedef struct INFORM
{ 

  /**
   * number of flips since last change    
   **/
  int step;			


  /**
   * Cost of inserting or removing an action node
   **/        
  node_cost cost;


  /**
   * TRUE if the action will be removed at this time step
   **/
  Bool being_removed;	

  /**
   * Pointer to the level index of the node
   **/
  int *level;


  /**
   * Index of the action/fact node
   **/
  int position;

 
  /**
   * If it is a:
   * FACT, then w_is_goal corresponds to the number of action nodes of which it is a precondition  
   * ACTION, then w_is_goal is 1 if the action makes a fact supported 
   * NOOP,  then w_is_goal is 1 if the noop propagates  a supported fact
   **/
  int w_is_goal;	


  /**
   *   TRUE if the node is in the action subgraph
   **/
  int w_is_used;		

 
 /**
  *  Number of actions that make the fact node supported
  **/
  int w_is_true;	

  /**
   * TRUE if the noop corresponds to a precondition of type over_all
   **/
  int w_is_overall;

  /**
   * -1 if the node is a fact node
   *  1 if the node is a noop node
   *  2 if the node is an action node
   **/
  short int action_fact;

 /**
  * position in the list of unsupported facts or in the list of mutex actions
  * otherwise  -1 
  **/
  int false_position;

  /** 
   * for action nodes: 
   * add corresponds to a list of supported noops mutex with the current 
   * action node; preco corresponds to a list of noop preconditions of 
   * actions in the following levels that are mutex with the current action node  
   **/
  noop_not_in * treated,  *add,  *preco;

  /**
   * If a fact node $f$ of the action graph is unsupported, then
   * $Time(f)$ is undefined, otherwise it is the minimum over the
   * temporal values assigned to the actions supporting it.  If the
   * temporal value of every precondition node of an action node $a$
   * is undefined, and there is no action node with a temporal value
   * that must precede $a$ according to $\Omega$, then $Time(a)$ is
   * set to the duration of $a$; otherwise for fact nodes : Time(f)
   * $Time(a)$ is the sum of the duration of $a$ and the maximum over
   * the temporal values of its precondition nodes and the temporal
   * values of the action nodes that must precede $a$.
   **/
  float time_f;		

  /**
   * for fact nodes: pointer to the action that makes $f$ supported 
   **/
  struct INFORM *action_f;    

  /**
   * Action position in the constraint matrix 
   **/
  int ord_pos;	

}
inform, *inform_list;


/**
 * Data structure for storing different types of constraints  
 **/
typedef struct CONSTRAINTS
{
  int action;
  int fact;


/**
 * pointer to the constraint level  
 **/
  int *level;


/**
 * Constraint type:
 *  -1 -- (default initializiation)
 *  C_T_TREATED_CL -- for future works
 *  C_T_UNSUP_FACT -- unsupported fact
 *  C_T_UNSUP_NUM_FACT -- unsupported numeric fact
 **/
  short constraint_type;
}
constraints, *constraints_list;


/**
 * Reachability information
 **/


/**
 * Data structure for store the reachability information of 
 * an unsupported fact al level *level.
 * It is used in  vectlevel[*level]->dg_facts_array[fact_position]
 **/
typedef struct _DG_ACT
{
 
  /**
   * Num_acts
   **/
  int num_actions;

  /**
   * Bestaction
   **/
  int best_act;

  /**
   * Sum_{a\in ACTS} Cost(a) 
   **/
  float cost;

  /**
   * Time_fact
   **/
  float duration;

  /**
   *  $\mu_c  cost + \mu_t duration$
   **/
  float totcost;

  /**
   *  Current level
   **/ 
  int *level;

  int stop;

  struct _DG_ACT *next;
  struct _DG_ACT *prev;

}
_dg_act;


typedef struct _DG_ACT dg_inform, *dg_inform_list;


/**
 * Global data structure for reachability information and settings
 **/
struct
{
  /**
   *  Initial (state) reachability information
   **/

  /**
   * lpg allocates an array of dg_inform structure that contains 
   * an order list ordered by cost of the actions that support every fact
   **/ 
  dg_inform **init_facts_array;	


  /**
   * dg_facts_min_duration[f] corresponds to $Time\_fact(f,1)$:
   * initial temporal value to any unsupported fact node representing $f$
   **/ 
  float *dg_facts_min_duration;



  /**
   *  Data structures used in ComputeReachInf.c and H_relaxed.c
   **/ 
  int *bit_vect_facts;
  int *bit_vect_actions;

  int *list_ef_define_cost;
  int *list_ft_define_cost;
  int num_actions_define_cost;
  int num_facts_define_cost;
  float weight_facts_define_cost;

  int *ri_bit_vect_facts;
  int *ri_supp_bit_vect_facts;
  int *ri_bit_vect_actions;
  
  int *ri_list_ef_define_cost;
  int *ri_list_ft_define_cost;
  int ri_num_actions_define_cost;
  int ri_num_facts_define_cost;
  float ri_cost_actions_define_cost;
  float ri_time_actions_define_cost;
    
  int *local_bit_vect_facts,  *local_bit_vect_actions;
  int *ri_best_act_for_facts;
  float *ri_cost_of_facts, *ri_duration_of_facts, *ri_tot_cost_of_facts;
  int *ri_num_actions_of_facts;
  int temp_removed_act;

  constraints_list constr;
  float min_start_time;

  float cost_actions_define_cost;
  float time_actions_define_cost;

  float * common_values; 


  inform_list facts_mutex[MAX_LENGTH_H];
  int num_facts_mutex;

  /** 
   * Vector where every element gives an estimate about 
   * when the corresponding facts could be supported 
   **/
  float * estimate_time_facts; 


  dg_inform_list dg_inform_array;
  dg_inform_list dg_delete_array;
  int a_start_level;
}
Hvar;



/**
 *  LPG settings
 **/

struct
  {
  /**
   *  It: valore dinamico del coefficiente di noise espresso in percentuale
   **
   *  Eng: used for implement a dynamic noise (percentual notation)
   **/ 
    int numerator;

  /**
   *  Initial noise value  (percentual notation)
   **/ 
    int init_numerator;

 
  /**
   *  It: 
   **
   *  Eng: Constant actually set to 100 for handling noise
   **/ 
    int denominator;

   /**
    * numtry: max number of flips in each iteration
    * numrun:  max number of iterations
    **/
    int numtry,  numrun;

   /**
    *  kind of local search used (walkplan, tabuplan, ecc.)   
    **/
    int local;

    int fast_greedy;

   /**
    *  dynamic and initial tabu length
    **/ 
    int tabu_length,  init_tabu_length,  tabu_pos;

   /**
    *  Relative weight of the unsupported fact inconsistencies 
    * respect to the other kind of inconsistencies
    **/ 
    float weight_fact;

   /**
    * Coefficients  used for compute the evaluation  function $E(a)^i$ 
    * of inserting $a$ in the current action subgraph
    * prec_par --> (alpha^i) for the unsupported preconditions
    * excl_par -->  (beta^i) for the mutex exclusions
    * add_effect_par --> (gamma^i) for the unsupported preconditions that become supported
    **/     

    float prec_par,  excl_par,  add_effect_par,  add_effect_goal;

   /**
    * Coefficients  used for compute the evaluation  function $E(a)^r$ 
    * for removing $a$ from the current action subgraph
    * used_prec_par --> (alpha^r) for the unsupported preconditions removed
    * used_excl_par --> (beta^r) for the mutex exclusions
    * used_add_effect_par -->(gamma^r) for the supported preconditions that become unsupported
    **/     
    float used_prec_par,  used_excl_par,  used_add_effect_par,  used_add_effect_goal;


    float delta,  inc_run;
    short int choose_false;
    int static_noise;
    int print;

   /**
    * Different kinds of initialization 
    **/ 
    int initialize,  choose_1_2_level;
 
    int count_escl;
    int rnd;
    int assign;
    int num_act_cons;

    /**
     * number of actions with Mutual Exclusion relationships in the graph 
     */
    int num_m_e;  

    /**
     * number of not satisfied precondictions 
     **/
    int num_prec;			
    int num_false_act,  num_false_fa,   num_false_num_fa,   num_false_tot;

    /** 
     * steps for one level expansion 
     **/
    int num_expansion;		
    int num_step;
 
   /**
    * Action subgraph information 
    **/ 
   int curr_min_time,  curr_plan_length,  init_plan_length,  max_plan_length,
      fixpoint_plan_length, max_temp_vect;

    float timeout;
    int search_type;
    
    /** 
     * In local search: save the graph when the number of 
     * inconsistences are below this value 
     **/
    int best_min_inc;
    
    /** 
     * Number of minimum inconsistences found 
     **/
    int min_inc,  levels;
    
    /** 
     * Current number of not-supported actions or with ME 
     **/
    int num_actions;
    
    /** 
     * Minimum number of not-supported actions or with ME 
     **/
    int min_num_actions;
    int accurate_cost;
    int found_plan;
    int double_move;
    float partial_timeout;

    /**
     * lenght of the input (-P option) solution plan  
     **/
    int input_plan_lenght;	

    /**
     * to choose whether to use noop propagation or not 
     **/
    int noopmode;		
    int max_num_ft_block;
    
    /**
     * maximum level of temporary_actions
     **/
    int max_temp_level;
    
    /**
     * Total cost of the actions in the current action subgraph 
     **/
    float total_cost,  best_cost;
    float total_time,  best_time,  min_action_time;
    int max_num_facts,  max_num_actions;
    int optimize;
    float cost;
    int increase_type;		// Type of increasing level 
    
    //  LM  
    int self_stabilizing,  num_solutions;
    int    lm_multilevel;
    float  goal_lambda;
    short  flag_decr_lm_goal;
    float s_s_step,  sqr_s_s;
    float s_s_step_me, sqr_s_s_me;
    float weight_cost,  orig_weight_cost,  weight_time,  orig_weight_time;
    int lagrange_multipl;       // 0-non lagrange multipl ; 1- use of lagrange multipl 
    int current_lm;		// Index of the last update of lacal minima 
    int is_lm;			// Is the current configuration a local minima? 
    int fix_all;
    int orig_local_search;	// Memorizzo il tipo di ricerca locale per riimpostarlo successivamente durante il processo di ottimizzazione;
    int down_vectlevel;		// Utilizzato per tests, da rimuovere successivamente 
    int max_num_solutions;
    int consider_current_level;
    Bool do_best_first;
    int do_adapt_first, do_fast_adapt;
    int approximation_level;	// Grado di precisione utilizzato dalle varie funzioni di valutazione (0 basso; >0 piu' elevato) 
    int incr_mutex;
    Bool temporal_plan;
    Bool forward_time;
    int info_search;
    Bool  lowmemory;
    State * curr_goal_state;
    PlanAction * gplan_actions;
    
    //TRUE se esistono azioni con durata non fissa 
    Bool variable_duration;
    
    //TRUE se ci sono azioni durative 
    Bool durative_actions_in_domain;
    Bool non_strips_domain;
    Bool validate;
    Bool maximize_plan;
    PlanAction * temp_plan_actions;
    Bool numeric_precs_present;
    int inc_choice_type;
    int inc_choice_command_line;
    int initialize_from;
    int count_num_try;
    int dummy_pos;
    Bool inst_duplicate_param;
    int advanced_temporal_setting;
    Bool verbose;
    Bool noout;
    Bool out_file_name;
    float max_cputime;
    Bool constraint_type;
    Bool total_time_goal; // TRUE ultimo goal soddisfatto, FALSE ultimo fatto soddisfatto 
    int verify_init, verify_Af, verify_inc_choice;
    PlNode * numeric_goal_PlNode;
    int first_execution_cri; // Execution number of Compute_reachability_informations
    int cri_update_all_facts; // Extended update of cri in insert_action_in_vectlevel
    int cri_evaluate_preconditions; 
    int H_positive_benefits; // When lpg removes an action, in heuristic esimate it subtracts front the overall cost the cost of the action that will be removed
    int relaxed_examination_type;
    int evaluation_function; // In order to test different evaluation functions
    int relax_list_fact_cost; // "list" management of the reachability informations (it: gestione a lista informazioni di raggiungibilita') 
  }
GpG;



struct
{
  int best_action;
  int best_level;
  int ls_continue;
  float max_act_incons, max_act_cost, max_act_time;
  int num_actions;
  float best_cost;
  float curr_cost;
  float lamda_prec,  lamda_me;
}
local_search; // Global variables for the local search phase 



typedef struct NEIGHBORHOOD
{
  int act_level;
  int act_pos;

  short constraint_type;

  int fact_treated;
  int new_level;

  node_cost cost;

}
neighb, *neighb_list;







/*
 * Data structure for a level of the Action Subgraph:
 * Fact nodes level level
 * Noop nodes
 * Action Nodes
 */
typedef struct LEVEL
{
  int num_prec;		/* number of precondition of almost one action in level L */
  int *prec_vect;	/* preconditions bit array */
  int num_fact;		/* number of true facts on level L */
  int *fact_vect;	/* fact bit array */
  inform * fact;	/* ptr -> fact level L */
  int num_true_crit;	/* Fatti resi veri da una sola azione nel piano e 
			   precondizioni di almeno una azione liv. L */
  int *true_crit_vect;	/* bit array */
  int num_false_crit;	/* false fact in level L, but precondition of an action */
  int *false_crit_vect;	/* REDUNDANT */
  int num_actions;	/* true actions at level L */
  
  int *noop_act_vect;		/* noop bit array */
  int *noop_prec_act_vect;	/* noop precondition bit array */
  inform action;		/* pointer to all actions of a level(for sppedup only */
  inform * noop_act;		/* pointer for all noop actions of a level */
  int level;
  int modified;		// The bitvectors must be restored 
  
  // Dati temporanei utilizzati per definire costo azioni 
  int *orig_prec_vect;
  int *orig_fact_vect;
  int *orig_true_crit_vect;
  int *orig_false_crit_vect;
  int *orig_act_vect;	/* copy of actions bit array */
  int *orig_noop_act_vect;	/* copy of  noop bit array */
  int *orig_noop_prec_act_vect;	/*  copy of noop precondition bit array */
  float max_action_time;	// Used for max action duration of level 
  int max_action_time_position;

  float  *lambda_prec;  /* LA */
  float  *lambda_me;
  short  flag_decr_lm;  
  
  NumInfo * numeric;
     
  dg_inform ** dg_facts_array;
  int *local_bit_vect_facts;
  Bool effect_at_start_present;
}
def_level, *def_level_list;



/*
 *  ----------------------------- GLOBAL VARIABLES ----------------------------
 */


// Used in inst_utils

/* local globals for this part
 */


extern int cvar_hash_table[HASH_SIZE];
extern int tot, compl;

extern char *lvar_names[MAX_VARS];
extern int lvar_types[MAX_VARS];
extern int **l_vals, **lstar_vals, **r_vals, **tested_vars;

extern int gnum_ft_block;
extern int gnum_ef_block;

/* for facts and mutex */
extern int *F;			/*[MAX_RELEVANT_FACTS/32+1]; */





// Used for mutex relationships

extern int **FT_FT_mutex, **FT_EF_mutex, **EF_EF_mutex, **EF_FT_mutex;

extern int gnum_mutex;
extern int gmutex_level;

extern int total_ft_ef_mutex;
extern int total_ef_ft_mutex;
extern int total_ef_ef_mutex;
extern int total_ft_ft_mutex;





// Used for utility functions


extern char temp_name[256];

extern node_cost *fact_costs; //[MAX_MAX_NODES];

extern struct tms start_time;

extern int *new_true_facts;
extern int *new_false_facts;



// Used during the LocalSearch

extern FtConn *gnoop_conn;
extern int num_try;
extern int return_count;
extern unsigned int seed;

extern constraints_list treated_c_l[MAX_FALSE];
extern constraints_list unsup_fact[MAX_FALSE];
extern constraints_list unsup_num_fact[MAX_FALSE];

extern neighb_list neighb_vect[MAX_MAX_NODES];
extern int num_neighborhood;

/* final sort of action in temp_vect */  
extern int *pos_temp_vect;

extern def_level * vectlevel[MAX_PLAN_LENGTH + 1];
extern def_level * temp_vectlevel[MAX_PLAN_LENGTH + 1];


extern inform_list *remove_act_chain;
extern int ind_remove_act_chain;


extern noop_not_in *noop_free_list; // Used for action <--> noop mutex */

extern unsigned long tot_alloc_mem_size;

extern char fct_file[MAX_LENGTH];

extern float build_ad_time, fixpoint_time;


// Used fot the temporal information

extern char **mat_ord; 
extern inform_list *act_ord_vect;
extern int num_act_ord;
extern short *prop_level_index;



// Used for numerical quantities

extern NumVar **gfullnum_initial;
extern int gnum_fullnum_initial;
extern int gnum_fullnum_blocks;

extern int max_num_value;

extern int *gis_not_appliable;
extern int *gis_inertial;
extern int gnum_block_compvar;

extern CompositeNumVar *gcomp_var;
extern float  *gcomp_var_value;
extern float  *gcomp_var_value_before;

extern int gnum_comp_var;
extern int goptimization_exp;


#endif

