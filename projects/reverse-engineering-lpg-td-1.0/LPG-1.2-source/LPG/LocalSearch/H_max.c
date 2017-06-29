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
 * File: H_MAX.c
 * Description: heuristic H_MAX.
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/



#include "lpg.h"
#include "inst_utils.h"
#include "H_max.h"
#include "utilities.h"
#include "LpgOutput.h"
#include "output.h"
#include "numeric.h"
#include "H_relaxed.h"


void
remove_temp_action (int act_pos, int level)
{
  int i;
  EfConn *act;
  static int first_call = 0, last_action = -1;
  inform_list infAction;
  noop_not_in *temp;

  if (first_call == 0)
    {

      new_true_facts = alloc_vect (gnum_ft_block);
      new_false_facts = alloc_vect (gnum_ft_block);
    }

  if (last_action >= 0)
    {

      memset (new_true_facts, 0, gnum_ft_block * sizeof (int));

      for (i = 0; i < gef_conn[last_action].num_A; i++)
	{

	  if (gef_conn[last_action].A[i] < 0)
	    continue;

	  new_false_facts[GUID_BLOCK (gef_conn[last_action].A[i])] = 0;
	}
    }

  last_action = act_pos;
  act = &gef_conn[act_pos];

  infAction = GET_ACTION_OF_LEVEL (level);

  for (i = 0; i < gef_conn[last_action].num_A; i++)
    {

      if (gef_conn[last_action].A[i] < 0)
	continue;
      new_false_facts[GUID_BLOCK (gef_conn[last_action].A[i])] |=
	GUID_MASK (gef_conn[last_action].A[i]);
    }


  if (gef_conn[last_action].sf != NULL)
    {
      for (i = 0; i < gef_conn[last_action].sf->num_A_start; i++)
	{

	  if (gef_conn[last_action].sf->A_start[i] < 0)
	    continue;
	  new_false_facts[GUID_BLOCK (gef_conn[last_action].sf->A_start[i])]
	    |= GUID_MASK (gef_conn[last_action].sf->A_start[i]);
	}
    }

  temp = infAction->add;
  while (temp != NULL)
    {
      new_true_facts[GUID_BLOCK (temp->position)] |=
	GUID_MASK (temp->position);
      temp = temp->next;
    }

  if (gef_conn[last_action].sf != NULL)
    {

      for (i = 0; i < gef_conn[last_action].num_D; i++)
	{
	  if (gef_conn[last_action].D[i] < 0)
	    continue;

	  new_true_facts[GUID_BLOCK (gef_conn[last_action].D[i])] |=
	    GUID_MASK (gef_conn[last_action].D[i]);
	}

      for (i = 0; i < gef_conn[last_action].sf->num_D_start; i++)
	{
	  if (is_fact_in_additive_effects
	      (last_action, gef_conn[last_action].sf->D_start[i]))
	    continue;

	  if (gef_conn[last_action].sf->D_start[i] < 0)
	    continue;
	  new_true_facts[GUID_BLOCK (gef_conn[last_action].sf->D_start[i])] |=
	    GUID_MASK (gef_conn[last_action].sf->D_start[i]);

	}
    }

}








inline float
fast_insertion_action_cost (int act_pos, int level, int temp_removed_action)
{
  register int temp, i, *ptr;
  int diff;
  register EfConn *act;
  float total, prec_par, excl_par, add_effect_par;


  act = &gef_conn[act_pos];

  prec_par = GpG.prec_par;
  excl_par = GpG.excl_par;
  add_effect_par = GpG.add_effect_par;

  total = 0.0;

  if (DEBUG4)
    printf ("\n\n Evalutate action");

  if (DEBUG3)
    printf ("\nFAST_INSERTION  Act: %s, level. %d",
	    print_op_name_string (act_pos, temp_name), level);

  if (prec_par)
    {

      if (level <= temp_removed_action)
	{
	  for (i = 0, temp = 0; i < act->num_precond; i++)
	    temp +=
	      count_bit1 (act->bit_precond[i].
			  uid_mask & (~vectlevel[level]->
				      fact_vect[act->bit_precond[i].
						uid_block]));

	  for (i = 0; i < act->num_PC; i++)
	    if (act->PC[i] < 0)
	      if (!is_num_prec_satisfied_in_common_level (-act->PC[i]))
		temp++;

	  if (act->sf != NULL)
	    {
	      for (i = 0; i < act->sf->num_PC_overall; i++)

		if (act->sf->PC_overall[i] >= 0)
		  {
		    if (vectlevel[level]->fact[act->sf->PC_overall[i]].
			w_is_true <= 0
			&& !is_fact_in_additive_effects_start (act_pos,
							       act->sf->
							       PC_overall[i]))
		      temp++;
		  }
		else
		  if (!is_num_prec_satisfied_in_common_level
		      (-act->sf->PC_overall[i]))
		  temp++;


	      for (i = 0; i < act->sf->num_PC_end; i++)

		if (act->sf->PC_end[i] >= 0)
		  {
		    if (vectlevel[level]->fact[act->sf->PC_end[i]].
			w_is_true <= 0
			&& !is_fact_in_additive_effects_start (act_pos,
							       act->sf->
							       PC_end[i]))
		      temp++;
		  }
		else if (!is_num_prec_satisfied (-act->sf->PC_end[i], level))
		  temp++;

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

      total = prec_par * temp;

      if (DEBUG3)
	printf ("\n P: %d", temp);

    }

  if (excl_par)
    {

      if (level <= temp_removed_action)
	{
	  temp = count_mutex_action (act_pos, level);
	}
      else
	for (temp = 0, i = 0; i < gnum_ft_block; i++)
	  if (vectlevel[level]->prec_vect[i])
	    temp +=
	      count_bit1 (CONVERT_ACTION_TO_VERTEX (act_pos)->
			  ft_exclusive_vect[i] &
			  ((vectlevel[level]->
			    fact_vect[i] | new_true_facts[i]) &
			   (~new_false_facts[i])) & vectlevel[level]->
			  prec_vect[i]);

      total += excl_par * temp;


      if (DEBUG3)
	printf ("  ME: %d", temp);

    }
  if (add_effect_par)
    {
      level++;

      ptr = vectlevel[level]->false_crit_vect;

      if (level <= temp_removed_action)
	{
	  for (i = 0, temp = 0; i < act->num_add_effect; i++)
	    temp +=
	      count_bit1 (act->bit_add_effect[i].
			  uid_mask & *(ptr +
				       act->bit_add_effect[i].uid_block));

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
	for (i = 0, temp = 0; i < act->num_add_effect; i++)
	  temp +=
	    count_bit1 (act->bit_add_effect[i].
			uid_mask & (*(ptr + act->bit_add_effect[i].uid_block)
				    | (vectlevel[level]->
				       true_crit_vect[act->bit_add_effect[i].
						      uid_block] &
				       new_false_facts[act->bit_add_effect[i].
						       uid_block])));

      total += add_effect_par * temp;
      if (DEBUG3)
	printf ("  Add-E: %d", temp);
    }

  if (GpG.local == 2)
    {
      diff = num_try;
      if (diff < GpG.tabu_length && diff > 0)
	total += GpG.delta * (GpG.tabu_length - diff);
    }


  if (DEBUG3)
    printf ("\n -> tot %f", total);


  return (total);
}








float
compute_max_fact_cost (inform_list Fact, node_cost_list n_cost,
		       int action_level)
{

  register int temp, k = 0, j;
  register FtConn *tofix;
  auto float total, cost, prec, mutex, prec_act_cost;
  inform_list inf_fact, inf_noop;
  int best_action = -1, best_level = 0, best_act_type = 0, el, cel, level;
  node_cost loc_n_cost, best_n_cost;;


  level = *Fact->level;
  prec_act_cost = 0.0;
  inf_noop = NULL;

  if (fact_is_supported (Fact->position, level) && level <= action_level)
    {

      if (DEBUG4)
	printf ("\n\n Fact supported: %s\n",
		print_ft_name_string (Fact->position, temp_name));

      n_cost->weight = 0.0;
      n_cost->act_cost = 0.0;

      if (GpG.temporal_plan)
	n_cost->act_time = Fact->time_f;


      return (0.0);
    }



  if (DEBUG4)
    {
      printf ("\n ||||| MAX_COST  Fact %s, position %d, level %d",
	      print_ft_name_string (Fact->position, temp_name),
	      Fact->position, level);

    }


  total = get_fact_cost (Fact->position, *Fact->level, &loc_n_cost);
  if (total >= 0 && total < MAX_COST)
    {

      n_cost->weight = loc_n_cost.weight;
      n_cost->act_cost = loc_n_cost.act_cost;
      n_cost->act_time = loc_n_cost.act_time;
      return total;
    }

  if (level <= 0)
    return 0;


  total = 0.0;
  best_n_cost.weight = MAX_COST;
  best_n_cost.act_cost = MAX_COST;
  best_n_cost.act_time = MAX_COST;


  tofix = &gft_conn[Fact->position];

  if (level <= 0)
    return 0;

  if (CHECK_NOOP_POS (Fact->position, level - 1))
    {
      if (check_mutex_noop (Fact->position, level - 1) >= 0)
	inf_noop = CONVERT_NOOP_TO_INFORM (Fact->position, level - 1);
      else
	{
	  inf_noop = CONVERT_NOOP_TO_INFORM (Fact->position, level - 1);



	  cost = 0.0;

	  if (level >= action_level
	      && CONVERT_FACT_TO_INFORM (Fact->position,
					 level - 1)->w_is_true <= 1)
	    cost++;
	  else
	    if (!CONVERT_FACT_TO_INFORM (Fact->position, level - 1)->
		w_is_true)
	    cost++;


	  if (best_n_cost.weight >= cost)
	    {

	      loc_n_cost.act_cost = 0.0;
	      loc_n_cost.act_time = 0.0;

	      if (!
		  (best_n_cost.weight == cost
		   && weight_cost (&best_n_cost) < weight_cost (&loc_n_cost)))
		{
		  best_action = inf_noop->position;
		  best_level = *inf_noop->level;
		  best_act_type = IS_NOOP;
		  best_n_cost.weight = cost;
		  best_n_cost.act_cost = loc_n_cost.act_cost;
		  best_n_cost.act_time = loc_n_cost.act_time;

		}
	    }

	}
    }

  for (j = 0; j < gft_conn[tofix->position].num_A; j++)
    {
      cel = gft_conn[tofix->position].A[j];

      if (is_fact_in_delete_effects (cel, tofix->position))
	continue;

      if (cel < 0)
	continue;

      if (CHECK_ACTION_POS (cel, level - 1))
	{

	  cost = fast_insertion_action_cost (cel, level - 1, action_level);
	  if (best_n_cost.weight >= cost)
	    {

	      loc_n_cost.act_cost = get_action_cost (cel);
	      loc_n_cost.act_time = get_action_time (cel, level - 1);

	      if (best_n_cost.weight == cost
		  && weight_cost (&best_n_cost) <= weight_cost (&loc_n_cost))
		continue;

	      best_action = cel;
	      best_level = level - 1;
	      best_act_type = IS_ACTION;
	      best_n_cost.weight = cost;
	      best_n_cost.act_cost = loc_n_cost.act_cost;
	      best_n_cost.act_time = loc_n_cost.act_time;

	      if (best_n_cost.weight <= 0)
		break;
	    }
	}

    }

  if (best_action < 0)
    {

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
	  n_cost->weight = MAX_COST;
	  n_cost->act_cost = MAX_COST;

	  return (MAX_COST);
	}

    }

  n_cost->weight = 0.0;
  n_cost->act_cost = 0.0;
  n_cost->act_time = 0.0;


  if (best_act_type != IS_ACTION)
    {

      inf_fact = CONVERT_FACT_TO_INFORM (best_action, best_level);
      temp = compute_max_fact_cost (inf_fact, &loc_n_cost, action_level);

      if (n_cost->act_cost < loc_n_cost.act_cost
	  && loc_n_cost.act_cost < MAX_COST)
	n_cost->act_cost = loc_n_cost.act_cost;

      if (n_cost->act_time < loc_n_cost.act_time
	  && loc_n_cost.act_time < MAX_COST)
	n_cost->act_time = loc_n_cost.act_time;

      prec = temp;
      n_cost->weight = loc_n_cost.weight;

    }
  else
    {



      for (j = 0, k = 0, prec = 0.0; j < gef_conn[best_action].num_PC; j++)
	{
	  el = gef_conn[best_action].PC[j];

	  if (el < 0)
	    continue;

	  if (CHECK_FACT_POS (el, best_level))
	    {

	      inf_fact = CONVERT_FACT_TO_INFORM (el, best_level);
	      temp =
		compute_max_fact_cost (inf_fact, &loc_n_cost, action_level);


	      if (GpG.accurate_cost == COMPUTE_MAX_COST)
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
	      else if (GpG.accurate_cost == COMPUTE_ADD_COST)
		{
		  if (loc_n_cost.act_cost < MAX_COST)
		    n_cost->act_cost += loc_n_cost.act_cost;
		  if (loc_n_cost.act_time < MAX_COST)
		    n_cost->act_time += loc_n_cost.act_time;


		  prec += temp;
		  n_cost->weight += loc_n_cost.weight;


		}

	    }

	}


      if (gef_conn[best_action].sf != NULL)
	{
	  for (j = 0; j < gef_conn[best_action].sf->num_PC_overall; j++)
	    {
	      el = gef_conn[best_action].sf->PC_overall[j];

	      if (el < 0)
		continue;

	      if (is_fact_in_additive_effects_start (best_action, el))
		{
		  printf
		    ("\nskip_precondition: continue... is_fact_in_additive_effects_start");
		  continue;
		}

	      if (CHECK_FACT_POS (el, best_level))
		{

		  inf_fact = CONVERT_FACT_TO_INFORM (el, best_level);
		  temp =
		    compute_max_fact_cost (inf_fact, &loc_n_cost,
					   action_level);


		  if (GpG.accurate_cost == COMPUTE_MAX_COST)
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
		  else if (GpG.accurate_cost == COMPUTE_ADD_COST)
		    {
		      if (loc_n_cost.act_cost < MAX_COST)
			n_cost->act_cost += loc_n_cost.act_cost;
		      if (loc_n_cost.act_time < MAX_COST)
			n_cost->act_time += loc_n_cost.act_time;


		      prec += temp;
		      n_cost->weight += loc_n_cost.weight;


		    }

		}

	    }

	}





      if (gef_conn[best_action].sf != NULL)
	{
	  for (j = 0; j < gef_conn[best_action].sf->num_PC_end; j++)
	    {
	      el = gef_conn[best_action].sf->PC_end[j];

	      if (el < 0)
		continue;

	      if (is_fact_in_additive_effects_start (best_action, el))
		continue;

	      if (CHECK_FACT_POS (el, best_level))
		{

		  inf_fact = CONVERT_FACT_TO_INFORM (el, best_level);


		  temp =
		    compute_max_fact_cost (inf_fact, &loc_n_cost,
					   action_level);


		  if (GpG.accurate_cost == COMPUTE_MAX_COST)
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


			  if (FALSE && (temp *= local_search.lamda_prec) >
			      local_search.best_cost)
			    {

			      if (DEBUG4)
				printf ("\n MAX_ACT_COST stop");
			      return temp;
			    }

			}
		    }
		  else if (GpG.accurate_cost == COMPUTE_ADD_COST)
		    {
		      if (loc_n_cost.act_cost < MAX_COST)
			n_cost->act_cost += loc_n_cost.act_cost;
		      if (loc_n_cost.act_time < MAX_COST)
			n_cost->act_time += loc_n_cost.act_time;


		      prec += temp;
		      n_cost->weight += loc_n_cost.weight;

		    }

		}

	    }

	}

    }

  if (best_act_type != IS_ACTION)
    {
      total = 0.0;

      mutex = count_mutex_noop (best_action, best_level);
    }
  else
    {
      total = 1.0;

      n_cost->act_cost += get_action_cost (best_action);
      n_cost->act_time += get_action_time (best_action, best_level);
      mutex = count_mutex_action (best_action, best_level);

      total += mutex;
    }
  total += prec;

  n_cost->weight = total;


  if (DEBUG4)
    printf
      ("\n\n ||| END MAX_COST  Fact %s, position %d, level %d\n   total %f PREC %.2f ME %.2f act_cost %.2f act_time %.2f ",
       print_ft_name_string (tofix->position, temp_name), tofix->position,
       level, total, prec, mutex, n_cost->act_cost, n_cost->act_time);

  set_fact_cost (Fact, n_cost);

  return total;

}











float
max_action_cost (neighb_list neighb_act)
{
  register int i, unsat_facts = 0;
  int level, j, diff = 0, next_level;
  register EfConn *act;
  auto float total, prec_par, excl_par, add_effect_par, temp = 0.0, prec, eff;
  inform_list inf_fact;
  inform fact;
  int el, fact_pos, ind_level;
  auto float precond, mutex, effect, act_cost;
  node_cost loc_n_cost, best_prec_cost, best_eff_cost;

  precond = mutex = effect = act_cost = 0.0;


  level = neighb_act->act_level;
  act = &gef_conn[neighb_act->act_pos];

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
      neighb_act->cost.act_cost =
	(-1) * (get_action_cost (neighb_act->act_pos));
      neighb_act->cost.act_time =
	(-1) * get_action_time (neighb_act->act_pos, level);

      prec_par = GpG.used_prec_par;
      excl_par = GpG.used_excl_par;
      add_effect_par = GpG.used_add_effect_par;
    }
  else
    {
      neighb_act->cost.act_cost = get_action_cost (neighb_act->act_pos);

      neighb_act->cost.act_time =
	get_action_time (neighb_act->act_pos, level);

      prec_par = GpG.prec_par;
      excl_par = GpG.excl_par;
      add_effect_par = GpG.add_effect_par;
    }

  if (DEBUG4)
    printf ("\n\n Evalutate action");


  if (DEBUG3)
    printf ("\nMAX COST Act: %s, level %d\n  ",
	    print_op_name_string (act->position, temp_name), level);

  best_prec_cost.weight = 0.0;

  best_prec_cost.act_cost = 0.0;

  best_prec_cost.act_time = 0.0;

  if (GpG.temporal_plan)
    for (ind_level = level; ind_level >= 0; ind_level--)
      {
	if (check_mutex_action (act->position, ind_level) >= 0
	    && vectlevel[ind_level]->action.w_is_used > 0)
	  if (best_prec_cost.act_time <
	      GET_ACTION_OF_LEVEL (ind_level)->time_f)
	    best_prec_cost.act_time = GET_ACTION_OF_LEVEL (ind_level)->time_f;
      }

  if (prec_par)
    {
      unsat_facts = 0;


      for (prec = 0.0, diff = 0, j = 0; j < gef_conn[act->position].num_PC;
	   j++)
	{
	  el = gef_conn[act->position].PC[j];

	  if (el < 0)
	    {
	      loc_n_cost.weight = 0.0;
	      loc_n_cost.act_cost = 0.0;
	      loc_n_cost.act_time = 0.0;


	      fact.position = el;
	      fact.level = &vectlevel[level]->level;
	      temp =
		compute_dg_facts_cost (fact.position, *fact.level,
				       &loc_n_cost, level);
	      if (GpG.accurate_cost == COMPUTE_MAX_COST)
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
	      else if (GpG.accurate_cost == COMPUTE_ADD_COST)
		{
		  prec += temp;
		  best_prec_cost.weight += loc_n_cost.weight;
		  best_prec_cost.act_cost += loc_n_cost.act_cost;
		  best_prec_cost.act_time += loc_n_cost.act_time;
		}
	      continue;

	    }

	  if (CHECK_FACT_POS (el, level))
	    {
	      if (DEBUG4)
		{
		  printf ("\n\n %d +++++ MAX_COST Prec_start  Act %s ",
			  ++diff, print_op_name_string (act->position,
							temp_name));
		  printf ("\n  fact %s\n",
			  print_ft_name_string (el, temp_name));
		}

	      loc_n_cost.weight = 0.0;

	      loc_n_cost.act_cost = 0.0;

	      loc_n_cost.act_time = 0.0;



	      inf_fact = CONVERT_FACT_TO_INFORM (el, level);
	      temp = compute_max_fact_cost (inf_fact, &loc_n_cost, level);

	      if (GpG.accurate_cost == COMPUTE_MAX_COST
		  && inf_fact->w_is_true <= 0)
		unsat_facts++;

	      if (DEBUG4)
		{
		  printf ("\n\n %d +++ END MAX_COST Prec_start  Act %s ",
			  ++diff, print_op_name_string (act->position,
							temp_name));
		  printf
		    ("\n  fact %s, COST  weight %f cost %f time %f unsatisf_prec %d ",
		     print_ft_name_string (el, temp_name), loc_n_cost.weight,
		     loc_n_cost.act_cost, loc_n_cost.act_time, unsat_facts);
		}

	      if (GpG.accurate_cost == COMPUTE_MAX_COST)
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

		      if (FALSE
			  && (temp *=
			      local_search.lamda_prec) >
			  local_search.best_cost)
			return temp;

		    }
		}
	      else if (GpG.accurate_cost == COMPUTE_ADD_COST)
		{
		  prec += temp;

		  best_prec_cost.weight += loc_n_cost.weight;

		  best_prec_cost.act_cost += loc_n_cost.act_cost;

		  best_prec_cost.act_time += loc_n_cost.act_time;


		}


	    }

	}


      if (gef_conn[act->position].sf != NULL)
	{
	  for (j = 0; j < gef_conn[act->position].sf->num_PC_overall; j++)
	    {
	      el = gef_conn[act->position].sf->PC_overall[j];

	      if (el < 0)
		{

		  loc_n_cost.weight = 0.0;
		  loc_n_cost.act_cost = 0.0;
		  loc_n_cost.act_time = 0.0;


		  fact.position = el;
		  temp =
		    compute_dg_facts_cost (fact.position, *fact.level,
					   &loc_n_cost, level);
		  if (GpG.accurate_cost == COMPUTE_MAX_COST)
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
		  else if (GpG.accurate_cost == COMPUTE_ADD_COST)
		    {
		      prec += temp;
		      best_prec_cost.weight += loc_n_cost.weight;
		      best_prec_cost.act_cost += loc_n_cost.act_cost;
		      best_prec_cost.act_time += loc_n_cost.act_time;
		    }
		  continue;

		}


	      if (is_fact_in_additive_effects_start (act->position, el))
		continue;

	      if (CHECK_FACT_POS (el, level))
		{
		  if (DEBUG4)
		    {
		      printf ("\n\n %d +++++ MAX_COST Prec_overall  Act %s ",
			      ++diff, print_op_name_string (act->position,
							    temp_name));
		      printf ("\n  fact %s\n",
			      print_ft_name_string (el, temp_name));
		    }

		  loc_n_cost.weight = 0.0;

		  loc_n_cost.act_cost = 0.0;

		  loc_n_cost.act_time = 0.0;



		  inf_fact = CONVERT_FACT_TO_INFORM (el, level);
		  temp = compute_max_fact_cost (inf_fact, &loc_n_cost, level);

		  if (GpG.accurate_cost == COMPUTE_MAX_COST
		      && inf_fact->w_is_true <= 0)
		    unsat_facts++;

		  if (DEBUG4)
		    {
		      printf
			("\n\n %d +++ END MAX_COST Prec_overall  Act %s ",
			 ++diff, print_op_name_string (act->position,
						       temp_name));
		      printf
			("\n  fact %s, COST  weight %f cost %f time %f unsatisf_prec %d ",
			 print_ft_name_string (el, temp_name),
			 loc_n_cost.weight, loc_n_cost.act_cost,
			 loc_n_cost.act_time, unsat_facts);
		    }

		  if (GpG.accurate_cost == COMPUTE_MAX_COST)
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

			  if (FALSE
			      && (temp *=
				  local_search.lamda_prec) >
			      local_search.best_cost)
			    return temp;

			}
		    }
		  else if (GpG.accurate_cost == COMPUTE_ADD_COST)
		    {
		      prec += temp;

		      best_prec_cost.weight += loc_n_cost.weight;

		      best_prec_cost.act_cost += loc_n_cost.act_cost;

		      best_prec_cost.act_time += loc_n_cost.act_time;


		    }


		}

	    }


	  for (j = 0; j < gef_conn[act->position].sf->num_PC_end; j++)
	    {
	      el = gef_conn[act->position].sf->PC_end[j];

	      if (el < 0)
		{

		  loc_n_cost.weight = 0.0;
		  loc_n_cost.act_cost = 0.0;
		  loc_n_cost.act_time = 0.0;


		  fact.position = el;
		  temp =
		    compute_dg_facts_cost (fact.position, *fact.level,
					   &loc_n_cost, level);
		  if (GpG.accurate_cost == COMPUTE_MAX_COST)
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
		  else if (GpG.accurate_cost == COMPUTE_ADD_COST)
		    {
		      prec += temp;
		      best_prec_cost.weight += loc_n_cost.weight;
		      best_prec_cost.act_cost += loc_n_cost.act_cost;
		      best_prec_cost.act_time += loc_n_cost.act_time;
		    }
		  continue;

		}

	      if (is_fact_in_additive_effects_start (act->position, el))
		continue;


	      if (CHECK_FACT_POS (el, level))
		{
		  if (DEBUG4)
		    {
		      printf ("\n\n %d +++++ MAX_COST Prec_end  Act %s ",
			      ++diff, print_op_name_string (act->position,
							    temp_name));
		      printf ("\n  fact %s\n",
			      print_ft_name_string (el, temp_name));
		    }

		  loc_n_cost.weight = 0.0;

		  loc_n_cost.act_cost = 0.0;

		  loc_n_cost.act_time = 0.0;



		  inf_fact = CONVERT_FACT_TO_INFORM (el, level);
		  temp = compute_max_fact_cost (inf_fact, &loc_n_cost, level);

		  if (GpG.accurate_cost == COMPUTE_MAX_COST
		      && inf_fact->w_is_true <= 0)
		    unsat_facts++;

		  if (DEBUG4)
		    {
		      printf ("\n\n %d +++ MAX_COST Prec_end  Act %s ",
			      ++diff, print_op_name_string (act->position,
							    temp_name));
		      printf
			("\n  fact %s, COST  weight %f cost %f time %f unsatisf_prec %d ",
			 print_ft_name_string (el, temp_name),
			 loc_n_cost.weight, loc_n_cost.act_cost,
			 loc_n_cost.act_time, unsat_facts);
		    }

		  if (GpG.accurate_cost == COMPUTE_MAX_COST)
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

			  if (FALSE
			      && (temp *=
				  local_search.lamda_prec) >
			      local_search.best_cost)
			    return temp;

			}
		    }
		  else if (GpG.accurate_cost == COMPUTE_ADD_COST)
		    {
		      prec += temp;

		      best_prec_cost.weight += loc_n_cost.weight;

		      best_prec_cost.act_cost += loc_n_cost.act_cost;

		      best_prec_cost.act_time += loc_n_cost.act_time;


		    }


		}

	    }
	}

      total = prec_par * prec;
      precond = local_search.lamda_prec * total;

      if (DEBUG4)
	printf ("\n\n<< Evalutate precondition END");

      if (DEBUG3)
	printf ("  Temp Num. P: %f", prec);

    }

  if (excl_par)
    {
      if (DEBUG4)
	{
	  printf ("\n\n %d +++++ MAX_COST Mutex  Act :", ++diff);
	  print_op_name (neighb_act->act_pos);
	}

      temp = count_mutex_action (act->position, level);

      mutex = excl_par * temp;
      total += mutex;
      mutex *= local_search.lamda_me;

    }

  if (DEBUG4)
    printf ("\n\n<< Evalutate mutex END");

  if (DEBUG3)
    printf ("  Temp  ME: %f", temp);




  best_eff_cost.weight = 0.0;

  best_eff_cost.act_cost = 0.0;

  best_eff_cost.act_time = 0.0;

  if (add_effect_par)
    {
      next_level = level + 1;
      if (neighb_act->constraint_type == C_T_REMOVE_ACTION)
	{

	  unsat_facts = 0;

	  remove_temp_action (neighb_act->act_pos, neighb_act->act_level);

	}
      eff = 0.0;
      for (i = 0; i < gef_conn[neighb_act->act_pos].num_A; i++)
	{
	  fact_pos = gef_conn[neighb_act->act_pos].A[i];

	  if (fact_pos < 0)
	    continue;

	  loc_n_cost.weight = 0.0;
	  loc_n_cost.act_cost = 0.0;
	  loc_n_cost.act_time = 0.0;

	  if (neighb_act->constraint_type == C_T_REMOVE_ACTION)
	    {

	      if (CONVERT_FACT_TO_INFORM (fact_pos, next_level)->w_is_true ==
		  1
		  && CONVERT_FACT_TO_INFORM (fact_pos, next_level)->w_is_goal)
		{


		  if (next_level > GpG.curr_plan_length)
		    {
		      continue;

		    }

		  if (DEBUG4)
		    {
		      printf ("\n\n %d +++++ MAX_COST End_eff  Act %s",
			      ++diff,
			      print_op_name_string (neighb_act->act_pos,
						    temp_name));
		      printf ("\n  fact %s\n",
			      print_ft_name_string (fact_pos, temp_name));
		    }

		  temp =
		    compute_max_fact_cost (CONVERT_FACT_TO_INFORM
					   (fact_pos, next_level),
					   &loc_n_cost, level);

		  if (GpG.accurate_cost == COMPUTE_MAX_COST
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
		      printf ("\n\n %d +++++ MAX_COST End_eff  Act %s",
			      ++diff,
			      print_op_name_string (neighb_act->act_pos,
						    temp_name));
		      printf ("\n  fact %s\n",
			      print_ft_name_string (fact_pos, temp_name));
		    }


		  temp =
		    compute_max_fact_cost (&vectlevel[next_level]->
					   fact[fact_pos], &loc_n_cost,
					   next_level);

		}
	    }

	  if (DEBUG4)
	    {
	      printf ("\n\n %d END MAX_COST End_eff Act: %s ", ++diff,
		      print_op_name_string (neighb_act->act_pos, temp_name));
	      printf ("\n  fact %s, COST  weight %f cost %f time %f",
		      print_ft_name_string (fact_pos, temp_name),
		      loc_n_cost.weight, loc_n_cost.act_cost,
		      loc_n_cost.act_time);
	    }

	  if (GpG.accurate_cost == COMPUTE_MAX_COST)
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
	  else if (GpG.accurate_cost == COMPUTE_ADD_COST)
	    {


	      best_eff_cost.act_cost += loc_n_cost.act_cost;

	      best_eff_cost.act_time += loc_n_cost.act_time;

	      eff += temp;

	      best_eff_cost.weight += loc_n_cost.weight;

	      effect += temp * local_search.lamda_prec;

	    }


	}


      if (gef_conn[neighb_act->act_pos].sf != NULL)
	{
	  for (i = 0; i < gef_conn[neighb_act->act_pos].sf->num_A_start; i++)
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


		      if (next_level > GpG.curr_plan_length)
			{
			  continue;

			}

		      if (DEBUG4)
			{
			  printf ("\n\n %d +++++ MAX_COST Start_eff  Act %s",
				  ++diff,
				  print_op_name_string (neighb_act->act_pos,
							temp_name));
			  printf ("\n  fact %s\n",
				  print_ft_name_string (fact_pos, temp_name));
			}


		      temp =
			compute_max_fact_cost (CONVERT_FACT_TO_INFORM
					       (fact_pos, next_level),
					       &loc_n_cost, level);

		      if (GpG.accurate_cost == COMPUTE_MAX_COST
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
			  printf ("\n\n %d +++++ MAX_COST Start_eff  Act %s",
				  ++diff,
				  print_op_name_string (neighb_act->act_pos,
							temp_name));
			  printf ("\n  fact %s\n",
				  print_ft_name_string (fact_pos, temp_name));
			}

		      temp =
			compute_max_fact_cost (&vectlevel[next_level]->
					       fact[fact_pos], &loc_n_cost,
					       next_level);

		    }
		}

	      if (DEBUG4)
		{
		  printf ("\n\n %d END MAX_COST Start_eff Act %s ", ++diff,
			  print_op_name_string (neighb_act->act_pos,
						temp_name));
		  printf ("\n  fact %s, COST  weight %f cost %f time %f",
			  print_ft_name_string (fact_pos, temp_name),
			  loc_n_cost.weight, loc_n_cost.act_cost,
			  loc_n_cost.act_time);
		}

	      if (GpG.accurate_cost == COMPUTE_MAX_COST)
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
	      else if (GpG.accurate_cost == COMPUTE_ADD_COST)
		{


		  best_eff_cost.act_cost += loc_n_cost.act_cost;

		  best_eff_cost.act_time += loc_n_cost.act_time;

		  eff += temp;

		  best_eff_cost.weight += loc_n_cost.weight;

		  effect += temp * local_search.lamda_prec;

		}


	    }
	}
      total += add_effect_par * eff;
      effect *= add_effect_par;

      if (DEBUG4)
	printf ("\n\n<< Evalutate effect END");

      if (DEBUG3)
	printf ("  Temp Add-E: %f, effects %f total %f  ", temp, effect,
		total);
    }





  if (GpG.lagrange_multipl)
    total = precond + mutex + effect;

  if (unsat_facts > 1)
    total += (unsat_facts - 1);

  if (GpG.local == 2)
    {
      diff = num_try;
      if (diff < GpG.tabu_length && diff > 0)
	total += GpG.delta * (GpG.tabu_length - diff);
      if (DEBUG6)
	printf (" -> tot %f", total);
    }


  neighb_act->cost.weight = total;
  neighb_act->cost.act_cost +=
    best_eff_cost.act_cost + best_prec_cost.act_cost;
  neighb_act->cost.act_time +=
    best_eff_cost.act_time + best_prec_cost.act_time;

  if (GpG.num_solutions && neighb_act->constraint_type == C_T_INSERT_ACTION)
    {
      if (GpG.orig_weight_cost > 0 && GpG.best_cost * 1.4 > GpG.total_cost)
	neighb_act->cost.act_cost += get_action_cost (neighb_act->act_pos);

      if (GpG.orig_weight_time > 0 && GpG.best_time * 1.4 > GpG.total_time)
	neighb_act->cost.act_time +=
	  (10.0 + get_action_time (neighb_act->act_pos, level));
    }

  if (DEBUG4)
    printf ("\n\n<<<< Evalutate action END  Act: %s",
	    print_op_name_string (act->position, temp_name));

  if (DEBUG3)
    printf ("\n -> tot %f", total);

  if (DEBUG4)
    printf
      ("\n--------------------------------------------------------------\n\n");


  return (total);
}
