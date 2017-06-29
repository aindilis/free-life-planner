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
 * File: ComputeReachInf.c
 * Description: Compute Reachability Information.
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/




#include <values.h>
#include <math.h>
#include "lpg.h"
#include "inst_utils.h"
#include "H_relaxed.h"
#include "ComputeReachInf.h"
#include "H_max.h"
#include "utilities.h"
#include "numeric.h"
#include "LpgOutput.h"
#include "output.h"
#include "LpgTime.h"
#include "LocalSearch.h"
#include "ActionSubgraph.h"


void
allocate_reachability_information_data ()
{


  Hvar.init_facts_array =
    (dg_inform_list *) calloc (gnum_ft_conn, sizeof (dg_inform_list));
  Hvar.dg_facts_min_duration =
    (float *) calloc (gnum_ft_conn, sizeof (float));

  if (Hvar.estimate_time_facts == NULL)
    Hvar.estimate_time_facts =
      (float *) calloc (gnum_ft_conn, sizeof (float));

  Hvar.local_bit_vect_facts = alloc_vect (gnum_ft_block);
  Hvar.local_bit_vect_actions = alloc_vect (gnum_ef_block);

  if (Hvar.common_values == NULL)
    Hvar.common_values = (float *) calloc (max_num_value, sizeof (float));

  Hvar.list_ef_define_cost = (int *) calloc (MAX_LENGTH_H, sizeof (int));
  Hvar.list_ft_define_cost = (int *) calloc (MAX_LENGTH_H, sizeof (int));

  Hvar.bit_vect_facts = alloc_vect (gnum_ft_block);
  Hvar.bit_vect_actions = alloc_vect (gnum_ef_block);


  Hvar.ri_best_act_for_facts = alloc_vect (gnum_ft_conn);
  Hvar.ri_tot_cost_of_facts = calloc (gnum_ft_conn, sizeof (float));
  Hvar.ri_cost_of_facts = calloc (gnum_ft_conn, sizeof (float));
  Hvar.ri_duration_of_facts = calloc (gnum_ft_conn, sizeof (float));
  Hvar.ri_num_actions_of_facts = calloc (gnum_ft_conn, sizeof (int));

  Hvar.ri_list_ef_define_cost = (int *) calloc (MAX_LENGTH_H, sizeof (int));
  Hvar.ri_list_ft_define_cost = (int *) calloc (MAX_LENGTH_H, sizeof (int));
  Hvar.ri_bit_vect_facts = alloc_vect (gnum_ft_block);
  Hvar.ri_supp_bit_vect_facts = alloc_vect (gnum_ft_block);
  Hvar.ri_bit_vect_actions = alloc_vect (gnum_ef_block);
  Hvar.ri_num_actions_define_cost = 0;
  Hvar.ri_num_facts_define_cost = 0;

  memset (Hvar.ri_best_act_for_facts, INFINITY, gnum_ft_conn * sizeof (int));

}





void
cri_insert_fact_inlist (int pos)
{

  if (pos < 0)
    return;

  if (GET_BIT (Hvar.ri_bit_vect_facts, pos))
    return;

  if (Hvar.ri_num_facts_define_cost >= MAX_LENGTH_H)
    {
      exit (0);
    }

  Hvar.ri_list_ft_define_cost[Hvar.ri_num_facts_define_cost++] = pos;

  SET_BIT (Hvar.ri_bit_vect_facts, pos);

}





int
cri_insert_action_inlist (int pos)
{
  int i;

  if (GET_BIT (Hvar.ri_bit_vect_actions, pos) != 0)
    {
      return 0;
    }

  if (Hvar.ri_num_actions_define_cost >= MAX_LENGTH_H)
    {
      exit (0);
    }

  Hvar.ri_list_ef_define_cost[Hvar.ri_num_actions_define_cost++] = pos;
  SET_BIT (Hvar.ri_bit_vect_actions, pos);

  Hvar.ri_cost_actions_define_cost += get_action_cost (pos);

  for (i = 0; i < gef_conn[pos].num_A; i++)
    {
      if (gef_conn[pos].A[i] >= 0)
	SET_BIT (Hvar.ri_supp_bit_vect_facts, gef_conn[pos].A[i]);

    }


  if (gef_conn[pos].sf != NULL)
    for (i = 0; i < gef_conn[pos].sf->num_A_start; i++)
      {
	if (is_fact_in_delete_effects (pos, gef_conn[pos].sf->A_start[i]))
	  continue;

	if (gef_conn[pos].sf->A_start[i] >= 0)
	  SET_BIT (Hvar.ri_supp_bit_vect_facts, gef_conn[pos].sf->A_start[i]);

      }


  return 1;

}





int
cri_get_Action_for_fct (int fact_pos)
{

  if (fact_pos < 0)
    return INFINITY;

  if (Hvar.ri_best_act_for_facts[fact_pos] == INFINITY)
    {
      printf ("\nError act for fct %d ", fact_pos);
      print_ft_name (fact_pos);
    }

  return (Hvar.ri_best_act_for_facts[fact_pos]);

}

void
reset_cri_list ()
{
  Hvar.ri_num_actions_define_cost = 0;
  Hvar.ri_num_facts_define_cost = 0;
  Hvar.ri_cost_actions_define_cost = 0.0;
  Hvar.ri_time_actions_define_cost = 0.0;

  reset_bitarray (Hvar.ri_bit_vect_facts, gnum_ft_block);
  reset_bitarray (Hvar.ri_bit_vect_actions, gnum_ef_block);

}



void
cri_insert_list_action_preconditions (int index)
{
  int i = 0;

  for (i = 0; i < gef_conn[index].num_PC; i++)
    {
      cri_insert_fact_inlist (gef_conn[index].PC[i]);
    }

  if (gef_conn[index].sf)
    {
      for (i = 0; i < gef_conn[index].sf->num_PC_overall; i++)
	{
	  if (!is_fact_in_additive_effects_start
	      (index, gef_conn[index].sf->PC_overall[i]))
	    cri_insert_fact_inlist (gef_conn[index].sf->PC_overall[i]);
	}

      for (i = 0; i < gef_conn[index].sf->num_PC_end; i++)
	{
	  if (!is_fact_in_additive_effects_start
	      (index, gef_conn[index].sf->PC_end[i]))
	    cri_insert_fact_inlist (gef_conn[index].sf->PC_end[i]);
	}
    }

}


void
cri_insert_list_fact_additive (int index)
{

  if (!GET_BIT (Hvar.ri_supp_bit_vect_facts, index))
    {
      if (Hvar.ri_best_act_for_facts[index] != INITIAL_ACTION)
	{
	  cri_insert_action_inlist (Hvar.ri_best_act_for_facts[index]);
	  cri_insert_list_action_preconditions (Hvar.
						ri_best_act_for_facts[index]);
	}
    }
}






float
cri_predict_cost_relaxed (int action, float *duration_act, int *num_actions)
{
  int i, indx, fact;
  float cost = 0.0, duration = 0.0;


  reset_cri_list ();



  indx = 0;


  for (i = 0; i < gef_conn[action].num_PC; i++)
    cri_insert_fact_inlist (gef_conn[action].PC[i]);

  if (gef_conn[action].sf != NULL)
    {


      for (i = 0; i < gef_conn[action].sf->num_PC_overall; i++)
	if (!is_fact_in_additive_effects_start
	    (action, gef_conn[action].sf->PC_overall[i]))
	  cri_insert_fact_inlist (gef_conn[action].sf->PC_overall[i]);


      for (i = 0; i < gef_conn[action].sf->num_PC_end; i++)
	if (!is_fact_in_additive_effects_start
	    (action, gef_conn[action].sf->PC_end[i]))
	  cri_insert_fact_inlist (gef_conn[action].sf->PC_end[i]);
    }



  if (GpG.relaxed_examination_type)
    {


      do
	{
	  fact = Hvar.ri_list_ft_define_cost[indx++];

	  cri_insert_list_fact_additive (fact);
	}
      while (indx < Hvar.ri_num_facts_define_cost);

    }
  else
    {

      do
	{
	  Hvar.ri_num_facts_define_cost -= 1;
	  fact = Hvar.ri_list_ft_define_cost[Hvar.ri_num_facts_define_cost];

	  cri_insert_list_fact_additive (fact);
	}
      while (Hvar.ri_num_facts_define_cost > 0);

    }

  cri_insert_action_inlist (action);

  *num_actions = Hvar.ri_num_actions_define_cost;

  cost = Hvar.ri_cost_actions_define_cost;

  duration = 0.0;

  *duration_act = duration;


  return (cost);
}


float
cri_predict_cost_sum (int action, int *num_actions)
{
  int idx, i;
  float cost, temp;
  int best_act, num_act;
  cost = 0.0;
  num_act = 0;


  for (i = 0; i < gef_conn[action].num_PC; i++)
    {
      idx = gef_conn[action].PC[i];

      if (idx < 0)
	continue;

      best_act = cri_get_Action_for_fct (idx);
      if (best_act >= 0)
	{
	  cost += Hvar.ri_cost_of_facts[idx];
	  num_act += Hvar.ri_num_actions_of_facts[idx];

	}
    }

  if (gef_conn[action].sf != NULL)
    {

      for (i = 0; i < gef_conn[action].sf->num_PC_overall; i++)
	{
	  idx = gef_conn[action].sf->PC_overall[i];

	  if (idx < 0)
	    continue;

	  if (is_fact_in_additive_effects_start
	      (action, gef_conn[action].sf->PC_overall[i]))
	    continue;


	  best_act = cri_get_Action_for_fct (idx);
	  if (best_act >= 0)
	    {
	      cost += Hvar.ri_cost_of_facts[idx];
	      num_act += Hvar.ri_num_actions_of_facts[idx];

	    }

	}



      for (i = 0; i < gef_conn[action].sf->num_PC_end; i++)
	{
	  idx = gef_conn[action].sf->PC_end[i];

	  if (idx < 0)
	    continue;

	  if (is_fact_in_additive_effects_start
	      (action, gef_conn[action].sf->PC_end[i]))
	    continue;

	  best_act = cri_get_Action_for_fct (idx);
	  if (best_act >= 0)
	    {
	      cost += Hvar.ri_cost_of_facts[idx];
	      num_act += Hvar.ri_num_actions_of_facts[idx];

	    }

	}
    }
  temp = get_action_cost (action);
  cost = cost + temp;
  num_act++;

  *num_actions = num_act;

  return (cost);
}




float
cri_predict_cost_max (int action, int *num_actions)
{
  int idx, i;
  float cost, temp;
  int best_act, num_act;


  cost = 0.0;
  num_act = 0;


  for (i = 0; i < gef_conn[action].num_PC; i++)
    {
      idx = gef_conn[action].PC[i];

      if (idx < 0)
	continue;

      best_act = cri_get_Action_for_fct (idx);

      if (best_act >= 0 && Hvar.ri_num_actions_of_facts[idx] > num_act)
	{

	  cost = Hvar.ri_cost_of_facts[idx];
	  num_act = Hvar.ri_num_actions_of_facts[idx];

	}
    }

  if (gef_conn[action].sf != NULL)
    {
      for (i = 0; i < gef_conn[action].sf->num_PC_overall; i++)
	{
	  idx = gef_conn[action].sf->PC_overall[i];

	  if (idx < 0)
	    continue;

	  if (is_fact_in_additive_effects_start
	      (action, gef_conn[action].sf->PC_overall[i]))
	    continue;

	  best_act = cri_get_Action_for_fct (idx);

	  if (best_act >= 0 && Hvar.ri_num_actions_of_facts[idx] > num_act)
	    {
	      cost = Hvar.ri_cost_of_facts[idx];
	      num_act = Hvar.ri_num_actions_of_facts[idx];

	    }
	}

      for (i = 0; i < gef_conn[action].sf->num_PC_end; i++)
	{
	  idx = gef_conn[action].sf->PC_end[i];

	  if (idx < 0)
	    continue;

	  if (is_fact_in_additive_effects_start
	      (action, gef_conn[action].sf->PC_end[i]))
	    continue;

	  best_act = cri_get_Action_for_fct (idx);

	  if (best_act >= 0 && Hvar.ri_num_actions_of_facts[idx] > num_act)
	    {
	      cost = Hvar.ri_cost_of_facts[idx];
	      num_act = Hvar.ri_num_actions_of_facts[idx];

	    }
	}

    }
  temp = get_action_cost (action);
  cost += temp;
  num_act++;

  *num_actions = num_act;


  return (cost);
}










float
cri_predict_duration (int action, float *duration_act)
{
  int idx;
  int i;
  float maxdur = 0.0;
  int best_act;

  for (i = 0; i < gef_conn[action].num_PC; i++)
    {
      idx = gef_conn[action].PC[i];

      if (idx < 0)
	continue;

      best_act = cri_get_Action_for_fct (idx);
      if (best_act >= 0)
	{
	  if (Hvar.ri_duration_of_facts[idx] > maxdur)
	    {
	      maxdur = Hvar.ri_duration_of_facts[idx];

	    }
	}
    }

  if (gef_conn[action].sf != NULL)
    {
      for (i = 0; i < gef_conn[action].sf->num_PC_overall; i++)
	{
	  idx = gef_conn[action].sf->PC_overall[i];

	  if (idx < 0)
	    continue;

	  if (is_fact_in_additive_effects_start (action, idx))
	    continue;

	  best_act = cri_get_Action_for_fct (idx);
	  if (best_act >= 0)
	    {
	      if (Hvar.ri_duration_of_facts[idx] > maxdur)
		{
		  maxdur = Hvar.ri_duration_of_facts[idx];

		}

	    }
	}

      for (i = 0; i < gef_conn[action].sf->num_PC_end; i++)
	{
	  idx = gef_conn[action].sf->PC_end[i];

	  if (idx < 0)
	    continue;

	  if (is_fact_in_additive_effects_start (action, idx))
	    continue;

	  best_act = cri_get_Action_for_fct (idx);
	  if (best_act >= 0)
	    {
	      if (Hvar.ri_duration_of_facts[idx] -
		  get_action_time (action, 0) > maxdur)
		{
		  maxdur =
		    Hvar.ri_duration_of_facts[idx] - get_action_time (action,
								      0);
		}

	    }
	  else if (best_act == INFINITY)
	    {
	      int j;
	      printf ("\n Az level %d con prec non supp: %d   ",
		      gef_conn[action].level, action);
	      print_op_name (action);
	      printf ("\n Preconditions");

	      for (j = 0; j < gef_conn[action].sf->num_PC_end; j++)
		{
		  printf ("\n Fact %d :  ", gef_conn[action].sf->PC_end[j]);
		  print_ft_name (gef_conn[action].sf->PC_end[j]);
		}
	    }
	}
    }



  *duration_act = maxdur;
  return (maxdur);
}










void
cri_insert_ftcost (int fact_pos, float cost, float duration,
		   int num_actions, int best_ef)
{

  float totcost = 0.0;


  if (GpG.orig_weight_cost)
    totcost = (cost * GpG.orig_weight_cost);

  if (GpG.orig_weight_time)
    totcost += (duration * GpG.orig_weight_time);

  if (Hvar.ri_best_act_for_facts[fact_pos] == INFINITY ||
      Hvar.ri_num_actions_of_facts[fact_pos] > num_actions ||
      (Hvar.ri_num_actions_of_facts[fact_pos] == num_actions &&
       Hvar.ri_tot_cost_of_facts[fact_pos] > totcost))
    {
      Hvar.ri_best_act_for_facts[fact_pos] = best_ef;
      Hvar.ri_num_actions_of_facts[fact_pos] = num_actions;

      Hvar.ri_cost_of_facts[fact_pos] = cost;
      Hvar.ri_duration_of_facts[fact_pos] = duration;
      Hvar.ri_tot_cost_of_facts[fact_pos] = totcost;
    }

  if (GpG.first_execution_cri == 0
      && Hvar.dg_facts_min_duration[fact_pos] > duration)
    Hvar.dg_facts_min_duration[fact_pos] = duration;
}







void
cri_activate_distgraph_ef (int index)
{

  int i, num_actions = 0;
  float cost, duration, duration_max, duration_list, act_dur;

  cri_predict_duration (index, &duration_max);

  if (GpG.cri_evaluate_preconditions >= COMPUTE_DG_LIST_COST
      || GpG.verify_init)
    {
      reset_bitarray (Hvar.ri_supp_bit_vect_facts, gnum_ft_block);
      cost = cri_predict_cost_relaxed (index, &duration_list, &num_actions);
    }
  else if (GpG.cri_evaluate_preconditions == COMPUTE_DG_SUM_COST)
    cost = cri_predict_cost_sum (index, &num_actions);
  else
    cost = cri_predict_cost_max (index, &num_actions);

  act_dur = get_action_time (index, 0);

  if (duration_max >= duration_list)
    duration = act_dur + duration_max;
  else
    duration = act_dur + duration_list;

  if (gef_conn[index].sf != NULL)
    for (i = 0; i < gef_conn[index].sf->num_A_start; i++)
      {
	if (is_fact_in_delete_effects (index, gef_conn[index].sf->A_start[i]))
	  {
	    if (gef_conn[index].sf->A_start[i] >= 0)
	      cri_insert_ftcost (gef_conn[index].sf->A_start[i], MAX_COST,
				 duration - act_dur, MAX_COST, index);


	  }
	else if (gef_conn[index].sf->A_start[i] >= 0)
	  cri_insert_ftcost (gef_conn[index].sf->A_start[i], cost,
			     duration - act_dur, num_actions, index);
      }

  for (i = 0; i < gef_conn[index].num_A; i++)
    {
      if (gef_conn[index].A[i] >= 0)
	cri_insert_ftcost (gef_conn[index].A[i], cost,
			   duration, num_actions, index);
    }

}



inline void
cri_heuristic_for_action (int action, int level)
{
  static int *initial_local_bit_vect_facts = NULL;
  register int i, j, k, el = 0, fact_pos, skip = FALSE;
  float max_prec_cost, max_prec_duration, new_cost, new_duration;
  int max_prec_num_actions = 0;

  max_prec_cost = 0.0;
  max_prec_duration = 0.0;
  new_cost = 0.0;
  new_duration = 0.0;

  reset_bitarray (Hvar.local_bit_vect_facts, gnum_ft_block);
  reset_bitarray (Hvar.local_bit_vect_actions, gnum_ef_block);

  memset (Hvar.ri_best_act_for_facts, INFINITY, gnum_ft_conn * sizeof (int));
  memset (Hvar.ri_tot_cost_of_facts, 0, gnum_ft_conn * sizeof (float));
  memset (Hvar.ri_cost_of_facts, 0, gnum_ft_conn * sizeof (float));
  memset (Hvar.ri_duration_of_facts, 0, gnum_ft_conn * sizeof (float));
  memset (Hvar.ri_num_actions_of_facts, 0, gnum_ft_conn * sizeof (int));

  reset_bitarray (Hvar.bit_vect_facts, gnum_ft_block);
  reset_bitarray (Hvar.bit_vect_actions, gnum_ef_block);

  Hvar.num_actions_define_cost = 0;
  Hvar.num_facts_define_cost = 0;
  Hvar.cost_actions_define_cost = 0.0;
  Hvar.time_actions_define_cost = 0.0;

  reset_bitarray (Hvar.ri_bit_vect_facts, gnum_ft_block);
  reset_bitarray (Hvar.ri_bit_vect_actions, gnum_ef_block);

  Hvar.ri_num_actions_define_cost = 0;
  Hvar.ri_num_facts_define_cost = 0;
  Hvar.ri_cost_actions_define_cost = 0.0;
  Hvar.ri_time_actions_define_cost = 0.0;



  if (initial_local_bit_vect_facts == NULL)
    initial_local_bit_vect_facts = alloc_vect (gnum_ft_block);

  memcpy (Hvar.local_bit_vect_facts, vectlevel[level]->fact_vect,
	  gnum_ft_block * sizeof (int));


  for (i = 0; i < gef_conn[action].num_PC; i++)
    {
      if (gef_conn[action].PC[i] < 0)
	continue;

      if (GET_BIT (Hvar.local_bit_vect_facts, gef_conn[action].PC[i]) == 0)
	{
	  SET_BIT (Hvar.local_bit_vect_facts, gef_conn[action].PC[i]);

	  ls_insert_fact_inlist (gef_conn[action].PC[i]);

	  remove_mutex_facts_in_bitvect_and_update_num_actions (gef_conn
								[action].
								PC[i],
								Hvar.
								local_bit_vect_facts);

	}
      else if (GpG.cri_update_all_facts == 1)
	ls_insert_fact_inlist (gef_conn[action].PC[i]);
    }



  if (gef_conn[action].sf)
    {
      for (i = 0; i < gef_conn[action].sf->num_PC_overall; i++)
	{
	  if (gef_conn[action].sf->PC_overall[i] < 0)
	    continue;

	  if (is_fact_in_additive_effects_start
	      (action, gef_conn[action].sf->PC_overall[i]))
	    continue;

	  if (GET_BIT
	      (Hvar.local_bit_vect_facts,
	       gef_conn[action].sf->PC_overall[i]) == 0)
	    {
	      SET_BIT (Hvar.local_bit_vect_facts,
		       gef_conn[action].sf->PC_overall[i]);

	      ls_insert_fact_inlist (gef_conn[action].sf->PC_overall[i]);

	      remove_mutex_facts_in_bitvect_and_update_num_actions (gef_conn
								    [action].
								    sf->
								    PC_overall
								    [i],
								    Hvar.
								    local_bit_vect_facts);
	    }
	  else if (GpG.cri_update_all_facts == 1)
	    ls_insert_fact_inlist (gef_conn[action].sf->PC_overall[i]);

	}

      for (i = 0; i < gef_conn[action].sf->num_PC_end; i++)
	{
	  if (gef_conn[action].sf->PC_end[i] < 0)
	    continue;

	  if (is_fact_in_additive_effects
	      (action, gef_conn[action].sf->PC_end[i]))
	    continue;

	  if (GET_BIT
	      (Hvar.local_bit_vect_facts,
	       gef_conn[action].sf->PC_end[i]) == 0)
	    {
	      SET_BIT (Hvar.local_bit_vect_facts,
		       gef_conn[action].sf->PC_end[i]);

	      ls_insert_fact_inlist (gef_conn[action].sf->PC_end[i]);

	      remove_mutex_facts_in_bitvect_and_update_num_actions (gef_conn
								    [action].
								    sf->
								    PC_end[i],
								    Hvar.
								    local_bit_vect_facts);
	    }
	  else if (GpG.cri_update_all_facts == 1)
	    ls_insert_fact_inlist (gef_conn[action].sf->PC_end[i]);
	}
    }


  if (gef_conn[action].sf)
    for (i = 0; i < gef_conn[action].sf->num_A_start; i++)
      {
	if (gef_conn[action].sf->A_start[i] < 0)
	  continue;

	if (is_fact_in_delete_effects
	    (action, gef_conn[action].sf->A_start[i]))
	  continue;

	if (GET_BIT
	    (Hvar.local_bit_vect_facts, gef_conn[action].sf->A_start[i]) == 0)
	  {
	    SET_BIT (Hvar.local_bit_vect_facts,
		     gef_conn[action].sf->A_start[i]);

	    ls_insert_fact_inlist (gef_conn[action].sf->A_start[i]);

	    remove_mutex_facts_in_bitvect_and_update_num_actions (gef_conn
								  [action].
								  sf->
								  A_start[i],
								  Hvar.
								  local_bit_vect_facts);
	  }
      }


  for (i = 0; i < gef_conn[action].num_A; i++)
    {

      if (gef_conn[action].A[i] < 0)
	continue;

      if (GET_BIT (Hvar.local_bit_vect_facts, gef_conn[action].A[i]) == 0)
	{
	  SET_BIT (Hvar.local_bit_vect_facts, gef_conn[action].A[i]);

	  ls_insert_fact_inlist (gef_conn[action].A[i]);

	  remove_mutex_facts_in_bitvect_and_update_num_actions (gef_conn
								[action].A[i],
								Hvar.
								local_bit_vect_facts);
	}
    }


  memcpy (initial_local_bit_vect_facts, Hvar.local_bit_vect_facts,
	  gnum_ft_block * sizeof (int));


  for (i = 0; i < Hvar.num_facts_define_cost; i++)
    {
      fact_pos = Hvar.list_ft_define_cost[i];

      if (GET_BIT (Hvar.local_bit_vect_facts, fact_pos) == 0
	  && Hvar.ri_cost_of_facts[fact_pos] == 0.0)
	continue;

      for (j = 0; j < gft_conn[fact_pos].num_PC; j++)
	{

	  el = gft_conn[fact_pos].PC[j];

	  if (el < 0)
	    continue;

	  if (GET_BIT (Hvar.local_bit_vect_actions, el))
	    {
	      continue;
	    }

	  else
	    {
	      skip = FALSE;

	      max_prec_num_actions = 0;
	      max_prec_duration = max_prec_cost = 0.0;


	      for (k = 0; k < gef_conn[el].num_PC; k++)
		{
		  if (gef_conn[el].PC[k] < 0)
		    continue;


		  if (Hvar.ri_best_act_for_facts[gef_conn[el].PC[k]] < 0 &&
		      GET_BIT (initial_local_bit_vect_facts,
			       gef_conn[el].PC[k]) == 0)
		    {
		      skip = TRUE;
		      break;
		    }
		  else
		    {
		      if (Hvar.ri_cost_of_facts[gef_conn[el].PC[k]] >
			  max_prec_cost)
			max_prec_cost =
			  Hvar.ri_cost_of_facts[gef_conn[el].PC[k]];

		      if (CONVERT_FACT_TO_INFORM (gef_conn[el].PC[k], level)->
			  time_f > max_prec_duration)
			max_prec_duration =
			  CONVERT_FACT_TO_INFORM (gef_conn[el].PC[k],
						  level)->time_f;

		      else if (Hvar.
			       ri_duration_of_facts[gef_conn[el].PC[k]] >
			       max_prec_duration)
			max_prec_duration =
			  Hvar.ri_duration_of_facts[gef_conn[el].PC[k]];
		      if (Hvar.
			  ri_num_actions_of_facts[gef_conn[el].PC[k]] >
			  max_prec_num_actions)
			max_prec_num_actions =
			  Hvar.ri_num_actions_of_facts[gef_conn[el].PC[k]];
		    }


		}

	      if (skip)
		continue;

	      if (gef_conn[el].sf != NULL)
		{


		  for (k = 0; k < gef_conn[el].sf->num_PC_overall; k++)

		    {
		      if (gef_conn[el].sf->PC_overall[k] < 0)
			continue;
		      if (is_fact_in_additive_effects_start
			  (el, gef_conn[el].sf->PC_overall[k]))
			continue;




		      if (Hvar.
			  ri_best_act_for_facts[gef_conn[el].sf->
						PC_overall[k]] < 0
			  && GET_BIT (initial_local_bit_vect_facts,
				      gef_conn[el].sf->PC_overall[k]) == 0)
			{

			  skip = TRUE;
			  break;

			}
		      else
			{

			  if (Hvar.
			      ri_cost_of_facts[gef_conn[el].sf->
					       PC_overall[k]] > max_prec_cost)
			    max_prec_cost =
			      Hvar.ri_cost_of_facts[gef_conn[el].sf->
						    PC_overall[k]];



			  if (CONVERT_FACT_TO_INFORM
			      (gef_conn[el].sf->PC_overall[k],
			       level)->time_f > max_prec_duration)
			    max_prec_duration =
			      CONVERT_FACT_TO_INFORM (gef_conn[el].sf->
						      PC_overall[k],
						      level)->time_f;

			  else if (Hvar.
				   ri_duration_of_facts[gef_conn[el].sf->
							PC_overall[k]] >
				   max_prec_duration)
			    max_prec_duration =
			      Hvar.ri_duration_of_facts[gef_conn[el].sf->
							PC_overall[k]];
			  if (Hvar.
			      ri_num_actions_of_facts[gef_conn[el].sf->
						      PC_overall[k]] >
			      max_prec_num_actions)
			    max_prec_num_actions =
			      Hvar.ri_num_actions_of_facts[gef_conn[el].
							   sf->PC_overall[k]];
			}


		    }


		  if (skip)
		    continue;




		  for (k = 0; k < gef_conn[el].sf->num_PC_end; k++)
		    {
		      if (gef_conn[el].sf->PC_end[k] < 0)
			continue;

		      if (is_fact_in_additive_effects_start
			  (el, gef_conn[el].sf->PC_end[k]))
			continue;

		      if (Hvar.
			  ri_best_act_for_facts[gef_conn[el].sf->PC_end[k]] <
			  0
			  && GET_BIT (initial_local_bit_vect_facts,
				      gef_conn[el].sf->PC_end[k]) == 0)
			{

			  skip = TRUE;
			  break;

			}
		      else
			{

			  if (Hvar.
			      ri_cost_of_facts[gef_conn[el].sf->
					       PC_end[k]] > max_prec_cost)
			    max_prec_cost =
			      Hvar.ri_cost_of_facts[gef_conn[el].sf->
						    PC_end[k]];

			  if (CONVERT_FACT_TO_INFORM
			      (gef_conn[el].sf->PC_end[k],
			       level)->time_f > max_prec_duration)
			    max_prec_duration =
			      CONVERT_FACT_TO_INFORM (gef_conn[el].sf->
						      PC_end[k],
						      level)->time_f;

			  else if (Hvar.
				   ri_duration_of_facts[gef_conn[el].sf->
							PC_end[k]] -
				   get_action_time (el,
						    level) >
				   max_prec_duration)
			    max_prec_duration =
			      Hvar.ri_duration_of_facts[gef_conn[el].sf->
							PC_end[k]] -
			      get_action_time (el, level);
			  if (Hvar.
			      ri_num_actions_of_facts[gef_conn[el].sf->
						      PC_end[k]] >
			      max_prec_num_actions)
			    max_prec_num_actions =
			      Hvar.ri_num_actions_of_facts[gef_conn[el].
							   sf->PC_end[k]];
			}
		    }


		}

	    }


	  if (skip)
	    continue;


	  if (GpG.cri_evaluate_preconditions == COMPUTE_DG_LIST_COST)
	    {
	      memcpy (Hvar.ri_supp_bit_vect_facts,
		      initial_local_bit_vect_facts,
		      gnum_ft_block * sizeof (int));
	      new_cost =
		cri_predict_cost_relaxed (el, &new_duration,
					  &max_prec_num_actions);

	    }
	  else
	    {
	      new_cost = max_prec_cost + get_action_cost (el);

	      max_prec_num_actions++;
	    }

	  new_duration = max_prec_duration + get_action_time (el, level);

	  SET_BIT (Hvar.local_bit_vect_actions, el);

	  for (k = 0; k < gef_conn[el].num_A; k++)
	    if (gef_conn[el].A[k] >= 0)
	      {
		SET_BIT (Hvar.local_bit_vect_facts, gef_conn[el].A[k]);


		if (GET_BIT (Hvar.bit_vect_facts, gef_conn[el].A[k]) == 0
		    || Hvar.ri_num_actions_of_facts[gef_conn[el].A[k]] >
		    max_prec_num_actions
		    || (Hvar.ri_num_actions_of_facts[gef_conn[el].A[k]] ==
			max_prec_num_actions
			&& (GpG.orig_weight_cost *
			    Hvar.ri_cost_of_facts[gef_conn[el].A[k]] +
			    GpG.orig_weight_time *
			    Hvar.ri_duration_of_facts[gef_conn[el].A[k]]) >
			(GpG.orig_weight_cost * new_cost +
			 GpG.orig_weight_time * new_duration)))
		  {
		    ls_insert_fact_inlist (gef_conn[el].A[k]);
		    Hvar.ri_cost_of_facts[gef_conn[el].A[k]] = new_cost;
		    Hvar.ri_duration_of_facts[gef_conn[el].A[k]] =
		      new_duration;
		    Hvar.ri_num_actions_of_facts[gef_conn[el].A[k]] =
		      max_prec_num_actions;
		    Hvar.ri_best_act_for_facts[gef_conn[el].A[k]] = el;
		  }
	      }


	  if (gef_conn[el].sf)
	    for (k = 0; k < gef_conn[el].sf->num_A_start; k++)
	      if (gef_conn[el].sf->A_start[k] >= 0)
		{
		  SET_BIT (Hvar.local_bit_vect_facts,
			   gef_conn[el].sf->A_start[k]);



		  if (GET_BIT
		      (Hvar.bit_vect_facts, gef_conn[el].sf->A_start[k]) == 0
		      || Hvar.ri_num_actions_of_facts[gef_conn[el].sf->
						      A_start[k]] >
		      max_prec_num_actions
		      || (Hvar.
			  ri_num_actions_of_facts[gef_conn[el].sf->
						  A_start[k]] ==
			  max_prec_num_actions
			  && (GpG.orig_weight_cost *
			      Hvar.ri_cost_of_facts[gef_conn[el].sf->
						    A_start[k]] +
			      GpG.orig_weight_time *
			      Hvar.ri_duration_of_facts[gef_conn[el].sf->
							A_start[k]]) >
			  (GpG.orig_weight_cost * new_cost +
			   GpG.orig_weight_time * new_duration)))
		    {
		      if (is_fact_in_delete_effects
			  (el, gef_conn[el].sf->A_start[k]))
			continue;
		      ls_insert_fact_inlist (gef_conn[el].sf->A_start[k]);
		      Hvar.ri_cost_of_facts[gef_conn[el].sf->A_start[k]] =
			new_cost;
		      Hvar.ri_duration_of_facts[gef_conn[el].sf->A_start[k]] =
			new_duration;
		      Hvar.ri_num_actions_of_facts[gef_conn[el].sf->
						   A_start[k]] =
			max_prec_num_actions;
		      Hvar.ri_best_act_for_facts[gef_conn[el].sf->
						 A_start[k]] = el;
		    }
		}

	}
    }

}







void
set_init_computed_dg_costs ()
{
  int i;
  static int level_init = -1;
  dg_inform **loc_dg_facts_array;


  GpG.first_execution_cri++;

  loc_dg_facts_array = Hvar.init_facts_array;


  for (i = 0; i < gnum_ft_conn; i++)
    {
      loc_dg_facts_array[i] =
	update_dg_fact_list (i, &level_init, Hvar.ri_num_actions_of_facts[i],
			     Hvar.ri_best_act_for_facts[i],
			     Hvar.ri_duration_of_facts[i],
			     Hvar.ri_cost_of_facts[i]);
    }
  if (DEBUG5)
    print_cri_computed_costs (-1);

}






void
remove_mutex_facts_in_bitvect_and_update_num_actions (int fact, int *bit_vect)
{
  int i, k, b_vect, temp1;

  for (i = 0; i < gnum_ft_block; i++)
    {
      b_vect = bit_vect[i];
      b_vect &= ~gft_conn[fact].ft_exclusive_vect[i];

      if (b_vect == bit_vect[i])
	continue;

      temp1 = bit_vect[i] & (~b_vect);
      k = 32;
      while (temp1)
	{
	  k--;
	  if (temp1 & FIRST_1)
	    {
	      Hvar.ri_num_actions_of_facts[i * 32 + k] = MAXINT;
	    }

	  temp1 <<= 1;

	}
      bit_vect[i] = b_vect;

    }
}





void
set_computed_dg_costs (int level)
{
  int i, j, j1, k, temp;

  for (j1 = j = 0; j < gnum_ft_conn; j1++, j += 32)
    {
      {
	temp = Hvar.bit_vect_facts[j1];

	k = 32;
	while (temp)
	  {
	    k--;

	    if (temp & FIRST_1)
	      {
		i = j + k;


		if (i >= gnum_ft_conn)
		  continue;

		if (Hvar.ri_num_actions_of_facts[i] > 0
		    || vectlevel[level]->fact[i].w_is_true > 0)
		  {

		    if (vectlevel[level]->dg_facts_array[i] == NULL)
		      vectlevel[level]->dg_facts_array[i] =
			update_dg_fact_list (i, &vectlevel[level]->level,
					     Hvar.ri_num_actions_of_facts[i],
					     Hvar.ri_best_act_for_facts[i],
					     Hvar.ri_duration_of_facts[i],
					     Hvar.ri_cost_of_facts[i]);

		    vectlevel[level]->dg_facts_array[i]->num_actions =
		      Hvar.ri_num_actions_of_facts[i];
		    vectlevel[level]->dg_facts_array[i]->cost =
		      Hvar.ri_cost_of_facts[i];
		    vectlevel[level]->dg_facts_array[i]->duration =
		      Hvar.ri_duration_of_facts[i];
		    vectlevel[level]->dg_facts_array[i]->best_act =
		      Hvar.ri_best_act_for_facts[i];
		    vectlevel[level]->dg_facts_array[i]->totcost =
		      Hvar.ri_cost_of_facts[i] * GpG.orig_weight_cost +
		      Hvar.ri_duration_of_facts[i] * GpG.orig_weight_time;


		  }
	      }
	    temp <<= 1;
	  }
      }

      memcpy (vectlevel[level]->local_bit_vect_facts, Hvar.bit_vect_facts,
	      gnum_ft_block * sizeof (int));
    }
}






void
reset_computed_dg_costs (int level)
{
  int i, j, k, j1, temp;

  for (j1 = j = 0; j < gnum_ft_conn; j1++, j += 32)
    {
      temp = vectlevel[level]->local_bit_vect_facts[j1];
      vectlevel[level]->local_bit_vect_facts[j1] = 0;
      k = 32;
      while (temp)
	{
	  k--;

	  if (temp & FIRST_1)
	    {
	      i = j + k;

	      if (i >= gnum_ft_conn)
		continue;

	      delete_dg_fact_list (i, &vectlevel[level]->level);


	      vectlevel[level]->dg_facts_array[i] = NULL;


	    }
	  temp <<= 1;
	}
    }

}
