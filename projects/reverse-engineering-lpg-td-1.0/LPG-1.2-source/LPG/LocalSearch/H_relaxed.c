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
 * File: H_relaxed.c
 * Description: Computes the RelaxedPlan heuristic (ICAPS-03).
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



struct _DG_ACT *
calloc_dg_inform ()
{
  static int index = 0;

  dg_inform_list dg_node = NULL;



  if (Hvar.dg_delete_array == NULL
      && (Hvar.dg_inform_array == NULL || index == DG_ARRAY_DIM))
    {
      index = 0;
      Hvar.dg_inform_array =
	(dg_inform_list) calloc (DG_ARRAY_DIM, sizeof (_dg_act));

    }

  if (Hvar.dg_delete_array != NULL)
    {

      dg_node = Hvar.dg_delete_array;
      Hvar.dg_delete_array = Hvar.dg_delete_array->prev;
    }
  else
    {

      dg_node = &Hvar.dg_inform_array[index];
      index++;
    }
  return (dg_node);
}

void
free_dg_inform (dg_inform_list dg_node)
{
  dg_node->prev = Hvar.dg_delete_array;
  dg_node->next = NULL;
  Hvar.dg_delete_array = dg_node;

}

dg_inform_list
update_dg_fact_list (int fact_pos, int *level_ptr, int num_actions,
		     int best_act, float duration, float cost)
{

  int l1, indlevel;
  dg_inform_list dg_fact_node;

  l1 = *level_ptr;

  dg_fact_node = calloc_dg_inform ();

  if (l1 >= 0)
    {
      if (vectlevel[l1]->dg_facts_array[fact_pos])
	{
	  free_dg_inform (vectlevel[l1]->dg_facts_array[fact_pos]);
	  vectlevel[l1]->dg_facts_array[fact_pos] = NULL;
	}
      vectlevel[l1]->dg_facts_array[fact_pos] = dg_fact_node;

    }
  dg_fact_node->level = level_ptr;
  dg_fact_node->num_actions = num_actions;
  dg_fact_node->best_act = best_act;
  dg_fact_node->duration = duration;
  dg_fact_node->cost = cost;
  dg_fact_node->totcost =
    cost * GpG.orig_weight_cost + duration * GpG.orig_weight_time;
  dg_fact_node->stop = FALSE;
  dg_fact_node->prev = NULL;
  dg_fact_node->next = NULL;

  if (DEBUG5)
    {
      printf
	("\nSRI: fact reachability info: fact %s level %d best_action %s num_action %d duration %.2f cost %.2f",
	 print_ft_name_string (fact_pos, temp_name), l1,
	 print_op_name_string (dg_fact_node->best_act, temp_name),
	 dg_fact_node->num_actions, dg_fact_node->duration,
	 dg_fact_node->cost);
    }

  if (l1 < 0)
    {
      dg_fact_node->stop = TRUE;
      return dg_fact_node;
    }

  if (GpG.relax_list_fact_cost == FALSE)
    {
      return dg_fact_node;
    }


  if (GET_ACTION_POSITION_OF_LEVEL (l1) >= 0)
    {
      if (check_mutex_noop (fact_pos, l1))
	{
	  if (DEBUG5)
	    {
	      printf ("\nSRI: Mutex between %s at level %d and %s",
		      print_op_name_string (GET_ACTION_POSITION_OF_LEVEL (l1),
					    temp_name), l1,
		      print_ft_name_string (fact_pos, temp_name));
	    }
	  dg_fact_node->stop = TRUE;
	}
    }
  for (indlevel = l1 - 1; indlevel >= 0; indlevel--)
    if (vectlevel[indlevel]->dg_facts_array[fact_pos])
      {
	dg_fact_node->next =
	  vectlevel[indlevel]->dg_facts_array[fact_pos]->next;
	vectlevel[indlevel]->dg_facts_array[fact_pos]->next = dg_fact_node;

	dg_fact_node->prev = vectlevel[indlevel]->dg_facts_array[fact_pos];
	if (dg_fact_node->next != NULL)
	  dg_fact_node->next->prev = dg_fact_node;
	break;
      }

  if (DEBUG5)
    printf
      ("\nSRI: Find a previous dg_fact_node fact_pos: fact_pos %d at level: %d",
       fact_pos, indlevel);

  if (indlevel < 0)
    {
      dg_fact_node->next = Hvar.init_facts_array[fact_pos]->next;

      if (Hvar.init_facts_array[fact_pos]->next != NULL)
	Hvar.init_facts_array[fact_pos]->next->prev = dg_fact_node;

      dg_fact_node->prev = Hvar.init_facts_array[fact_pos];
      Hvar.init_facts_array[fact_pos]->next = dg_fact_node;
    }

  return (dg_fact_node);
}

void
delete_dg_fact_list (int fact_pos, int *level)
{

  dg_inform_list dg_fact_node;
  int l1;

  l1 = *level;

  if (DEBUG5)
    printf ("\nSRI: Delete dg_fact_node fact_pos: %d level: %d", fact_pos,
	    l1);

  dg_fact_node = vectlevel[l1]->dg_facts_array[fact_pos];

  if (dg_fact_node == NULL)
    return;

  if (dg_fact_node->prev)
    dg_fact_node->prev->next = dg_fact_node->next;

  if (dg_fact_node->next)
    dg_fact_node->next->prev = dg_fact_node->prev;

  dg_fact_node->cost = 0.0;
  dg_fact_node->totcost = 0.0;
  dg_fact_node->duration = 0.0;
  dg_fact_node->level = NULL;
  dg_fact_node->num_actions = -1;
  dg_fact_node->best_act = -1;
  dg_fact_node->stop = FALSE;

  free_dg_inform (dg_fact_node);

}


int
get_dg_fact_cost (register int fact_pos, register int level,
		  dg_inform_list * loc_dg_cost)
{
  register int l1;
  register dg_inform_list index_dg;
  dg_inform_list best_dg_cost = NULL;
  static int *action_listed;

  for (l1 = level; l1 >= 0; l1--)
    if (vectlevel[l1]->dg_facts_array[fact_pos])
      {
	best_dg_cost = vectlevel[l1]->dg_facts_array[fact_pos];
	if (GpG.relax_list_fact_cost == FALSE)
	  {
	    *loc_dg_cost = best_dg_cost;

	    if (DEBUG5)
	      printf
		("\nGRI1: get fact reachability info: fact %s level %d best_action %s num_action %d duration %.2f cost %.2f",
		 print_ft_name_string (fact_pos, temp_name), l1,
		 print_op_name_string (best_dg_cost->best_act, temp_name),
		 best_dg_cost->num_actions, best_dg_cost->duration,
		 best_dg_cost->cost);

	    return l1;
	  }
	else
	  break;
      }

  if (l1 < 0)
    {
      *loc_dg_cost = best_dg_cost = Hvar.init_facts_array[fact_pos];

      if (DEBUG5)
	printf
	  ("\nGRI2: get fact reachability info: fact %s level %d best_action %s num_action %d duration %.2f cost %.2f",
	   print_ft_name_string (fact_pos, temp_name), l1,
	   print_op_name_string (best_dg_cost->best_act, temp_name),
	   best_dg_cost->num_actions, best_dg_cost->duration,
	   best_dg_cost->cost);

      return 0;
    }


  if (action_listed == NULL)
    action_listed = (int *) calloc (gnum_ef_block, sizeof (int));

  memset (action_listed, 0, gnum_ef_block * sizeof (int));


  for (index_dg = best_dg_cost; index_dg; index_dg = index_dg->prev)
    {
      if (index_dg->stop == TRUE)
	break;
      if (GET_BIT (action_listed, index_dg->best_act))
	{
	  continue;
	}

      SET_BIT (action_listed, index_dg->best_act);

      if (best_dg_cost->num_actions > index_dg->num_actions
	  || (best_dg_cost->num_actions == index_dg->num_actions
	      && best_dg_cost->totcost > index_dg->totcost))
	{

	  if (DEBUG5)
	    {
	      printf
		("\n GRI3: Find better RI: fact %s level %d best_action %s num_action %d duration %.2f cost %.2f",
		 print_ft_name_string (fact_pos, temp_name),
		 *best_dg_cost->level,
		 print_op_name_string (best_dg_cost->best_act, temp_name),
		 best_dg_cost->num_actions, best_dg_cost->duration,
		 best_dg_cost->cost);
	    }

	  best_dg_cost = index_dg;

	}
      if (*index_dg->level <= -1)
	break;
    }


  *loc_dg_cost = best_dg_cost;
  if (DEBUG5)
    printf
      ("\nGRI4: get fact reachability info: fact %s level %d best_action %s num_action %d duration %.2f cost %.2f",
       print_ft_name_string (fact_pos, temp_name), l1,
       print_op_name_string (best_dg_cost->best_act, temp_name),
       best_dg_cost->num_actions, best_dg_cost->duration, best_dg_cost->cost);

  if (*best_dg_cost->level >= 0)
    return (*best_dg_cost->level);
  else
    return (0);

}


int
get_dg_fact_cost2 (register int fact_pos, register int level,
		   dg_inform_list * loc_dg_cost)
{

  register int l1;



  for (l1 = level; l1 >= 0; l1--)
    if (vectlevel[l1]->dg_facts_array[fact_pos])
      {
	*loc_dg_cost = vectlevel[l1]->dg_facts_array[fact_pos];
	return l1;
      }


  *loc_dg_cost = Hvar.init_facts_array[fact_pos];




  return 0;
}



float
compute_constr_fact (int constraint, int fact_pos, int level, int initialize)
{



  int num_constr = 0, i, lev1, action, el;
  dg_inform_list loc_dg_cost;

  if (initialize)
    reset_bitarray (Hvar.bit_vect_actions, gnum_ef_block);


  if (constraint >= MAX_FALSE)
    {
      exit (0);
    }


  lev1 = get_dg_fact_cost (fact_pos, level, &loc_dg_cost);



  action = loc_dg_cost->best_act;


  if (action < 0)
    {

      return 0;
    }


  if (GET_BIT (Hvar.bit_vect_actions, action))
    {

      return 0;
    }



  SET_BIT (Hvar.bit_vect_actions, action);



  for (i = 0; i < GpG.num_false_fa; i++)
    {
      if (i == constraint)
	continue;

      if (lev1 > *unsup_fact[i]->level)
	continue;

      if (gef_conn[action].ft_exclusive_vect[GUID_BLOCK (unsup_fact[i]->fact)]
	  & (GUID_MASK (unsup_fact[i]->fact)))
	{
	  num_constr++;
	}

    }

  for (i = 0; i < gef_conn[action].num_PC; i++)
    {
      el = gef_conn[action].PC[i];
      if (el < 0)
	continue;


      if (fact_is_supported (el, lev1))
	{
	  continue;
	}

      num_constr += compute_constr_fact (constraint, el, lev1, 0);

    }

  if (gef_conn[action].sf != NULL)
    {

      for (i = 0; i < gef_conn[action].sf->num_PC_overall; i++)
	{
	  el = gef_conn[action].sf->PC_overall[i];
	  if (el < 0)
	    continue;

	  if (is_fact_in_additive_effects_start
	      (action, gef_conn[action].sf->PC_overall[i]))
	    continue;

	  if (fact_is_supported (el, lev1))
	    {
	      continue;
	    }

	  num_constr += compute_constr_fact (constraint, el, lev1, 0);

	}

      for (i = 0; i < gef_conn[action].sf->num_PC_end; i++)
	{
	  el = gef_conn[action].sf->PC_end[i];
	  if (el < 0)
	    continue;
	  if (is_fact_in_additive_effects_start
	      (action, gef_conn[action].sf->PC_end[i]))
	    continue;


	  if (fact_is_supported (el, lev1))
	    {
	      continue;
	    }

	  num_constr += compute_constr_fact (constraint, el, lev1, 0);
	}
    }

  return num_constr;

}











void
ls_insert_fact_inlist (int pos)
{
  if (pos < 0)
    return;

  if (GET_BIT (Hvar.bit_vect_facts, pos))
    return;

  if (Hvar.num_facts_define_cost >= MAX_LENGTH_H)
    {
      exit (0);
    }


  Hvar.list_ft_define_cost[Hvar.num_facts_define_cost++] = pos;

  SET_BIT (Hvar.bit_vect_facts, pos);

}


int
insert_action_inlist (int pos)
{





  if (GET_BIT (Hvar.bit_vect_actions, pos) != 0)
    {
      return 0;

    }


  if (Hvar.num_actions_define_cost >= MAX_LENGTH_H)
    {
      exit (0);
    }



  Hvar.list_ef_define_cost[Hvar.num_actions_define_cost++] = pos;

  SET_BIT (Hvar.bit_vect_actions, pos);
  Hvar.cost_actions_define_cost += get_action_cost (pos);


  return 1;

}





float
dg_insertion_action_cost (int act_pos, int level, int temp_removed_action,
			  node_cost_list n_cost)
{
  register int i, *ptr;
  int diff, el;
  register EfConn *act;
  float total, prec_par, excl_par, add_effect_par, temp;
  dg_inform_list loc_dg_cost;
  int cost_unsup_num_fact = 1;



  prec_par = GpG.prec_par;
  excl_par = GpG.excl_par;
  add_effect_par = GpG.add_effect_par;

  total = 0.0;
  n_cost->weight = 0.0;
  n_cost->num_actions = 0;
  n_cost->act_cost = 0.0;
  n_cost->act_time = 0.0;

  act = &gef_conn[act_pos];

  if (DEBUG5)
    printf ("\n >>> DG_INSERTION  Act: %s, level %d ",
	    print_op_name_string (act_pos, temp_name), level);

  if (prec_par)
    {
      if (level <= temp_removed_action)
	{

	  for (i = 0, temp = 0.0; i < gef_conn[act_pos].num_PC; i++)
	    {
	      el = gef_conn[act_pos].PC[i];


	      if (el < 0)
		{
		  if (!is_num_prec_satisfied_in_common_level (el))
		    {
		      n_cost->weight += cost_unsup_num_fact;
		      n_cost->num_actions += 1;
		      n_cost->act_cost += 1;
		    }
		  continue;
		}

	      if (fact_is_supported (el, level))
		{
		  continue;
		}


	      if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST
		  && GET_BIT (Hvar.bit_vect_facts, el))
		{
		  if (n_cost->act_time <
		      CONVERT_FACT_TO_INFORM (el, level)->time_f)
		    n_cost->act_time =
		      CONVERT_FACT_TO_INFORM (el, level)->time_f;

		  continue;
		}



	      get_dg_fact_cost (el, level - 1, &loc_dg_cost);

	      if (loc_dg_cost == NULL)
		{
		  n_cost->weight = MAX_COST;
		  n_cost->num_actions = MAX_COST;
		  n_cost->act_time = MAX_COST;
		  return (MAX_COST);
		}
	      else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
		{
		  n_cost->weight += (float) loc_dg_cost->num_actions;
		  n_cost->num_actions += loc_dg_cost->num_actions;

		  n_cost->act_cost += loc_dg_cost->cost;
		  if (n_cost->act_time < loc_dg_cost->duration)
		    n_cost->act_time = loc_dg_cost->duration;
		}
	      else
		if (GpG.accurate_cost == COMPUTE_DG_MAX_COST
		    || GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
		{
		  if (n_cost->weight < (float) loc_dg_cost->num_actions)
		    n_cost->weight = (float) loc_dg_cost->num_actions;
		  if (n_cost->num_actions < loc_dg_cost->num_actions)
		    n_cost->num_actions = loc_dg_cost->num_actions;
		  if (n_cost->act_cost < loc_dg_cost->cost)
		    n_cost->act_cost = loc_dg_cost->cost;
		  if (n_cost->act_time < loc_dg_cost->duration)
		    n_cost->act_time = loc_dg_cost->duration;
		}

	    }



	  if (gef_conn[act_pos].sf != NULL)
	    {
	      for (i = 0, temp = 0.0;
		   i < gef_conn[act_pos].sf->num_PC_overall; i++)
		{
		  el = gef_conn[act_pos].sf->PC_overall[i];



		  if (el < 0)
		    {
		      if (!is_num_prec_satisfied_in_common_level (el))
			{

			  n_cost->weight += 1;
			  n_cost->num_actions += 1;
			  n_cost->act_cost += 1;
			}
		      continue;
		    }

		  if (is_fact_in_additive_effects_start
		      (act_pos, gef_conn[act_pos].sf->PC_overall[i]))
		    continue;

		  if (fact_is_supported (el, level))
		    {
		      if (n_cost->act_time <
			  CONVERT_FACT_TO_INFORM (el, level)->time_f)
			n_cost->act_time =
			  CONVERT_FACT_TO_INFORM (el, level)->time_f;

		      continue;
		    }


		  if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST
		      && GET_BIT (Hvar.bit_vect_facts, el))
		    {
		      continue;
		    }



		  get_dg_fact_cost (el, level - 1, &loc_dg_cost);

		  if (loc_dg_cost == NULL)
		    {
		      n_cost->weight = MAX_COST;
		      n_cost->num_actions = MAX_COST;
		      n_cost->act_time = MAX_COST;
		      return (MAX_COST);
		    }
		  else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
		    {
		      n_cost->weight += (float) loc_dg_cost->num_actions;
		      n_cost->num_actions += loc_dg_cost->num_actions;

		      n_cost->act_cost += loc_dg_cost->cost;
		      if (n_cost->act_time < loc_dg_cost->duration)
			n_cost->act_time = loc_dg_cost->duration;
		    }
		  else
		    if (GpG.accurate_cost == COMPUTE_DG_MAX_COST
			|| GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
		    {
		      if (n_cost->weight < (float) loc_dg_cost->num_actions)
			n_cost->weight = (float) loc_dg_cost->num_actions;
		      if (n_cost->num_actions < loc_dg_cost->num_actions)
			n_cost->num_actions = loc_dg_cost->num_actions;
		      if (n_cost->act_cost < loc_dg_cost->cost)
			n_cost->act_cost = loc_dg_cost->cost;
		      if (n_cost->act_time < loc_dg_cost->duration)
			n_cost->act_time = loc_dg_cost->duration;
		    }

		}



	      for (i = 0, temp = 0.0; i < gef_conn[act_pos].sf->num_PC_end;
		   i++)
		{
		  el = gef_conn[act_pos].sf->PC_end[i];


		  if (el < 0)
		    {
		      if (!is_num_prec_satisfied_in_common_level (el))
			{
			  n_cost->weight += 1;
			  n_cost->num_actions += 1;
			  n_cost->act_cost += 1;
			}
		      continue;
		    }

		  if (is_fact_in_additive_effects
		      (act_pos, gef_conn[act_pos].sf->PC_end[i]))
		    continue;

		  if (fact_is_supported (el, level))
		    {
		      continue;
		    }


		  if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST
		      && GET_BIT (Hvar.bit_vect_facts, el))
		    {
		      if (n_cost->act_time <
			  CONVERT_FACT_TO_INFORM (el, level)->time_f)
			n_cost->act_time =
			  CONVERT_FACT_TO_INFORM (el, level)->time_f;


		      continue;
		    }



		  get_dg_fact_cost (el, level - 1, &loc_dg_cost);

		  if (loc_dg_cost == NULL)
		    {
		      n_cost->weight = MAX_COST;
		      n_cost->num_actions = MAX_COST;
		      n_cost->act_time = MAX_COST;
		      return (MAX_COST);
		    }
		  else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
		    {
		      n_cost->weight += (float) loc_dg_cost->num_actions;
		      n_cost->num_actions += loc_dg_cost->num_actions;

		      n_cost->act_cost += loc_dg_cost->cost;
		      if (n_cost->act_time < loc_dg_cost->duration)
			n_cost->act_time = loc_dg_cost->duration;
		    }
		  else
		    if (GpG.accurate_cost == COMPUTE_DG_MAX_COST
			|| GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
		    {
		      if (n_cost->weight < (float) loc_dg_cost->num_actions)
			n_cost->weight = (float) loc_dg_cost->num_actions;
		      if (n_cost->num_actions < loc_dg_cost->num_actions)
			n_cost->num_actions = loc_dg_cost->num_actions;
		      if (n_cost->act_cost < loc_dg_cost->cost)
			n_cost->act_cost = loc_dg_cost->cost;
		      if (n_cost->act_time < loc_dg_cost->duration)
			n_cost->act_time = loc_dg_cost->duration;
		    }

		}

	    }

	}
      else
	for (i = 0, temp = 0; i < act->num_precond; i++)
	  temp +=
	    count_bit1 (act->bit_precond[i].uid_mask &
			(((~vectlevel[level]->
			   fact_vect[act->bit_precond[i].
				     uid_block]) & (~new_true_facts[act->
								    bit_precond
								    [i].
								    uid_block]))
			 | new_false_facts[act->bit_precond[i].uid_block]));



      total = prec_par * n_cost->weight;


      if (DEBUG5)
	printf ("\n Num. P: %.2f", n_cost->weight);



    }

  if (excl_par)
    {
      if (TRUE || level <= temp_removed_action)
	{
	  temp = (float) count_mutex_noop_at_start (act_pos, level);
	}
      else
	for (temp = 0.0, i = 0; i < gnum_ft_block; i++)
	  if (vectlevel[level]->prec_vect[i])
	    temp +=
	      (float) count_bit1 (CONVERT_ACTION_TO_VERTEX (act_pos)->
				  ft_exclusive_vect[i] &
				  (((vectlevel[level]->
				     fact_vect[i]) | new_true_facts[i]) &
				   (~new_false_facts[i])) & vectlevel[level]->
				  prec_vect[i]);

      total += excl_par * temp;

      if (DEBUG5)
	printf ("\n  M.E.: %.2f   ", temp);

    }







  if (add_effect_par)
    {
      level++;

      ptr = vectlevel[level]->false_crit_vect;


      if (level <= temp_removed_action)
	{
	  for (i = 0, temp = 0.0; i < act->num_add_effect; i++)
	    temp +=
	      (float) count_bit1 (act->bit_add_effect[i].
				  uid_mask & *(ptr +
					       act->bit_add_effect[i].
					       uid_block));

	  if (act->sf != NULL)
	    {
	      for (i = 0; i < act->sf->num_A_start; i++)
		if (act->sf->A_start[i] >= 0)
		  if (vectlevel[level + 1]->fact[act->sf->A_start[i]].
		      w_is_goal >= 1
		      && vectlevel[level +
				   1]->fact[act->sf->A_start[i]].w_is_true <=
		      0
		      && !is_fact_in_delete_effects (act_pos,
						     act->sf->A_start[i]))
		    temp++;
	    }

	}
      else
	for (i = 0, temp = 0.0; i < act->num_add_effect; i++)
	  temp +=
	    (float) count_bit1 (act->bit_add_effect[i].
				uid_mask &
				(*(ptr + act->bit_add_effect[i].uid_block) |
				 (vectlevel[level]->
				  true_crit_vect[act->bit_add_effect[i].
						 uid_block] &
				  new_false_facts[act->bit_add_effect[i].
						  uid_block])));

      total += add_effect_par * temp;
      if (DEBUG5)
	printf (" Add-E: %.2f,  ", temp);
    }


  if (GpG.local == 2)
    {
      diff = num_try - act->step;
      if (diff < GpG.tabu_length && diff > 0)
	total += GpG.delta * (GpG.tabu_length - diff);
      if (DEBUG6)
	printf (" tabu tot %f", total);
    }

  total++;
  n_cost->act_cost += get_action_cost (act_pos);
  n_cost->act_time += get_action_time (act_pos, 0);
  n_cost->weight = total;
  n_cost->num_actions++;


  if (DEBUG3)
    printf (" tot %f", total);
  if (DEBUG5)
    printf (" -> tot %f\n", total);


  return (total);

}















float
dg_action_cost (neighb_list neighb_act)
{
  register int temp1, k, unsat_facts = 0;
  int level, i, j, k1, diff = 0, next_level, act_pos, init_w, init_na, ord;

  register EfConn *act;
  auto float total, prec_par, excl_par, add_effect_par, temp = 0.0, prec =
    0.0, eff;
  inform_list inf_fact;
  int el = 0, fact_pos, ind_level;
  auto float precond, mutex, effect, act_cost;
  node_cost loc_n_cost, best_prec_cost, best_eff_cost, best_mutex_cost;
  float duration, act_time, act_time2, init_cost;
  int act_mutex;



  if (Hvar.estimate_time_facts == NULL)
    Hvar.estimate_time_facts =
      (float *) calloc (gnum_ft_conn, sizeof (float));

  memset (Hvar.estimate_time_facts, 0, gnum_ft_conn * sizeof (float));



  precond = mutex = effect = act_cost = 0.0;
  Hvar.cost_actions_define_cost = 0.0;
  Hvar.time_actions_define_cost = 0.0;

  best_prec_cost.weight = 0.0;

  best_prec_cost.act_cost = 0.0;

  best_prec_cost.act_time = 0.0;


  best_mutex_cost.weight = 0.0;

  best_mutex_cost.act_cost = 0.0;

  best_mutex_cost.act_time = 0.0;


  best_eff_cost.weight = 0.0;

  best_eff_cost.act_cost = 0.0;

  best_eff_cost.act_time = 0.0;



  Hvar.min_start_time = MAXFLOAT;
  local_search.ls_continue = TRUE;


  level = neighb_act->act_level;
  act = &gef_conn[neighb_act->act_pos];


  loc_n_cost.weight = 0.0;

  loc_n_cost.act_cost = 0.0;

  loc_n_cost.act_time = 0.0;



  total = 0.0;


  if (GpG.lm_multilevel)
    {
      local_search.lamda_prec = vectlevel[level]->lambda_prec[act->position];
      local_search.lamda_me = vectlevel[level]->lambda_me[act->position];
    }

  else
    {
      local_search.lamda_prec = act->lamda_prec;
      local_search.lamda_me = act->lamda_me;
    }

  if (neighb_act->constraint_type == C_T_REMOVE_ACTION)
    {

      neighb_act->cost.act_cost = 0.0;
      neighb_act->cost.act_time = 0.0;

      if (GpG.maximize_plan || GpG.H_positive_benefits)
	{
	  Hvar.cost_actions_define_cost -=
	    get_action_cost (neighb_act->act_pos);

	  neighb_act->cost.act_cost -= get_action_cost (neighb_act->act_pos);
	}

      prec_par = GpG.used_prec_par;
      excl_par = GpG.used_excl_par;
      add_effect_par = GpG.used_add_effect_par;
      Hvar.temp_removed_act = neighb_act->act_pos;

    }
  else
    {


      neighb_act->cost.act_cost = 0.0;

      neighb_act->cost.act_time =
	get_action_time (neighb_act->act_pos, level);



      prec_par = GpG.prec_par;
      excl_par = GpG.excl_par;
      add_effect_par = GpG.add_effect_par;

      Hvar.temp_removed_act = -1;

      if (GpG.temporal_plan)
	for (ind_level = level; ind_level >= 0; ind_level--)
	  {
	    if (vectlevel[ind_level]->action.w_is_used <= 0)
	      continue;

	    if (check_mutex_action (act->position, ind_level) >= 0)
	      {
		if (GpG.constraint_type <= 1)
		  {
		    if (best_prec_cost.act_time <
			GET_ACTION_OF_LEVEL (ind_level)->time_f)
		      best_prec_cost.act_time =
			GET_ACTION_OF_LEVEL (ind_level)->time_f;
		  }
		else
		  {

		    ord =
		      constraint_type (act->position,
				       GET_ACTION_POSITION_OF_LEVEL
				       (ind_level));

		    if (ord == EA_SB)
		      {
			if (best_prec_cost.act_time <
			    GET_ACTION_OF_LEVEL (ind_level)->time_f)
			  best_prec_cost.act_time =
			    GET_ACTION_OF_LEVEL (ind_level)->time_f;
		      }
		    else
		      {
			if (ord == EA_EB)
			  {
			    if (best_prec_cost.act_time <
				GET_ACTION_OF_LEVEL (ind_level)->time_f -
				get_action_time (act->position, level))
			      best_prec_cost.act_time =
				GET_ACTION_OF_LEVEL (ind_level)->time_f -
				get_action_time (act->position, level);
			  }
			else if (ord == SA_SB)
			  {
			    if (best_prec_cost.act_time <
				GET_ACTION_OF_LEVEL (ind_level)->time_f -
				get_action_time (GET_ACTION_POSITION_OF_LEVEL
						 (ind_level), ind_level))
			      best_prec_cost.act_time =
				GET_ACTION_OF_LEVEL (ind_level)->time_f -
				get_action_time (GET_ACTION_POSITION_OF_LEVEL
						 (ind_level), ind_level);
			  }
			else
			  {
			    if (ord == SA_EB)
			      {
				if (best_prec_cost.act_time <
				    GET_ACTION_OF_LEVEL (ind_level)->time_f -
				    get_action_time
				    (GET_ACTION_POSITION_OF_LEVEL (ind_level),
				     ind_level) -
				    get_action_time (act->position, level))
				  best_prec_cost.act_time =
				    GET_ACTION_OF_LEVEL (ind_level)->time_f -
				    get_action_time
				    (GET_ACTION_POSITION_OF_LEVEL (ind_level),
				     ind_level) -
				    get_action_time (act->position, level);
			      }
			    else
			      {
				if (ord == EA_EB__SA_SB)
				  {
				    if (get_action_time (act->position, level)
					<
					get_action_time
					(GET_ACTION_POSITION_OF_LEVEL
					 (ind_level), ind_level))
				      {

					if (best_prec_cost.act_time <
					    GET_ACTION_OF_LEVEL (ind_level)->
					    time_f -
					    get_action_time (act->position,
							     level))
					  best_prec_cost.act_time =
					    GET_ACTION_OF_LEVEL (ind_level)->
					    time_f -
					    get_action_time (act->position,
							     level);
				      }
				    else
				      {
					if (best_prec_cost.act_time <
					    GET_ACTION_OF_LEVEL (ind_level)->
					    time_f -
					    get_action_time
					    (GET_ACTION_POSITION_OF_LEVEL
					     (ind_level), ind_level))
					  best_prec_cost.act_time =
					    GET_ACTION_OF_LEVEL (ind_level)->
					    time_f -
					    get_action_time
					    (GET_ACTION_POSITION_OF_LEVEL
					     (ind_level), ind_level);

				      }
				  }
			      }
			  }
		      }
		  }
	      }
	  }
    }


  if (DEBUG3)
    {
      if (DEBUG4)
	printf ("\n\nEvalutate action");
      printf ("\nDG_ACTION Act: %s, level %d\n  ",
	      print_op_name_string (act->position, temp_name), level);

    }


  if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
    {


      Hvar.num_actions_define_cost = 0;
      Hvar.num_facts_define_cost = 0;
      Hvar.weight_facts_define_cost = 0.0;


      reset_bitarray (Hvar.bit_vect_facts, gnum_ft_block);
      reset_bitarray (Hvar.bit_vect_actions, gnum_ef_block);

    }



  if (prec_par)
    {

      unsat_facts = 0;

      for (prec = 0.0, diff = 0, j = 0;
	   j < gef_conn[act->position].num_PC && local_search.ls_continue;
	   j++)
	{
	  el = gef_conn[act->position].PC[j];

	  if (CHECK_FACT_POS (el, level))
	    {
	      if (DEBUG4)
		{
		  printf ("\n\n %d +++++ DG_MAX_COST Prec_start  Act %s",
			  ++diff, print_op_name_string (act->position,
							temp_name));
		  printf ("\n  fact %s\n",
			  print_ft_name_string (el, temp_name));
		}


	      loc_n_cost.weight = 0.0;

	      loc_n_cost.act_cost = 0.0;

	      loc_n_cost.act_time = 0.0;




	      if (el < 0)
		{
		  if (is_num_prec_satisfied_in_common_level (el))
		    continue;
		}
	      else if (fact_is_supported (el, level))
		{

		  if (DEBUG4)
		    printf ("\n Level %d Supported fact %s ", level,
			    print_ft_name_string (el, temp_name));


		  inf_fact = CONVERT_FACT_TO_INFORM (el, level);



		  if (GpG.temporal_plan)
		    {
		      if (best_prec_cost.act_time < inf_fact->time_f)
			{
			  best_prec_cost.act_time = inf_fact->time_f;

			  if (Hvar.time_actions_define_cost <
			      best_prec_cost.act_time)
			    Hvar.time_actions_define_cost =
			      best_prec_cost.act_time;


			}

		    }


		  continue;
		}



	      if (el >= 0 && GpG.accurate_cost >= COMPUTE_DG_LIST_COST
		  && GET_BIT (Hvar.bit_vect_facts, el))
		{
		  continue;
		}

	      temp = compute_dg_facts_cost (el, level, &loc_n_cost, level);




	      if (el >= 0 && GpG.accurate_cost == COMPUTE_DG_MAX_COST
		  && CONVERT_FACT_TO_INFORM (el, level)->w_is_true <= 0)
		unsat_facts++;

	      if (DEBUG4)
		{
		  printf ("\n\n %d +++ END DG_MAX_COST Prec_start  Act %s ",
			  ++diff, print_op_name_string (act->position,
							temp_name));
		  printf
		    ("\n  fact %s, COST  weight %f cost %f time %f unsatisf_prec %d ",
		     print_ft_name_string (el, temp_name), loc_n_cost.weight,
		     loc_n_cost.act_cost, loc_n_cost.act_time, unsat_facts);
		}

	      if (GpG.accurate_cost == COMPUTE_DG_MAX_COST)
		{

		  if (best_prec_cost.act_cost < loc_n_cost.act_cost
		      && loc_n_cost.act_cost < MAX_COST)
		    best_prec_cost.act_cost = loc_n_cost.act_cost;


		  if (best_prec_cost.act_time < loc_n_cost.act_time
		      && loc_n_cost.act_time < MAX_COST)
		    best_prec_cost.act_time = loc_n_cost.act_time;


		  if (prec < temp && temp < MAX_COST)
		    {

		      prec = temp;
		      best_prec_cost.weight = loc_n_cost.weight;


		    }
		}
	      else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
		{
		  prec += temp;

		  best_prec_cost.weight += loc_n_cost.weight;

		  best_prec_cost.act_cost += loc_n_cost.act_cost;

		  best_prec_cost.act_time += loc_n_cost.act_time;


		}
	      else if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
		{
		  prec += temp;





		  if (best_prec_cost.act_time < loc_n_cost.act_time
		      && loc_n_cost.act_time < MAX_COST)
		    best_prec_cost.act_time = loc_n_cost.act_time;


		}

	    }
	  if (Hvar.time_actions_define_cost < best_prec_cost.act_time)
	    Hvar.time_actions_define_cost = best_prec_cost.act_time;



	}




      if (gef_conn[act->position].sf != NULL && local_search.ls_continue)
	{
	  for (j = 0;
	       j < gef_conn[act->position].sf->num_PC_overall
	       && local_search.ls_continue; j++)
	    {
	      el = gef_conn[act->position].sf->PC_overall[j];


	      if (is_fact_in_additive_effects_start (act->position, el))
		continue;

	      if (CHECK_FACT_POS (el, level))
		{
		  if (DEBUG4)
		    {
		      printf
			("\n\n %d +++++ DG_MAX_COST Prec_overall  Act %s",
			 ++diff, print_op_name_string (act->position,
						       temp_name));
		      printf ("\n  fact %s\n",
			      print_ft_name_string (el, temp_name));
		    }

		  loc_n_cost.weight = 0.0;

		  loc_n_cost.act_cost = 0.0;

		  loc_n_cost.act_time = 0.0;



		  if (el < 0)
		    {
		      if (is_num_prec_satisfied_in_common_level (el))
			continue;
		    }
		  else if (fact_is_supported (el, level))
		    {
		      inf_fact = CONVERT_FACT_TO_INFORM (el, level);

		      if (GpG.temporal_plan)
			{
			  if (best_prec_cost.act_time < inf_fact->time_f)
			    {
			      best_prec_cost.act_time = inf_fact->time_f;

			    }


			  if (Hvar.time_actions_define_cost <
			      best_prec_cost.act_time)
			    Hvar.time_actions_define_cost =
			      best_prec_cost.act_time;



			}

		      continue;
		    }



		  if (el >= 0 && GpG.accurate_cost >= COMPUTE_DG_LIST_COST
		      && GET_BIT (Hvar.bit_vect_facts, el))
		    {
		      continue;
		    }

		  temp =
		    compute_dg_facts_cost (el, level, &loc_n_cost, level);


		  if (el >= 0 && GpG.accurate_cost == COMPUTE_DG_MAX_COST
		      && CONVERT_FACT_TO_INFORM (el, level)->w_is_true <= 0)
		    unsat_facts++;
		  if (DEBUG4)
		    {
		      printf
			("\n\n %d +++ END DG_MAX_COST Prec_overall  Act %s ",
			 ++diff, print_op_name_string (act->position,
						       temp_name));
		      printf
			("\n  fact %s, COST  weight %f cost %f time %f unsatisf_prec %d ",
			 print_ft_name_string (el, temp_name),
			 loc_n_cost.weight, loc_n_cost.act_cost,
			 loc_n_cost.act_time, unsat_facts);


		    }

		  if (GpG.accurate_cost == COMPUTE_DG_MAX_COST)
		    {

		      if (best_prec_cost.act_cost < loc_n_cost.act_cost
			  && loc_n_cost.act_cost < MAX_COST)
			best_prec_cost.act_cost = loc_n_cost.act_cost;


		      if (best_prec_cost.act_time < loc_n_cost.act_time
			  && loc_n_cost.act_time < MAX_COST)
			best_prec_cost.act_time = loc_n_cost.act_time;


		      if (prec < temp && temp < MAX_COST)
			{

			  prec = temp;
			  best_prec_cost.weight = loc_n_cost.weight;


			}
		    }
		  else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
		    {
		      prec += temp;

		      best_prec_cost.weight += loc_n_cost.weight;

		      best_prec_cost.act_cost += loc_n_cost.act_cost;

		      best_prec_cost.act_time += loc_n_cost.act_time;


		    }
		  else if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
		    {
		      prec += temp;





		      if (best_prec_cost.act_time < loc_n_cost.act_time
			  && loc_n_cost.act_time < MAX_COST)
			best_prec_cost.act_time = loc_n_cost.act_time;


		    }

		}

	      if (Hvar.time_actions_define_cost < best_prec_cost.act_time)
		Hvar.time_actions_define_cost = best_prec_cost.act_time;



	    }


	  for (j = 0;
	       j < gef_conn[act->position].sf->num_PC_end
	       && local_search.ls_continue; j++)
	    {
	      el = gef_conn[act->position].sf->PC_end[j];




	      if (is_fact_in_additive_effects_start (act->position, el))
		continue;


	      if (CHECK_FACT_POS (el, level))
		{
		  if (DEBUG4)
		    {
		      printf ("\n\n %d +++++ DG_MAX_COST Prec_end  Act %s",
			      ++diff, print_op_name_string (act->position,
							    temp_name));
		      printf ("\n  fact %s\n",
			      print_ft_name_string (el, temp_name));
		    }

		  loc_n_cost.weight = 0.0;

		  loc_n_cost.act_cost = 0.0;

		  loc_n_cost.act_time = 0.0;




		  if (el < 0)
		    {
		      if (is_num_prec_satisfied_in_common_level (el))
			continue;
		    }
		  else if (fact_is_supported (el, level))
		    {
		      inf_fact = CONVERT_FACT_TO_INFORM (el, level);
		      if (GpG.temporal_plan)
			{
			  if (best_prec_cost.act_time < inf_fact->time_f)
			    {
			      best_prec_cost.act_time = inf_fact->time_f;

			    }


			  if (Hvar.time_actions_define_cost <
			      best_prec_cost.act_time)
			    Hvar.time_actions_define_cost =
			      best_prec_cost.act_time;


			}


		      continue;
		    }



		  if (el >= 0 && GpG.accurate_cost >= COMPUTE_DG_LIST_COST
		      && GET_BIT (Hvar.bit_vect_facts, el))
		    {
		      continue;
		    }


		  temp =
		    compute_dg_facts_cost (el, level, &loc_n_cost, level);




		  if (el >= 0 && GpG.accurate_cost == COMPUTE_DG_MAX_COST
		      && CONVERT_FACT_TO_INFORM (el, level)->w_is_true <= 0)
		    unsat_facts++;

		  if (DEBUG4)
		    {
		      printf ("\n\n %d +++ END DG_MAX_COST Prec_end  Act %s ",
			      ++diff, print_op_name_string (act->position,
							    temp_name));
		      printf
			("\n  fact %s, COST  weight %f cost %f time %f unsatisf_prec %d ",
			 print_ft_name_string (el, temp_name),
			 loc_n_cost.weight, loc_n_cost.act_cost,
			 loc_n_cost.act_time, unsat_facts);


		    }
		  if (GpG.accurate_cost == COMPUTE_DG_MAX_COST)
		    {

		      if (best_prec_cost.act_cost < loc_n_cost.act_cost
			  && loc_n_cost.act_cost < MAX_COST)
			best_prec_cost.act_cost = loc_n_cost.act_cost;


		      if (best_prec_cost.act_time < loc_n_cost.act_time
			  && loc_n_cost.act_time < MAX_COST)
			best_prec_cost.act_time = loc_n_cost.act_time;


		      if (prec < temp && temp < MAX_COST)
			{

			  prec = temp;
			  best_prec_cost.weight = loc_n_cost.weight;


			}
		    }
		  else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
		    {
		      prec += temp;

		      best_prec_cost.weight += loc_n_cost.weight;

		      best_prec_cost.act_cost += loc_n_cost.act_cost;

		      best_prec_cost.act_time += loc_n_cost.act_time;


		    }
		  else if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
		    {
		      prec += temp;





		      if (best_prec_cost.act_time < loc_n_cost.act_time
			  && loc_n_cost.act_time < MAX_COST)
			best_prec_cost.act_time = loc_n_cost.act_time;


		    }

		}

	      if (Hvar.time_actions_define_cost < best_prec_cost.act_time)
		Hvar.time_actions_define_cost = best_prec_cost.act_time;



	    }
	}










      total = prec_par * prec;

      precond = local_search.lamda_prec * total;


      if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
	{

	  if (GpG.lagrange_multipl)
	    best_prec_cost.weight =
	      (Hvar.weight_facts_define_cost +
	       Hvar.num_actions_define_cost) * local_search.lamda_prec;
	  else
	    best_prec_cost.weight =
	      Hvar.weight_facts_define_cost + Hvar.num_actions_define_cost;

	  best_prec_cost.act_cost = Hvar.cost_actions_define_cost;


	  best_prec_cost.num_actions = Hvar.num_actions_define_cost;

	}
      if (DEBUG3)
	{
	  printf ("  Temp Num. P: %f weight %.2f ", prec,
		  best_prec_cost.weight);

	  if (DEBUG4)
	    printf ("\n\n<< Evalutate precondition END");

	}
    }




  if (excl_par)
    {

      init_w = Hvar.weight_facts_define_cost;
      init_na = Hvar.num_actions_define_cost;
      init_cost = Hvar.cost_actions_define_cost;

      if (DEBUG4)
	{
	  printf ("\n\n %d +++++  DG_MAX_COST Mutex  Act :", ++diff);
	  print_op_name (act->position);
	}





      if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
	{

	  for (j = 0; j < gef_conn[act->position].num_A; j++)
	    {
	      el = gef_conn[act->position].A[j];
	      if (el < 0)
		{

		  apply_numeric_effect_in_common_level (el);
		  continue;
		}



	      SET_BIT (Hvar.bit_vect_facts, el);

	    }

	  if (gef_conn[act->position].sf)
	    for (j = 0; j < gef_conn[act->position].sf->num_A_start; j++)
	      {
		el = gef_conn[act->position].sf->A_start[j];
		if (el < 0)
		  {

		    apply_numeric_effect_in_common_level (el);
		    continue;
		  }

		if (is_fact_in_delete_effects (act->position, el))
		  continue;


		SET_BIT (Hvar.bit_vect_facts, el);

	      }




	}





      temp = 0.0;



      for (i = 0, j = 0; j < gnum_ft_conn; i++, j += 32)
	{
	  int lev_iv;
	  if (level >= GpG.curr_plan_length)
	    lev_iv = level - 1;
	  else
	    lev_iv = level;

	  temp1 =
	    act->ft_exclusive_vect[i] & vectlevel[lev_iv]->
	    prec_vect[i] & (vectlevel[lev_iv]->fact_vect[i]);


	  k = 32;
	  while (temp1)
	    {
	      k--;
	      if (temp1 & FIRST_1)
		{
		  k1 = j + k;
		  temp +=
		    compute_dg_facts_cost (k1, level, &loc_n_cost, level);


		  if (GpG.accurate_cost == COMPUTE_DG_MAX_COST)
		    {

		      if (best_mutex_cost.act_cost < loc_n_cost.act_cost
			  && loc_n_cost.act_cost < MAX_COST)
			best_mutex_cost.act_cost = loc_n_cost.act_cost;


		      if (best_mutex_cost.act_time < loc_n_cost.act_time
			  && loc_n_cost.act_time < MAX_COST)
			best_mutex_cost.act_time = loc_n_cost.act_time;


		      if (prec < temp && temp < MAX_COST)
			{

			  prec = temp;
			  best_mutex_cost.weight = loc_n_cost.weight;

			  if (FALSE
			      && (temp *=
				  local_search.lamda_prec) >
			      local_search.best_cost)
			    return temp;

			}
		    }
		  else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
		    {
		      prec += temp;

		      best_mutex_cost.weight += loc_n_cost.weight;

		      best_mutex_cost.act_cost += loc_n_cost.act_cost;

		      best_mutex_cost.act_time += loc_n_cost.act_time;


		    }
		  else if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
		    {
		      prec += temp;

		      best_mutex_cost.weight += loc_n_cost.weight;

		      best_mutex_cost.act_cost += loc_n_cost.act_cost;

		      if (best_mutex_cost.act_time < loc_n_cost.act_time
			  && loc_n_cost.act_time < MAX_COST)
			best_mutex_cost.act_time = loc_n_cost.act_time;


		    }

		  if (DEBUG4)
		    {
		      printf ("\n\n %d +++ END DG_MAX_COST Mutex  Act %s",
			      ++diff, print_op_name_string (act->position,
							    temp_name));
		      printf
			("\n  fact %s, COST  weight %f cost %f time %f",
			 print_ft_name_string (k1, temp_name),
			 loc_n_cost.weight, loc_n_cost.act_cost,
			 loc_n_cost.act_time);
		    }
		}
	      temp1 <<= 1;
	    }

	}




      mutex = excl_par * best_mutex_cost.weight;
      total += mutex;
      mutex *= local_search.lamda_me;


      if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
	{

	  if (GpG.lagrange_multipl)
	    best_mutex_cost.weight =
	      (Hvar.weight_facts_define_cost - init_w +
	       Hvar.num_actions_define_cost -
	       init_na) * local_search.lamda_me;
	  else
	    best_mutex_cost.weight =
	      Hvar.weight_facts_define_cost - init_w +
	      Hvar.num_actions_define_cost - init_na;

	  best_mutex_cost.act_cost =
	    Hvar.cost_actions_define_cost - init_cost;

	}

    }




  if (DEBUG3)
    {
      printf ("  Temp  M.E.: %f", temp);

      if (DEBUG4)
	printf ("\n\n<< Evalutate mutex END");

    }





  if (add_effect_par)
    {


      init_w = Hvar.weight_facts_define_cost;
      init_na = Hvar.num_actions_define_cost;
      init_cost = Hvar.cost_actions_define_cost;

      local_search.lamda_prec = MIN_POS_S_S;


      next_level = level + 1;
      if (neighb_act->constraint_type == C_T_REMOVE_ACTION)
	{

	  unsat_facts = 0;

	  remove_temp_action (neighb_act->act_pos, neighb_act->act_level);

	}
      eff = 0.0;
      for (i = 0;
	   i < gef_conn[neighb_act->act_pos].num_A
	   && local_search.ls_continue; i++)
	{
	  fact_pos = gef_conn[neighb_act->act_pos].A[i];


	  if (fact_pos < 0)
	    continue;

	  loc_n_cost.weight = 0.0;
	  loc_n_cost.act_cost = 0.0;
	  loc_n_cost.act_time = 0.0;


	  if (fact_pos > 0)
	    {
	      for (j = neighb_act->act_level; j < GpG.curr_plan_length; j++)
		if (CONVERT_FACT_TO_INFORM (fact_pos, j)->w_is_used)
		  break;

	      if (GET_ACTION_POSITION_OF_LEVEL (j) >= 0)
		{
		  if (j == GpG.curr_plan_length)
		    {
		      local_search.lamda_prec =
			MAX (local_search.lamda_prec, GpG.goal_lambda);
		    }
		  else
		    {
		      if (GpG.lm_multilevel)
			local_search.lamda_prec =
			  MAX (local_search.lamda_prec,
			       vectlevel[j]->
			       lambda_prec[GET_ACTION_POSITION_OF_LEVEL (j)]);
		      else
			local_search.lamda_prec =
			  MAX (local_search.lamda_prec,
			       CONVERT_ACTION_TO_VERTEX
			       (GET_ACTION_POSITION_OF_LEVEL (j))->
			       lamda_prec);

		    }
		}
	    }
	  if (neighb_act->constraint_type == C_T_REMOVE_ACTION)
	    {

	      if (CONVERT_FACT_TO_INFORM (fact_pos, next_level)->w_is_true ==
		  1
		  && CONVERT_FACT_TO_INFORM (fact_pos, next_level)->w_is_goal)
		{


		  if (FALSE)
		    for (next_level = level + 1;
			 next_level <= GpG.curr_plan_length; next_level++)
		      if ((CONVERT_FACT_TO_INFORM (fact_pos, next_level)->
			   w_is_used))
			break;

		  if (next_level > GpG.curr_plan_length)
		    {
		      continue;

		    }
		  if (DEBUG4)
		    {
		      printf ("\n\n %d +++++ DG_MAX_COST End_eff  Act %s",
			      ++diff, print_op_name_string (act->position,
							    temp_name));
		      printf ("\n  fact %s\n",
			      print_ft_name_string (fact_pos, temp_name));
		    }




		  if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST
		      && GET_BIT (Hvar.bit_vect_facts, fact_pos))
		    {

		      if (DEBUG4)
			printf
			  ("\nFact %s already supported in COMPUTE_DG_LIST_COST, level %d",
			   print_ft_name_string (fact_pos, temp_name), level);


		      continue;
		    }
		  temp =
		    compute_dg_facts_cost (fact_pos, next_level, &loc_n_cost,
					   level);


		  if (GpG.accurate_cost == COMPUTE_DG_MAX_COST
		      && CONVERT_FACT_TO_INFORM (fact_pos,
						 next_level)->w_is_true <= 1)
		    unsat_facts++;


		}

	    }
	  else
	    {
	      if (CONVERT_FACT_TO_INFORM (fact_pos, next_level)->w_is_true ==
		  0
		  && CONVERT_FACT_TO_INFORM (fact_pos, next_level)->w_is_goal)
		{

		  if (DEBUG4)
		    {
		      printf ("\n\n %d +++++ DG_MAX_COST End_eff  Act %s ",
			      ++diff, print_op_name_string (act->position,
							    temp_name));
		      printf ("\n  fact %s\n",
			      print_ft_name_string (fact_pos, temp_name));
		    }



		  if (fact_is_supported (fact_pos, next_level))
		    {

		      if (DEBUG4)
			printf ("\n  Supported fact %s, level %d",
				print_ft_name_string (fact_pos, temp_name),
				next_level);


		      continue;
		    }



		  if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST
		      && GET_BIT (Hvar.bit_vect_facts, fact_pos))
		    {

		      if (DEBUG4)
			printf
			  ("\nFact %s already supported in COMPUTE_DG_LIST_COST, level %d",
			   print_ft_name_string (el, temp_name), next_level);



		      continue;
		    }
		  temp =
		    compute_dg_facts_cost (fact_pos, next_level, &loc_n_cost,
					   next_level);



		}
	    }



	  if (DEBUG4)
	    {
	      printf ("\n\n %d +++ DG_MAX_COST End_eff  Act %s ", ++diff,
		      print_op_name_string (neighb_act->act_pos, temp_name));
	      printf ("\n  fact %s, COST  weight %f cost %f time %f",
		      print_ft_name_string (fact_pos, temp_name),
		      loc_n_cost.weight, loc_n_cost.act_cost,
		      loc_n_cost.act_time);
	    }

	  if (GpG.accurate_cost == COMPUTE_DG_MAX_COST)
	    {
	      if (best_eff_cost.act_cost < loc_n_cost.act_cost
		  && loc_n_cost.act_cost < MAX_COST)
		best_eff_cost.act_cost = loc_n_cost.act_cost;


	      if (best_eff_cost.act_time < loc_n_cost.act_time
		  && loc_n_cost.act_time < MAX_COST)
		best_eff_cost.act_time = loc_n_cost.act_time;


	      if (eff < temp && temp < MAX_COST)
		{
		  eff = temp;

		  best_eff_cost.weight = loc_n_cost.weight;


		  effect = temp * local_search.lamda_prec;


		}
	    }
	  else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
	    {


	      best_eff_cost.act_cost += loc_n_cost.act_cost;

	      best_eff_cost.act_time += loc_n_cost.act_time;

	      eff += temp;

	      best_eff_cost.weight += loc_n_cost.weight;

	      effect += temp * local_search.lamda_prec;

	    }

	  else if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
	    {


	      best_eff_cost.act_cost += loc_n_cost.act_cost;



	      eff += temp;


	      if (best_eff_cost.act_time < loc_n_cost.act_time
		  && loc_n_cost.act_time < MAX_COST)
		best_eff_cost.act_time = loc_n_cost.act_time;

	      effect += temp * local_search.lamda_prec;

	    }

	}






      if (gef_conn[neighb_act->act_pos].sf != NULL)
	{
	  for (i = 0;
	       i < gef_conn[neighb_act->act_pos].sf->num_A_start
	       && local_search.ls_continue; i++)
	    {
	      fact_pos = gef_conn[neighb_act->act_pos].sf->A_start[i];

	      if (is_fact_in_delete_effects (neighb_act->act_pos, fact_pos))
		continue;



	      if (fact_pos < 0)
		continue;

	      loc_n_cost.weight = 0.0;
	      loc_n_cost.act_cost = 0.0;
	      loc_n_cost.act_time = 0.0;





	      if (neighb_act->constraint_type == C_T_REMOVE_ACTION)
		{

		  if (CONVERT_FACT_TO_INFORM (fact_pos, next_level)->
		      w_is_true == 1
		      && CONVERT_FACT_TO_INFORM (fact_pos,
						 next_level)->w_is_goal)
		    {


		      if (FALSE)
			for (next_level = level + 1;
			     next_level <= GpG.curr_plan_length; next_level++)
			  if ((CONVERT_FACT_TO_INFORM (fact_pos, next_level)->
			       w_is_used))
			    break;

		      if (next_level > GpG.curr_plan_length)
			{
			  if (DEBUG3)
			    MSG_ERROR ("Fact precondition of NO action");
			  continue;

			}
		      if (DEBUG4)
			{
			  printf
			    ("\n\n %d +++++ DG_MAX_COST Start_eff  Act %s ",
			     ++diff, print_op_name_string (act->position,
							   temp_name));
			  printf ("\n  fact %s\n",
				  print_ft_name_string (fact_pos, temp_name));

			}







		      if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST
			  && GET_BIT (Hvar.bit_vect_facts, fact_pos))
			{

			  if (DEBUG4)
			    printf
			      ("\nFact %s already supported in COMPUTE_DG_LIST_COST, level %d",
			       print_ft_name_string (el, temp_name), level);


			  continue;
			}
		      temp =
			compute_dg_facts_cost (fact_pos, level, &loc_n_cost,
					       level);


		      if (GpG.accurate_cost == COMPUTE_DG_MAX_COST
			  && CONVERT_FACT_TO_INFORM (fact_pos,
						     next_level)->w_is_true <=
			  1)
			unsat_facts++;


		    }

		}
	      else
		{
		  if (CONVERT_FACT_TO_INFORM (fact_pos, next_level)->
		      w_is_true == 0
		      && CONVERT_FACT_TO_INFORM (fact_pos,
						 next_level)->w_is_goal)
		    {


		      if (DEBUG4)
			{
			  printf
			    ("\n\n %d +++++ DG_MAX_COST Start_eff  Act %s ",
			     ++diff, print_op_name_string (act->position,
							   temp_name));
			  printf ("\n  fact %s\n",
				  print_ft_name_string (fact_pos, temp_name));
			}


		      if (fact_is_supported (fact_pos, next_level))
			{

			  if (DEBUG4)
			    printf ("\n  Supported fact %s, level %d",
				    print_ft_name_string (fact_pos,
							  temp_name),
				    next_level);



			  continue;
			}



		      if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST
			  && GET_BIT (Hvar.bit_vect_facts, fact_pos))
			{

			  if (DEBUG4)
			    printf
			      ("\nFact %s already supported in COMPUTE_DG_LIST_COST, level %d",
			       print_ft_name_string (el, temp_name),
			       next_level);


			  continue;
			}
		      temp =
			compute_dg_facts_cost (fact_pos, next_level,
					       &loc_n_cost, next_level);



		    }
		}

	      if (DEBUG4)
		{
		  printf ("\n\n %d +++++ DG_MAX_COST Start_eff  Act %s ",
			  ++diff, print_op_name_string (neighb_act->act_pos,
							temp_name));
		  printf ("\n  fact %s, COST  weight %f cost %f time %f",
			  print_ft_name_string (fact_pos, temp_name),
			  loc_n_cost.weight, loc_n_cost.act_cost,
			  loc_n_cost.act_time);
		}

	      if (GpG.accurate_cost == COMPUTE_DG_MAX_COST)
		{
		  if (best_eff_cost.act_cost < loc_n_cost.act_cost
		      && loc_n_cost.act_cost < MAX_COST)
		    best_eff_cost.act_cost = loc_n_cost.act_cost;


		  if (best_eff_cost.act_time < loc_n_cost.act_time
		      && loc_n_cost.act_time < MAX_COST)
		    best_eff_cost.act_time = loc_n_cost.act_time;


		  if (eff < temp && temp < MAX_COST)
		    {
		      eff = temp;

		      best_eff_cost.weight = loc_n_cost.weight;


		      effect = temp * local_search.lamda_prec;


		    }
		}
	      else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
		{


		  best_eff_cost.act_cost += loc_n_cost.act_cost;

		  best_eff_cost.act_time += loc_n_cost.act_time;

		  eff += temp;

		  best_eff_cost.weight += loc_n_cost.weight;

		  effect += temp * local_search.lamda_prec;

		}

	      else if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
		{


		  best_eff_cost.act_cost += loc_n_cost.act_cost;



		  eff += temp;


		  if (best_eff_cost.act_time < loc_n_cost.act_time
		      && loc_n_cost.act_time < MAX_COST)
		    best_eff_cost.act_time = loc_n_cost.act_time;

		  effect += temp * local_search.lamda_prec;

		}

	    }


	}




      total += add_effect_par * eff;
      effect *= add_effect_par;




      if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
	{
	  if (GpG.lagrange_multipl)
	    best_eff_cost.weight =
	      (Hvar.weight_facts_define_cost - init_w +
	       Hvar.num_actions_define_cost -
	       init_na) * local_search.lamda_prec;
	  else
	    best_eff_cost.weight =
	      Hvar.weight_facts_define_cost - init_w +
	      Hvar.num_actions_define_cost - init_na;

	  best_eff_cost.act_cost = Hvar.cost_actions_define_cost - init_cost;


	  best_eff_cost.num_actions = Hvar.num_actions_define_cost - init_na;

	}



      if (DEBUG3)
	{
	  printf ("   Temp Add-E: %f, effects %f ", temp, effect);
	  if (DEBUG4)
	    printf ("\n\n<< Evalutate effect END");
	}
    }



  if (GpG.num_solutions <= 0
      || neighb_act->constraint_type == C_T_INSERT_ACTION)
    {
      init_w = Hvar.weight_facts_define_cost;
      init_na = Hvar.num_actions_define_cost;
      init_cost = Hvar.cost_actions_define_cost;


      insert_action_inlist (neighb_act->act_pos);



      best_eff_cost.weight +=
	(Hvar.weight_facts_define_cost - init_w +
	 Hvar.num_actions_define_cost - init_na);
      best_eff_cost.act_cost += (Hvar.cost_actions_define_cost - init_cost);

    }




  if (GpG.lagrange_multipl)
    total = precond + mutex + effect;



  if (unsat_facts > 1)
    total += (unsat_facts - 1);




  if (GpG.accurate_cost < COMPUTE_DG_LIST_COST)
    {

      neighb_act->cost.weight = total;

      neighb_act->cost.act_cost +=
	best_eff_cost.act_cost + best_mutex_cost.act_cost +
	best_prec_cost.act_cost;
      neighb_act->cost.act_time +=
	best_eff_cost.act_time + best_mutex_cost.act_time +
	best_prec_cost.act_time;
      neighb_act->cost.num_actions =
	1 + best_eff_cost.num_actions + best_mutex_cost.num_actions +
	best_prec_cost.num_actions;
    }
  else if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
    {
      if (GpG.lagrange_multipl)
	neighb_act->cost.weight =
	  best_prec_cost.weight + best_mutex_cost.weight +
	  best_eff_cost.weight;
      else
	neighb_act->cost.weight =
	  Hvar.weight_facts_define_cost + Hvar.num_actions_define_cost;

      neighb_act->cost.act_cost = Hvar.cost_actions_define_cost;
      neighb_act->cost.num_actions = Hvar.num_actions_define_cost;

      neighb_act->cost.act_time +=
	best_eff_cost.act_time + best_prec_cost.act_time;


      if (neighb_act->cost.act_time < Hvar.time_actions_define_cost)
	{
	  neighb_act->cost.act_time = Hvar.time_actions_define_cost;
	}




      temp = 0.0;



      duration = 0.0;

      if (FALSE && GpG.orig_weight_time > 0
	  && GpG.advanced_temporal_setting > 0)
	{

	  if (Hvar.min_start_time < MAXFLOAT)
	    duration = Hvar.min_start_time;

	  for (i = 0; i < Hvar.num_actions_define_cost; i++)
	    {

	      act_pos = Hvar.list_ef_define_cost[i];
	      if (act_pos < 0)
		continue;

	      act_time = temp = get_action_time (act_pos, level);


	      for (j = i + 1; j < Hvar.num_actions_define_cost; j++)
		{

		  if (Hvar.list_ef_define_cost[j] < 0)
		    continue;


		  if (GpG.advanced_temporal_setting == 1)
		    act_mutex =
		      are_mutex_ops (act_pos, Hvar.list_ef_define_cost[j]);
		  else
		    act_mutex = 0;

		  if (act_mutex)
		    continue;


		  act_time2 =
		    get_action_time (Hvar.list_ef_define_cost[j], level);

		  if (temp > act_time2)
		    {
		      temp -=
			get_action_time (Hvar.list_ef_define_cost[j], level);
		      Hvar.list_ef_define_cost[j] = -1;
		    }
		  else
		    {

		      act_time -= temp;
		      break;
		    }

		}

	      duration += act_time;
	    }
	}


      if (duration > neighb_act->cost.act_time)
	neighb_act->cost.act_time = duration;

      if (GpG.local == 2)
	{
	  diff = num_try - act->step;
	  if (diff < GpG.tabu_length && diff > 0)
	    {
	      total += GpG.delta * (GpG.tabu_length - diff);
	      best_eff_cost.weight += GpG.delta * (GpG.tabu_length - diff);
	    }

	}




    }



  if (GpG.num_solutions && neighb_act->constraint_type == C_T_INSERT_ACTION)
    {
      if (GpG.orig_weight_cost > 0 && GpG.best_cost * 1.5 > GpG.total_cost)
	{

	  neighb_act->cost.act_cost += get_action_cost (neighb_act->act_pos);
	  Hvar.cost_actions_define_cost +=
	    get_action_cost (neighb_act->act_pos);

	}
      if (GpG.orig_weight_time > 0 && GpG.best_time * 1.5 > GpG.total_time)
	{
	  neighb_act->cost.act_time +=
	    (10.0 + get_action_time (neighb_act->act_pos, level));
	  Hvar.time_actions_define_cost +=
	    (10.0 + get_action_time (neighb_act->act_pos, level));

	}
    }


  if (DEBUG4)
    {
      printf ("\n\n<<<< Evalutate action END  Act: %s",
	      print_op_name_string (act->position, temp_name));
      printf
	("\n--------------------------------------------------------------\n\n");

      if (DEBUG6)
	printf ("\n -> tot %f", total);

    }


  return (total);
}


















inline float
compute_dg_facts_cost (int Fact_position, int level, node_cost_list n_cost,
		       int action_level)
{

  register int temp;
  int curr_level, k, j;

  inform_list inf_noop;
  FtConn *tofix;
  float total = 0.0, cost = 0.0, prec = 0.0, mutex = 0.0, prec_act_cost = 0.0;

  int best_action = -1, best_level = 0, best_act_type = 0;
  int el;
  int cel;
  int stop;
  node_cost loc_n_cost, best_n_cost;



  prec_act_cost = 0.0;


  if (local_search.ls_continue == 0)
    return 0.0;


  stop = 3;
  if (Hvar.weight_facts_define_cost > local_search.max_act_incons)
    stop--;

  if (GpG.orig_weight_cost <= 0
      || Hvar.cost_actions_define_cost >= local_search.max_act_cost)
    stop--;


  if (GpG.orig_weight_time <= 0
      || Hvar.time_actions_define_cost >= local_search.max_act_time)
    stop--;


  if (stop <= 0)
    {

      if (DEBUG4)
	printf ("\n Stop compute_dg_cost ");


      local_search.ls_continue = 0;
      return 0.0;
    }
  if (local_search.ls_continue == 0)
    return 0.0;

  if (Hvar.constr->constraint_type == C_T_UNSUP_FACT
      && Hvar.constr->fact == Fact_position)
    {
      Hvar.weight_facts_define_cost = MAX_COST;

      Hvar.cost_actions_define_cost = MAX_COST;

      Hvar.time_actions_define_cost = MAX_COST;

      n_cost->act_cost = MAX_COST;
      n_cost->act_time = MAX_COST;
      total = MAX_COST;

      return (MAX_COST);
    }


  inf_noop = NULL;
  if (DEBUG4)
    {
      printf ("\n ||||| DG_COST  Fact %s, position %d, level %d\n",
	      print_ft_name_string (Fact_position, temp_name), Fact_position,
	      level);
    }


  if (level <= 0)
    return 0.0;





  if (level > action_level)
    level--;


  if (Fact_position >= 0)
    {

      total = 0.0;
      best_action = -1;
      best_n_cost.weight = MAXFLOAT;
      best_n_cost.act_cost = MAXFLOAT;
      best_n_cost.act_time = MAXFLOAT;



      tofix = &gft_conn[Fact_position];



      for (curr_level = level; curr_level >= level; curr_level--)
	{

	  if (GET_ACTION_POSITION_OF_LEVEL (curr_level) < 0
	      && curr_level != (level))
	    {
	      continue;

	    }


	  for (j = 0; j < gft_conn[tofix->position].num_A; j++)
	    {
	      cel = gft_conn[tofix->position].A[j];

	      if (Hvar.temp_removed_act == cel
		  && gft_conn[tofix->position].num_A > 1)
		{
		  if (GpG.orig_weight_time <= 0)
		    {
		      continue;
		    }
		  else
		    n_cost->weight++;
		}
	      if (GpG.verify_Af || CHECK_ACTION_POS (cel, curr_level))
		{


		  if (is_fact_in_delete_effects (cel, tofix->position))
		    continue;

		  if (tofix->position < 0)
		    continue;












		  cost =
		    dg_insertion_action_cost (cel, curr_level, action_level,
					      &loc_n_cost);


		  if (Hvar.temp_removed_act == cel)
		    {
		      loc_n_cost.weight++;
		    }





		  if (best_n_cost.weight >= loc_n_cost.weight)

		    {

		      if (best_n_cost.weight == loc_n_cost.weight
			  && weight_cost (&best_n_cost) <=
			  weight_cost (&loc_n_cost))

			continue;



		      best_action = cel;
		      best_level = curr_level;
		      best_act_type = IS_ACTION;
		      best_n_cost.weight = loc_n_cost.weight;
		      best_n_cost.act_cost = loc_n_cost.act_cost;
		      best_n_cost.act_time = loc_n_cost.act_time;

		      if (best_n_cost.weight <= 0)
			break;
		    }


		}

	    }

	  if (best_n_cost.weight <= 0)
	    break;


	}

    }
  else
    {


      Hvar.weight_facts_define_cost += 1;
      n_cost->weight =
	choose_best_action_for_unsup_num_fact (Fact_position, level,
					       &best_action, &best_level,
					       action_level);
      best_act_type = IS_ACTION;
      if (n_cost->weight <= 0)
	{

	  return 0.0;
	}

    }


  if (best_action < 0)
    {
      if (Fact_position < 0)
	return 0.0;

      if (inf_noop)
	{
	  best_action = inf_noop->position;
	  best_level = *inf_noop->level;
	  best_act_type = IS_NOOP;

	  n_cost->weight = 1.0;
	  n_cost->act_cost = 1.0;

	}
      else
	{
	  Hvar.weight_facts_define_cost = MAX_COST;
	  n_cost->weight = MAX_COST;
	  n_cost->act_cost = MAX_COST;
	  n_cost->act_time = MAX_COST;

	  return (MAX_COST);
	}

    }

  n_cost->weight = 0.0;
  n_cost->act_cost = 0.0;
  n_cost->act_time = 0.0;







  total = 1.0;
  if (GpG.advanced_temporal_setting > 0)
    {
      n_cost->act_cost += get_action_cost (best_action);
      n_cost->act_time += get_action_time (best_action, best_level);
    }

  mutex = count_mutex_noop_at_start (best_action, best_level);


  Hvar.weight_facts_define_cost += mutex;

  total += mutex;








  if (TRUE && GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
    {
      if (insert_action_inlist (best_action) == 0)
	{
	  Hvar.weight_facts_define_cost++;
	  return 1.0;
	}
    }


  if (Hvar.temp_removed_act == best_action)
    {

      Hvar.weight_facts_define_cost += 2.0;
    }












  for (j = 0, k = 0, prec = 0.0;
       j < gef_conn[best_action].num_PC && local_search.ls_continue; j++)
    {
      el = gef_conn[best_action].PC[j];



      if (CHECK_FACT_POS (el, best_level))
	{

	  if (el < 0)
	    {
	      if (is_num_prec_satisfied_in_common_level (el))
		continue;
	    }
	  else if (fact_is_supported (el, level))
	    {
	      if (GpG.temporal_plan)
		{
		  if (Hvar.min_start_time >
		      CONVERT_FACT_TO_INFORM (el, level)->time_f)
		    Hvar.min_start_time =
		      CONVERT_FACT_TO_INFORM (el, level)->time_f;

		  if (n_cost->act_time <
		      CONVERT_FACT_TO_INFORM (el, level)->time_f)
		    {
		      n_cost->act_time =
			CONVERT_FACT_TO_INFORM (el, level)->time_f;

		    }



		  if (Hvar.time_actions_define_cost < n_cost->act_time)
		    Hvar.time_actions_define_cost = n_cost->act_time;


		}



	      continue;
	    }



	  if (el >= 0 && GpG.accurate_cost >= COMPUTE_DG_LIST_COST
	      && GET_BIT (Hvar.bit_vect_facts, el))
	    {
	      if (n_cost->act_time < Hvar.estimate_time_facts[el])
		{
		  n_cost->act_time = Hvar.estimate_time_facts[el];

		}



	      if (Hvar.time_actions_define_cost < n_cost->act_time)
		Hvar.time_actions_define_cost = n_cost->act_time;


	      continue;
	    }

	  temp = compute_dg_facts_cost (el, level, &loc_n_cost, action_level);


	  if (GpG.accurate_cost == COMPUTE_DG_MAX_COST)
	    {


	      if (n_cost->act_cost < loc_n_cost.act_cost
		  && loc_n_cost.act_cost < MAX_COST)
		n_cost->act_cost = loc_n_cost.act_cost;

	      if (n_cost->act_time < loc_n_cost.act_time
		  && loc_n_cost.act_time < MAX_COST)
		n_cost->act_time = loc_n_cost.act_time;

	      if (prec < temp)
		{
		  prec = temp;
		  n_cost->weight = loc_n_cost.weight;


		  if (FALSE
		      && (temp *=
			  local_search.lamda_prec) > local_search.best_cost)
		    {

		      if (DEBUG4)
			printf ("\n MAX_ACT_COST stop");
		      return temp;
		    }

		}
	    }
	  else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
	    {
	      if (loc_n_cost.act_cost < MAX_COST)
		n_cost->act_cost += loc_n_cost.act_cost;
	      if (loc_n_cost.act_time < MAX_COST)
		n_cost->act_time += loc_n_cost.act_time;


	      prec += temp;
	      n_cost->weight += loc_n_cost.weight;


	    }
	  else if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
	    {


	      if (n_cost->act_time < loc_n_cost.act_time
		  && loc_n_cost.act_time < MAX_COST)
		n_cost->act_time = loc_n_cost.act_time;

	      if (prec < loc_n_cost.weight)
		{
		  prec = loc_n_cost.weight;



		}
	    }





	}

      if (Hvar.time_actions_define_cost < n_cost->act_time)
	Hvar.time_actions_define_cost = n_cost->act_time;
    }






  if (gef_conn[best_action].sf != NULL)
    {
      for (j = 0;
	   j < gef_conn[best_action].sf->num_PC_overall
	   && local_search.ls_continue; j++)
	{
	  el = gef_conn[best_action].sf->PC_overall[j];

	  if (is_fact_in_additive_effects_start (best_action, el))
	    {
	      continue;
	    }

	  if (CHECK_FACT_POS (el, best_level))
	    {

	      if (el < 0)
		{
		  if (is_num_prec_satisfied_in_common_level (el))
		    continue;
		}
	      else if (fact_is_supported (el, level))
		{
		  if (GpG.temporal_plan && el > 0)
		    {
		      if (Hvar.min_start_time >
			  CONVERT_FACT_TO_INFORM (el, level)->time_f)
			Hvar.min_start_time =
			  CONVERT_FACT_TO_INFORM (el, level)->time_f;


		      if (n_cost->act_time <
			  CONVERT_FACT_TO_INFORM (el, level)->time_f)
			{
			  n_cost->act_time =
			    CONVERT_FACT_TO_INFORM (el, level)->time_f;
			}
		    }

		  continue;
		}




	      if (el >= 0 && GpG.accurate_cost >= COMPUTE_DG_LIST_COST
		  && GET_BIT (Hvar.bit_vect_facts, el))
		{
		  if (n_cost->act_time < Hvar.estimate_time_facts[el])
		    {
		      n_cost->act_time = Hvar.estimate_time_facts[el];

		    }



		  if (Hvar.time_actions_define_cost < n_cost->act_time)
		    Hvar.time_actions_define_cost = n_cost->act_time;



		  continue;
		}

	      temp =
		compute_dg_facts_cost (el, level, &loc_n_cost, action_level);


	      if (GpG.accurate_cost == COMPUTE_DG_MAX_COST)
		{


		  if (n_cost->act_cost < loc_n_cost.act_cost
		      && loc_n_cost.act_cost < MAX_COST)
		    n_cost->act_cost = loc_n_cost.act_cost;

		  if (n_cost->act_time < loc_n_cost.act_time
		      && loc_n_cost.act_time < MAX_COST)
		    n_cost->act_time = loc_n_cost.act_time;

		  if (prec < temp)
		    {
		      prec = temp;
		      n_cost->weight = loc_n_cost.weight;


		      if (FALSE
			  && (temp *=
			      local_search.lamda_prec) >
			  local_search.best_cost)
			{

			  if (DEBUG4)
			    printf ("\n MAX_ACT_COST stop");
			  return temp;
			}

		    }
		}
	      else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
		{
		  if (loc_n_cost.act_cost < MAX_COST)
		    n_cost->act_cost += loc_n_cost.act_cost;
		  if (loc_n_cost.act_time < MAX_COST)
		    n_cost->act_time += loc_n_cost.act_time;


		  prec += temp;
		  n_cost->weight += loc_n_cost.weight;


		}

	      else if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
		{


		  if (n_cost->act_time < loc_n_cost.act_time
		      && loc_n_cost.act_time < MAX_COST)
		    n_cost->act_time = loc_n_cost.act_time;

		  if (prec < loc_n_cost.weight)
		    {
		      prec = loc_n_cost.weight;



		    }
		}





	    }

	  if (Hvar.time_actions_define_cost < n_cost->act_time)
	    Hvar.time_actions_define_cost = n_cost->act_time;
	}

    }


  if (!GpG.advanced_temporal_setting)
    {
      n_cost->act_cost += get_action_cost (best_action);
      n_cost->act_time += get_action_time (best_action, best_level);
    }




  if (gef_conn[best_action].sf != NULL)
    {
      for (j = 0;
	   j < gef_conn[best_action].sf->num_PC_end
	   && local_search.ls_continue; j++)
	{
	  el = gef_conn[best_action].sf->PC_end[j];




	  if (is_fact_in_additive_effects_start (best_action, el))
	    continue;

	  if (CHECK_FACT_POS (el, best_level))
	    {

	      if (el < 0)
		{
		  if (is_num_prec_satisfied_in_common_level (el))
		    continue;
		}
	      else if (fact_is_supported (el, level))
		{
		  if (GpG.temporal_plan)
		    {
		      if (Hvar.min_start_time >
			  CONVERT_FACT_TO_INFORM (el, level)->time_f)
			Hvar.min_start_time =
			  CONVERT_FACT_TO_INFORM (el, level)->time_f;

		      if (n_cost->act_time <
			  CONVERT_FACT_TO_INFORM (el, level)->time_f)
			{
			  n_cost->act_time =
			    CONVERT_FACT_TO_INFORM (el, level)->time_f;

			}
		    }


		  continue;
		}




	      if (el >= 0 && GpG.accurate_cost >= COMPUTE_DG_LIST_COST
		  && GET_BIT (Hvar.bit_vect_facts, el))
		{
		  if (n_cost->act_time <
		      (Hvar.estimate_time_facts[el] -
		       get_action_time (best_action, best_level)))
		    {
		      n_cost->act_time =
			Hvar.estimate_time_facts[el] -
			get_action_time (best_action, best_level);

		    }



		  if (Hvar.time_actions_define_cost <
		      n_cost->act_time - get_action_time (best_action,
							  best_level))
		    Hvar.time_actions_define_cost =
		      n_cost->act_time - get_action_time (best_action,
							  best_level);







		  continue;
		}

	      temp =
		compute_dg_facts_cost (el, level, &loc_n_cost, action_level);


	      if (GpG.accurate_cost == COMPUTE_DG_MAX_COST)
		{


		  if (n_cost->act_cost < loc_n_cost.act_cost
		      && loc_n_cost.act_cost < MAX_COST)
		    n_cost->act_cost = loc_n_cost.act_cost;

		  if (n_cost->act_time < loc_n_cost.act_time
		      && loc_n_cost.act_time < MAX_COST)
		    n_cost->act_time = loc_n_cost.act_time;

		  if (prec < temp)
		    {
		      prec = temp;
		      n_cost->weight = loc_n_cost.weight;


		      if (FALSE
			  && (temp *=
			      local_search.lamda_prec) >
			  local_search.best_cost)
			{

			  if (DEBUG4)
			    printf ("\n MAX_ACT_COST stop");
			  return temp;
			}

		    }
		}
	      else if (GpG.accurate_cost == COMPUTE_DG_SUM_COST)
		{
		  if (loc_n_cost.act_cost < MAX_COST)
		    n_cost->act_cost += loc_n_cost.act_cost;
		  if (loc_n_cost.act_time < MAX_COST)
		    n_cost->act_time += loc_n_cost.act_time;


		  prec += temp;
		  n_cost->weight += loc_n_cost.weight;


		}
	      else if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
		{


		  if (n_cost->act_time < loc_n_cost.act_time
		      && loc_n_cost.act_time < MAX_COST)
		    n_cost->act_time = loc_n_cost.act_time;

		  if (prec < loc_n_cost.weight)
		    {
		      prec = loc_n_cost.weight;



		    }
		}






	    }

	  if (Hvar.time_actions_define_cost < n_cost->act_time)
	    Hvar.time_actions_define_cost = n_cost->act_time;

	}

    }









  if (Hvar.time_actions_define_cost < n_cost->act_time)
    Hvar.time_actions_define_cost = n_cost->act_time;


  n_cost->weight = total;


  if (DEBUG4)
    printf
      ("\n\n ||| END DG_COST  Fact %s, position %d, level %d\n  total %f PREC %.2f ME %.2f act_cost %.2f act_time %.2f -- Hvar.weight_facts_define_cost %.2f",
       print_ft_name_string (Fact_position, temp_name), Fact_position, level,
       total, prec, mutex, n_cost->act_cost, n_cost->act_time,
       Hvar.weight_facts_define_cost);









  if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
    {


      if (gef_conn[best_action].sf)
	for (j = 0; j < gef_conn[best_action].sf->num_A_start; j++)
	  {
	    el = gef_conn[best_action].sf->A_start[j];

	    if (el < 0)
	      {

		apply_numeric_effect_in_common_level (el);
		continue;

	      }
	    if (is_fact_in_delete_effects (best_action, el))
	      continue;



	    if (Hvar.estimate_time_facts[el] <= 0)
	      Hvar.estimate_time_facts[el] = n_cost->act_time;


	    SET_BIT (Hvar.bit_vect_facts, el);

	  }


      for (j = 0; j < gef_conn[best_action].num_A; j++)
	{
	  el = gef_conn[best_action].A[j];
	  if (el < 0)
	    {

	      apply_numeric_effect_in_common_level (el);
	      continue;

	    }


	  if (Hvar.estimate_time_facts[el] <= 0)
	    Hvar.estimate_time_facts[el] =
	      n_cost->act_time + get_action_time (best_action, best_level);



	  SET_BIT (Hvar.bit_vect_facts, el);


	}


    }




  stop = 3;
  if (Hvar.weight_facts_define_cost > local_search.max_act_incons)
    stop--;

  if (GpG.orig_weight_cost <= 0
      || Hvar.cost_actions_define_cost >= local_search.max_act_cost)
    stop--;


  if (GpG.orig_weight_time <= 0
      || Hvar.time_actions_define_cost >= local_search.max_act_time)
    stop--;


  if (stop <= 0)
    {

      if (DEBUG4)
	printf ("\n Stop compute_dg_cost ");

      local_search.ls_continue = 0;
    }
  return total;

}











int
choose_best_action_for_unsup_num_fact (int Fact_position, int level,
				       int *best_action, int *best_level,
				       int action_level)
{
  int curr_level, j;
  static action_set neighbors;
  auto float total, cost;
  int best_act = -1, best_lev = 0, best_act_type;
  int cel;
  node_cost loc_n_cost, best_n_cost;


  total = 0.0;
  best_act = -1;
  best_n_cost.weight = MAX_COST;
  best_n_cost.act_cost = MAX_COST;
  best_n_cost.act_time = MAX_COST;

  curr_level = level;
  neighbors.num_A = 0;

  create_neighborhood_for_compvar (Fact_position, 1, 0, &neighbors, 1, level);

  for (j = 0; j < neighbors.num_A; j++)
    {


      cel = neighbors.A[j];
      if (Hvar.temp_removed_act == cel && neighbors.num_A > 1)
	{
	  continue;
	}
      if (CHECK_ACTION_POS (cel, curr_level))
	{
	  if (GpG.accurate_cost <= COMPUTE_ADD_COST)
	    {
	      cost =
		fast_insertion_action_cost (cel, curr_level, action_level);
	      loc_n_cost.act_cost = get_action_cost (cel);
	      loc_n_cost.act_time = get_action_time (cel, level);
	    }
	  else
	    cost =
	      dg_insertion_action_cost (cel, curr_level, action_level,
					&loc_n_cost);
	  if (best_n_cost.weight >= cost)
	    {
	      if (best_n_cost.weight == cost
		  && weight_cost (&best_n_cost) <= weight_cost (&loc_n_cost))
		continue;
	      best_act = cel;
	      best_lev = curr_level;
	      best_act_type = IS_ACTION;
	      best_n_cost.weight = loc_n_cost.weight;
	      best_n_cost.act_cost = loc_n_cost.act_cost;
	      best_n_cost.act_time = loc_n_cost.act_time;
	      if (best_n_cost.weight <= 0)
		break;
	    }
	}
    }



  *best_action = best_act;
  *best_level = best_lev;
  return 1.0;

}
