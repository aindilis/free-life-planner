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
 * File: LocalSearch.c
 * Description: Local search method
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
#include "LpgTime.h"
#include "check.h"
#include "numeric.h"
#include "ActionSubgraph.h"
#include "H_relaxed.h"
#include "H_max.h"
#include "utilities.h"
#include "LpgOutput.h"
#include "output.h"




float
fast_action_cost (neighb_list neighb_act)
{
  register int temp = 0, temp1, i, k;
  int *ptr, level, j, diff;
  register EfConn *act;
  auto float total, prec_par, excl_par, add_effect_par;
  auto float precond, mutex, effect, act_cost;
  precond = mutex = effect = act_cost = 0.0;
  level = neighb_act->act_level;
  neighb_act->cost.act_cost = 0.0;
  neighb_act->cost.act_time = 0.0;
  total = 0.0;

  act = CONVERT_ACTION_TO_VERTEX (neighb_act->act_pos);


  if (neighb_act->constraint_type == C_T_REMOVE_ACTION)
    {
      prec_par = GpG.used_prec_par;
      excl_par = GpG.used_excl_par;
      add_effect_par = GpG.used_add_effect_par;
    }

  else
    {
      prec_par = GpG.prec_par;
      excl_par = GpG.excl_par;
      add_effect_par = GpG.add_effect_par;
    }
  if (DEBUG4)
    printf ("\n\n Evalutate action");
  if (DEBUG3)
    printf ("\nFAST_COST  Act: %s, level %d\n",
	    print_op_name_string (neighb_act->act_pos, temp_name), level);

  if (prec_par)
    {
      for (i = 0, temp = 0; i < act->num_precond; i++)
	temp +=
	  count_bit1 (act->bit_precond[i].
		      uid_mask & (~vectlevel[level]->
				  fact_vect[act->bit_precond[i].uid_block]));

      total = prec_par * temp;
      precond = act->lamda_prec * total;
      if (DEBUG3)
	printf ("  Num. P: %d", temp);
    }

  for (i = 0; i < gnum_ft_block; i++)
    temp +=
      count_bit1 (act->ft_exclusive_vect[i] & vectlevel[level]->
		  noop_act_vect[i] & vectlevel[level]->noop_prec_act_vect[i]);
  mutex = excl_par * temp;
  total += mutex;
  mutex *= act->lamda_me;
  if (DEBUG3)
    printf ("  ME: %d", temp);

  if (add_effect_par)
    {
      level++;
      if (neighb_act->constraint_type == C_T_REMOVE_ACTION)
	ptr = vectlevel[level]->true_crit_vect;

      else
	ptr = vectlevel[level]->false_crit_vect;
      if (neighb_act->constraint_type == C_T_REMOVE_ACTION
	  && !GpG.accurate_cost)
	for (i = 0, temp = 0; i < act->num_add_effect; i++)
	  {
	    k = 32;
	    j = act->bit_add_effect[i].uid_block * 32;
	    temp1 =
	      act->bit_add_effect[i].uid_mask & *(ptr +
						  act->bit_add_effect[i].
						  uid_block);

	    while (temp1)
	      {
		k--;
		if ((temp1 & FIRST_1))
		  {
		    temp += vectlevel[level]->fact[j + k].w_is_goal;
		    effect += CONVERT_FACT_TO_VERTEX (j + k)->lamda_prec;
		  }
		temp1 <<= 1;
	      }
	  }

      else
	{
	  for (i = 0, temp = 0; i < act->num_add_effect; i++)
	    temp +=
	      count_bit1 (act->bit_add_effect[i].
			  uid_mask & *(ptr +
				       act->bit_add_effect[i].uid_block));

	  effect = temp;
	}
      total += add_effect_par * temp;
      effect *= add_effect_par;
      if (DEBUG3)
	printf ("  Add-E: %d", temp);
    }




  if (GpG.local == 2)
    {
      diff = num_try - CONVERT_ACTION_TO_VERTEX (neighb_act->act_pos)->step;
      if (diff < GpG.tabu_length && diff > 0)
	total += GpG.delta * (GpG.tabu_length - diff);
      if (DEBUG6)
	printf (" -> tot %f", total);
    }
  neighb_act->cost.weight = total;

  if (DEBUG3)
    printf ("\n -> tot %f", total);
  return (total);
}



float
action_cost (neighb_list Action)
{


  memcpy (Hvar.common_values, vectlevel[Action->act_level]->numeric->values,
	  gnum_comp_var * sizeof (float));

  if (GpG.accurate_cost == COMPUTE_MAX_COST)
    return max_action_cost (Action);

  else if (GpG.accurate_cost >= COMPUTE_DG_SUM_COST)
    dg_action_cost (Action);

  else
    return fast_action_cost (Action);

  return 0;
}


int
action_eff_cost (register inform_list infAction)
{
  register int i, temp;
  int *ptr, level, cel;
  EfConn *act;
  level = *infAction->level + 1;

  act = &gef_conn[infAction->position];
  if (infAction->w_is_used)
    ptr = vectlevel[level]->true_crit_vect;

  else
    {

      exit (0);
    }

  for (i = 0, temp = 0; i < act->num_add_effect; i++)
    temp +=
      count_bit1 (act->bit_add_effect[i].
		  uid_mask & *(ptr + act->bit_add_effect[i].uid_block));

  if (gef_conn[infAction->position].sf != NULL)
    for (i = 0; i < gef_conn[infAction->position].sf->num_A_start; i++)
      {
	cel = gef_conn[infAction->position].sf->A_start[i];
	if (cel < 0)
	  continue;
	if (is_fact_in_delete_effects (infAction->position, cel))
	  continue;
	if (vectlevel[level]->fact[cel].w_is_goal)
	  temp++;
      }
  return (temp);
}






float
find_min (constraints_list inf_tofix, int *pos_temp_vect, int num,
	  int *num_min, int *num_neg)
{
  register int i, j;
  int pos = 1, neg = 1, stop, stop1;
  float cost, minor_cost = MAXFLOAT;
  float max_act_incons, max_act_cost, max_act_time, weight, weight_c,
    weight_t;
  max_act_incons = 0.0;
  max_act_cost = 0.0;
  max_act_time = 0.0;
  weight = weight_c = weight_t = 0.0;

  local_search.best_cost = MAXFLOAT;
  local_search.num_actions = 0;


  local_search.max_act_incons = MAXFLOAT;
  local_search.max_act_cost = MAXFLOAT;
  local_search.max_act_time = MAXFLOAT;

  local_search.best_cost = MAXFLOAT;
  for (i = num - 1; i >= 0; i--)
    {






      action_cost (*(neighb_vect + i));

      if (max_act_incons < (*(neighb_vect + i))->cost.weight
	  && (*(neighb_vect + i))->cost.weight < MAX_COST)
	max_act_incons = (*(neighb_vect + i))->cost.weight;
      if (max_act_cost < (*(neighb_vect + i))->cost.act_cost
	  && (*(neighb_vect + i))->cost.act_cost < MAX_COST)
	max_act_cost = (*(neighb_vect + i))->cost.act_cost;
      if (max_act_time < (*(neighb_vect + i))->cost.act_time
	  && (*(neighb_vect + i))->cost.act_time < MAX_COST)
	max_act_time = (*(neighb_vect + i))->cost.act_time;
      if (i == (num - 1))
	{
	  local_search.max_act_incons = (*(neighb_vect + i))->cost.weight;
	  local_search.max_act_cost = (*(neighb_vect + i))->cost.act_cost;
	  local_search.max_act_time = (*(neighb_vect + i))->cost.act_time;
	  if (DEBUG6)
	    printf
	      ("\n local_search.max_act_incons %.2f  local_search.max_act_cost %.2f  local_search.max_act_time %.2f ",
	       local_search.max_act_incons, local_search.max_act_cost,
	       local_search.max_act_time);
	}

      else
	{
	  stop1 = stop = 3;
	  if ((*(neighb_vect + i))->cost.weight <=
	      local_search.max_act_incons)
	    stop--;
	  if ((*(neighb_vect + i))->cost.weight >=
	      local_search.max_act_incons)
	    stop1--;
	  if (GpG.orig_weight_cost <= 0
	      || (*(neighb_vect + i))->cost.act_cost <=
	      local_search.max_act_cost)
	    stop--;
	  if (GpG.orig_weight_cost <= 0
	      || (*(neighb_vect + i))->cost.act_cost >=
	      local_search.max_act_cost)
	    stop1--;
	  if (GpG.orig_weight_time <= 0
	      || (*(neighb_vect + i))->cost.act_time <=
	      local_search.max_act_time)
	    stop--;
	  if (GpG.orig_weight_time <= 0
	      || (*(neighb_vect + i))->cost.act_time >=
	      local_search.max_act_time)
	    stop1--;
	  if (DEBUG6)
	    printf
	      ("\n (*(neighb_vect+i))->cost.weight %.2f   (*(neighb_vect+i))->cost.act_cost %.2f   (*(neighb_vect+i))->cost.act_time %.2f ",
	       (*(neighb_vect + i))->cost.weight,
	       (*(neighb_vect + i))->cost.act_cost,
	       (*(neighb_vect + i))->cost.act_time);
	  if (stop <= 0)
	    {
	      local_search.max_act_incons = (*(neighb_vect + i))->cost.weight;
	      local_search.max_act_cost = (*(neighb_vect + i))->cost.act_cost;
	      local_search.max_act_time = (*(neighb_vect + i))->cost.act_time;
	      if (DEBUG6)
		printf
		  ("\n local_search.max_act_incons %.2f   local_search.max_act_cost %.2f   local_search.max_act_time %.2f ",
		   local_search.max_act_incons, local_search.max_act_cost,
		   local_search.max_act_time);
	    }

	  else if (stop1 > 0)
	    {
	      if (local_search.max_act_incons <
		  (*(neighb_vect + i))->cost.weight)
		local_search.max_act_incons =
		  (*(neighb_vect + i))->cost.weight;
	      if (local_search.max_act_cost <
		  (*(neighb_vect + i))->cost.act_cost)
		local_search.max_act_cost =
		  (*(neighb_vect + i))->cost.act_cost;
	      if (local_search.max_act_time <
		  (*(neighb_vect + i))->cost.act_time)
		local_search.max_act_time =
		  (*(neighb_vect + i))->cost.act_time;
	      if (DEBUG6)
		printf
		  ("\n local_search.max_act_incons %.2f   local_search.max_act_cost %.2f   local_search.max_act_time %.2f ",
		   local_search.max_act_incons, local_search.max_act_cost,
		   local_search.max_act_time);
	    }
	}
    }

  if (GpG.evaluation_function == 1 || GpG.evaluation_function == 2)
    {

      if (max_act_cost && GpG.orig_weight_cost)
	weight_c = GpG.orig_weight_cost * max_act_cost;
      if (max_act_time && GpG.orig_weight_time)
	weight_t = GpG.orig_weight_time * max_act_time;

      weight = weight_c + weight_t;


      if (GpG.num_false_tot > 0 && GpG.evaluation_function == 1)
	{
	  weight *= GpG.num_false_tot;
	}

      if (weight <= 0.0)
	weight = 1.0;
    }
  else
    {
      if (max_act_cost && GpG.orig_weight_cost)
	weight_c = GpG.orig_weight_cost / max_act_cost;
      if (max_act_time && GpG.orig_weight_time)
	weight_t = GpG.orig_weight_time / max_act_time;

      if (GpG.num_false_tot > 0)
	{
	  weight_c /= GpG.num_false_tot;
	  weight_t /= GpG.num_false_tot;
	}

      weight = weight_c + weight_t;

    }




  if (DEBUG6)
    {
      printf
	("\n \n#########################################################");
      printf
	("\n ======= WEIGHT %.2f  wc %.2f wt %.2f max_inc %.2f max_cost %.2f max time %.2f",
	 weight, GpG.weight_cost, GpG.weight_time, max_act_incons,
	 max_act_cost, max_act_time);
    }
  if (DEBUG3)
    printf ("\n\n>< EVALUTATION RESULTS ><\n");

  for (i = 0; i < num; i++)
    {



      if (GpG.evaluation_function == 0)
	{
	  cost =
	    max_act_incons * ((*(neighb_vect + i))->cost.act_cost * weight_c +
			      (*(neighb_vect + i))->cost.act_time *
			      weight_t) + (*(neighb_vect + i))->cost.weight;
	}
      else
	{



	  cost =
	    max_act_incons *
	    (((*(neighb_vect + i))->cost.act_cost * GpG.orig_weight_cost +
	      (*(neighb_vect + i))->cost.act_time * GpG.orig_weight_time) /
	     weight) + (*(neighb_vect + i))->cost.weight;

	}

      if (DEBUG3)
	printf
	  ("\n neighborhood Act %s, level %d \n   Incons %.2f   Cost %.2f   Time %.2f   Tot weight cost %.2f ",
	   print_op_name_string ((*(neighb_vect + i))->act_pos, temp_name),
	   (*(neighb_vect + i))->act_level, (*(neighb_vect + i))->cost.weight,
	   (*(neighb_vect + i))->cost.act_cost,
	   (*(neighb_vect + i))->cost.act_time, cost);

      if (minor_cost > 0)
	{
	  if ((minor_cost - cost) > MIN_VALUE)
	    {
	      pos = 1;
	      *pos_temp_vect = i;
	      minor_cost = cost;
	      local_search.best_cost = cost;
	    }

	  else if (abs (cost - minor_cost) < MIN_VALUE)
	    {


	      for (j = 0; j < pos; j++)

		{
		  if (j >= MAX_MAX_NODES)
		    {
		      exit (1);
		    }



		  if (i >= MAX_MAX_NODES)
		    {
		      exit (1);
		    }


		  if ((*(neighb_vect + i))->act_pos ==
		      (*(neighb_vect + *(pos_temp_vect + j)))->act_pos)
		    break;
		}
	      if (j == pos)
		*(pos_temp_vect + pos++) = i;
	    }
	}

      else if (cost <= 0.0)
	{
	  if (cost < minor_cost)
	    {
	      pos = 1;
	      if (neg >= MAX_MAX_NODES)
		{
		  exit (1);
		}



	      *(pos_temp_vect + neg++) = *pos_temp_vect;
	      *pos_temp_vect = i;
	      minor_cost = cost;
	      local_search.best_cost = cost;
	    }

	  else if (cost == minor_cost)
	    {


	      for (j = 0; j < pos; j++)
		{
		  if (j >= MAX_MAX_NODES)
		    {
		      exit (1);
		    }


		  if (i >= MAX_MAX_NODES)
		    {
		      exit (1);
		    }


		  if ((*(neighb_vect + i))->act_pos ==
		      (*(neighb_vect + *(pos_temp_vect + j)))->act_pos)
		    break;
		}
	      if (j == pos)
		{
		  *(pos_temp_vect + neg++) = *(pos_temp_vect + pos);
		  *(pos_temp_vect + pos++) = i;
		}
	    }

	  else
	    *(pos_temp_vect + neg++) = i;
	}
    }
  *num_min = pos;
  *num_neg = neg;

  return (minor_cost);
}




float
check_value (float new)
{
  if (new < MIN_POS_S_S)
    {


      return MIN_POS_S_S;
    }

  else if (new > MAX_POS_S_S)
    {


      return MAX_POS_S_S;
    }

  return new;
}





void
update_precond ()
{
  int false_fa_pos, action_pos;
  float step_prec_incr = GpG.s_s_step;
  constraints_list inconsistenza;



  for (false_fa_pos = 0; false_fa_pos < GpG.num_false_fa; false_fa_pos++)
    {
      inconsistenza = unsup_fact[false_fa_pos];

      if (*inconsistenza->level == GpG.curr_plan_length)
	{
	  GpG.goal_lambda = check_value (GpG.goal_lambda + step_prec_incr);
	  GpG.flag_decr_lm_goal = 1;
	  continue;
	}

      if (GET_ACTION_OF_LEVEL (*inconsistenza->level)->w_is_used)
	{
	  action_pos = GET_ACTION_POSITION_OF_LEVEL (*inconsistenza->level);


	  CONVERT_ACTION_TO_VERTEX (action_pos)->lamda_prec =
	    check_value (CONVERT_ACTION_TO_VERTEX (action_pos)->lamda_prec +
			 step_prec_incr);
	  if (CONVERT_ACTION_TO_VERTEX (action_pos)->flag_decr_lm == 0)
	    CONVERT_ACTION_TO_VERTEX (action_pos)->flag_decr_lm = 1;
	  if (CONVERT_ACTION_TO_VERTEX (action_pos)->flag_decr_lm == 2)
	    CONVERT_ACTION_TO_VERTEX (action_pos)->flag_decr_lm = 3;
	}

    }
}




void
update_mutex (int pos_action)
{

  float step_me_incr = GpG.s_s_step;

  CONVERT_ACTION_TO_VERTEX (pos_action)->lamda_me =
    check_value (CONVERT_ACTION_TO_VERTEX (pos_action)->lamda_me +
		 step_me_incr);
  if (CONVERT_ACTION_TO_VERTEX (pos_action)->flag_decr_lm == 0)
    CONVERT_ACTION_TO_VERTEX (pos_action)->flag_decr_lm = 2;
  if (CONVERT_ACTION_TO_VERTEX (pos_action)->flag_decr_lm == 1)
    CONVERT_ACTION_TO_VERTEX (pos_action)->flag_decr_lm = 3;
}


void
update_decr_me_prec ()
{
  int pos_action;
  int cont_level;
  float step_me_decr, step_prec_decr;

  step_prec_decr = GpG.sqr_s_s;
  step_me_decr = GpG.sqr_s_s;



  if (GpG.flag_decr_lm_goal == 0)
    GpG.goal_lambda = check_value (GpG.goal_lambda - step_prec_decr);
  else
    GpG.flag_decr_lm_goal = 0;


  for (cont_level = 0; cont_level < GpG.curr_plan_length; cont_level++)
    {
      if (CHECK_ACTION_OF_LEVEL (cont_level) == FALSE)
	continue;

      pos_action = GET_ACTION_POSITION_OF_LEVEL (cont_level);

      switch (CONVERT_ACTION_TO_VERTEX (pos_action)->flag_decr_lm)
	{

	case 0:
	  CONVERT_ACTION_TO_VERTEX (pos_action)->lamda_prec =
	    check_value (CONVERT_ACTION_TO_VERTEX (pos_action)->lamda_prec -
			 step_prec_decr);
	  CONVERT_ACTION_TO_VERTEX (pos_action)->lamda_me =
	    check_value (CONVERT_ACTION_TO_VERTEX (pos_action)->lamda_me -
			 step_me_decr);
	  break;

	case 1:
	  CONVERT_ACTION_TO_VERTEX (pos_action)->lamda_me =
	    check_value (CONVERT_ACTION_TO_VERTEX (pos_action)->lamda_me -
			 step_me_decr);
	  CONVERT_ACTION_TO_VERTEX (pos_action)->flag_decr_lm = 0;
	  break;

	case 2:
	  CONVERT_ACTION_TO_VERTEX (pos_action)->lamda_prec =
	    check_value (CONVERT_ACTION_TO_VERTEX (pos_action)->lamda_prec -
			 step_prec_decr);
	  CONVERT_ACTION_TO_VERTEX (pos_action)->flag_decr_lm = 0;
	  break;

	case 3:
	  CONVERT_ACTION_TO_VERTEX (pos_action)->flag_decr_lm = 0;
	  break;
	}

    }

}





void
update_precond_multilevel ()
{
  int false_fa_pos, action_pos;
  float step_prec_incr = GpG.s_s_step;
  constraints_list inconsistenza;



  for (false_fa_pos = 0; false_fa_pos < GpG.num_false_fa; false_fa_pos++)
    {

      inconsistenza = unsup_fact[false_fa_pos];

      if (*inconsistenza->level == GpG.curr_plan_length)
	{
	  GpG.goal_lambda = check_value (GpG.goal_lambda + step_prec_incr);
	  GpG.flag_decr_lm_goal = 1;
	  continue;
	}

      if (GET_ACTION_OF_LEVEL (*inconsistenza->level)->w_is_used)
	{




	  action_pos = GET_ACTION_POSITION_OF_LEVEL (*inconsistenza->level);
	  vectlevel[*inconsistenza->level]->lambda_prec[action_pos] =
	    check_value (vectlevel[*inconsistenza->level]->
			 lambda_prec[action_pos] + step_prec_incr);
	  if (vectlevel[*inconsistenza->level]->flag_decr_lm == 0)
	    vectlevel[*inconsistenza->level]->flag_decr_lm = 1;
	  if (vectlevel[*inconsistenza->level]->flag_decr_lm == 2)
	    vectlevel[*inconsistenza->level]->flag_decr_lm = 3;
	}

    }
}



void
update_decr_me_prec_multilevel ()
{
  int pos_level;
  float step_me_decr, step_prec_decr;

  step_prec_decr = GpG.sqr_s_s;
  step_me_decr = GpG.sqr_s_s;


  if (GpG.flag_decr_lm_goal == 0)
    GpG.goal_lambda = check_value (GpG.goal_lambda - step_prec_decr);
  else
    GpG.flag_decr_lm_goal = 0;

  for (pos_level = 0; pos_level < GpG.max_plan_length; pos_level++)
    {

      if (!(GET_ACTION_OF_LEVEL (pos_level)->w_is_used))
	continue;
      switch (vectlevel[pos_level]->flag_decr_lm)
	{

	case 0:
	  vectlevel[pos_level]->
	    lambda_prec[GET_ACTION_POSITION_OF_LEVEL (pos_level)] =
	    check_value (vectlevel[pos_level]->
			 lambda_prec[GET_ACTION_POSITION_OF_LEVEL (pos_level)]
			 - step_prec_decr);
	  vectlevel[pos_level]->
	    lambda_me[GET_ACTION_POSITION_OF_LEVEL (pos_level)] =
	    check_value (vectlevel[pos_level]->
			 lambda_me[GET_ACTION_POSITION_OF_LEVEL (pos_level)] -
			 step_me_decr);
	  break;
	case 1:
	  vectlevel[pos_level]->
	    lambda_me[GET_ACTION_POSITION_OF_LEVEL (pos_level)] =
	    check_value (vectlevel[pos_level]->
			 lambda_me[GET_ACTION_POSITION_OF_LEVEL (pos_level)] -
			 step_me_decr);
	  vectlevel[pos_level]->flag_decr_lm = 0;
	  break;

	case 2:
	  vectlevel[pos_level]->
	    lambda_prec[GET_ACTION_POSITION_OF_LEVEL (pos_level)] =
	    check_value (vectlevel[pos_level]->
			 lambda_prec[GET_ACTION_POSITION_OF_LEVEL (pos_level)]
			 - step_prec_decr);
	  vectlevel[pos_level]->flag_decr_lm = 0;
	  break;

	case 3:
	  vectlevel[pos_level]->flag_decr_lm = 0;
	  break;
	}

    }

}



void
update_mutex_multilevel (int level, int pos_action)
{

  float step_me_incr = GpG.s_s_step;

  vectlevel[level]->lambda_me[pos_action] =
    check_value (vectlevel[level]->lambda_me[pos_action] + step_me_incr);
  if (vectlevel[level]->flag_decr_lm == 0)
    vectlevel[level]->flag_decr_lm = 2;
  if (vectlevel[level]->flag_decr_lm == 1)
    vectlevel[level]->flag_decr_lm = 3;
}



int
not_tabu (int tofix)
{
  int diff;
  if (GpG.local != 1)
    return TRUE;
  diff = num_try - CONVERT_ACTION_TO_VERTEX (tofix)->step;
  if (diff < GpG.tabu_length && diff >= 0)
    return FALSE;

  else
    return TRUE;
}






int
choose_actions (register inform_list inform_tofix, int initialize)
{
  int i, j, level, propagation, skip;
  EfConn *o_tofix;
  FtConn *tofix, *temp_tofix;
  inform_list inf_action, noop_fact;
  register int el, cel;
  float cost, cost1;
  neighb temp_act;
  noop_not_in *temp_noop;
  skip = 0;

  if (initialize != 0)
    reset_neighborhood ();

  if (*inform_tofix->level > GpG.curr_plan_length)
    return 0;

  if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
    return (choose_actions_dg_list (inform_tofix, initialize));



  if (inform_tofix->action_fact != IS_FACT)
    {

      o_tofix = CONVERT_ACTION_TO_VERTEX (inform_tofix->position);
      temp_noop = inform_tofix->treated;
      while (temp_noop)
	{


	  if (vectlevel[*inform_tofix->level + 1]->
	      fact[temp_noop->position].w_is_goal
	      && vectlevel[*inform_tofix->level]->fact[temp_noop->position].
	      w_is_true)
	    break;
	  temp_noop = temp_noop->next;
	}
      if (temp_noop == NULL)
	return 0;

      temp_act.act_pos = vectlevel[*inform_tofix->level]->action.position;
      temp_act.act_level = *inform_tofix->level;
      temp_act.constraint_type = C_T_REMOVE_ACTION;
      insert_element_in_neighb (&temp_act);

      for (i = *inform_tofix->level + 1; i < GpG.curr_plan_length; i++)
	if (vectlevel[i]->fact[temp_noop->position].w_is_used)
	  {
	    temp_act.act_pos = vectlevel[i]->action.position;
	    temp_act.act_level = i;
	    temp_act.constraint_type = C_T_REMOVE_ACTION;

	    insert_element_in_neighb (&temp_act);
	    break;
	  }


      for (i = *inform_tofix->level; i > 0; i--)
	if (vectlevel[i]->fact[temp_noop->position].w_is_true
	    &&
	    is_fact_in_additive_effects (GET_ACTION_POSITION_OF_LEVEL
					 (i - 1), temp_noop->position))
	  {
	    temp_act.act_pos = vectlevel[i - 1]->action.position;
	    temp_act.act_level = i - 1;
	    temp_act.constraint_type = C_T_REMOVE_ACTION;

	    insert_element_in_neighb (&temp_act);
	    break;
	  }
      return num_neighborhood;
    }

  else
    {



      tofix = &gft_conn[inform_tofix->position];
      level = *inform_tofix->level;

      temp_tofix = tofix;

      while (level < GpG.curr_plan_length && initialize > 0)
	{


	  propagation = 1;
	  inf_action = GET_ACTION_OF_LEVEL (level);
	  el = inf_action->position;


	  if (is_fact_in_preconditions (el, tofix->position)
	      && inf_action->w_is_used && not_tabu (el))
	    {

	      temp_act.act_pos = el;
	      temp_act.act_level = level;
	      temp_act.constraint_type = C_T_REMOVE_ACTION;
	      if (FALSE && (cost1 = action_eff_cost (inf_action)) <= 0.0
		  && GpG.fast_greedy)
		{


		  if (DEBUG3)
		    printf ("\nNull cost for action %s  cost %f",
			    print_op_name_string (temp_act.act_pos,
						  temp_name), cost1);
		  insert_remove_action (temp_act.act_pos, temp_act.act_level,
					temp_act.constraint_type,
					GpG.approximation_level);

		}

	      else
		{
		  insert_element_in_neighb (&temp_act);
		  propagation = 0;
		}
	    }

	  if (is_fact_in_preconditions_overall (el, tofix->position)
	      && !is_fact_in_additive_effects_start (el, tofix->position)
	      && inf_action->w_is_used && not_tabu (el))
	    {

	      temp_act.act_pos = el;
	      temp_act.act_level = level;
	      temp_act.constraint_type = C_T_REMOVE_ACTION;
	      if (FALSE && (cost1 = action_eff_cost (inf_action)) <= 0.0
		  && GpG.fast_greedy)
		{


		  if (DEBUG3)
		    printf ("\nNull cost for action %s  cost %f",
			    print_op_name_string (temp_act.act_pos,
						  temp_name), cost1);

		  insert_remove_action (temp_act.act_pos,
					temp_act.act_level,
					temp_act.constraint_type,
					GpG.approximation_level);

		}

	      else
		{
		  insert_element_in_neighb (&temp_act);
		  propagation = 0;
		}
	    }
	  inf_action = GET_ACTION_OF_LEVEL (level - 1);
	  if (level == 0)
	    inf_action = GET_ACTION_OF_LEVEL (level);
	  el = inf_action->position;


	  if (is_fact_in_preconditions_end (el, tofix->position)
	      & !(is_fact_in_additive_effects_start (el, tofix->position))
	      && inf_action->w_is_used && not_tabu (el))
	    {

	      temp_act.act_pos = el;
	      temp_act.act_level = level - 1;
	      temp_act.constraint_type = C_T_REMOVE_ACTION;
	      if (FALSE && (cost1 = action_eff_cost (inf_action)) <= 0.0
		  && GpG.fast_greedy)
		{


		  if (DEBUG3)
		    printf ("\nNull cost for action %s  cost %f",
			    print_op_name_string (temp_act.act_pos,
						  temp_name), cost1);

		  insert_remove_action (temp_act.act_pos,
					temp_act.act_level,
					temp_act.constraint_type,
					GpG.approximation_level);

		}

	      else
		{
		  insert_element_in_neighb (&temp_act);
		  propagation = 0;
		  skip = 1;
		}
	    }
	  if (level < GpG.curr_plan_length
	      && vectlevel[level]->noop_act[inform_tofix->position].
	      w_is_goal && propagation == 1)

	    {
	      level++;
	    }

	  else
	    break;
	}
      level = *inform_tofix->level;
      if (GpG.num_solutions && initialize > 0 && level > 0)
	{


	  for (level--; level >= 0; level--)
	    if (GET_ACTION_POSITION_OF_LEVEL (level) >= 0
		&& check_mutex_noop (inform_tofix->position, level) >= 0)
	      break;
	  if (level >= 0)
	    {
	      inf_action = GET_ACTION_OF_LEVEL (level);
	      el = inf_action->position;
	      if (vectlevel[level]->fact[inform_tofix->position].w_is_true
		  && not_tabu (el))
		{

		  temp_act.act_pos = el;
		  temp_act.act_level = level;
		  temp_act.constraint_type = C_T_REMOVE_ACTION;
		  if (FALSE && (cost1 = action_eff_cost (inf_action)) <= 0.0
		      && GpG.fast_greedy)
		    {


		      if (DEBUG3)
			printf ("\nNull cost for action %s  cost %f",
				print_op_name_string (temp_act.act_pos,
						      temp_name), cost1);

		      insert_remove_action (temp_act.act_pos,
					    temp_act.act_level,
					    temp_act.constraint_type,
					    GpG.approximation_level);

		    }

		  else
		    {
		      insert_element_in_neighb (&temp_act);
		      propagation = 0;
		      skip = 1;
		    }
		}
	    }
	}
      if (initialize > 0)
	{
	  if (num_neighborhood == 0
	      && *inform_tofix->level < GpG.curr_plan_length)
	    {
	      if (inform_tofix->w_is_goal)
		{

		  inform_tofix->w_is_goal = 0;
		  vectlevel[*inform_tofix->level]->
		    prec_vect[GUID_BLOCK (tofix->position)] &=
		    ~(GUID_MASK (tofix->position));
		  vectlevel[*inform_tofix->level]->
		    true_crit_vect[GUID_BLOCK (tofix->position)] &=
		    ~(GUID_MASK (tofix->position));
		  vectlevel[*inform_tofix->level]->
		    false_crit_vect[GUID_BLOCK (tofix->position)] &=
		    ~(GUID_MASK (tofix->position));
		}

	      return num_neighborhood;
	    }
	}
      level = *inform_tofix->level;
      noop_fact = NULL;


      if (GpG.consider_current_level && initialize > 0)
	{
	  for (j = 0; j < gft_conn[tofix->position].num_A; j++)
	    if (CHECK_ACTION_POS
		((cel = gft_conn[tofix->position].A[j]), level)
		&& !is_fact_in_delete_effects (cel, tofix->position))
	      {
		temp_act.act_pos = cel;
		temp_act.act_level = level;
		temp_act.constraint_type = C_T_INSERT_ACTION;
		if (not_tabu (cel))
		  {

		    insert_element_in_neighb (&temp_act);
		  }
	      }
	}
      level--;
      if (level < 0)
	level = 0;

      if ((skip != 1 || initialize == FALSE)
	  && GET_ACTION_OF_LEVEL (level)->position >= 0
	  && CHECK_NOOP_POS (tofix->position, level)
	  && check_mutex_noop (tofix->position, level) == -1)
	{


	  for (j = 0; j < gft_conn[tofix->position].num_A; j++)
	    if (CHECK_ACTION_POS
		((cel = gft_conn[tofix->position].A[j]), level)
		&& !is_fact_in_delete_effects (cel, tofix->position))
	      {
		temp_act.act_pos = cel;
		temp_act.act_level = level;
		temp_act.constraint_type = C_T_INSERT_ACTION;


		if (CHECK_ACTION_POSTION_OF_LEVEL (cel, level))
		  {

		  }

		else if (not_tabu (cel))
		  {

		    if (FALSE
			&& (cost = fast_action_cost (&temp_act)) <= 0.0
			&& GpG.fast_greedy && !GpG.num_solutions)
		      {


			if (DEBUG3)
			  printf ("\nNull cost for action %s  cost %f",
				  print_op_name_string (temp_act.act_pos,
							temp_name), cost);

			insert_remove_action (temp_act.act_pos,
					      temp_act.act_level,
					      temp_act.constraint_type,
					      GpG.approximation_level);

			return -1;
		      }

		    else
		      {

			insert_element_in_neighb (&temp_act);

		      }
		  }
		if (num_neighborhood > GpG.num_act_cons)
		  break;
	      }


	  if (level > 0 && CHECK_NOOP_POS (tofix->position, level)
	      && check_mutex_noop (tofix->position, level) == -1)
	    {
	      noop_fact = &vectlevel[level]->fact[tofix->position];


	      i = choose_actions (noop_fact, FALSE);
	      if (i < 0)
		return i;
	    }
	}
    }

  return (num_neighborhood);
}






int
choose_actions_dg_list (register inform_list inform_tofix, int initialize)
{

  int i, j, level, curr_level, action_level, next_level, propagation, skip;
  EfConn *o_tofix;
  FtConn *tofix, *temp_tofix;
  inform_list inf_action, noop_fact;

  register int el;
  register int cel;
  float cost, cost1, total;
  neighb temp_act;
  noop_not_in *temp_noop;
  int best_action = -1, best_level = 0, best_act_type;
  node_cost loc_n_cost, best_n_cost;
  int num_lev_neighb_act = 0, max_lev_neighb_act = 4;
  static int *elem_lev_neighb_act = NULL;
  float best_weight_cost = 0.0, loc_weight_cost = 0.0;
  dg_inform_list loc_dg_cost;




  if (elem_lev_neighb_act == NULL)
    elem_lev_neighb_act =
      (int *) calloc (max_lev_neighb_act + 1, sizeof (int));


  skip = 0;

  if (initialize != 0)
    {

      reset_neighborhood ();
    }
  if (*inform_tofix->level > GpG.curr_plan_length)
    return 0;

  if (inform_tofix->action_fact != IS_FACT)
    {
      o_tofix = CONVERT_ACTION_TO_VERTEX (inform_tofix->position);

      temp_noop = inform_tofix->treated;
      while (temp_noop)
	{
	  if (vectlevel[*inform_tofix->level + 1]->fact[temp_noop->position].
	      w_is_goal
	      && vectlevel[*inform_tofix->level]->fact[temp_noop->position].
	      w_is_true)
	    break;
	  temp_noop = temp_noop->next;
	}
      if (temp_noop == NULL)
	return 0;
      temp_act.act_pos = vectlevel[*inform_tofix->level]->action.position;
      temp_act.act_level = *inform_tofix->level;
      temp_act.constraint_type = C_T_REMOVE_ACTION;
      insert_element_in_neighb (&temp_act);
      for (i = *inform_tofix->level + 1; i < GpG.curr_plan_length; i++)
	if (vectlevel[i]->fact[temp_noop->position].w_is_used)
	  {

	    temp_act.act_pos = vectlevel[i]->action.position;
	    temp_act.act_level = i;
	    temp_act.constraint_type = C_T_REMOVE_ACTION;
	    insert_element_in_neighb (&temp_act);
	    break;

	  }


      for (i = *inform_tofix->level; i > 0; i--)
	if (vectlevel[i]->fact[temp_noop->position].w_is_true
	    &&
	    is_fact_in_additive_effects (GET_ACTION_POSITION_OF_LEVEL (i - 1),
					 temp_noop->position))
	  {
	    temp_act.act_pos = vectlevel[i - 1]->action.position;
	    temp_act.act_level = i - 1;
	    temp_act.constraint_type = C_T_REMOVE_ACTION;

	    insert_element_in_neighb (&temp_act);
	    break;

	  }

      return num_neighborhood;


    }
  else
    {



      tofix = &gft_conn[inform_tofix->position];
      level = *inform_tofix->level;


      temp_tofix = tofix;




      while (level < GpG.curr_plan_length && initialize > 0)
	{


	  propagation = 1;

	  inf_action = GET_ACTION_OF_LEVEL (level);
	  el = inf_action->position;


	  if (is_fact_in_preconditions (el, tofix->position)
	      && inf_action->w_is_used && not_tabu (el))
	    {
	      temp_act.act_pos = el;
	      temp_act.act_level = level;
	      temp_act.constraint_type = C_T_REMOVE_ACTION;

	      if (FALSE && (cost1 = action_eff_cost (inf_action)) <= 0.0
		  && GpG.fast_greedy)
		{
		  if (DEBUG4)
		    printf ("\nCOSTO nullo %f", cost1);
		  insert_remove_action (temp_act.act_pos, temp_act.act_level,
					temp_act.constraint_type,
					GpG.approximation_level);
		}
	      else
		{
		  insert_element_in_neighb (&temp_act);
		  propagation = 0;
		}

	    }



	  if (is_fact_in_preconditions_overall (el, tofix->position)
	      && !is_fact_in_additive_effects_start (el, tofix->position)
	      && inf_action->w_is_used && not_tabu (el))
	    {

	      temp_act.act_pos = el;
	      temp_act.act_level = level;
	      temp_act.constraint_type = C_T_REMOVE_ACTION;

	      if (FALSE && (cost1 = action_eff_cost (inf_action)) <= 0.0
		  && GpG.fast_greedy)
		{
		  if (DEBUG4)
		    printf ("\nCOSTO nullo %f ", cost1);
		  insert_remove_action (temp_act.act_pos, temp_act.act_level,
					temp_act.constraint_type,
					GpG.approximation_level);
		}
	      else
		{
		  insert_element_in_neighb (&temp_act);
		  propagation = 0;
		}

	    }

	  if (level > 0)
	    {
	      inf_action = GET_ACTION_OF_LEVEL (level);
	      el = inf_action->position;


	      if (is_fact_in_preconditions_end (el, tofix->position)
		  &&
		  !(is_fact_in_additive_effects_start (el, tofix->position))
		  && inf_action->w_is_used && not_tabu (el))
		{

		  temp_act.act_pos = el;
		  temp_act.act_level = level;
		  temp_act.constraint_type = C_T_REMOVE_ACTION;

		  if (FALSE && (cost1 = action_eff_cost (inf_action)) <= 0.0
		      && GpG.fast_greedy)
		    {
		      if (DEBUG4)
			printf ("\nCOSTO nullo %f", cost1);
		      insert_remove_action (temp_act.act_pos,
					    temp_act.act_level,
					    temp_act.constraint_type,
					    GpG.approximation_level);
		    }
		  else
		    {
		      insert_element_in_neighb (&temp_act);
		      propagation = 0;
		      skip = 1;
		    }

		}

	    }

	  if (level < GpG.curr_plan_length
	      && vectlevel[level]->noop_act[inform_tofix->position].w_is_goal
	      && propagation == 1)
	    {
	      level++;

	    }
	  else
	    break;


	}





      level = *inform_tofix->level;

      if (GpG.num_solutions && initialize > 0 && level > 0)
	{
	  for (level--; level >= 0; level--)
	    if (GET_ACTION_POSITION_OF_LEVEL (level) >= 0
		&& check_mutex_noop (inform_tofix->position, level) >= 0)
	      break;

	  if (level >= 0)
	    {
	      inf_action = GET_ACTION_OF_LEVEL (level);
	      el = inf_action->position;

	      if (vectlevel[level]->fact[inform_tofix->position].w_is_true
		  && not_tabu (el))
		{
		  temp_act.act_pos = el;
		  temp_act.act_level = level;
		  temp_act.constraint_type = C_T_REMOVE_ACTION;

		  if (FALSE && (cost1 = action_eff_cost (inf_action)) <= 0.0
		      && GpG.fast_greedy)
		    {
		      if (DEBUG4)
			printf ("\nCOSTO nullo %f", cost1);
		      insert_remove_action (temp_act.act_pos,
					    temp_act.act_level,
					    temp_act.constraint_type,
					    GpG.approximation_level);

		    }
		  else
		    {
		      insert_element_in_neighb (&temp_act);
		      propagation = 0;
		      skip = 1;
		    }

		}
	    }
	}







      level = *inform_tofix->level;



      if (initialize > 0)
	{
	  if (num_neighborhood == 0
	      && *inform_tofix->level < GpG.curr_plan_length)
	    {
	      if (inform_tofix->w_is_goal)
		{
		  inform_tofix->w_is_goal = 0;
		  vectlevel[*inform_tofix->level]->
		    prec_vect[GUID_BLOCK (tofix->position)] &=
		    ~(GUID_MASK (tofix->position));
		  vectlevel[*inform_tofix->level]->
		    true_crit_vect[GUID_BLOCK (tofix->position)] &=
		    ~(GUID_MASK (tofix->position));
		  vectlevel[*inform_tofix->level]->
		    false_crit_vect[GUID_BLOCK (tofix->position)] &=
		    ~(GUID_MASK (tofix->position));


		}
	      return num_neighborhood;
	    }

	}
      level = *inform_tofix->level;
      noop_fact = NULL;


      action_level = level + 1;


      for (next_level = level - 1; next_level > 0; next_level--)
	if (!
	    (CHECK_NOOP_POS (tofix->position, next_level)
	     && check_mutex_noop (tofix->position, next_level) < 0))
	  break;


      if (GpG.accurate_cost >= COMPUTE_DG_LIST_COST)
	{
	  get_dg_fact_cost (tofix->position, level, &loc_dg_cost);
	  local_search.best_action = loc_dg_cost->best_act;
	  local_search.best_level = level;
	}
      else
	local_search.best_action = -1;

      for (j = 0; j < gft_conn[tofix->position].num_A; j++)
	{

	  total = 0.0;
	  best_action = -1;
	  best_n_cost.weight = MAXFLOAT;
	  best_n_cost.act_cost = MAXFLOAT;
	  best_n_cost.act_time = MAXFLOAT;

	  for (num_lev_neighb_act = 0, curr_level = level;
	       curr_level > next_level; curr_level--)
	    {
	      if (GET_ACTION_POSITION_OF_LEVEL (curr_level) < 0
		  && curr_level != (level))
		{
		  continue;

		}
	      if (CHECK_ACTION_POS
		  ((cel = gft_conn[tofix->position].A[j]), curr_level)
		  && !is_fact_in_delete_effects (cel, tofix->position))
		{
		  cost =
		    dg_insertion_action_cost (cel, curr_level, action_level,
					      &loc_n_cost);
		  if (best_n_cost.weight >= cost)
		    {

		      if (best_n_cost.weight == cost)
			{
			  loc_weight_cost = weight_cost (&loc_n_cost);

			  if (best_weight_cost == loc_weight_cost)
			    {

			      elem_lev_neighb_act[num_lev_neighb_act] =
				curr_level;
			      if (max_lev_neighb_act > num_lev_neighb_act)
				num_lev_neighb_act++;

			      continue;

			    }

			  if (best_weight_cost > loc_weight_cost)
			    continue;

			}


		      num_lev_neighb_act = 0;
		      elem_lev_neighb_act[num_lev_neighb_act] = curr_level;
		      num_lev_neighb_act++;
		      best_action = cel;
		      best_level = curr_level;
		      best_act_type = IS_ACTION;
		      best_n_cost.weight = loc_n_cost.weight;
		      best_n_cost.act_cost = loc_n_cost.act_cost;
		      best_n_cost.act_time = loc_n_cost.act_time;
		      best_weight_cost = weight_cost (&best_n_cost);

		    }

		}
	      else
		break;

	    }

	  if (best_action >= 0)
	    {
	      temp_act.act_pos = best_action;

	      temp_act.constraint_type = C_T_INSERT_ACTION;
	      if (not_tabu (best_action))
		{
		  for (i = 0; i < num_lev_neighb_act; i++)
		    {
		      temp_act.act_level = elem_lev_neighb_act[i];


		      if (local_search.best_action != best_action
			  || local_search.best_level != temp_act.act_level)
			insert_element_in_neighb (&temp_act);
		    }
		}
	    }
	}




      if (local_search.best_action >= 0
	  && CHECK_ACTION_POS (local_search.best_action,
			       local_search.best_level))
	{
	  temp_act.constraint_type = C_T_INSERT_ACTION;

	  temp_act.act_pos = local_search.best_action;
	  temp_act.act_level = local_search.best_level;
	  insert_element_in_neighb (&temp_act);
	}

    }

  return (num_neighborhood);
}




int
fix (constraints_list treated_c_l)
{
  EfConn *act;
  float best = 0.0;
  inform_list inform_tofix;
  int num, num_min, num_neg, choice, level, max_time = GpG.curr_plan_length;
  Hvar.constr = treated_c_l;
  local_search.best_action = -1;
  inform_tofix =
    CONVERT_NOOP_TO_INFORM (treated_c_l->fact, *treated_c_l->level);
  if (DEBUG2)
    printf ("\n### INC CHOICE:\n  Treated fact: %s, level %d\n",
	    print_noop_name_string (treated_c_l->fact, temp_name),
	    *treated_c_l->level);

  if (inform_tofix->false_position < 0)
    {
      GpG.num_false_act--;

      return 0;

    }
  num = choose_actions_treated_fact (inform_tofix, max_time);
  if (num <= 0)
    {
      if (GpG.local == 1)
	return -1;

      else
	{

	  remove_treated_noop (inform_tofix);

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
    best = find_min (treated_c_l, pos_temp_vect, num, &num_min, &num_neg);

  else
    {
      num_min = 1;
      pos_temp_vect[0] = 0;
    }
  GpG.is_lm = 0;
  if (num == 1)
    {
      choice = 0;

      neighb_vect[choice]->cost.weight = 0.0;
      neighb_vect[choice]->cost.act_cost = 0.0;
      neighb_vect[choice]->cost.act_time = 0.0;

    }
  else if (best > 0)
    {
      GpG.is_lm = 1;

      if ((MY_RANDOM % GpG.denominator) < GpG.numerator)
	{
	  if (DEBUG1)
	    printf ("\n Random choice");
	  choice = MY_RANDOM % num;
	}
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
  act = &gef_conn[neighb_vect[choice]->act_pos];
  level = neighb_vect[choice]->act_level;
  if (DEBUG2)
    {

      printf
	("\n\n=== Action choosen treated fact: %s, num %d, level %d \n     Incons %.3f   Cost %.3f   Time %.3f ",
	 print_op_name_string (act->position, temp_name), GpG.count_num_try,
	 level, neighb_vect[choice]->cost.weight,
	 neighb_vect[choice]->cost.act_cost,
	 neighb_vect[choice]->cost.act_time);
      if (DEBUG6)
	print_actions_in_subgraph ();
      if (DEBUG6)
	{
	  if (inform_tofix->action_fact == IS_FACT)
	    my_print_plan ((*inform_tofix->level) - 1);
	  my_print_plan (*inform_tofix->level);
	}
    }

  if (neighb_vect[choice]->constraint_type == C_T_REMOVE_ACTION)
    choice =
      insert_remove_action (neighb_vect[choice]->act_pos, level,
			    C_T_REMOVE_ACTION, GpG.approximation_level);

  else if (neighb_vect[choice]->constraint_type == C_T_INSERT_ACTION)
    choice =
      insert_remove_action (neighb_vect[choice]->act_pos, level,
			    C_T_INSERT_ACTION, GpG.approximation_level);

  if (DEBUG3)
    {

    }
  return (choice);

}

int
fix_unsup_fact (constraints_list unsup_fact)
{
  int num, num_min, num_neg, choice, level;
  EfConn *act;
  float best = 0.0;
  inform_list inform_tofix;
  Hvar.constr = unsup_fact;
  local_search.best_action = -1;
  inform_tofix =
    CONVERT_FACT_TO_INFORM (unsup_fact->fact, *unsup_fact->level);
  if (DEBUG2)
    {
      printf
	("\n\n### INC CHOICE:\n  Unsupported fact: position %d, level %d fact name : ",
	 unsup_fact->fact, *unsup_fact->level);
      print_ft_name (unsup_fact->fact);
      printf ("\n");
    }

  if (inform_tofix->action_fact == IS_FACT &&
      (inform_tofix->w_is_goal <= 0 || inform_tofix->w_is_true))
    {





      if (DEBUG3)
	printf ("\n Constraint removed: %s, level %d is_goal %d is_true %d",
		print_ft_name_string (unsup_fact->fact, temp_name),
		*inform_tofix->level, inform_tofix->w_is_goal,
		inform_tofix->w_is_true);
      remove_false_fact (inform_tofix);
      return (0);
    }
  num =
    choose_actions (CONVERT_FACT_TO_INFORM
		    (unsup_fact->fact, *unsup_fact->level), TRUE);

  if (num <= 0)

    {
      if (GpG.local == 1)
	return -1;
      else
	{

	  remove_false_fact (inform_tofix);
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
    best = find_min (unsup_fact, pos_temp_vect, num, &num_min, &num_neg);

  else

    {
      num_min = 1;
      pos_temp_vect[0] = 0;
    }
  GpG.is_lm = 0;
  if (num == 1)
    {
      choice = 0;

      neighb_vect[choice]->cost.weight = 0.0;
      neighb_vect[choice]->cost.act_cost = 0.0;
      neighb_vect[choice]->cost.act_time = 0.0;

    }
  else if (best > 0)
    {
      GpG.is_lm = 1;
      if ((MY_RANDOM % GpG.denominator) < GpG.numerator)
	{
	  choice = MY_RANDOM % num;

	  if (DEBUG1)
	    printf ("\n Random choice= %d", choice);
	}
      else if (num_min == 1)
	{
	  choice = pos_temp_vect[0];
	  if (DEBUG5)
	    printf ("\n Num min=1, choice 0");
	}
      else
	{
	  choice = pos_temp_vect[(MY_RANDOM % num_min)];
	  if (DEBUG5)
	    printf ("\nChoice= %d ", choice);

	}
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
	("\n\n=== Action choosen unsup fact: %s, num %d, level %d choice %d \n     Incons %.3f   Cost %.3f   Time %.3f ",
	 print_op_name_string (act->position, temp_name), GpG.count_num_try,
	 level, choice, neighb_vect[choice]->cost.weight,
	 neighb_vect[choice]->cost.act_cost,
	 neighb_vect[choice]->cost.act_time);
      if (DEBUG3)
	print_actions_in_subgraph ();
    }

  choice =
    insert_remove_action (neighb_vect[choice]->act_pos,
			  neighb_vect[choice]->act_level,
			  neighb_vect[choice]->constraint_type,
			  GpG.approximation_level);

  return (choice);
}



int
choose_min_cost_unsup_fact ()
{
  int i, best = 0, min_level = 0;
  float min = 100000.0, min_constr = 100000.0, curr_min_constr =
    100000.0, tot;
  dg_inform_list loc_dg_cost;

  if (GpG.inc_choice_type == MIN_LEVEL_INC
      || GpG.inc_choice_type == MIN_LEVEL_COST_INC
      || GpG.inc_choice_type == MIN_LEVEL_CONSTR_INC)
    min_level = 100000;
  else if (GpG.inc_choice_type == MAX_LEVEL_INC)
    min_level = 0;
  if (GpG.inc_choice_type == MIN_COST_INC
      || GpG.inc_choice_type == MIN_LEVEL_COST_INC)
    min = 100000.0;
  if (GpG.inc_choice_type == MAX_COST_INC)
    min = 0.0;

  best = MY_RANDOM % GpG.num_false_fa;



  for (i = GpG.num_false_fa - 1; i >= 0; i--)
    {


      if (GpG.inc_choice_type == MIN_LEVEL_INC)
	{

	  if (min_level > *unsup_fact[i]->level
	      || (min_level == *unsup_fact[i]->level && MY_RANDOM % 2))
	    {
	      min_level = *unsup_fact[i]->level;
	      best = i;
	    }
	}
      else if (GpG.inc_choice_type == MIN_LEVEL_COST_INC)
	{

	  if (min_level > *unsup_fact[i]->level)
	    {
	      min_level = *unsup_fact[i]->level;
	      best = i;
	      get_dg_fact_cost (unsup_fact[i]->fact,
				*unsup_fact[i]->level - 1, &loc_dg_cost);
	      if (GpG.verify_inc_choice)
		min = loc_dg_cost->num_actions;
	      else
		min = loc_dg_cost->totcost;
	    }
	  else if (min_level == *unsup_fact[i]->level)
	    {

	      get_dg_fact_cost (unsup_fact[i]->fact,
				*unsup_fact[i]->level - 1, &loc_dg_cost);
	      if (GpG.verify_inc_choice)
		tot = loc_dg_cost->num_actions;
	      else
		tot = loc_dg_cost->totcost;

	      if (min > tot || (min == tot && MY_RANDOM & FIRST_1))
		{
		  min = tot;
		  best = i;
		}



	    }
	}
      else if (GpG.inc_choice_type == MIN_LEVEL_CONSTR_INC)
	{

	  if (min_level > *unsup_fact[i]->level)
	    {
	      min_level = *unsup_fact[i]->level;
	      best = i;
	      min_constr =
		compute_constr_fact (i, unsup_fact[i]->fact,
				     *unsup_fact[i]->level - 1, 1);
	      get_dg_fact_cost (unsup_fact[i]->fact,
				*unsup_fact[i]->level - 1, &loc_dg_cost);
	      if (GpG.verify_inc_choice)
		min = loc_dg_cost->num_actions;
	      else
		min = loc_dg_cost->totcost;
	    }
	  else if (min_level == *unsup_fact[i]->level)
	    {

	      curr_min_constr =
		compute_constr_fact (i, unsup_fact[i]->fact,
				     *unsup_fact[i]->level - 1, 1);
	      get_dg_fact_cost (unsup_fact[i]->fact,
				*unsup_fact[i]->level - 1, &loc_dg_cost);
	      if (GpG.verify_inc_choice)
		tot = loc_dg_cost->num_actions;
	      else
		tot = loc_dg_cost->totcost;


	      if (min_constr < curr_min_constr
		  || (min_constr == curr_min_constr
		      && (min > tot || (min == tot && (MY_RANDOM % 2)))))
		{
		  min = tot;
		  best = i;
		  min_constr = curr_min_constr;
		}



	    }
	}
      else if (GpG.inc_choice_type == MIN_LEVEL_MIN_CONSTR_INC)
	{

	  if (min_level > *unsup_fact[i]->level)
	    {
	      min_level = *unsup_fact[i]->level;
	      best = i;
	      min_constr =
		compute_constr_fact (i, unsup_fact[i]->fact,
				     *unsup_fact[i]->level - 1, 1);
	      get_dg_fact_cost (unsup_fact[i]->fact,
				*unsup_fact[i]->level - 1, &loc_dg_cost);
	      if (GpG.verify_inc_choice)
		min = loc_dg_cost->num_actions;
	      else
		min = loc_dg_cost->totcost;

	      min = loc_dg_cost->totcost;
	    }
	  else if (min_level == *unsup_fact[i]->level)
	    {

	      curr_min_constr =
		compute_constr_fact (i, unsup_fact[i]->fact,
				     *unsup_fact[i]->level - 1, 1);
	      get_dg_fact_cost (unsup_fact[i]->fact,
				*unsup_fact[i]->level - 1, &loc_dg_cost);
	      if (GpG.verify_inc_choice)
		tot = loc_dg_cost->num_actions;
	      else
		tot = loc_dg_cost->totcost;


	      if (min_constr > curr_min_constr
		  || (min_constr == curr_min_constr
		      && (min > tot || (min == tot && (MY_RANDOM % 2)))))
		{
		  min = tot;
		  best = i;
		  min_constr = curr_min_constr;
		}



	    }
	}
      else if (GpG.inc_choice_type == MAX_LEVEL_INC)
	{

	  if (min_level < *unsup_fact[i]->level)
	    {
	      min_level = *unsup_fact[i]->level;
	      best = i;
	    }
	}
      else if (GpG.inc_choice_type == MIN_COST_INC)
	{

	  get_dg_fact_cost (unsup_fact[i]->fact, *unsup_fact[i]->level - 1,
			    &loc_dg_cost);

	  if (GpG.verify_inc_choice)
	    tot = loc_dg_cost->num_actions;
	  else
	    tot = loc_dg_cost->totcost;


	  if (min > tot || (min == tot && MY_RANDOM % 2))
	    {
	      min = tot;
	      best = i;
	    }
	}
      else if (GpG.inc_choice_type == MAX_COST_INC)
	{

	  get_dg_fact_cost (unsup_fact[i]->fact, *unsup_fact[i]->level - 1,
			    &loc_dg_cost);
	  if (GpG.verify_inc_choice)
	    tot = loc_dg_cost->num_actions;
	  else
	    tot = loc_dg_cost->totcost;

	  if (min < tot || (MY_RANDOM % 2))
	    {
	      min = tot;
	      best = i;
	    }
	}


    }

  return best;
}






int
choose_act_fa (int min_time)
{
  int tot;
  int unsup_pos;
  int fix_num;
  float k_weight_num_bool = 1;

  if (GpG.num_false_fa <= 0 && GpG.num_false_act <= 0
      && GpG.num_false_num_fa <= 0)
    {
      if (GpG.optimize == 0 || GpG.num_solutions == 0)
	return 0;

      else
	{

	  choose_random_act ();
	}

      if (GpG.num_false_fa <= 0 && GpG.num_false_act <= 0
	  && GpG.num_false_num_fa <= 0)
	{
	  return 0;
	}

    }


  if (DEBUG3)
    {
      print_unsup_fact_vect ();
      print_unsup_num_facts ();
    }
  if (DEBUG6 && GpG.accurate_cost >= COMPUTE_DG_SUM_COST)
    print_cost_of_unsupported_facts ();






  if (GpG.num_false_fa <= 0 && GpG.num_false_num_fa <= 0)
    return fix (treated_c_l[MY_RANDOM % GpG.num_false_act]);

  else if (GpG.num_false_act <= 0)
    {

      fix_num =
	(MY_RANDOM %
	 (int) (GpG.num_false_fa +
		k_weight_num_bool * GpG.num_false_num_fa)) >
	(GpG.num_false_fa);

      if (GpG.num_false_num_fa == 0)
	fix_num = FALSE;
      if (GpG.num_false_fa == 0)
	fix_num = TRUE;

      if (fix_num)
	{
	  assert (GpG.num_false_num_fa > 0);
	  unsup_pos = MY_RANDOM % GpG.num_false_num_fa;
	  if (GpG.inc_choice_type != RANDOM_INC)
	    unsup_pos = choose_min_cost_unsup_num_fact ();
	  return fix_unsup_num_fact (unsup_num_fact[unsup_pos]);
	}

      else
	{
	  assert (GpG.num_false_fa > 0);
	  unsup_pos = MY_RANDOM % GpG.num_false_fa;
	  if (GpG.inc_choice_type != RANDOM_INC)
	    unsup_pos = choose_min_cost_unsup_fact ();
	  return fix_unsup_fact (unsup_fact[unsup_pos]);
	}

    }







  GpG.num_m_e = GpG.num_false_act;
  GpG.num_prec = GpG.num_false_fa;



  tot = (int) ceil (GpG.weight_fact * GpG.num_prec + GpG.num_m_e);



  if ((MY_RANDOM % tot) < GpG.num_m_e)
    {
      unsup_pos = MY_RANDOM % GpG.num_false_act;
      return fix (treated_c_l[unsup_pos]);
    }

  else
    {

      fix_num =
	(MY_RANDOM %
	 (int) (GpG.num_false_fa +
		k_weight_num_bool * GpG.num_false_num_fa)) >
	(GpG.num_false_fa);

      if (GpG.num_false_num_fa == 0)
	fix_num = FALSE;
      if (GpG.num_false_fa == 0)
	fix_num = TRUE;

      if (fix_num)
	{
	  assert (GpG.num_false_num_fa > 0);
	  unsup_pos = MY_RANDOM % GpG.num_false_num_fa;
	  if (GpG.inc_choice_type != RANDOM_INC)
	    unsup_pos = choose_min_cost_unsup_num_fact ();
	  return fix_unsup_num_fact (unsup_num_fact[unsup_pos]);
	}

      else
	{
	  assert (GpG.num_false_fa > 0);
	  unsup_pos = MY_RANDOM % GpG.num_false_fa;
	  if (GpG.inc_choice_type != RANDOM_INC)
	    unsup_pos = choose_min_cost_unsup_fact ();
	  return fix_unsup_fact (unsup_fact[unsup_pos]);
	}

    }

}







int
LocalSearch (State * start_state, State * end_state,
	     PlanAction ** plan_actions)
{
  int nulflip = 0, result =
    0, loc_init_time, tot_numtry, num_try_print, num_run;

  struct tms search_start, search_end, cputime;
  float plan_time, time_elapsed;
  int optimize = 0;

  GpG.count_num_try = 0;
  GpG.curr_goal_state = end_state;


  srandom (seed);
  gnoop_conn = gft_conn;
  times (&search_start);
  loc_init_time = GpG.curr_plan_length = GpG.max_plan_length - 1;


  do
    {
      tot_numtry = GpG.numtry;
      if (GpG.numrun <= 0 || GpG.numtry <= 0)
	return (-1);
      if (DEBUG0 && !DEBUG1)
	printf ("\n\nSearching ('.' = every 50 search steps):\n");
      for (num_run = 0; num_run < GpG.numrun; num_run++)
	{


	  if (FALSE && num_run == 1)
	    {
	      printf ("\nSTART1");
	      GpG.info_search = 5;

	    }

	  tot_numtry *= GpG.inc_run;
	  GpG.curr_plan_length = loc_init_time;

	  if (DEBUG0 && !DEBUG1 && num_run > 0)
	    printf (" Restart ");
	  initialize (start_state, end_state, num_run);
	  if (DEBUG6)
	    my_print_plan_all (GpG.curr_plan_length);
	  if (DEBUG1)
	    printf ("\n\n\n\n-----SEARCH START----- ");
	  fflush (stdout);

	  GpG.num_false_tot =
	    GpG.num_false_act + GpG.num_false_fa + GpG.num_false_num_fa;

	  if (optimize)
	    set_heuristic_parameters (num_run, GpG.num_solutions);

	  for (num_try = 1, num_try_print = 1; num_try < tot_numtry;
	       num_try++, num_try_print++, GpG.count_num_try++)
	    {


	      if (num_try_print >= 50)
		{
		  if (GpG.max_cputime > 0.0)
		    {
		      times (&cputime);

		      plan_time = DeltaTime (glob_start_time, cputime);

		      if (plan_time > GpG.max_cputime)
			{
			  printf ("\n\nMax cpu time exceeded\n\n");
			  exit (0);
			}

		      time_elapsed =
			(float) ((cputime.tms_utime - start_time.tms_utime +
				  cputime.tms_stime -
				  start_time.tms_stime) / 100.0);

		      if (time_elapsed > GpG.timeout
			  && GpG.found_plan != FALSE)
			{
			  printf ("TIMEOUT\n");
			  GpG.found_plan = TIMEOUT;
			  return (num_try + num_run * GpG.numtry);
			}

		    }

		  if (GpG.num_actions > 20
		      && GpG.curr_plan_length - GpG.num_actions * 3 >
		      GpG.curr_plan_length)
		    compress_vectlevel ();

		  if (DEBUG0 && !DEBUG1)
		    printf (".");
		  num_try_print = 1;
		}

	      set_param (GpG.num_false_tot);


	      if (DEBUG2)
		printf ("\n\n\n\n\n@@@@@ Search Step: %d", num_try);
	      {
		result = choose_act_fa (GpG.curr_plan_length);

		if (result == FALSE)
		  ++nulflip;
	      }

	      GpG.num_false_tot =
		GpG.num_false_act + GpG.num_false_fa + GpG.num_false_num_fa;

	      if (GpG.num_false_tot == 0)
		tot_numtry = 500;


	      if (GpG.num_false_tot == 0)
		{
		  check_num_prec ();

		  GpG.num_false_tot =
		    GpG.num_false_act + GpG.num_false_fa +
		    GpG.num_false_num_fa;

		}
	      if (GpG.num_false_tot == 0)
		optimize = continue_to_optimize ();

	      if (GpG.num_false_tot == 0
		  && (optimize == FALSE
		      || (num_try > 100
			  && GpG.initialize_from != INIT_EMPTY_PLAN)))
		break;
	      if (GpG.curr_plan_length >= MAX_PLAN_LENGTH)
		break;



	      if (GpG.lm_multilevel)
		{

		  update_precond_multilevel ();
		  update_decr_me_prec_multilevel ();

		}
	      else
		{
		  update_precond ();
		  update_decr_me_prec ();

		}

	    }

	  if (!GpG.temporal_plan && GpG.num_false_tot == 0)
	    compress_plan ();

	  if (GpG.num_false_act || GpG.num_false_fa || GpG.num_false_num_fa
	      || optimize)
	    {
	      if (DEBUG0 && !DEBUG1)
		{
		  if (optimize)
		    printf (" found solution of bad quality.");

		  else
		    printf (" search limit exceeded.");
		}
	    }

	  else
	    {
	      times (&search_end);
	      GpG.do_best_first = FALSE;
	      if (GpG.inc_choice_command_line)
		GpG.inc_choice_type = GpG.inc_choice_command_line;

	      plan_time = DeltaTime (search_start, search_end);

	      times (&glob_end_time);
	      gtotal_time =
		(float) ((glob_end_time.tms_utime -
			  glob_start_time.tms_utime +
			  glob_end_time.tms_stime -
			  glob_start_time.tms_stime) / 100.0);

	      if (DEBUG0 && !DEBUG1)
		printf (" solution found: ");
	      if (DEBUG5)
		{
		  print_num_levels_and_actions ();
		  if (GpG.temporal_plan)
		    print_temporal_plan (GpG.curr_plan_length);
		}
	      if (!GpG.noout)
		store_adapted_temporal_plan (GpG.curr_plan_length,
					     gcmd_line.fct_file_name,
					     plan_time);
	      store_curr_plan (GpG.curr_plan_length, plan_actions);
	      if (DEBUG0)
		if (GpG.num_solutions >= GpG.max_num_solutions || GpG.noout)
		  print_actions_in_plan ();
	      if (GpG.maximize_plan && GpG.total_cost < 0)
		printf
		  ("\nSolution number: %d\nTotal time:      %.2f\nSearch time:     %.2f\nActions:         %d\nExecution cost:  %.2f\nDuration:        %.3f\nPlan quality:    %.3f",
		   GpG.num_solutions, gtotal_time, plan_time, GpG.num_actions,
		   GpG.total_cost * (-1), GpG.total_time,
		   GpG.total_cost * GpG.orig_weight_cost * (-1) +
		   GpG.total_time * GpG.orig_weight_time);
	      else
		printf
		  ("\nSolution number: %d\nTotal time:      %.2f\nSearch time:     %.2f\nActions:         %d\nExecution cost:  %.2f\nDuration:        %.3f\nPlan quality:    %.3f",
		   GpG.num_solutions, gtotal_time, plan_time, GpG.num_actions,
		   GpG.total_cost, GpG.total_time,
		   GpG.total_cost * GpG.orig_weight_cost +
		   GpG.total_time * GpG.orig_weight_time);

	      if (!GpG.noout)
		{
		  printf ("\n     Plan file:");
		  if (GpG.out_file_name)
		    printf ("       %s_%d.SOL", gcmd_line.out_file_name,
			    GpG.num_solutions);

		  else
		    printf ("       plan_%s_%d.SOL", gcmd_line.fct_file_name,
			    GpG.num_solutions);
		}

	      fflush (stdout);


	      if (GpG.max_num_solutions > 0
		  && GpG.num_solutions >= GpG.max_num_solutions)
		return (0);

	      break;
	    }
	}
    }
  while (GpG.optimize && GpG.num_solutions > 0);

  return (num_try + num_run * GpG.numtry);
}
