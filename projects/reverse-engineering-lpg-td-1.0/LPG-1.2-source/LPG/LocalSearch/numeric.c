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
 * File: numeric.c
 * Description: Numeric values manager for the LPG planner.
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/




#include "lpg.h"
#include "numeric.h"
#include "utilities.h"
#include "LpgOutput.h"
#include "inst_utils.h"
#include "H_relaxed.h"
#include "LocalSearch.h"
#include "ActionSubgraph.h"
#include "LpgTime.h"


void
sub_affects_list (int cv_index, int *bitarray)
{
  IntList *il;
  RESET_BIT (bitarray, cv_index);
  for (il = gcomp_var[cv_index].affects; il; il = il->next)
    {
      if (GET_BIT (bitarray, il->item))
	{
	  RESET_BIT (bitarray, il->item);
	  sub_affects_list (il->item, bitarray);
	}
    }
}


void
search_for_dynamic_vals_in_exp (int cvar_ind, DescNumEff * descnumeff)
{
  CompositeNumVar *cv;

  cv = &gcomp_var[cvar_ind];


  if ((cv->first_op != -1) && (!GET_BIT (gis_inertial, cv->first_op)))
    {

      descnumeff->rvals[descnumeff->num_rvals++] = cv->first_op;
      if (descnumeff->num_rvals >= MAX_R_VALS)
	{
	  printf ("\n\nMax number of Rvals reached; increase max_r_vals\n\n");
	  exit (1);
	}

      if (cvar_ind != cv->first_op)
	search_for_dynamic_vals_in_exp (cv->first_op, descnumeff);
    }

  if ((cv->second_op != -1) && (!GET_BIT (gis_inertial, cv->second_op)))
    {

      descnumeff->rvals[descnumeff->num_rvals++] = cv->second_op;
      if (descnumeff->num_rvals >= MAX_R_VALS)
	{
	  printf ("\n\nMax number of Rvals reached; increase max_r_vals\n\n");
	  exit (1);
	}

      if (cvar_ind != cv->second_op)
	search_for_dynamic_vals_in_exp (cv->second_op, descnumeff);
    }

}


void
create_descnum_for_numeff_of (DescNumEff * descnumeff, int cvar_index,
			      int ef_conn, Bool is_at_start)
{
  CompositeNumVar *cv;

  cv = &gcomp_var[-cvar_index];

  descnumeff->index = -cvar_index;

  descnumeff->lval = cv->first_op;

  descnumeff->is_at_start = is_at_start;

  search_for_dynamic_vals_in_exp (cv->second_op, descnumeff);
}



void
create_descnumeff_of_efconn (int index)
{
  int ef;
  int num_numeric_effects = 0;
  int n_ef = 0;


  for (ef = 0; ef < gef_conn[index].num_A; ef++)

    if (gef_conn[index].A[ef] < 0)
      num_numeric_effects++;

  if (gef_conn[index].sf)
    for (ef = 0; ef < gef_conn[index].sf->num_A_start; ef++)

      if (gef_conn[index].sf->A_start[ef] < 0)
	num_numeric_effects++;

  gef_conn[index].num_numeric_effects = num_numeric_effects;

  if (num_numeric_effects == 0)
    return;

  gef_conn[index].numeric_effects =
    (DescNumEff *) calloc (num_numeric_effects, sizeof (DescNumEff));


  for (ef = 0; ef < gef_conn[index].num_A; ef++)
    if (gef_conn[index].A[ef] < 0)
      create_descnum_for_numeff_of (&gef_conn[index].numeric_effects[n_ef++],
				    gef_conn[index].A[ef], index, FALSE);
  if (gef_conn[index].sf)
    for (ef = 0; ef < gef_conn[index].sf->num_A_start; ef++)
      if (gef_conn[index].sf->A_start[ef] < 0)
	create_descnum_for_numeff_of (&gef_conn[index].
				      numeric_effects[n_ef++],
				      gef_conn[index].sf->A_start[ef], index,
				      TRUE);
}


void
create_descnumeff_of_efconns (void)
{
  int i;



  for (i = 0; i < gnum_ef_conn; i++)
    create_descnumeff_of_efconn (i);
}





void
set_modified_var (int act_pos, int level, int num_eff)
{

  if (gef_conn[act_pos].numeric_effects[num_eff].is_at_start)
    SET_BIT (vectlevel[level]->numeric->modified_vars_start,
	     gef_conn[act_pos].numeric_effects[num_eff].lval);
  else
    SET_BIT (vectlevel[level]->numeric->modified_vars_end,
	     gef_conn[act_pos].numeric_effects[num_eff].lval);
}



void
apply_numeric_effect_at_end (int act_pos, int level, int num_eff)
{
  DescNumEff *numeric_effect;
  int num_numeric_effs;


  numeric_effect = &gef_conn[act_pos].numeric_effects[num_eff];
  num_numeric_effs = gef_conn[act_pos].num_numeric_effects;

  eval_comp_var_non_recursive (numeric_effect->index,
			       vectlevel[level + 1]->numeric->values,
			       vectlevel[level + 1]->numeric->values,
			       level, level + 1);


  set_modified_var (act_pos, level + 1, num_eff);
}



void
set_usedvars_array (int act_pos, int level)
{
  int i, j;
  DescNumEff *numeric_effs;
  int num_numeric_effs;
  int *usedvars = vectlevel[level]->numeric->used_vars;


  numeric_effs = gef_conn[act_pos].numeric_effects;
  num_numeric_effs = gef_conn[act_pos].num_numeric_effects;

  if (numeric_effs)

    for (i = 0; i < num_numeric_effs; i++)

      for (j = 0; j < numeric_effs[i].num_rvals; j++)
	SET_BIT (usedvars, numeric_effs[i].rvals[j]);
}



void
apply_numeric_effects_of_action (int act_pos, int level)
{
  int i;
  DescNumEff *numeric_effs;
  int num_numeric_effs;


  assert (0 <= act_pos);
  assert (act_pos < gnum_ef_conn);
  assert (0 <= level);
  assert (level < GpG.curr_plan_length);
  assert (vectlevel[level]);
  assert (vectlevel[level]->numeric);


  insert_values_unsup_num_fact (0, -1, -1);


  numeric_effs = gef_conn[act_pos].numeric_effects;
  num_numeric_effs = gef_conn[act_pos].num_numeric_effects;

  set_usedvars_array (act_pos, level);

  if (numeric_effs)
    {
      for (i = 0; i < num_numeric_effs; i++)
	if (numeric_effs[i].is_at_start)
	  apply_numeric_effect_at_end (act_pos, level, i);
      refresh_cvars (level + 1);
      for (i = 0; i < num_numeric_effs; i++)
	if (!numeric_effs[i].is_at_start)
	  apply_numeric_effect_at_end (act_pos, level, i);

      refresh_cvars (level + 1);


      propagate_cvars (level + 1, GpG.curr_plan_length);
    }


  insert_values_unsup_num_fact (2, -1, -1);
}


void
apply_numeric_effect_at_start (int act_pos, int level, int num_eff)
{
  DescNumEff *numeric_effect;
  int num_numeric_effs;


  numeric_effect = &gef_conn[act_pos].numeric_effects[num_eff];
  num_numeric_effs = gef_conn[act_pos].num_numeric_effects;

  vectlevel[level]->effect_at_start_present = TRUE;


  eval_comp_var_non_recursive (numeric_effect->index,
			       vectlevel[level]->numeric->values,
			       vectlevel[level]->numeric->values_after_start,
			       level, level);

  memcpy (vectlevel[level + 1]->numeric->values,
	  vectlevel[level]->numeric->values_after_start,
	  gnum_comp_var * sizeof (float));
  refresh_cvars (level + 1);

  set_modified_var (act_pos, level + 1, num_eff);


}


void
remove_numeric_effects_of_action (int act_pos, int level)
{
  int l1;
  DescNumEff *numeric_effs;
  int num_numeric_effs;


  assert (0 <= act_pos);
  assert (act_pos < gnum_ef_conn);
  assert (0 <= level);
  assert (level < GpG.curr_plan_length);
  assert (vectlevel[level]);
  assert (vectlevel[level]->numeric);


  insert_values_unsup_num_fact (0, -1, -1);

  l1 = level + 1;
  numeric_effs = gef_conn[act_pos].numeric_effects;
  num_numeric_effs = gef_conn[act_pos].num_numeric_effects;

  memset (vectlevel[level]->numeric->used_vars, 0,
	  sizeof (int) * gnum_block_compvar);
  memset (vectlevel[level]->numeric->modified_vars_start, 0,
	  sizeof (int) * gnum_block_compvar);
  memset (vectlevel[level]->numeric->modified_vars_end, 0,
	  sizeof (int) * gnum_block_compvar);


  if (numeric_effs)
    {

      copy_num_values_from_level_to_level (level, l1, TRUE);
      memcpy (vectlevel[level + 1]->numeric->values_after_start,
	      vectlevel[level]->numeric->values_after_start,
	      gnum_comp_var * sizeof (float));

      propagate_cvars (level + 1, GpG.curr_plan_length);
    }

  insert_values_unsup_num_fact (2, -1, -1);

}



void
eval_comp_var_non_recursive (int cv_index, float *in_vect, float *out_vect,
			     int in_level, int out_level)
{
  int first_op = gcomp_var[cv_index].first_op;
  int second_op = gcomp_var[cv_index].second_op;
  float old_value;


  switch (gcomp_var[cv_index].operator)
    {
    case INCREASE_OP:
      out_vect[first_op] = in_vect[first_op] + in_vect[second_op];
      break;
    case DECREASE_OP:
      out_vect[first_op] = in_vect[first_op] - in_vect[second_op];
      break;
    case SCALE_UP_OP:
      out_vect[first_op] = in_vect[first_op] * in_vect[second_op];
      break;
    case SCALE_DOWN_OP:
      out_vect[first_op] = in_vect[first_op] / in_vect[second_op];
      break;
    case ASSIGN_OP:
      out_vect[first_op] = in_vect[second_op];
      break;
    case MUL_OP:
      out_vect[cv_index] = in_vect[first_op] * in_vect[second_op];
      break;
    case DIV_OP:
      out_vect[cv_index] = in_vect[first_op] / in_vect[second_op];
      break;
    case PLUS_OP:
      out_vect[cv_index] = in_vect[first_op] + in_vect[second_op];
      break;
    case MINUS_OP:
      out_vect[cv_index] = in_vect[first_op] - in_vect[second_op];
      break;
    case UMINUS_OP:
      out_vect[cv_index] = -in_vect[first_op];
      break;
    case LESS_THAN_OP:
      old_value = out_vect[cv_index];
      out_vect[cv_index] = (float) (in_vect[first_op] < in_vect[second_op]);

      if ((old_value > 0.5) && (out_vect[cv_index] < 0.5))
	{
	  insert_values_unsup_num_fact (1, cv_index, out_level);

	}


      if ((old_value < 0.5) && (out_vect[cv_index] > 0.5))
	{
	  insert_values_unsup_num_fact (1, cv_index, out_level);
	}
      break;
    case LESS_THAN_OR_EQUAL_OP:
      old_value = out_vect[cv_index];
      out_vect[cv_index] = (float) (in_vect[first_op] <= in_vect[second_op]);

      if ((old_value > 0.5) && (out_vect[cv_index] < 0.5))
	{
	  insert_values_unsup_num_fact (1, cv_index, out_level);
	}


      if ((old_value < 0.5) && (out_vect[cv_index] > 0.5))
	{
	  insert_values_unsup_num_fact (1, cv_index, out_level);
	}
      break;
    case EQUAL_OP:
      old_value = out_vect[cv_index];
      out_vect[cv_index] =
	(float) (in_vect[first_op] - in_vect[second_op]) <= MAX_APPROX;

      if ((old_value > 0.5) && (out_vect[cv_index] < 0.5))
	{
	  insert_values_unsup_num_fact (1, cv_index, out_level);

	}


      if ((old_value < 0.5) && (out_vect[cv_index] > 0.5))
	{
	  insert_values_unsup_num_fact (1, cv_index, out_level);
	}
      break;
    case GREATER_THAN_OP:
      old_value = out_vect[cv_index];
      out_vect[cv_index] = (float) (in_vect[first_op] > in_vect[second_op]);

      if ((old_value > 0.5) && (out_vect[cv_index] < 0.5))
	{
	  insert_values_unsup_num_fact (1, cv_index, out_level);

	}


      if ((old_value < 0.5) && (out_vect[cv_index] > 0.5))
	{
	  insert_values_unsup_num_fact (1, cv_index, out_level);
	}
      break;
    case GREATER_OR_EQUAL_OP:
      old_value = out_vect[cv_index];
      out_vect[cv_index] = (float) (in_vect[first_op] >= in_vect[second_op]);

      if ((old_value > 0.5) && (out_vect[cv_index] < 0.5))
	{
	  insert_values_unsup_num_fact (1, cv_index, out_level);

	}


      if ((old_value < 0.5) && (out_vect[cv_index] > 0.5))
	{
	  insert_values_unsup_num_fact (1, cv_index, out_level);
	}
      break;
    default:
      break;
    }
}


void
add_affects_list (int cv_index, int *bitarray)
{
  IntList *il;
  for (il = gcomp_var[cv_index].affects; il; il = il->next)
    SET_BIT (bitarray, il->item);
}


void
refresh_cvars (int level)
{
  int *modifieds, i;


  modifieds = (int *) calloc (gnum_block_compvar, sizeof (int));


  if (level <= 0)
    {
      printf ("ERR: refresh_cvars(): level can't be <= 0");
      exit (1);
    }


  for (i = 0; i < gnum_block_compvar; i++)
    modifieds[i] =
      vectlevel[level]->numeric->modified_vars_start[i] | vectlevel[level]->
      numeric->modified_vars_end[i];


  for (i = 0; i < gnum_comp_var; i++)
    if (GET_BIT (modifieds, i))
      {
	switch (gcomp_var[i].operator)
	  {
	  case INCREASE_OP:
	  case DECREASE_OP:
	  case SCALE_UP_OP:
	  case SCALE_DOWN_OP:
	  case ASSIGN_OP:
	    break;
	  default:



	    eval_comp_var_non_recursive (i,
					 vectlevel[level]->numeric->values,
					 vectlevel[level]->numeric->values,
					 level, level);


	    add_affects_list (i, modifieds);
	  }
      }
  free (modifieds);
}


void
which_effects (int level, int *to_propagate, int *to_apply)
{
  int i, j;
  int action_pos = vectlevel[level]->action.position;
  DescNumEff *num_eff;

  if (action_pos < 0)
    return;

  for (i = 0; i < gef_conn[action_pos].num_numeric_effects; i++)
    {
      num_eff = &(gef_conn[action_pos].numeric_effects[i]);

      if (GET_BIT (to_propagate, num_eff->lval))
	{
	  SET_BIT (to_apply, i);

	  continue;
	}

      for (j = 0; j < num_eff->num_rvals; j++)
	if (GET_BIT (to_propagate, num_eff->rvals[j]))
	  {
	    SET_BIT (to_apply, i);
	    break;
	  }
    }
}


void
propagate_cvars (int level_from, int level_to)
{
  int level, i, ind;
  int *propagation;
  int *to_be_applied;
  int action_pos;
  float dur_before = 0.0, dur_after, dur_act_follow;


  if ((level_from < 0) || (level_to < 0) || (level_to < level_from))
    {
      printf ("\n\nIncorrect parameters\nlevel_from=%d; level_to=%d\n\n",
	      level_from, level_to);
      exit (1);
    }

  propagation = (int *) calloc (gnum_block_compvar, sizeof (int));

  to_be_applied = (int *) calloc (MAX_NUM_EFFS / 32 + 1, sizeof (int));
  for (level = level_from; level < level_to; level++)
    {


      if (vectlevel[level]->action.being_removed)
	break;

      action_pos = vectlevel[level]->action.position;
      if (action_pos < 0)
	{
	  copy_num_values_from_level_to_level (level, level + 1, TRUE);
	  continue;
	}

      memset (to_be_applied, 0, (MAX_NUM_EFFS / 32 + 1) * sizeof (int));

      for (i = 0; i < gnum_block_compvar; i++)
	{

	  propagation[i] =
	    vectlevel[level]->numeric->
	    modified_vars_start[i] | vectlevel[level]->numeric->
	    modified_vars_end[i];


	  propagation[i] =
	    propagation[i] | (vectlevel[level + 1]->numeric->
			      used_vars[i] | vectlevel[level +
						       1]->numeric->
			      modified_vars_start[i] | vectlevel[level +
								 1]->numeric->
			      modified_vars_end[i]);
	}


      which_effects (level, propagation, to_be_applied);

      memcpy (vectlevel[level + 1]->numeric->values,
	      vectlevel[level]->numeric->values,
	      gnum_comp_var * sizeof (float));
      memcpy (vectlevel[level]->numeric->values_after_start,
	      vectlevel[level]->numeric->values,
	      gnum_comp_var * sizeof (float));

      if (level < level_to - 1)
	if (GET_ACTION_POSITION_OF_LEVEL (level + 1) >= 0 &&
	    CONVERT_ACTION_TO_VERTEX (GET_ACTION_POSITION_OF_LEVEL
				      (level + 1))->duration_rvals != NULL)
	  dur_before =
	    get_action_time (vectlevel[level + 1]->action.position,
			     level + 1);

      for (i = 0; i < gef_conn[action_pos].num_numeric_effects; i++)
	if (TRUE || GET_BIT (to_be_applied, i))
	  {
	    if (gef_conn[action_pos].numeric_effects[i].is_at_start)
	      apply_numeric_effect_at_start (action_pos, level, i);
	    else
	      apply_numeric_effect_at_end (action_pos, level, i);
	  }



      refresh_cvars (level + 1);

      if (level < level_to - 1)
	if (GET_ACTION_POSITION_OF_LEVEL (level + 1) >= 0 &&
	    CONVERT_ACTION_TO_VERTEX (GET_ACTION_POSITION_OF_LEVEL
				      (level + 1))->duration_rvals != NULL)
	  {
	    dur_after =
	      get_action_time (vectlevel[level + 1]->action.position,
			       level + 1);
	    if (abs (dur_after - dur_before) > MIN_DELTA_TIME
		&& GpG.temporal_plan)
	      {
		insert_propagation_list (GET_ACTION_OF_LEVEL (level));

		if (GpG.constraint_type != 0)
		  for (ind = 0; ind < num_act_ord; ind++)
		    if (mat_ord[GET_ACTION_OF_LEVEL (level)->ord_pos][ind] ==
			EA_EB__SA_SB)
		      {
			dur_act_follow =
			  get_action_time (act_ord_vect[ind]->position,
					   *act_ord_vect[ind]->level);
			if ((dur_before < dur_act_follow
			     && dur_after > dur_act_follow)
			    || (dur_before > dur_act_follow
				&& dur_after < dur_act_follow))
			  insert_propagation_list (act_ord_vect[ind]);
		      }
	      }

	    if (abs (dur_after - dur_before) > MIN_DELTA_TIME
		&& GpG.temporal_plan == 0)
	      printf
		("\nWarning! This problem should be restarted with -temporal 1 option\n");
	  }
    }

  free (propagation);
  free (to_be_applied);
}


Bool
is_num_prec_satisfied (int cvar, int level)
{
  if (cvar < 0)
    cvar = -cvar;

  return (abs (vectlevel[level]->numeric->values[cvar] - 1) < MAX_APPROX);
}


Bool
is_num_prec_satisfied_in_common_level (int cvar)
{
  cvar = abs (cvar);
  assert (cvar >= 0);

  if (cvar >= max_num_value)
    {
      max_num_value += MAX_NUM_INC;

      gcomp_var =
	(CompositeNumVar *) realloc (gcomp_var,
				     max_num_value *
				     sizeof (CompositeNumVar));
      memset (&gcomp_var[max_num_value - MAX_NUM_INC], 0,
	      MAX_NUM_INC * sizeof (CompositeNumVar));

      gcomp_var_value =
	(float *) realloc (gcomp_var_value, max_num_value * sizeof (float));
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

  if (abs (Hvar.common_values[cvar] - 1) < MAX_APPROX)
    {

      if (DEBUG4)
	{
	  printf ("\n Supported num prec in common level");
	  print_cvar_tree (cvar, 0);
	}

    }

  else
    {
      if (DEBUG4)
	{
	  printf ("\n Not supported num prec in common level");
	  print_cvar_tree (cvar, 0);
	}
    }
  return (abs (Hvar.common_values[cvar] - 1) < MAX_APPROX);

}






void
apply_numeric_effect_in_common_level (int num_eff)
{
  int i;
  int first_op;
  int second_op;
  float *out_vect = Hvar.common_values;
  float *in_vect = Hvar.common_values;

  num_eff = -num_eff;
  assert (num_eff >= 0);
  first_op = gcomp_var[num_eff].first_op;
  second_op = gcomp_var[num_eff].second_op;

  switch (gcomp_var[num_eff].operator)
    {
    case INCREASE_OP:
      out_vect[first_op] = in_vect[first_op] + in_vect[second_op];
      break;
    case DECREASE_OP:
      out_vect[first_op] = in_vect[first_op] - in_vect[second_op];
      break;
    case SCALE_UP_OP:
      out_vect[first_op] = in_vect[first_op] * in_vect[second_op];
      break;
    case SCALE_DOWN_OP:
      out_vect[first_op] = in_vect[first_op] / in_vect[second_op];
      break;
    case ASSIGN_OP:
      out_vect[first_op] = in_vect[second_op];
      break;
    case MUL_OP:
      out_vect[num_eff] = in_vect[first_op] * in_vect[second_op];
      break;
    case DIV_OP:
      out_vect[num_eff] = in_vect[first_op] / in_vect[second_op];
      break;
    case PLUS_OP:
      out_vect[num_eff] = in_vect[first_op] + in_vect[second_op];
      break;
    case MINUS_OP:
      out_vect[num_eff] = in_vect[first_op] - in_vect[second_op];
      break;
    case UMINUS_OP:
      out_vect[num_eff] = -in_vect[first_op];
      break;
    case LESS_THAN_OP:
      out_vect[num_eff] = (float) (in_vect[first_op] < in_vect[second_op]);
      break;
    case LESS_THAN_OR_EQUAL_OP:
      out_vect[num_eff] = (float) (in_vect[first_op] <= in_vect[second_op]);
      break;
    case EQUAL_OP:
      out_vect[num_eff] =
	(float) (in_vect[first_op] - in_vect[second_op]) <= MAX_APPROX;
      break;
    case GREATER_THAN_OP:
      out_vect[num_eff] = (float) (in_vect[first_op] > in_vect[second_op]);
      break;
    case GREATER_OR_EQUAL_OP:
      out_vect[num_eff] = (float) (in_vect[first_op] >= in_vect[second_op]);
      break;
    default:
      printf ("\nWarning7448\n");
      break;
    }


  for (i = 0; i < gnum_comp_var; i++)
    {
      first_op = gcomp_var[i].first_op;
      second_op = gcomp_var[i].second_op;
      switch (gcomp_var[i].operator)
	{
	case INCREASE_OP:
	case DECREASE_OP:
	case SCALE_UP_OP:
	case SCALE_DOWN_OP:
	case ASSIGN_OP:
	case FIX_NUMBER:
	case VARIABLE_OP:
	case MINIMIZE_OP:
	case MAXIMIZE_OP:
	  break;
	case MUL_OP:
	  out_vect[i] = in_vect[first_op] * in_vect[second_op];
	  break;
	case DIV_OP:
	  out_vect[i] = in_vect[first_op] / in_vect[second_op];
	  break;
	case PLUS_OP:
	  out_vect[i] = in_vect[first_op] + in_vect[second_op];
	  break;
	case MINUS_OP:
	  out_vect[i] = in_vect[first_op] - in_vect[second_op];
	  break;
	case UMINUS_OP:
	  out_vect[i] = -in_vect[first_op];
	  break;
	case LESS_THAN_OP:
	  if (out_vect[i] > 0.5)
	    break;
	  out_vect[i] = (float) (in_vect[first_op] < in_vect[second_op]);
	  break;
	case LESS_THAN_OR_EQUAL_OP:
	  if (out_vect[i] > 0.5)
	    break;
	  out_vect[i] = (float) (in_vect[first_op] <= in_vect[second_op]);
	  break;
	case EQUAL_OP:
	  if (out_vect[i] > 0.5)
	    break;
	  out_vect[i] = (float) (in_vect[first_op] - in_vect[second_op]) <=
	    MAX_APPROX;
	  break;
	case GREATER_THAN_OP:
	  if (out_vect[i] > 0.5)
	    break;
	  out_vect[i] = (float) (in_vect[first_op] > in_vect[second_op]);
	  break;
	case GREATER_OR_EQUAL_OP:
	  if (out_vect[i] > 0.5)
	    break;
	  out_vect[i] = (float) (in_vect[first_op] >= in_vect[second_op]);
	  break;
	default:
	  printf ("\nWarning37448\n");
	  break;
	}
    }

}




float
try_num_eff_in_level (int cv_index, float *in_vect, float *out_vect)
{
  CompositeNumVar *cv = &gcomp_var[cv_index];
  int first_op = cv->first_op;
  int second_op = cv->second_op;
  float tmp;

  assert (cv_index >= 0);
  first_op = gcomp_var[cv_index].first_op;
  second_op = gcomp_var[cv_index].second_op;

  switch (cv->operator)
    {
    case FIX_NUMBER:
    case VARIABLE_OP:
      out_vect[cv_index] = in_vect[cv_index];
      return out_vect[cv_index];
      break;
    case MUL_OP:
      out_vect[cv_index] =
	try_num_eff_in_level (first_op, in_vect,
			      out_vect) * try_num_eff_in_level (second_op,
								in_vect,
								out_vect);
      return out_vect[cv_index];
      break;
    case DIV_OP:
      tmp = try_num_eff_in_level (second_op, in_vect, out_vect);
      if (tmp == 0)
	{
	  printf ("\n\nWARNING: Division by zero in try_num_eff\n\n");
	  out_vect[cv_index] = 0;
	  return out_vect[cv_index];
	}
      out_vect[cv_index] =
	try_num_eff_in_level (first_op, in_vect, out_vect) / tmp;
      return out_vect[cv_index];
    case PLUS_OP:
      out_vect[cv_index] =
	try_num_eff_in_level (first_op, in_vect,
			      out_vect) + try_num_eff_in_level (second_op,
								in_vect,
								out_vect);
      return out_vect[cv_index];
      break;
    case MINUS_OP:
      out_vect[cv_index] =
	try_num_eff_in_level (first_op, in_vect,
			      out_vect) - try_num_eff_in_level (second_op,
								in_vect,
								out_vect);
      return out_vect[cv_index];
      break;
    case UMINUS_OP:
      out_vect[cv_index] =
	-try_num_eff_in_level (first_op, in_vect, out_vect);
      return out_vect[cv_index];
      break;
    case INCREASE_OP:
      out_vect[first_op] +=
	try_num_eff_in_level (second_op, in_vect, out_vect);
      return out_vect[first_op];
      break;
    case DECREASE_OP:
      out_vect[first_op] -=
	try_num_eff_in_level (second_op, in_vect, out_vect);
      return out_vect[first_op];
      break;
    case SCALE_UP_OP:
      out_vect[first_op] *=
	try_num_eff_in_level (second_op, in_vect, out_vect);
      return out_vect[first_op];
      break;
    case SCALE_DOWN_OP:
      out_vect[first_op] /= try_num_eff_in_level (second_op, in_vect,
						  out_vect);
      return out_vect[first_op];
      break;
    case ASSIGN_OP:
      out_vect[first_op] =
	try_num_eff_in_level (second_op, in_vect, out_vect);
      return out_vect[first_op];
      break;
    default:
      printf ("\n\nnot considered\n\n");
      exit (2);
      break;
    }

  return 0;
}




IntList *
free_intlist (IntList * ilist)
{
  IntList *nextil;

  while (ilist)
    {
      nextil = ilist->next;
      free (ilist);
      ilist = nextil;
    }
  return (IntList *) NULL;
}


void
set_numeric_flag ()
{
  int i, j;

  for (i = 0; i < gnum_ef_conn; i++)
    {

      for (j = 0; j < gef_conn[i].num_PC; j++)
	if (gef_conn[i].PC[j] < 0)
	  break;
      if (j != gef_conn[i].num_PC)
	{
	  gef_conn[i].is_numeric = TRUE;
	  GpG.numeric_precs_present = TRUE;
	  continue;
	}

      for (j = 0; j < gef_conn[i].num_A; j++)
	if (gef_conn[i].A[j] < 0)
	  break;
      if (j != gef_conn[i].num_A)
	{
	  gef_conn[i].is_numeric = TRUE;
	  continue;
	}

      if (gef_conn[i].sf)
	{

	  for (j = 0; j < gef_conn[i].sf->num_PC_overall; j++)
	    if (gef_conn[i].sf->PC_overall[j] < 0)
	      break;
	  if (j != gef_conn[i].sf->num_PC_overall)
	    {
	      GpG.numeric_precs_present = TRUE;
	      gef_conn[i].is_numeric = TRUE;
	      continue;
	    }

	  for (j = 0; j < gef_conn[i].sf->num_PC_end; j++)
	    if (gef_conn[i].sf->PC_end[j] < 0)
	      break;
	  if (j != gef_conn[i].sf->num_PC_end)
	    {
	      GpG.numeric_precs_present = TRUE;
	      gef_conn[i].is_numeric = TRUE;
	      continue;
	    }

	  for (j = 0; j < gef_conn[i].sf->num_A_start; j++)
	    if (gef_conn[i].sf->A_start[j] < 0)
	      break;
	  if (j != gef_conn[i].sf->num_A_start)
	    {
	      gef_conn[i].is_numeric = TRUE;
	      continue;
	    }
	}
    }
}


void
copy_num_values_from_level_to_level (int level_from, int level_to,
				     Bool check_variations)
{
  int i;
  float old_value, new_value;

  if (check_variations)
    {
      for (i = 0; i < gnum_comp_var; i++)
	{


	  if (abs
	      (vectlevel[level_to]->numeric->values[i] -
	       vectlevel[level_from]->numeric->values[i]) > MAX_APPROX)
	    {
	      switch (gcomp_var[i].operator)
		{
		case LESS_THAN_OP:
		case LESS_THAN_OR_EQUAL_OP:
		case EQUAL_OP:
		case GREATER_THAN_OP:
		case GREATER_OR_EQUAL_OP:
		  old_value = vectlevel[level_to]->numeric->values[i];
		  new_value = vectlevel[level_from]->numeric->values[i];

		  if ((old_value > 0.5) && (new_value < 0.5))
		    {
		      insert_values_unsup_num_fact (1, i, level_to);

		    }
		  break;
		default:
		  break;
		}
	      SET_BIT (vectlevel[level_to]->numeric->modified_vars_end, i);
	    }
	  if (abs
	      (vectlevel[level_to]->numeric->values_after_start[i] -
	       vectlevel[level_from]->numeric->values_after_start[i]) >
	      MAX_APPROX)
	    {
	      switch (gcomp_var[i].operator)
		{
		case LESS_THAN_OP:
		case LESS_THAN_OR_EQUAL_OP:
		case EQUAL_OP:
		case GREATER_THAN_OP:
		case GREATER_OR_EQUAL_OP:
		  old_value = vectlevel[level_to]->numeric->values[i];
		  new_value = vectlevel[level_from]->numeric->values[i];

		  if ((old_value > 0.5) && (new_value < 0.5))
		    {
		      insert_values_unsup_num_fact (1, i, level_to);

		    }
		  break;
		default:
		  break;
		}

	      SET_BIT (vectlevel[level_to]->numeric->modified_vars_start, i);
	    }
	}
    }

  memcpy (vectlevel[level_from]->numeric->values_after_start,
	  vectlevel[level_from]->numeric->values,
	  gnum_comp_var * sizeof (float));
  memcpy (vectlevel[level_to]->numeric->values,
	  vectlevel[level_from]->numeric->values,
	  gnum_comp_var * sizeof (float));
  memcpy (vectlevel[level_to]->numeric->values_after_start,
	  vectlevel[level_from]->numeric->values_after_start,
	  gnum_comp_var * sizeof (float));
  memset (vectlevel[level_from]->numeric->w_is_goal, 0,
	  gnum_comp_var * sizeof (short));
  memcpy (vectlevel[level_to]->numeric->w_is_used,
	  vectlevel[level_from]->numeric->w_is_used,
	  sizeof (short) * gnum_comp_var);

}




IntList *
copy_intlist (IntList * src)
{
  IntList *tmpil;
  IntList *ret;
  IntList *el;
  IntList *prev = NULL;

  ret = NULL;
  if (src == NULL)
    return NULL;
  for (tmpil = src; tmpil; tmpil = tmpil->next)
    {
      el = new_IntList ();
      if (tmpil == src)
	ret = el;
      el->item = tmpil->item;
      if (prev)
	prev->next = el;
      prev = el;
    }
  return ret;
}


void
add_intlist_to_intlist (IntList * to_queue, IntList ** first_list)
{
  IntList *tmpil;
  IntList *copy_il;

  copy_il = copy_intlist (to_queue);
  if (!*first_list)
    {
      *first_list = copy_il;
      return;
    }


  for (tmpil = *first_list; tmpil; tmpil = tmpil->next)
    if (!tmpil->next)
      {
	tmpil->next = copy_il;
	break;
      }
}

void
make_false_all_checks_on_not_init (void)
{
  int i;
  IntList *il;
  IntList *tmpil;

  CompositeNumVar *cv;
  for (i = 0; i < gnum_comp_var; i++)
    {
      cv = &gcomp_var[i];
      if ((cv->operator== VARIABLE_OP) && (cv->first_op == -1))
	{
	  il = copy_intlist (cv->affects);

	  for (tmpil = il; tmpil; tmpil = tmpil->next)
	    add_intlist_to_intlist (gcomp_var[tmpil->item].affects, &il);
	  for (tmpil = il; tmpil; tmpil = tmpil->next)
	    {
	      switch (gcomp_var[tmpil->item].operator )
		{
		case EQUAL_OP:
		case GREATER_THAN_OP:
		case GREATER_OR_EQUAL_OP:
		case LESS_THAN_OP:
		case LESS_THAN_OR_EQUAL_OP:
		  gcomp_var[tmpil->item].operator= FIX_NUMBER;
		  gcomp_var[tmpil->item].first_op = -1;
		  gcomp_var[tmpil->item].second_op = -1;
		  GCOMP_VAR_VALUE (tmpil->item) = 0.0;
		  gcomp_var[tmpil->item].affects =
		    free_intlist (gcomp_var[tmpil->item].affects);

		  SET_BIT (gis_inertial, tmpil->item);
		  break;
		default:
		  break;
		}
	    }
	}
    }
}





void
insert_unsup_numeric_fact (int cv_index, int level)
{
  if (GET_BIT (gis_inertial, cv_index))
    {
      if (DEBUG3)
	{
	  printf ("\nERROR: inertial fact insert in unsup list\n");
	  print_cvar_tree (cv_index, level);
	  printf ("\n");
	  fflush (stdout);
	}
    }

  if (DEBUG4)
    {
      printf ("\n New False Numeric Fact: \n ");
      print_cvar_tree (cv_index, level);
      printf ("\n level %d, position %d", level, cv_index);
    }

  if (vectlevel[level]->numeric->false_position[cv_index] >= 0)
    {
      return;
    }

  if (unsup_num_fact[GpG.num_false_num_fa] == NULL)
    unsup_num_fact[GpG.num_false_num_fa] =
      (constraints_list) malloc (sizeof (constraints));
  unsup_num_fact[GpG.num_false_num_fa]->fact = cv_index;
  unsup_num_fact[GpG.num_false_num_fa]->action = -1;
  unsup_num_fact[GpG.num_false_num_fa]->constraint_type = C_T_UNSUP_NUM_FACT;
  unsup_num_fact[GpG.num_false_num_fa]->level = &(vectlevel[level]->level);
  vectlevel[level]->numeric->false_position[cv_index] = GpG.num_false_num_fa;

  GpG.num_false_num_fa++;

  if (GpG.num_false_num_fa >= MAX_FALSE)
    {
      exit (1);
    }


}




float
get_numeric_fact_cost (inform_list Fact, node_cost_list n_cost)
{
  return -100.0;
}

void
set_numeric_fact_cost ()
{
}


void
remove_unsup_num_fact (int position)
{
  int removed_fct;
  int removed_fct_level;


  if (GpG.num_false_num_fa == 0)
    return;
  if (position < 0)
    return;
  assert (position < GpG.num_false_num_fa);
  if (!unsup_num_fact[position])
    return;
  removed_fct = unsup_num_fact[position]->fact;
  removed_fct_level = *unsup_num_fact[position]->level;



  if (DEBUG4)
    {
      printf ("\n New True Numeric Fact: \n ");
      print_cvar_tree (position, removed_fct_level);
      printf ("\n level %d, position %d", removed_fct_level, position);
    }


  if (vectlevel[removed_fct_level]->numeric->false_position[removed_fct] ==
      -1)
    return;

  vectlevel[removed_fct_level]->numeric->false_position[removed_fct] = -1;


  if ((GpG.num_false_num_fa > 1) && (position < GpG.num_false_num_fa - 1))
    {
      unsup_num_fact[position]->fact =
	unsup_num_fact[GpG.num_false_num_fa - 1]->fact;
      vectlevel[*(unsup_num_fact[GpG.num_false_num_fa - 1]->level)]->numeric->
	false_position[unsup_num_fact[GpG.num_false_num_fa - 1]->fact] =
	position;
      unsup_num_fact[position]->action =
	unsup_num_fact[GpG.num_false_num_fa - 1]->action;
      unsup_num_fact[position]->constraint_type =
	unsup_num_fact[GpG.num_false_num_fa - 1]->constraint_type;
      unsup_num_fact[position]->level =
	unsup_num_fact[GpG.num_false_num_fa - 1]->level;
    }

  GpG.num_false_num_fa--;
  assert (GpG.num_false_num_fa >= 0);

}



int
fix_unsup_num_fact (constraints_list unsup_numeric_fact)
{
  int num, num_min, num_neg, choice;
  float best = 0.0;
  EfConn *act;
  int level;

  assert (unsup_numeric_fact != NULL);

  if (DEBUG2)
    {
      printf
	("\n\n###INC CHOICE:\n  Unsupported numeric fact: position %d, level %d \n  fact name : ",
	 unsup_numeric_fact->fact, *unsup_numeric_fact->level);
      print_cvar_tree (unsup_numeric_fact->fact, *unsup_numeric_fact->level);
      printf ("\n");
    }

  if (DEBUG3)
    {
      int is_not_satisfied;
      is_not_satisfied =
	!is_num_prec_satisfied (unsup_numeric_fact->fact,
				*unsup_numeric_fact->level);
      if (!is_not_satisfied)
	print_num_levels_and_actions ();
    }

  num = choose_num_actions (unsup_numeric_fact, TRUE);

  if (num <= 0)
    {
      if (GpG.local == 1)
	return -1;
      else
	{
	  choose_random_act_cost ();
	  return (FALSE);
	}
    }





  if (DEBUG3)
    {
      printf ("\n>< NEIGHBORHOOD EVALUTATION ><  Num act: %d\n", num);
      if (num < 0)
	printf ("\n\n  ___NO ACTIONS\n");
      if (num == 1)
	printf ("\n\n  ___Only ONE action ENABLE");
    }


  if (num > 1)

    best =
      find_min (unsup_numeric_fact, pos_temp_vect, num, &num_min, &num_neg);
  else
    {
      num_min = 1;
      pos_temp_vect[0] = 0;
    }

  GpG.is_lm = 0;


  if (num == 1)
    choice = 0;
  else if (best > 0)
    {
      GpG.is_lm = 1;

      if ((MY_RANDOM % GpG.denominator) < GpG.numerator)
	choice = MY_RANDOM % num;
      else if (num_min == 1)
	choice = pos_temp_vect[0];
      else
	choice = pos_temp_vect[(MY_RANDOM % num_min)];
    }
  else
    {
      if (num_neg == 1)
	choice = pos_temp_vect[0];
      else if ((MY_RANDOM % GpG.denominator) < GpG.numerator)
	choice = pos_temp_vect[MY_RANDOM % num_neg];
      else if (num_min == 1)
	choice = pos_temp_vect[0];
      else
	choice = pos_temp_vect[(MY_RANDOM % num_min)];
    }

  act = CONVERT_ACTION_TO_VERTEX (neighb_vect[choice]->act_pos);
  level = neighb_vect[choice]->act_level;

  if (DEBUG2)
    {
      printf
	("\n\n=== Action choosen unsup numeric fact: %s, level %d \n     Incons %.3f   Cost %.3f   Time %.3f ",
	 print_op_name_string (act->position, temp_name), level,
	 neighb_vect[choice]->cost.weight, neighb_vect[choice]->cost.act_cost,
	 neighb_vect[choice]->cost.act_time);

      if (DEBUG3)
	print_actions_in_subgraph ();
    }




  choice =
    insert_remove_action (neighb_vect[choice]->act_pos,
			  neighb_vect[choice]->act_level,
			  neighb_vect[choice]->constraint_type,
			  GpG.approximation_level);
  return choice;
}



void
check_false_position ()
{
  int j, k = 0;
  Bool present[MAX_NUM_VALUE];

  memset (present, 0, MAX_NUM_VALUE * sizeof (Bool));
  while (TRUE)
    {
      if (vectlevel[k])
	for (j = 0; j < MAX_NUM_VALUE; j++)
	  {
	    int f_p = vectlevel[k]->numeric->false_position[j];
	    assert (f_p >= -1);
	    assert (f_p < GpG.num_false_num_fa);

	    if (f_p != -1)
	      {
		if (present[f_p] == TRUE)
		  assert (0);
		else
		  present[f_p] = TRUE;
	      }
	  }
      else
	break;
      k++;
    }

  for (j = 0; j < GpG.num_false_num_fa; j++)
    assert (j ==
	    vectlevel[*unsup_num_fact[j]->level]->numeric->
	    false_position[unsup_num_fact[j]->fact]);
}


int
choose_num_actions (constraints_list unsup_numeric_fact, int initialize)
{
  static action_set neighbors;
  neighb temp_act;
  int i, num_i;

  if (initialize != 0)
    {

      reset_neighborhood ();
    }

  neighbors.num_A = 0;
  memset (neighbors.A, 0, MAX_NUM_ACTIONS * sizeof (int));
  create_neighborhood_for_compvar (unsup_numeric_fact->fact, 1, 0, &neighbors,
				   1, *unsup_numeric_fact->level);

  temp_act.fact_treated = unsup_numeric_fact->fact;
  for (i = 0; i < neighbors.num_A; i++)
    {
      temp_act.act_pos = neighbors.A[i];
      temp_act.act_level = *unsup_numeric_fact->level;
      temp_act.constraint_type = C_T_INSERT_ACTION;
      insert_element_in_neighb (&temp_act);
    }
  num_i = neighbors.num_A;
  neighbors.num_A = 0;


  if (*unsup_numeric_fact->level < GpG.curr_plan_length)
    create_remotion_neighborhood_for_compvar (unsup_numeric_fact->fact,
					      *unsup_numeric_fact->level,
					      &neighbors);
  for (i = 0; i < neighbors.num_A; i++)
    {
      temp_act.act_pos = neighbors.A[i];
      temp_act.act_level = *unsup_numeric_fact->level;
      temp_act.constraint_type = C_T_REMOVE_ACTION;
      insert_element_in_neighb (&temp_act);
    }
  return num_i + neighbors.num_A;
}


void
clean_unsup_num_fact ()
{
  int i;

  for (i = 0; i < GpG.num_false_num_fa; i++)
    if (is_num_prec_satisfied
	(unsup_num_fact[i]->fact, *unsup_num_fact[i]->level))
      remove_unsup_num_fact (i);
}



int
choose_min_cost_unsup_num_fact ()
{
  int i, best = 0, min_level = 0;
  float min = 100000.0;




  if (GpG.inc_choice_type == MIN_LEVEL_INC
      || GpG.inc_choice_type == MIN_LEVEL_COST_INC
      || GpG.inc_choice_type == MIN_LEVEL_CONSTR_INC
      || GpG.inc_choice_type == MIN_COST_INC)
    min_level = 100000;
  else if (GpG.inc_choice_type == MAX_LEVEL_INC)
    min_level = 0;
  if (GpG.inc_choice_type == MIN_COST_INC
      || GpG.inc_choice_type == MIN_LEVEL_COST_INC)
    min = 100000.0;
  if (GpG.inc_choice_type == MAX_COST_INC)
    min = 0.0;


  for (i = GpG.num_false_num_fa - 1; i >= 0; i--)
    {


      if (GpG.inc_choice_type == MIN_LEVEL_INC
	  || GpG.inc_choice_type == MIN_LEVEL_COST_INC
	  || GpG.inc_choice_type == MIN_LEVEL_CONSTR_INC
	  || GpG.inc_choice_type == MIN_COST_INC)
	{

	  if (min_level > *unsup_num_fact[i]->level
	      || (min_level == *unsup_num_fact[i]->level && MY_RANDOM % 2))
	    {
	      min_level = *unsup_num_fact[i]->level;
	      best = i;
	    }
	}

      else
	if (GpG.inc_choice_type == MAX_LEVEL_INC
	    || GpG.inc_choice_type == MAX_COST_INC)
	{

	  if (min_level < *unsup_num_fact[i]->level)
	    {
	      min_level = *unsup_num_fact[i]->level;
	      best = i;
	    }
	}

    }

  return best;
}


void
insert_values_unsup_num_fact (int status, int fact, int level)
{
  static int num_facts = 0;
  static int fct_pos[MAX_NUM_VALUE];
  static int fct_level[MAX_NUM_VALUE];
  static Bool semaforo_verde = TRUE;
  int i, l;

  assert (status < 3);
  switch (status)
    {
    case 0:
      assert (semaforo_verde);
      assert (fact < 0);
      assert (level < 0);

      num_facts = 0;
      semaforo_verde = FALSE;
      break;

    case 1:
      assert (fact >= 0);
      assert (level >= 0);

      assert (num_facts < MAX_NUM_VALUE);
      for (i = 0; i < num_facts; i++)
	if (fct_pos[i] == fact)
	  break;

      if ((i != num_facts) && (num_facts != 0))
	fct_level[i] = MIN (level, fct_level[i]);
      else
	{
	  fct_pos[num_facts] = fact;
	  fct_level[num_facts] = level;
	  num_facts++;
	}
      break;


    case 2:
      assert (fact < 0);
      assert (level < 0);

      for (i = 0; i < num_facts; i++)
	{
	  int f = fct_pos[i];
	  for (l = fct_level[i]; l <= GpG.curr_plan_length; l++)
	    if (vectlevel[l]->numeric->w_is_goal[f])
	      {
		if (is_num_prec_satisfied (f, l))
		  {
		    if (vectlevel[l]->numeric->false_position[f] >= 0)
		      remove_unsup_num_fact (vectlevel[l]->numeric->
					     false_position[f]);
		  }
		else
		  {
		    if (vectlevel[l]->numeric->false_position[f] < 0)
		      insert_unsup_numeric_fact (f, l);
		  }
	      }
	}

      num_facts = 0;
      semaforo_verde = TRUE;
      break;
    }
}


void
remove_unappliable_actions ()
{
  int i, j;
  gis_not_appliable = calloc (gnum_ef_conn / 32 + 1, sizeof (int));
  for (i = 0; i < gnum_ef_conn; i++)
    {

      for (j = 0; j < gef_conn[i].num_PC; j++)
	{
	  if (gef_conn[i].PC[j] < 0)
	    if (gcomp_var[-gef_conn[i].PC[j]].operator == FIX_NUMBER)
	      if (GCOMP_VAR_VALUE (-gef_conn[i].PC[j]) < 0.5)
		{
		  SET_BIT (gis_not_appliable, i);
		  break;
		}
	}

      if (j != gef_conn[i].num_PC)
	continue;
      if (gef_conn[i].sf)
	{

	  for (j = 0; j < gef_conn[i].sf->num_PC_overall; j++)
	    {
	      if (gef_conn[i].sf->PC_overall[j] < 0)
		if (gcomp_var[-gef_conn[i].sf->PC_overall[j]].
		    operator == FIX_NUMBER)
		  if (GCOMP_VAR_VALUE (-gef_conn[i].sf->PC_overall[j]) < 0.5)
		    {
		      SET_BIT (gis_not_appliable, i);
		      break;
		    }
	    }

	  if (j != gef_conn[i].sf->num_PC_overall)
	    continue;

	  for (j = 0; j < gef_conn[i].sf->num_PC_end; j++)
	    {
	      if (gef_conn[i].sf->PC_end[j] < 0)
		if (gcomp_var[-gef_conn[i].sf->PC_end[j]].
		    operator == FIX_NUMBER)
		  if (GCOMP_VAR_VALUE (-gef_conn[i].sf->PC_end[j]) < 0.5)
		    {
		      SET_BIT (gis_not_appliable, i);
		      break;
		    }
	    }

	  if (j != gef_conn[i].sf->num_PC_end)
	    continue;
	}
    }
}



void
print_intlist (IntList * il)
{
  IntList *tmpil;

  printf ("\nINTLIST: ");
  for (tmpil = il; tmpil; tmpil = tmpil->next)
    printf ("%5d", tmpil->item);
}
