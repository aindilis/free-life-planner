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
 * File: inst_utils.c
 * Description: various functions for isupporting instantiation
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/





#include <math.h>
#include <values.h>
#include "lpg.h"
#include "parse.h"
#include "memory.h"
#include "inst_pre.h"
#include "numeric.h"
#include "inst_utils.h"
#include "output.h"
#include "utilities.h"





PlOperator *
search_name_in_plops (char *plop_name)
{
  PlOperator *op;

  for (op = gloaded_pl2ops; op; op = op->next)
    if (strcmp (op->name, plop_name) == SAME)
      return op;
  return NULL;
}

void
associate_PlOperator_with_EfConn (void)
{
  int i;

  for (i = 0; i < gnum_ef_conn; i++)
    {
      gef_conn[i].plop =
	search_name_in_plops (gop_conn[gef_conn[i].op].action->name);
      if (!gef_conn[i].plop)
	exit (1);

      if ((gef_conn[i].plop->is_odd) && (!gef_conn[i].sf))
	gef_conn[i].sf = new_SpecialFacts ();
    }
}

void
add_composite_vars (void)
{
  int i = 0;

  if (gis_inertial == NULL)
    gis_inertial = (int *) calloc (max_num_value / 32 + 1, sizeof (int));


  memset (gis_inertial, -1, (gnum_comp_var / 32 + 1) * sizeof (int));


  for (i = 0; i < gnum_comp_var; i++)
    {
      switch (gcomp_var[i].operator)
	{
	case INCREASE_OP:
	case DECREASE_OP:
	case SCALE_UP_OP:
	case SCALE_DOWN_OP:
	case ASSIGN_OP:
	  GCOMP_VAR_VALUE (i) = 0.0;
	  break;
	default:
	  break;
	}
    }


  index_duration_exps ();

  add_effects_to_comp_vars ();

  add_preconds_to_comp_vars ();

  if (gmetric_exp && gmetric_exp->sons
      && gmetric_exp->sons->connective == MAXIMIZE_CONN)
    GpG.maximize_plan = TRUE;

  goptimization_exp = index_in_cvars_of_expression (gmetric_exp, -1);

  calc_duration_of_actions ();
  calc_cost_of_actions ();

  gnum_block_compvar = gnum_comp_var / 32 + 1;

}


void
add_effects_to_comp_vars (void)
{
  int i, pos, fct;
  PlNode *effects;
  PlNode *n;
  PlNode *e;

  for (i = 0; i < gnum_ef_conn; i++)
    {
      effects = gef_conn[i].plop->effects;
      if (effects->connective == AND)
	effects = effects->sons;
      for (n = effects; n; n = n->next)
	{
	  e = n;
	  if ((e->connective == AT_START_CONN)
	      && (e->sons->connective == NOT))
	    {
	      pos =
		get_fct_pos_from_plnode (n->sons, i, gef_conn[i].D,
					 gef_conn[i].num_D);
	      if (pos == -1)
		{
		  continue;
		  exit (1);
		}
	      fct = gef_conn[i].D[pos];
	      gef_conn[i].D[pos] = gef_conn[i].D[--gef_conn[i].num_D];
	      gef_conn[i].sf->D_start[gef_conn[i].sf->num_D_start++] = fct;
	    }

	  if ((e->connective == AT_START_CONN)
	      && (e->sons->connective == ATOM))
	    {
	      pos =
		get_fct_pos_from_plnode (n->sons, i, gef_conn[i].A,
					 gef_conn[i].num_A);
	      if (pos == -1)
		{
		  continue;
		  exit (1);
		}
	      fct = gef_conn[i].A[pos];
	      gef_conn[i].A[pos] = gef_conn[i].A[--gef_conn[i].num_A];
	      gef_conn[i].sf->A_start[gef_conn[i].sf->num_A_start++] = fct;
	    }

	  if ((n->connective == AT_START_CONN)
	      || (n->connective == AT_END_CONN)
	      || (n->connective == OVER_ALL_CONN))
	    e = n->sons;
	  if ((e->connective == ATOM) || (e->connective == NOT))
	    continue;

	  if ((e->connective == INCREASE_CONN)
	      || (e->connective == DECREASE_CONN)
	      || (e->connective == SCALE_UP_CONN)
	      || (e->connective == SCALE_DOWN_CONN)
	      || (e->connective == ASSIGN_CONN))
	    {
	      if (n->connective == AT_START_CONN)
		gef_conn[i].sf->A_start[gef_conn[i].sf->num_A_start++] =
		  -index_in_cvars_of_expression (e, i);
	      if ((n->connective == AT_END_CONN)
		  || (e->connective == n->connective))
		gef_conn[i].A[gef_conn[i].num_A++] =
		  -index_in_cvars_of_expression (e, i);
	    }
	  else
	    {
	      printf ("\n\nadd_effects_to_comp_vars: Unexpected node: \n");
	      print_PlNode (e, 0);
	      exit (1);
	    }
	}
    }

}

void
add_preconds_to_comp_vars (void)
{
  int i, pos, fct;
  PlNode *n;
  PlNode *e;
  PlNode *preconds;

  for (i = 0; i < gnum_ef_conn; i++)
    {
      gef_conn[i].num_PC = gef_conn[i].num_PC;
      preconds = gef_conn[i].plop->preconds;
      if (preconds->connective == AND)
	preconds = preconds->sons;

      for (n = preconds; n; n = n->next)
	{
	  e = n;

	  if ((e->connective == OVER_ALL_CONN)
	      && (e->sons->connective == ATOM))
	    {
	      pos =
		get_fct_pos_from_plnode (n->sons, i, gef_conn[i].PC,
					 gef_conn[i].num_PC);
	      if (pos == -1)
		{
		  if (DEBUG2)
		    printf
		      ("\n\n3 fact generated by plnode wasn't found among the effects\n\n");
		  continue;

		}
	      fct = gef_conn[i].PC[pos];
	      gef_conn[i].PC[pos] = gef_conn[i].PC[--gef_conn[i].num_PC];
	      gef_conn[i].sf->PC_overall[gef_conn[i].sf->num_PC_overall++] =
		fct;
	    }


	  if ((e->connective == AT_END_CONN) && (e->sons->connective == ATOM))
	    {
	      pos =
		get_fct_pos_from_plnode (n->sons, i, gef_conn[i].PC,
					 gef_conn[i].num_PC);
	      if (pos == -1)
		{
		  continue;
		  exit (1);
		}
	      fct = gef_conn[i].PC[pos];
	      gef_conn[i].PC[pos] = gef_conn[i].PC[--gef_conn[i].num_PC];
	      gef_conn[i].sf->PC_end[gef_conn[i].sf->num_PC_end++] = fct;
	    }


	  if ((n->connective == AT_START_CONN)
	      || (n->connective == AT_END_CONN)
	      || (n->connective == OVER_ALL_CONN))
	    e = n->sons;
	  if ((e->connective == ATOM) || (e->connective == NOT))
	    continue;

	  if (e->connective == BIN_COMP)
	    {
	      e = e->sons;
	      switch (n->connective)
		{
		case OVER_ALL_CONN:
		  gef_conn[i].sf->PC_overall[gef_conn[i].sf->
					     num_PC_overall++] =
		    -index_in_cvars_of_expression (e, i);
		  break;
		case AT_END_CONN:
		  gef_conn[i].sf->PC_end[gef_conn[i].sf->num_PC_end++] =
		    -index_in_cvars_of_expression (e, i);
		  break;
		default:
		  gef_conn[i].PC[gef_conn[i].num_PC++] =
		    -index_in_cvars_of_expression (e, i);
		}
	    }
	  else
	    {
	      printf ("\n\nadd_preconds_to_comp_vars: Unexpected node: \n");
	      print_PlNode (e, 0);
	      exit (1);
	    }
	}
    }
}



void
calc_duration_of_actions (void)
{
  int i, ind;
  PlNode *duration;
  PlOperator *plop;

  for (plop = gloaded_pl2ops; plop; plop = plop->next)
    if (plop->duration)
      {
	GpG.durative_actions_in_domain = TRUE;
	break;
      }
  if (GpG.durative_actions_in_domain)
    GpG.min_action_time = MAXFLOAT;
  else
    GpG.min_action_time = STRIPS_ACTIONS_DURATION;
  for (i = 0; i < gnum_ef_conn; i++)
    {
      if (!GpG.durative_actions_in_domain)
	{
	  gef_conn[i].duration = STRIPS_ACTIONS_DURATION;
	  continue;
	}
      duration = gef_conn[i].plop->duration;
      if (duration)
	{
	  if (duration->sons->sons->connective != EQUAL_CONN)
	    {
	      printf
		("\n\nDuration inequalities are not supported by this version\n\n");
	      exit (1);
	    }
	  ind =
	    index_in_cvars_of_expression (duration->sons->sons->sons->next,
					  i);
	  gef_conn[i].duration = eval_comp_var (&gcomp_var[ind], ind);

	  gef_conn[i].duration = ROUND_TO_1_1000 (gef_conn[i].duration);


	  gef_conn[i].dur_var_index = ind;

	}
      if (gef_conn[i].duration < MIN_ACTION_DURATION)
	gef_conn[i].duration = MIN_ACTION_DURATION;
      if (gef_conn[i].duration < GpG.min_action_time)
	GpG.min_action_time = gef_conn[i].duration;
    }
}





void
calc_cost_of_actions (void)
{
  register int j, el, ind;
  int i, k;
  float opt_funct_before;
  float opt_funct_after;
  Bool same_cost = TRUE;
  int *already_added = alloc_vect (max_num_value / 32 + 1);
  int init_vect[MAX_FUNCTIONS];
  int num_functions;

  memcpy (gcomp_var_value_before, gcomp_var_value,
	  sizeof (int) * gnum_comp_var);

  if (goptimization_exp < 0)
    opt_funct_before = 0.0;
  else
    opt_funct_before =
      eval_comp_var (&gcomp_var[goptimization_exp], goptimization_exp);



  for (i = 0; i < gnum_ef_conn; i++)
    {
      num_functions = 0;

      if (goptimization_exp >= 0)
	{

	  apply_numeric_effects_of_efconn (i);

	  opt_funct_after =
	    eval_comp_var (&gcomp_var[goptimization_exp], goptimization_exp);
	  gef_conn[i].cost = opt_funct_after - opt_funct_before;
	}
      else
	gef_conn[i].cost = 1.0;


      if (GpG.maximize_plan && gef_conn[i].cost > 0)
	gef_conn[i].cost *= -1;

      if (gef_conn[i].cost <= MIN_ACTION_COST && gef_conn[i].cost >= 0)
	gef_conn[i].cost = MIN_ACTION_COST;
      else
	same_cost = FALSE;
      for (j = 0; j < gef_conn[i].num_A; j++)
	if (gef_conn[i].A[j] < 0)
	  {
	    el = -gef_conn[i].A[j];


	    init_vect[num_functions++] = gcomp_var[el].first_op;
	    if (num_functions > MAX_FUNCTIONS)
	      {
		printf ("\n Increase MAX_FUNCTIONS");
		exit (1);
	      }
	    if (gcomp_var[el].operator== ASSIGN_OP)
	      {
		ind = gcomp_var[el].first_op;
		if (!GET_BIT (already_added, ind))
		  {
		    SET_BIT (already_added, ind);
		    add_efconn_to_decrease_list_of (i, ind);
		    add_efconn_to_increase_list_of (i, ind);
		  }
	      }

	    if (gcomp_var[el].operator== INCREASE_OP
		|| gcomp_var[el].operator== SCALE_UP_OP)
	      {
		ind = gcomp_var[el].first_op;
		if (!GET_BIT (already_added, ind))
		  {
		    SET_BIT (already_added, ind);
		    add_efconn_to_increase_list_of (i, ind);
		  }
	      }

	    if (gcomp_var[el].operator== DECREASE_OP
		|| gcomp_var[el].operator== SCALE_DOWN_OP)
	      {
		ind = gcomp_var[el].first_op;
		if (!GET_BIT (already_added, ind))
		  {
		    SET_BIT (already_added, ind);
		    add_efconn_to_decrease_list_of (i, ind);
		  }
	      }



	  }
      for (k = 0; k < num_functions; k++)
	{
	  j = init_vect[k];

	  if (j > gnum_fullnum_initial)
	    printf ("\n Warning calc_cost ");
	  if (abs (GCOMP_VAR_VALUE (j) - GCOMP_VAR_VALUE_BEFORE (j)) <
	      MIN_VALUE)
	    continue;
	  if (GCOMP_VAR_VALUE (j) < GCOMP_VAR_VALUE_BEFORE (j))
	    {
	      add_efconn_to_decrease_list_of (i, j);
	    }
	  else if (GCOMP_VAR_VALUE (j) >= GCOMP_VAR_VALUE_BEFORE (j))
	    {
	      add_efconn_to_increase_list_of (i, j);
	    }
	}
      memcpy (gcomp_var_value, gcomp_var_value_before,
	      sizeof (int) * gnum_comp_var);

    }
  if (same_cost)
    for (i = 0; i < gnum_ef_conn; i++)
      gef_conn[i].cost = STRIPS_ACTIONS_COST;
  free (already_added);

}

void
apply_numeric_effects_of_efconn (int index)
{
  int j;
  for (j = 0; j < gef_conn[index].num_A; j++)
    {
      if (gef_conn[index].A[j] < 0)

	eval_comp_var (&gcomp_var[-gef_conn[index].A[j]],
		       -gef_conn[index].A[j]);
    }
}

int
index_in_cvars_of_expression (PlNode * pln, int ind)
{
  static int call = 0;
  CompositeNumVar cvar;
  float cvar_value = 0.0, cvar_value_before = 0.0;

  int found;
  int tmpindex;


  if (!pln)
    return -1;
  call++;
  switch (pln->connective)
    {
    case F_EXP:
    case METRIC_CONN:
      tmpindex = index_in_cvars_of_expression (pln->sons, ind);
      return tmpindex;


    case LESS_THAN_CONN:
    case LESS_THAN_OR_EQUAL_CONN:
    case GREATER_THAN_CONN:
    case GREATER_OR_EQUAL_CONN:
    case EQUAL_CONN:
    case MINIMIZE_CONN:
    case MAXIMIZE_CONN:
      cvar.operator = op_from_conn (pln->connective);
      cvar.first_op = index_in_cvars_of_expression (pln->sons, ind);
      cvar.second_op = index_in_cvars_of_expression (pln->sons->next, ind);
      cvar_value = eval_comp_var (&cvar, -1);
      cvar_value_before = cvar_value;
      found = search_cvar_in_cvars (&cvar, cvar_value);

      if (found != -1)
	{

	  if ((!GET_BIT (gis_inertial, cvar.first_op))
	      || (!GET_BIT (gis_inertial, cvar.second_op)))
	    sub_affects_list (found, gis_inertial);
	  return found;
	}

      if ((!GET_BIT (gis_inertial, cvar.first_op))
	  || (!GET_BIT (gis_inertial, cvar.second_op)))
	sub_affects_list (gnum_comp_var, gis_inertial);

      GCOMP_VAR_VALUE (gnum_comp_var) = cvar_value;
      GCOMP_VAR_VALUE_BEFORE (gnum_comp_var) = cvar_value_before;

      gcomp_var[gnum_comp_var].position = gnum_comp_var;
      copy_compvar (&(gcomp_var[gnum_comp_var++]), &cvar);


      if (gnum_comp_var >= max_num_value)
	{
	  max_num_value += MAX_NUM_INC;

	  gcomp_var =
	    (CompositeNumVar *) realloc (gcomp_var,
					 max_num_value *
					 sizeof (CompositeNumVar));
	  memset (&gcomp_var[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (CompositeNumVar));

	  gcomp_var_value =
	    (float *) realloc (gcomp_var_value,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gcomp_var_value_before =
	    (float *) realloc (gcomp_var_value_before,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value_before[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  Hvar.common_values =
	    (float *) realloc (Hvar.common_values,
			       max_num_value * sizeof (float));
	  memset (&Hvar.common_values[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gfullnum_initial =
	    (NumVar **) realloc (gfullnum_initial,
				 max_num_value * sizeof (NumVar *));
	  memset (&gfullnum_initial[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (NumVar *));

	  gis_inertial =
	    (int *) realloc (gis_inertial,
			     (max_num_value / 32 + 1) * sizeof (int));
	  memset (&gis_inertial[(max_num_value - MAX_NUM_INC) / 32 + 1], 0,
		  (MAX_NUM_INC / 32 + 1) * sizeof (int));
	}


      insert_cvar_in_hash (&gcomp_var[gnum_comp_var - 1]);
      add_index_to_affect_list_of (gnum_comp_var - 1, cvar.first_op);
      add_index_to_affect_list_of (gnum_comp_var - 1, cvar.second_op);
      return (gnum_comp_var - 1);
      break;

    case INCREASE_CONN:
    case DECREASE_CONN:
    case ASSIGN_CONN:
    case SCALE_UP_CONN:
    case SCALE_DOWN_CONN:
      cvar.operator = op_from_conn (pln->connective);
      cvar.first_op = index_in_cvars_of_expression (pln->sons, ind);
      cvar.second_op = index_in_cvars_of_expression (pln->sons->next, ind);

      sub_affects_list (cvar.first_op, gis_inertial);
      found = search_cvar_in_cvars (&cvar, -1);
      if (found != -1)
	{

	  sub_affects_list (cvar.first_op, gis_inertial);
	  return found;
	}

      RESET_BIT (gis_inertial, gnum_comp_var);
      GCOMP_VAR_VALUE (gnum_comp_var) = cvar_value;
      GCOMP_VAR_VALUE_BEFORE (gnum_comp_var) = cvar_value_before;


      gcomp_var[gnum_comp_var].position = gnum_comp_var;
      copy_compvar (&(gcomp_var[gnum_comp_var++]), &cvar);


      if (gnum_comp_var >= max_num_value)
	{
	  max_num_value += MAX_NUM_INC;

	  gcomp_var =
	    (CompositeNumVar *) realloc (gcomp_var,
					 max_num_value *
					 sizeof (CompositeNumVar));
	  memset (&gcomp_var[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (CompositeNumVar));

	  gcomp_var_value =
	    (float *) realloc (gcomp_var_value,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gcomp_var_value_before =
	    (float *) realloc (gcomp_var_value_before,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value_before[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  Hvar.common_values =
	    (float *) realloc (Hvar.common_values,
			       max_num_value * sizeof (float));
	  memset (&Hvar.common_values[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gfullnum_initial =
	    (NumVar **) realloc (gfullnum_initial,
				 max_num_value * sizeof (NumVar *));
	  memset (&gfullnum_initial[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (NumVar *));

	  gis_inertial =
	    (int *) realloc (gis_inertial,
			     (max_num_value / 32 + 1) * sizeof (int));
	  memset (&gis_inertial[(max_num_value - MAX_NUM_INC) / 32 + 1], 0,
		  (MAX_NUM_INC / 32 + 1) * sizeof (int));
	}


      insert_cvar_in_hash (&gcomp_var[gnum_comp_var - 1]);
      return (gnum_comp_var - 1);
      break;

    case PLUS_CONN:
    case MINUS_CONN:
    case MUL_CONN:
    case DIV_CONN:
      cvar.operator = op_from_conn (pln->connective);
      cvar.first_op = index_in_cvars_of_expression (pln->sons, ind);
      cvar.second_op = index_in_cvars_of_expression (pln->sons->next, ind);
      cvar_value = eval_comp_var (&cvar, -1);
      cvar_value_before = cvar_value;
      found = search_cvar_in_cvars (&cvar, cvar_value);
      if (found != -1)
	{

	  if ((!GET_BIT (gis_inertial, cvar.first_op))
	      || (!GET_BIT (gis_inertial, cvar.second_op)))
	    sub_affects_list (found, gis_inertial);
	  return found;
	}

      if ((!GET_BIT (gis_inertial, cvar.first_op))
	  || (!GET_BIT (gis_inertial, cvar.second_op)))
	sub_affects_list (gnum_comp_var, gis_inertial);
      GCOMP_VAR_VALUE (gnum_comp_var) = cvar_value;
      GCOMP_VAR_VALUE_BEFORE (gnum_comp_var) = cvar_value_before;


      gcomp_var[gnum_comp_var].position = gnum_comp_var;
      copy_compvar (&(gcomp_var[gnum_comp_var++]), &cvar);


      if (gnum_comp_var >= max_num_value)
	{
	  max_num_value += MAX_NUM_INC;

	  gcomp_var =
	    (CompositeNumVar *) realloc (gcomp_var,
					 max_num_value *
					 sizeof (CompositeNumVar));
	  memset (&gcomp_var[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (CompositeNumVar));

	  gcomp_var_value =
	    (float *) realloc (gcomp_var_value,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gcomp_var_value_before =
	    (float *) realloc (gcomp_var_value_before,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value_before[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  Hvar.common_values =
	    (float *) realloc (Hvar.common_values,
			       max_num_value * sizeof (float));
	  memset (&Hvar.common_values[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gfullnum_initial =
	    (NumVar **) realloc (gfullnum_initial,
				 max_num_value * sizeof (NumVar *));
	  memset (&gfullnum_initial[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (NumVar *));

	  gis_inertial =
	    (int *) realloc (gis_inertial,
			     (max_num_value / 32 + 1) * sizeof (int));
	  memset (&gis_inertial[(max_num_value - MAX_NUM_INC) / 32 + 1], 0,
		  (MAX_NUM_INC / 32 + 1) * sizeof (int));
	}


      insert_cvar_in_hash (&gcomp_var[gnum_comp_var - 1]);

      add_index_to_affect_list_of (gnum_comp_var - 1, cvar.first_op);
      add_index_to_affect_list_of (gnum_comp_var - 1, cvar.second_op);
      return (gnum_comp_var - 1);
      break;

    case UMINUS_CONN:
      cvar.operator = op_from_conn (pln->connective);
      cvar.first_op = index_in_cvars_of_expression (pln->sons, ind);
      cvar.second_op = -1;
      cvar_value = eval_comp_var (&cvar, -1);
      cvar_value_before = cvar_value;
      found = search_cvar_in_cvars (&cvar, cvar_value);

      if (found != -1)
	{

	  if (!GET_BIT (gis_inertial, cvar.first_op))
	    sub_affects_list (found, gis_inertial);
	  return found;
	}
      if (!GET_BIT (gis_inertial, cvar.first_op))
	sub_affects_list (gnum_comp_var, gis_inertial);
      GCOMP_VAR_VALUE (gnum_comp_var) = cvar_value;
      GCOMP_VAR_VALUE_BEFORE (gnum_comp_var) = cvar_value_before;


      gcomp_var[gnum_comp_var].position = gnum_comp_var;
      copy_compvar (&(gcomp_var[gnum_comp_var++]), &cvar);


      if (gnum_comp_var >= max_num_value)
	{
	  max_num_value += MAX_NUM_INC;

	  gcomp_var =
	    (CompositeNumVar *) realloc (gcomp_var,
					 max_num_value *
					 sizeof (CompositeNumVar));
	  memset (&gcomp_var[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (CompositeNumVar));

	  gcomp_var_value =
	    (float *) realloc (gcomp_var_value,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gcomp_var_value_before =
	    (float *) realloc (gcomp_var_value_before,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value_before[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  Hvar.common_values =
	    (float *) realloc (Hvar.common_values,
			       max_num_value * sizeof (float));
	  memset (&Hvar.common_values[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gfullnum_initial =
	    (NumVar **) realloc (gfullnum_initial,
				 max_num_value * sizeof (NumVar *));
	  memset (&gfullnum_initial[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (NumVar *));

	  gis_inertial =
	    (int *) realloc (gis_inertial,
			     (max_num_value / 32 + 1) * sizeof (int));
	  memset (&gis_inertial[(max_num_value - MAX_NUM_INC) / 32 + 1], 0,
		  (MAX_NUM_INC / 32 + 1) * sizeof (int));
	}


      insert_cvar_in_hash (&gcomp_var[gnum_comp_var - 1]);
      add_index_to_affect_list_of (gnum_comp_var - 1, cvar.first_op);
      return (gnum_comp_var - 1);
      break;

    case DURATION_VAR_ATOM:
      if (gef_conn[ind].dur_var_index == -1)
	printf ("\n\nwarning! shouldn't be -1\n\n");
      return gef_conn[ind].dur_var_index;

    case FN_HEAD:
      cvar.operator = VARIABLE_OP;
      cvar.first_op = get_numvar_index_of_fn_head (pln, ind);
      cvar.second_op = -1;


      if (cvar.first_op == -1)
	cvar_value = -1e6;
      else
	cvar_value = eval_comp_var (&cvar, -2);

      cvar_value_before = cvar_value;
      found = search_cvar_in_cvars (&cvar, cvar_value);
      if (found != -1)
	return found;
      GCOMP_VAR_VALUE (gnum_comp_var) = cvar_value;
      GCOMP_VAR_VALUE_BEFORE (gnum_comp_var) = cvar_value_before;

      gcomp_var[gnum_comp_var].position = gnum_comp_var;
      copy_compvar (&(gcomp_var[gnum_comp_var++]), &cvar);


      if (gnum_comp_var >= max_num_value)
	{
	  max_num_value += MAX_NUM_INC;

	  gcomp_var =
	    (CompositeNumVar *) realloc (gcomp_var,
					 max_num_value *
					 sizeof (CompositeNumVar));
	  memset (&gcomp_var[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (CompositeNumVar));

	  gcomp_var_value =
	    (float *) realloc (gcomp_var_value,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gcomp_var_value_before =
	    (float *) realloc (gcomp_var_value_before,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value_before[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  Hvar.common_values =
	    (float *) realloc (Hvar.common_values,
			       max_num_value * sizeof (float));
	  memset (&Hvar.common_values[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));
	  gfullnum_initial =
	    (NumVar **) realloc (gfullnum_initial,
				 max_num_value * sizeof (NumVar *));
	  memset (&gfullnum_initial[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (NumVar *));

	  gis_inertial =
	    (int *) realloc (gis_inertial,
			     (max_num_value / 32 + 1) * sizeof (int));
	  memset (&gis_inertial[(max_num_value - MAX_NUM_INC) / 32 + 1], 0,
		  (MAX_NUM_INC / 32 + 1) * sizeof (int));
	}


      insert_cvar_in_hash (&gcomp_var[gnum_comp_var - 1]);
      return (gnum_comp_var - 1);
      break;
    case NUM_EXP:
    case ATOM:
      cvar.operator = FIX_NUMBER;
      cvar.first_op = -1;
      cvar.second_op = -1;
      cvar_value = evaluate_exp (pln);
      cvar_value_before = cvar_value;
      found = search_cvar_in_cvars (&cvar, cvar_value);
      if (found != -1)
	return found;
      GCOMP_VAR_VALUE (gnum_comp_var) = cvar_value;
      GCOMP_VAR_VALUE_BEFORE (gnum_comp_var) = cvar_value_before;

      gcomp_var[gnum_comp_var].position = gnum_comp_var;
      copy_compvar (&(gcomp_var[gnum_comp_var++]), &cvar);


      if (gnum_comp_var >= max_num_value)
	{
	  max_num_value += MAX_NUM_INC;

	  gcomp_var =
	    (CompositeNumVar *) realloc (gcomp_var,
					 max_num_value *
					 sizeof (CompositeNumVar));
	  memset (&gcomp_var[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (CompositeNumVar));

	  gcomp_var_value =
	    (float *) realloc (gcomp_var_value,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gcomp_var_value_before =
	    (float *) realloc (gcomp_var_value_before,
			       max_num_value * sizeof (float));
	  memset (&gcomp_var_value_before[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  Hvar.common_values =
	    (float *) realloc (Hvar.common_values,
			       max_num_value * sizeof (float));
	  memset (&Hvar.common_values[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (float));

	  gfullnum_initial =
	    (NumVar **) realloc (gfullnum_initial,
				 max_num_value * sizeof (NumVar *));
	  memset (&gfullnum_initial[max_num_value - MAX_NUM_INC], 0,
		  MAX_NUM_INC * sizeof (NumVar *));

	  gis_inertial =
	    (int *) realloc (gis_inertial,
			     (max_num_value / 32 + 1) * sizeof (int));
	  memset (&gis_inertial[(max_num_value - MAX_NUM_INC) / 32 + 1], 0,
		  (MAX_NUM_INC / 32 + 1) * sizeof (int));
	}


      insert_cvar_in_hash (&gcomp_var[gnum_comp_var - 1]);
      return (gnum_comp_var - 1);
      break;

    case TOTAL_TIME_CONN:
      sub_affects_list (TOTAL_TIME_FUNCTION_INDEX, gis_inertial);
      return TOTAL_TIME_FUNCTION_INDEX;

    default:
      printf ("\n\nindex_in_cvars_of_expression: shouldnt get here\n\n");
      exit (1);
      return -1;
    }
}

float
eval_comp_var (CompositeNumVar * cv, int index)
{
  float tmp, cv_value;
  switch (cv->operator)
    {
    case FIX_NUMBER:
    case VARIABLE_OP:
      if (index < 0)
	{
	  if (index == -1)
	    printf ("\n Warning in eval_comp_var index <0 : %d", index);
	  return 0.0;
	}
      else
	return GCOMP_VAR_VALUE (index);
      break;
    case MUL_OP:
      return eval_comp_var (&(gcomp_var[cv->first_op]),
			    cv->first_op) *
	eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op);
      break;
    case DIV_OP:
      tmp = eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op);
      if (tmp == 0)
	printf ("\n\nWARNING: Division by zero in eval_comp_var\n\n");
      return eval_comp_var (&(gcomp_var[cv->first_op]), cv->first_op) / tmp;
      break;
    case PLUS_OP:
      return eval_comp_var (&(gcomp_var[cv->first_op]),
			    cv->first_op) +
	eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op);
      break;
    case MINUS_OP:
      return eval_comp_var (&(gcomp_var[cv->first_op]),
			    cv->first_op) -
	eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op);
      break;
    case UMINUS_OP:
      return -eval_comp_var (&(gcomp_var[cv->first_op]), cv->first_op);
      break;
    case INCREASE_OP:
      GCOMP_VAR_VALUE (cv->first_op) +=
	eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op);

      cv_value = GCOMP_VAR_VALUE (cv->first_op);
      if (index >= 0)
	GCOMP_VAR_VALUE (index) = cv_value;
      return cv_value;
      break;
    case DECREASE_OP:
      GCOMP_VAR_VALUE (cv->first_op) -=
	eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op);
      cv_value = GCOMP_VAR_VALUE (cv->first_op);
      if (index >= 0)
	GCOMP_VAR_VALUE (index) = cv_value;
      return cv_value;
      break;
    case SCALE_UP_OP:
      GCOMP_VAR_VALUE (cv->first_op) *=
	eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op);
      cv_value = GCOMP_VAR_VALUE (cv->first_op);
      if (index >= 0)
	GCOMP_VAR_VALUE (index) = cv_value;
      return cv_value;
      break;
    case SCALE_DOWN_OP:
      GCOMP_VAR_VALUE (cv->
		       first_op) /=
	eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op);
      cv_value = GCOMP_VAR_VALUE (cv->first_op);
      if (index >= 0)
	GCOMP_VAR_VALUE (index) = cv_value;
      return cv_value;
      break;
    case ASSIGN_OP:
      cv_value = eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op);
      GCOMP_VAR_VALUE (cv->first_op) = cv_value;
      if (index >= 0)
	GCOMP_VAR_VALUE (index) = cv_value;
      break;
    case MINIMIZE_OP:
      return eval_comp_var (&(gcomp_var[cv->first_op]), cv->first_op);
    case MAXIMIZE_OP:
      return -eval_comp_var (&(gcomp_var[cv->first_op]), cv->first_op);
    case EQUAL_OP:
      cv_value =
	(eval_comp_var (&(gcomp_var[cv->first_op]), cv->first_op) ==
	 eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op));
      if (index >= 0)
	GCOMP_VAR_VALUE (index) = cv_value;
      return cv_value;
    case GREATER_THAN_OP:
      cv_value =
	(eval_comp_var (&(gcomp_var[cv->first_op]), cv->first_op) >
	 eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op));
      if (index >= 0)
	GCOMP_VAR_VALUE (index) = cv_value;
      return cv_value;
    case GREATER_OR_EQUAL_OP:
      cv_value =
	(eval_comp_var (&(gcomp_var[cv->first_op]), cv->first_op) >=
	 eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op));
      if (index >= 0)
	GCOMP_VAR_VALUE (index) = cv_value;
      return cv_value;
    case LESS_THAN_OP:
      cv_value =
	(eval_comp_var (&(gcomp_var[cv->first_op]), cv->first_op) <
	 eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op));
      if (index >= 0)
	GCOMP_VAR_VALUE (index) = cv_value;
      return cv_value;
    case LESS_THAN_OR_EQUAL_OP:
      cv_value =
	(eval_comp_var (&(gcomp_var[cv->first_op]), cv->first_op) <=
	 eval_comp_var (&(gcomp_var[cv->second_op]), cv->second_op));
      if (index >= 0)
	GCOMP_VAR_VALUE (index) = cv_value;
      return cv_value;
    default:
      printf ("\nOperator %d not yet supported in expression evaluation\n\n",
	      cv->operator);
      exit (1);
      return -1;
      break;
    }

  return 0;
}



int
search_cvar_in_cvars (CompositeNumVar * cv, float cv_value)
{
  int hash_index;
  CompositeNumVar *to_check;
  int result = -1;
  tot++;
  hash_index = cvar_hash_index (cv);

  if (cvar_hash_table[hash_index] < 0)
    return -1;
  compl++;

  for (to_check = &gcomp_var[cvar_hash_table[hash_index]]; to_check;
       to_check = to_check->next)
    {
      if (to_check->operator != cv->operator)
	continue;
      if (to_check->first_op != cv->first_op)
	continue;
      if (to_check->second_op != cv->second_op)
	continue;
      if (to_check->operator == FIX_NUMBER)
	if (GCOMP_VAR_VALUE (to_check->position) != cv_value)
	  continue;
      result = to_check->position;
      break;
    }

  return result;



}

void
copy_compvar (CompositeNumVar * dest, CompositeNumVar * src)
{
  dest->operator = src->operator;
  dest->first_op = src->first_op;
  dest->second_op = src->second_op;
}

int
get_numvar_index_of_fn_head (PlNode * p, int index)
{
  static NumVar tmpvar;
  NumVar *nv;
  int i, j;

  create_numvar_from_fn_head (&tmpvar, p, index);
  for (i = 0; i < gnum_fullnum_initial; i++)
    {
      nv = gfullnum_initial[i];
      if (nv->function != tmpvar.function)
	continue;
      for (j = 0; j < gfunarity[nv->function]; j++)
	if (nv->args[j] != tmpvar.args[j])
	  break;
      if (j == gfunarity[nv->function])
	return i;
    }

  return -1;
}

void
create_numvar_from_fn_head (NumVar * ret, PlNode * plnvar, int i)
{
  TokenList *tl;
  int arg = 0;
  int ind;
  Token name;

  ret->function = index_in_function_table_of (plnvar->atom->item);
  for (tl = plnvar->atom->next; tl; tl = tl->next)
    {
      name = tl->item;

      if (i == -1)
	{
	  ret->args[arg++] = index_in_objects_table_of (name);
	  continue;
	}

      ind = get_index_of_arg (name, i);

      if (ind >= 0)
	ret->args[arg++] =
	  gop_conn[gef_conn[i].op].action->name_inst_table[ind];
      else
	ret->args[arg++] = get_index_of_constant (name);
    }

}

int
index_in_function_table_of (char *funct_name)
{
  int i;
  for (i = 0; i < gnum_functions; i++)
    {
      if (strcmp (funct_name, gfunctions[i]) == SAME)
	return i;
    }
  printf ("\n\nFunction name %s not found in function table\n\n", funct_name);
  exit (1);
  return -1;
}

int
index_in_objects_table_of (char *obj_name)
{
  int i;
  for (i = 0; i < gnum_constants; i++)
    {
      if (strcmp (obj_name, gconstants[i]) == SAME)
	return i;
    }
  printf ("\n\nObject name %s not found in objects table\n\n", obj_name);
  exit (1);
  return -1;
}


int
get_index_of_arg (char *arg, int efconn)
{
  TypedList *parms;
  TypedList *n;
  int i = 0;

  parms = gef_conn[efconn].plop->parse_params;
  for (n = parms; n; n = n->next)
    {
      if (strcmp (n->name, arg) == SAME)
	return i;
      i++;
    }
  return -1;
}


int
get_index_of_pred (char *arg)
{
  int i;

  for (i = 0; i < gnum_predicates; i++)
    {
      if (strcmp (gpredicates[i], arg) == SAME)
	return i;
    }
  printf ("\n\nPred name %s not found in params\n\n", arg);
  exit (1);
  return -1;
}


OPERATOR_TYPE
op_from_conn (Connective c)
{
  switch (c)
    {
    case MUL_CONN:
      return MUL_OP;
    case DIV_CONN:
      return DIV_OP;
    case PLUS_CONN:
      return PLUS_OP;
    case MINUS_CONN:
      return MINUS_OP;
    case UMINUS_CONN:
      return UMINUS_OP;
    case INCREASE_CONN:
      return INCREASE_OP;
    case DECREASE_CONN:
      return DECREASE_OP;
    case SCALE_UP_CONN:
      return SCALE_UP_OP;
    case SCALE_DOWN_CONN:
      return SCALE_DOWN_OP;
    case ASSIGN_CONN:
      return ASSIGN_OP;
    case LESS_THAN_CONN:
      return LESS_THAN_OP;
    case LESS_THAN_OR_EQUAL_CONN:
      return LESS_THAN_OR_EQUAL_OP;
    case EQUAL_CONN:
      return EQUAL_OP;
    case GREATER_THAN_CONN:
      return GREATER_THAN_OP;
    case GREATER_OR_EQUAL_CONN:
      return GREATER_OR_EQUAL_OP;
    case MINIMIZE_CONN:
      return MINIMIZE_OP;
    case MAXIMIZE_CONN:
      return MAXIMIZE_OP;
    default:
      printf ("\n\nNot defined\n\n");
      exit (1);
    }
  return -1;
}


void
set_used_vars_in_duration_exp (int efconn, int ncvar)
{
  if (ncvar == -1)
    return;
  switch (gcomp_var[ncvar].operator )
    {
    case MUL_OP:
    case DIV_OP:
    case MINUS_OP:
    case UMINUS_OP:
    case PLUS_OP:
    case EQUAL_OP:
      set_used_vars_in_duration_exp (efconn, gcomp_var[ncvar].first_op);
      set_used_vars_in_duration_exp (efconn, gcomp_var[ncvar].second_op);
      break;
    case VARIABLE_OP:
      if (!gef_conn[efconn].duration_rvals)
	gef_conn[efconn].duration_rvals = alloc_vect (gnum_comp_var / 32 + 1);
      SET_BIT (gef_conn[efconn].duration_rvals, gcomp_var[ncvar].first_op);
      break;
    case FIX_NUMBER:
      break;
    default:
      printf ("\n\nwrong cvar\n\n");
      fflush (stdout);
      assert (0);
      exit (1);
      break;
    }
}





void
propagate_inertias (void)
{
  int i;
  CompositeNumVar *cv;
  for (i = 0; i < gnum_comp_var; i++)
    {

      if (!GET_BIT (gis_inertial, i))
	continue;
      cv = &gcomp_var[i];
      switch (cv->operator)
	{
	case FIX_NUMBER:

	  break;
	case EQUAL_OP:
	case GREATER_THAN_OP:
	case GREATER_OR_EQUAL_OP:
	case LESS_THAN_OP:
	case LESS_THAN_OR_EQUAL_OP:
	  cv->operator= FIX_NUMBER;
	  cv->first_op = -1;
	  cv->second_op = -1;
	  GCOMP_VAR_VALUE (i) = eval_comp_var (cv, i);
	  cv->affects = free_intlist (cv->affects);

	  break;
	case MUL_OP:
	case DIV_OP:
	case PLUS_OP:
	case MINUS_OP:
	case UMINUS_OP:
	case MINIMIZE_OP:
	case MAXIMIZE_OP:
	case VARIABLE_OP:
	  cv->operator= FIX_NUMBER;
	  cv->first_op = -1;
	  cv->second_op = -1;
	  cv->affects = free_intlist (cv->affects);

	  break;
	default:
	  printf ("\n\nOP not supported by Propagate_inertias\n\n");
	  exit (1);
	  break;
	}
    }

  GpG.variable_duration = FALSE;
  for (i = 0; i < gnum_ef_conn; i++)
    {
      if (!GET_BIT (gis_inertial, gef_conn[i].dur_var_index))
	{
	  GpG.variable_duration = TRUE;

	  set_used_vars_in_duration_exp (i, gef_conn[i].dur_var_index);
	}
    }
}

int
get_num_of_effects_of (int index, TimeSpec ts, Bool is_additive)
{
  if (gef_conn[index].plop)
    {
      if (ts == AT_START_TIME)
	{
	  if (is_additive)
	    return gef_conn[index].plop->num_effects_start;
	  else
	    return gef_conn[index].plop->num_deleffects_start;
	}
      if (ts == AT_END_TIME)
	return gef_conn[index].plop->num_numeric_effects_end;
      printf ("\n\nor AT_START neither AT_END!!!\n\n");
      exit (1);
    }
  gef_conn[index].plop =
    search_name_in_plops (gop_conn[gef_conn[index].op].action->name);
  if (!gef_conn[index].plop)
    {
      printf ("\n\nop not found?!!!\n\n");
      exit (1);
    }
  if ((gef_conn[index].plop->is_odd) && (!gef_conn[index].sf))
    gef_conn[index].sf = new_SpecialFacts ();

  if (ts == AT_START_TIME)
    {
      if (is_additive)
	return gef_conn[index].plop->num_effects_start;
      else
	return gef_conn[index].plop->num_deleffects_start;
    }
  if (ts == AT_END_TIME)
    return gef_conn[index].plop->num_numeric_effects_end;
  printf ("\n\nor AT_START neither AT_END!!!\n\n");
  exit (1);
}

int
get_num_of_preconds_of (int index, TimeSpec ts)
{
  if (gef_conn[index].plop)
    {
      switch (ts)
	{
	case AT_START_TIME:
	  return gef_conn[index].plop->num_numeric_preconds_start;
	  break;
	case AT_END_TIME:
	  return gef_conn[index].plop->num_preconds_end;
	  break;
	case OVER_ALL_TIME:
	  return gef_conn[index].plop->num_preconds_overall;
	  break;
	default:
	  printf ("\n\nor AT_START neither AT_END neither OVER_ALL!!!\n\n");
	  exit (1);
	}
    }
  gef_conn[index].plop =
    search_name_in_plops (gop_conn[gef_conn[index].op].action->name);
  if (!gef_conn[index].plop)
    {
      printf ("\n\nop not found?!!!\n\n");
      exit (1);
    }
  if ((gef_conn[index].plop->is_odd) && (!gef_conn[index].sf))
    gef_conn[index].sf = new_SpecialFacts ();

  if (ts == AT_START_TIME)
    return gef_conn[index].plop->num_numeric_preconds_start;
  if (ts == AT_END_TIME)
    return gef_conn[index].plop->num_preconds_end;
  if (ts == OVER_ALL_TIME)
    return gef_conn[index].plop->num_preconds_overall;
  printf ("\n\nor AT_START neither AT_END neither OVER_ALL!!!\n\n");
  exit (1);
}


void
add_index_to_affect_list_of (int affected_cvar, int affecting_cvar)
{
  IntList *tmpil;
  tmpil = new_IntList ();
  tmpil->item = affected_cvar;

  if (affecting_cvar >= 0)
    {
      tmpil->next = gcomp_var[affecting_cvar].affects;
      gcomp_var[affecting_cvar].affects = tmpil;
    }
  else
    tmpil->next = NULL;
}


void
add_efconn_to_increase_list_of (int n_ef, int cvar)
{
  IntList *tmpil;
  tmpil = new_IntList ();
  tmpil->item = n_ef;
  tmpil->next = gcomp_var[cvar].increased_by;
  gcomp_var[cvar].increased_by = tmpil;

}

void
add_efconn_to_decrease_list_of (int n_ef, int cvar)
{
  IntList *tmpil;
  tmpil = new_IntList ();
  tmpil->item = n_ef;
  tmpil->next = gcomp_var[cvar].decreased_by;
  gcomp_var[cvar].decreased_by = tmpil;

}

int
get_fct_pos_from_plnode (PlNode * pln, int gef_index, int *start, int max)
{
  char String[MAX_LENGTH];
  int ind, i, j;
  int *cur;
  TokenList *tl;
  int cnt = 0;
  Fact node_f;
  Fact *gef_f;

  if (pln->connective == NOT)
    pln = pln->sons;
  if (pln->connective == ATOM)
    {
      strcpy (String, pln->atom->item);
      node_f.predicate = get_index_of_pred (String);
      for (tl = pln->atom->next; tl; tl = tl->next)
	{
	  ind = get_index_of_arg (tl->item, gef_index);
	  if (ind != -1)
	    node_f.args[cnt++] =
	      gop_conn[gef_conn[gef_index].op].action->inst_table[ind];
	  else

	    for (j = 0; j < gnum_constants; j++)
	      if (strcmp (tl->item, gconstants[j]) == SAME)
		{
		  node_f.args[cnt++] = j;
		  break;
		}
	}
      for (ind = 0; ind < max; ind++)
	{

	  cur = start + ind;
	  gef_f = &grelevant_facts[*cur];

	  if (node_f.predicate != gef_f->predicate)
	    continue;

	  for (i = 0; i < garity[node_f.predicate]; i++)
	    if (node_f.args[i] != gef_f->args[i])
	      break;

	  if (i == garity[node_f.predicate])
	    {
	      return ind;
	    }
	  else
	    continue;
	}
    }
  else
    {
      printf
	("\n\nget_fct_pos_from_plnode: PlNode is not a boolean fact\n\n");
      exit (1);
    }
  if (DEBUG2)
    {
      printf ("\n\nget_fct_pos_from_plnode: Fact not found: ");
      print_Fact (&node_f);
    }

  return -1;
}

void
create_neighborhood_for_compvar (int index, Bool Sign, Bool MulOrDiv,
				 action_set * neighbors, int start, int level)
{
  IntList *el;
  int i, j;
  static int *neighb_actions = NULL;
  float val_before, val_after;
  float out_vect[MAX_NUM_VALUE];
  float *in_vect;
  float diff;

  if (neighb_actions == NULL)
    neighb_actions = calloc (gnum_ef_conn / 32 + 1, sizeof (int));
  if (index < 0)
    {
      index = -index;

    }


  if (start == 1 && level != -1)
    memset (neighb_actions, 0, (gnum_ef_conn / 32 + 1) * sizeof (int));

  if (level >= 0)
    in_vect = vectlevel[level]->numeric->values;
  else
    in_vect = vectlevel[0]->numeric->values;

  switch (gcomp_var[index].operator)
    {
    case EQUAL_OP:
      printf
	("\n\nGeneration of neighborhood for EQUAL_OP not yet implemented\n\n");
      exit (1);
      break;
    case GREATER_THAN_OP:
    case GREATER_OR_EQUAL_OP:
      create_neighborhood_for_compvar (gcomp_var[index].first_op, Sign,
				       MulOrDiv, neighbors, 0, level);
      create_neighborhood_for_compvar (gcomp_var[index].second_op, !Sign,
				       MulOrDiv, neighbors, 0, level);
      break;
    case LESS_THAN_OP:
    case LESS_THAN_OR_EQUAL_OP:
      create_neighborhood_for_compvar (gcomp_var[index].first_op, !Sign,
				       MulOrDiv, neighbors, 0, level);
      create_neighborhood_for_compvar (gcomp_var[index].second_op, Sign,
				       MulOrDiv, neighbors, 0, level);
      break;
    case MUL_OP:
      create_neighborhood_for_compvar (gcomp_var[index].first_op, Sign, 1,
				       neighbors, 0, level);
      create_neighborhood_for_compvar (gcomp_var[index].second_op, Sign, 1,
				       neighbors, 0, level);
      break;
    case PLUS_OP:
      create_neighborhood_for_compvar (gcomp_var[index].first_op, Sign, 0,
				       neighbors, 0, level);
      create_neighborhood_for_compvar (gcomp_var[index].second_op, Sign, 0,
				       neighbors, 0, level);
      break;
    case DIV_OP:
      create_neighborhood_for_compvar (gcomp_var[index].first_op, Sign, 1,
				       neighbors, 0, level);
      create_neighborhood_for_compvar (gcomp_var[index].second_op, !Sign, 1,
				       neighbors, 0, level);
      break;
    case MINUS_OP:
      create_neighborhood_for_compvar (gcomp_var[index].first_op, Sign, 0,
				       neighbors, 0, level);
      create_neighborhood_for_compvar (gcomp_var[index].second_op, !Sign, 0,
				       neighbors, 0, level);
      break;
    case MAXIMIZE_OP:
      create_neighborhood_for_compvar (gcomp_var[index].first_op, Sign, 0,
				       neighbors, -2, level);
      break;
    case UMINUS_OP:
      create_neighborhood_for_compvar (gcomp_var[index].first_op, !Sign, 0,
				       neighbors, 0, level);
      break;
    case VARIABLE_OP:
      if (start == -2)
	{
	  if (Sign)
	    insert_els_in_neighborhood (gcomp_var[index].increased_by,
					neighbors);
	  else
	    insert_els_in_neighborhood (gcomp_var[index].decreased_by,
					neighbors);
	}
      else if (neighbors != NULL)
	{
	  if (Sign)
	    {
	      for (el = gcomp_var[index].increased_by; el; el = el->next)
		{
		  SET_BIT (neighb_actions, el->item);
		}
	    }
	  else
	    for (el = gcomp_var[index].decreased_by; el; el = el->next)
	      {
		SET_BIT (neighb_actions, el->item);
	      }
	}
      break;

    case FIX_NUMBER:
      break;

    default:
      printf ("\n\nshouldnt get this op %d  here\n\n",
	      gcomp_var[index].operator);
    }

  if (start == 1 && level != -1)
    {
      for (i = 0; i < gnum_ef_conn; i++)
	{

	  if (GET_BIT (neighb_actions, i))
	    {


	      memcpy (out_vect, in_vect, gnum_comp_var * sizeof (int));
	      val_before =
		out_vect[gcomp_var[index].first_op] -
		out_vect[gcomp_var[index].second_op];

	      for (j = 0; j < gef_conn[i].num_A; j++)
		if (gef_conn[i].A[j] < 0)
		  try_num_eff_in_level (-gef_conn[i].A[j], in_vect, out_vect);


	      val_after =
		out_vect[gcomp_var[index].first_op] -
		out_vect[gcomp_var[index].second_op];

	      diff = val_after - val_before;
	      if (((gcomp_var[index].operator== LESS_THAN_OP)
		   || (gcomp_var[index].operator== LESS_THAN_OR_EQUAL_OP))
		  && (diff < -MAX_APPROX))
		{
		  neighbors->A[neighbors->num_A++] = i;
		}
	      if (((gcomp_var[index].operator== GREATER_THAN_OP)
		   || (gcomp_var[index].operator== GREATER_OR_EQUAL_OP))
		  && (diff > MAX_APPROX))
		{
		  neighbors->A[neighbors->num_A++] = i;
		}
	    }
	}

    }

}

void
create_remotion_neighborhood_for_compvar (int fct_pos, int level,
					  action_set * neighbors)
{
  neighbors->A[(neighbors->num_A)++] = vectlevel[level]->action.position;
}



void
insert_els_in_neighborhood (IntList * ilist, action_set * neighbors)
{
  IntList *el;
  for (el = ilist; el; el = el->next)
    {
      if (neighbors == NULL)
	printf ("%d%s", el->item, el->next ? ", " : " ");
      else
	{
	  neighbors->A[(neighbors->num_A)++] = el->item;
	  if (neighbors->num_A >= MAX_NUM_ACTIONS)
	    {
	      printf ("\nWARNING: reached MAX_NUM_ACTIONS\n");
	      neighbors->num_A = MAX_NUM_ACTIONS - 1;
	      break;
	    }
	}
    }
}

int
cvar_hash_index (CompositeNumVar * cv)
{
  return (cv->operator + abs (cv->first_op) * 2563 +
	  abs (cv->second_op) * 2563 * 2563) % BIGPRIME % HASH_SIZE;
}

void
reset_cvar_hash_table ()
{
  memset (cvar_hash_table, -1, HASH_SIZE * sizeof (int));

}

void
insert_cvar_in_hash (CompositeNumVar * cv)
{
  int hash_index;

  hash_index = cvar_hash_index (cv);

  if (cvar_hash_table[hash_index] < 0)
    {


      cvar_hash_table[hash_index] = cv->position;
      cv->next = NULL;
    }
  else
    {

      cv->next = &gcomp_var[cvar_hash_table[hash_index]];
      cvar_hash_table[hash_index] = cv->position;
    }

}


void
set_tested_vars_for_cvar (int ef, int ncvar)
{
  if (ncvar == -1)
    return;

  if (!tested_vars)
    tested_vars = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);


  switch (gcomp_var[ncvar].operator )
    {
    case MUL_OP:
    case DIV_OP:
    case MINUS_OP:
    case UMINUS_OP:
    case PLUS_OP:
    case EQUAL_OP:
    case GREATER_THAN_OP:
    case GREATER_OR_EQUAL_OP:
    case LESS_THAN_OP:
    case LESS_THAN_OR_EQUAL_OP:
      set_tested_vars_for_cvar (ef, gcomp_var[ncvar].first_op);
      set_tested_vars_for_cvar (ef, gcomp_var[ncvar].second_op);
      break;
    case VARIABLE_OP:
      SET_BIT (tested_vars[ef], gcomp_var[ncvar].first_op);
      break;
    case FIX_NUMBER:
      break;
    default:
      printf ("\n\nwrong cvar\n\n");
      fflush (stdout);
      assert (0);
      exit (1);
      break;
    }
}


void
set_lvals_and_rvals (void)
{
  int i, j;
  for (i = 0; i < gnum_ef_conn; i++)
    {
      for (j = 0; j < gef_conn[i].num_A; j++)
	if (gef_conn[i].A[j] < 0)
	  {
	    SET_BIT (l_vals[i], gcomp_var[-gef_conn[i].A[j]].first_op);
	    if ((gcomp_var[-gef_conn[i].A[j]].operator == INCREASE_OP)
		|| (gcomp_var[-gef_conn[i].A[j]].operator == DECREASE_OP))
	      SET_BIT (lstar_vals[i], gcomp_var[-gef_conn[i].A[j]].first_op);
	    set_rvals_for_cvar (i, gcomp_var[-gef_conn[i].A[j]].second_op);
	  }
      if (gef_conn[i].sf)
	for (j = 0; j < gef_conn[i].sf->num_A_start; j++)
	  if (gef_conn[i].sf->A_start[j] < 0)
	    {
	      SET_BIT (l_vals[i],
		       gcomp_var[-gef_conn[i].sf->A_start[j]].first_op);
	      if ((gcomp_var[-gef_conn[i].sf->A_start[j]].
		   operator == INCREASE_OP)
		  || (gcomp_var[-gef_conn[i].sf->A_start[j]].
		      operator == DECREASE_OP))
		SET_BIT (lstar_vals[i],
			 gcomp_var[-gef_conn[i].sf->A_start[j]].first_op);
	      set_rvals_for_cvar (i,
				  gcomp_var[-gef_conn[i].sf->A_start[j]].
				  second_op);
	    }

      for (j = 0; j < gef_conn[i].num_PC; j++)
	if (gef_conn[i].PC[j] < 0)
	  {
	    set_tested_vars_for_cvar (i, -gef_conn[i].PC[j]);
	  }
      if (gef_conn[i].sf)
	{
	  for (j = 0; j < gef_conn[i].sf->num_PC_overall; j++)
	    if (gef_conn[i].sf->PC_overall[j] < 0)
	      {
		set_tested_vars_for_cvar (i, -gef_conn[i].sf->PC_overall[j]);
	      }
	  for (j = 0; j < gef_conn[i].sf->num_PC_end; j++)
	    if (gef_conn[i].sf->PC_end[j] < 0)
	      {
		set_tested_vars_for_cvar (i, -gef_conn[i].sf->PC_end[j]);
	      }
	}

    }
}


void
set_rvals_for_cvar (int ef, int ncvar)
{
  if (ncvar == -1)
    return;

  switch (gcomp_var[ncvar].operator )
    {
    case MUL_OP:
    case DIV_OP:
    case MINUS_OP:
    case UMINUS_OP:
    case PLUS_OP:
      set_rvals_for_cvar (ef, gcomp_var[ncvar].first_op);
      set_rvals_for_cvar (ef, gcomp_var[ncvar].second_op);
      break;
    case VARIABLE_OP:
      SET_BIT (r_vals[ef], gcomp_var[ncvar].first_op);
      break;
    case FIX_NUMBER:
      break;
    default:
      printf ("\n\nset_rvals_for_cvar:wrong cvar\n\n");
      exit (1);
      break;
    }
}


int **
alloc_matrix (int rows, int cols)
{
  int **matrix;
  int *ptr;
  int i;

  matrix = (int **) calloc (rows, sizeof (int *));
  assert (matrix);
  ptr = (int *) calloc (rows * cols, sizeof (int));

  assert (ptr);

  memset (ptr, 0, rows * cols * sizeof (int));

  matrix[0] = ptr;
  for (i = 1; i < rows; i++)
    matrix[i] = (int *) (matrix[i - 1] + cols);

  return matrix;
}


int *
alloc_vect (int n_els)
{
  int *vect;
  vect = (int *) calloc (n_els, sizeof (int));
  assert (vect);
  memset (vect, 0, n_els * sizeof (int));
  return vect;
}


void
index_duration_exps (void)
{
  int i;
  PlNode *duration;
  PlOperator *plop;

  for (plop = gloaded_pl2ops; plop; plop = plop->next)
    if (plop->duration)
      {
	GpG.durative_actions_in_domain = TRUE;
	break;
      }
  if (!GpG.durative_actions_in_domain)
    return;
  for (i = 0; i < gnum_ef_conn; i++)
    {
      duration = gef_conn[i].plop->duration;
      if (duration)
	{
	  if (duration->sons->sons->connective != EQUAL_CONN)
	    {
	      printf
		("\n\nDuration inequalities are not supported by this version\n\n");
	      exit (1);
	    }
	  gef_conn[i].dur_var_index =
	    index_in_cvars_of_expression (duration->sons->sons->sons->next,
					  i);
	}
    }
}




void
make_numgoal_state (PlNode * list)
{


  PlNode *tmp;

  for (tmp = list; tmp; tmp = tmp->next)
    ggoal_state.F[ggoal_state.num_F++] =
      -index_in_cvars_of_expression (tmp, -1);

}








int
position_in_functions_table (char *str)
{
  int i;

  for (i = 0; i < gnum_functions; i++)
    {
      if (str == gfunctions[i] || strcmp (str, gfunctions[i]) == SAME)
	{
	  break;
	}
    }

  return (i == gnum_functions) ? -1 : i;
}




void
make_NumVar (NumVar * f, PlNode * n, int num_vars)
{
  int m, i;
  TokenList *t;

  f->function = position_in_functions_table (n->atom->item);
  if (f->function == -1)
    {
      printf ("\nundeclared function %s used in domain definition\n\n",
	      n->atom->item);
      exit (1);
    }

  m = 0;
  for (t = n->atom->next; t; t = t->next)
    {
      if (t->item[0] == '?')
	{
	  for (i = num_vars - 1; i > -1; i--)
	    {
	      if (lvar_names[i] == t->item
		  || strcmp (lvar_names[i], t->item) == SAME)
		{
		  break;
		}
	    }
	  if (i == -1)
	    {
	      printf
		("\nundeclared variable %s in literal %s. check input files\n\n",
		 t->item, n->atom->item);
	      exit (1);
	    }
	  if (f->function != -1
	      && lvar_types[i] != gfunctions_args_type[f->function][m]
	      && !is_subtype (lvar_types[i],
			      gfunctions_args_type[f->function][m]))
	    {
	      printf
		("\ntype of var %s does not match type of arg %d of function %s\n\n",
		 lvar_names[i], m, gfunctions[f->function]);
	      exit (1);
	    }
	  f->args[m] = ENCODE_VAR (i);
	}
      else
	{
	  if ((f->args[m] = position_in_constants_table (t->item)) == -1)
	    {
	      printf
		("\nunknown constant %s in literal %s. check input files\n\n",
		 t->item, n->atom->item);
	      exit (1);
	    }
	  if (f->function != -1
	      && !gis_member[f->
			     args[m]][gfunctions_args_type[f->function][m]])
	    {
	      printf
		("\ntype mismatch: constant %s as arg %d of %s. check input files\n\n",
		 gconstants[f->args[m]], m, gfunctions[f->function]);
	      exit (1);
	    }
	}
      m++;
    }
  if (f->function == -1)
    {
      if (m != 2)
	{
	  printf
	    ("\nfound eq - function with %d arguments. check input files\n\n",
	     m);
	  exit (1);
	}
    }
  else
    {
      if (m != gfunarity[f->function])
	{
	  printf
	    ("\nfunction %s is declared to have %d (not %d) arguments. check input files\n\n",
	     gfunctions[f->function], gfunarity[f->function], m);
	  exit (1);
	}
    }
}


void
make_VarAssign (NumVar * f, PlNode * n, int num_vars)
{
  if (n->connective != EQUAL_CONN)
    {
      printf ("\nillegal (empty) atom used in domain. check input files\n\n");
      exit (1);
    }

  if (n->connective == EQUAL_CONN)
    {
      make_NumVar (f, n->sons, num_vars);
      f->value = evaluate_exp (n->sons->next);
    }
}

void
make_compvar (CompositeNumVar * c, int c_index, NumVar * nv, int nv_index)
{
  c->operator = VARIABLE_OP;
  c->first_op = nv_index;
  c->second_op = -1;
  GCOMP_VAR_VALUE (c_index) = nv->value;
  nv->gcomp_var_index = c_index;
}



float
evaluate_exp (PlNode * n)
{
  switch (n->connective)
    {
    case NUM_EXP:
      return evaluate_exp (n->sons);
    case ATOM:
      return atof (n->atom->item);
      break;
    case MUL_CONN:
      return evaluate_exp (n->sons) * evaluate_exp (n->sons->next);
      break;
    case DIV_CONN:
      return evaluate_exp (n->sons) / evaluate_exp (n->sons->next);
      break;
    case PLUS_CONN:
      return evaluate_exp (n->sons) + evaluate_exp (n->sons->next);
      break;
    case MINUS_CONN:
      return evaluate_exp (n->sons) - evaluate_exp (n->sons->next);
      break;
    case UMINUS_CONN:
      return -evaluate_exp (n->sons);
      break;
    default:
      printf
	("\nConnective %d not yet supported in expression evaluation\n\n",
	 n->connective);
      exit (1);
      return -1;
      break;
    }
  return 0;
}

void
add_dummy_effects (PlOperator * operators)
{
  PlOperator *o;
  PlNode *e, *ef, *eff, *last;
  Bool t = FALSE;

  for (o = operators; o; o = o->next)
    {
      if (o->effects)
	{
	  t = FALSE;
	  last = NULL;

	  ef = o->effects;
	  if (ef->connective == AND)
	    ef = ef->sons;
	  for (e = ef; e; e = e->next)
	    {
	      if (!e->next)
		last = e;
	      eff = e;
	      if ((eff->connective == AT_START_CONN)
		  || (eff->connective == AT_END_CONN)
		  || (eff->connective == OVER_ALL_CONN))
		eff = eff->sons;
	      if ((eff->connective == ATOM) || (eff->connective == NOT))
		{
		  t = TRUE;
		  break;
		}
	    }
	  if (!t)
	    {
	      add_dummy (last);
	    }
	}
    }
}


void
add_dummy (PlNode * pln)
{
  if (!pln)
    {
      printf ("\n\nNull PLNODE!!!\n\n");
      exit (1);
    }
  pln->next = new_PlNode (ATOM);
  pln->next->atom = new_TokenList ();
  pln->next->atom->item = new_Token (strlen (DUMMY_PRED_STRING) + 1);
  strcpy (pln->next->atom->item, DUMMY_PRED_STRING);
  pln->next->atom->next = NULL;
}






PlNode *
cp_PlNode (PlNode * pln)
{

  PlNode *result = (PlNode *) calloc (1, sizeof (PlNode));
  CHECK_PTR (result);

  result->connective = pln->connective;


  result->atom = copy_TokenList (pln->atom);

  if (pln->sons != NULL)
    result->sons = cp_PlNode (pln->sons);
  if (pln->next != NULL)
    result->next = cp_PlNode (pln->next);

  return result;

}

void
cp_PlNode2list (PlNode * pln, PlNode ** list)
{
  PlNode *tmp;

  tmp = *list;
  *list = cp_PlNode (pln);
  (*list)->next = tmp;
}



NumVar *
new_NumVar (void)
{

  NumVar *result = (NumVar *) calloc (1, sizeof (NumVar));
  CHECK_PTR (result);

  return result;

}

CompositeNumVar *
new_CompositeNumVar (OPERATOR_TYPE op)
{

  CompositeNumVar *result =
    (CompositeNumVar *) calloc (1, sizeof (CompositeNumVar));
  CHECK_PTR (result);
  result->operator = op;

  return result;

}

SpecialFacts *
new_SpecialFacts (void)
{

  SpecialFacts *result = (SpecialFacts *) calloc (1, sizeof (SpecialFacts));
  CHECK_PTR (result);

  return result;

}

IntList *
new_IntList (void)
{

  IntList *result = (IntList *) calloc (1, sizeof (IntList));
  CHECK_PTR (result);

  return result;

}




void
set_cost_and_time_coeffs (void)
{
  int father_cvar;



  if (gcomp_var[TOTAL_TIME_FUNCTION_INDEX].affects)
    {
      GpG.temporal_plan = TRUE;
      father_cvar = gcomp_var[TOTAL_TIME_FUNCTION_INDEX].affects->item;
    }

  else
    {
      GpG.orig_weight_time = 0;
      GpG.orig_weight_cost = 1;
      return;
    }



  if (gcomp_var[father_cvar].operator == MUL_OP)
    {

      if ((gcomp_var[father_cvar].first_op == TOTAL_TIME_FUNCTION_INDEX) &&
	  (gcomp_var[gcomp_var[father_cvar].second_op].operator = FIX_NUMBER))
	{
	  GpG.orig_weight_time =
	    GCOMP_VAR_VALUE (gcomp_var[father_cvar].second_op);
	  GpG.temporal_plan = TRUE;
	}

      else
	if ((gcomp_var[father_cvar].second_op == TOTAL_TIME_FUNCTION_INDEX)
	    && (gcomp_var[gcomp_var[father_cvar].first_op].
		operator = FIX_NUMBER))
	{
	  GpG.orig_weight_time =
	    GCOMP_VAR_VALUE (gcomp_var[father_cvar].first_op);
	  GpG.temporal_plan = TRUE;
	}

      else
	{
	  GpG.orig_weight_time = 1;
	  GpG.temporal_plan = TRUE;
	}
    }

  else
    GpG.orig_weight_time = 1;



  if (gcomp_var[father_cvar].operator == MUL_OP)
    father_cvar = gcomp_var[father_cvar].affects->item;
  if (gcomp_var[father_cvar].operator == MINIMIZE_OP)
    GpG.orig_weight_cost = 0;
  else
    GpG.orig_weight_cost = 1;

  if (GpG.orig_weight_time != 1)
    GpG.temporal_plan = TRUE;

}
