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
 * File: ActionSubgraph.c
 * Description: Action subgraph management 
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/






#include <math.h>
#include "lpg.h"
#include "inst_utils.h"
#include "LocalSearch.h"
#include "ActionSubgraph.h"
#include "ComputeReachInf.h"
#include "LpgOutput.h"
#include "output.h"
#include "utilities.h"
#include "numeric.h"
#include "H_relaxed.h"
#include "check.h"
#include "LpgTime.h"


static void
allocate_data_level (register int level)
{
  unsigned int i;
  inform supp, *next;


  memset (&supp, 0, sizeof (supp));
  supp.false_position = -1;
  supp.action_fact = IS_FACT;
  supp.level = &vectlevel[level]->level;

  next = vectlevel[level]->fact;
  for (i = 0; i <= GpG.max_num_facts; i++)
    {
      supp.position = i;
      memcpy (next, &supp, sizeof (inform));
      next++;
    }

  supp.action_fact = IS_NOOP;
  supp.preco = NULL;
  supp.add = NULL;

  next = vectlevel[level]->noop_act;
  for (i = 0; i <= GpG.max_num_facts; i++)
    {
      supp.position = i;
      memcpy (next, &supp, sizeof (inform));
      next++;
    }

  supp.action_fact = IS_ACTION;
  supp.preco = NULL;
  supp.add = NULL;

  next = &vectlevel[level]->action;

  supp.position = -1;
  memcpy (next, &supp, sizeof (inform));

}



def_level_list
allocate_level (void)
{
  def_level_list vect;
  int aux;
  vect = (def_level_list) malloc (sizeof (struct LEVEL));
  memset (vect, 0, sizeof (struct LEVEL));

  if (GpG.max_num_facts != 0)
    {
      vect->fact = (inform *) calloc (GpG.max_num_facts + 1, sizeof (inform));
      if (vect->fact == NULL)
	{
	  MSG_ERROR (WAR_NO_MEMORY);
	  exit (1);
	}
    }

  else
    {
      MSG_ERROR (" No facts \r\n");
    }

  if (GpG.max_num_facts != 0)
    {
      vect->noop_act =
	(inform *) calloc (GpG.max_num_facts + 1, sizeof (inform));
      if (vect->noop_act == NULL)
	{
	  MSG_ERROR (WAR_NO_MEMORY);
	  exit (1);
	}
    }

  else
    {
      MSG_ERROR (" No noops \r\n");
    }



  if (GpG.lm_multilevel)
    {
      vect->lambda_prec = (float *) calloc (gnum_ef_conn, sizeof (float));
      vect->lambda_me = (float *) calloc (gnum_ef_conn, sizeof (float));
    }
  vect->prec_vect = (int *) calloc (gnum_ft_block, sizeof (int));
  vect->fact_vect = (int *) calloc (gnum_ft_block, sizeof (int));
  vect->true_crit_vect = (int *) calloc (gnum_ft_block, sizeof (int));
  vect->false_crit_vect = (int *) calloc (gnum_ft_block, sizeof (int));
  vect->noop_act_vect = (int *) calloc (gnum_ft_block, sizeof (int));
  vect->noop_prec_act_vect = (int *) calloc (gnum_ft_block, sizeof (int));

  vect->numeric = (NumInfo *) calloc (1, sizeof (NumInfo));

  vect->numeric->values = (float *) calloc (gnum_comp_var, sizeof (float));
  vect->numeric->values_after_start =
    (float *) calloc (gnum_comp_var, sizeof (float));
  vect->numeric->modified_vars_start =
    (int *) calloc (gnum_block_compvar, sizeof (int));

  vect->numeric->modified_vars_end =
    (int *) calloc (gnum_block_compvar, sizeof (int));
  vect->numeric->used_vars =
    (int *) calloc (gnum_block_compvar, sizeof (int));
  vect->numeric->w_is_goal = (short *) calloc (gnum_comp_var, sizeof (short));
  vect->numeric->w_is_used = (short *) calloc (gnum_comp_var, sizeof (short));
  vect->numeric->false_position =
    (int *) calloc (gnum_comp_var, sizeof (int));

  if (GpG.lm_multilevel)
    for (aux = 0; aux < gnum_ef_conn; aux++)
      {
	vect->lambda_prec[aux] = 1.0;
	vect->lambda_me[aux] = 1.0;
      }


  memset (vect->numeric->values, 0, gnum_comp_var * sizeof (float));
  memset (vect->numeric->values_after_start, 0,
	  gnum_comp_var * sizeof (float));
  memset (vect->numeric->modified_vars_start, 0,
	  gnum_block_compvar * sizeof (int));
  memset (vect->numeric->modified_vars_end, 0,
	  gnum_block_compvar * sizeof (int));
  memset (vect->numeric->used_vars, 0, gnum_block_compvar * sizeof (int));
  memset (vect->numeric->w_is_goal, 0, gnum_comp_var * sizeof (short));
  memset (vect->numeric->w_is_used, 0, gnum_comp_var * sizeof (short));
  memset (vect->numeric->false_position, -1, gnum_comp_var * sizeof (int));


  if (GpG.accurate_cost >= 1)
    {
      vect->dg_facts_array =
	(dg_inform_list *) calloc (gnum_ft_conn, sizeof (dg_inform_list));
      memset (vect->dg_facts_array, 0,
	      gnum_ft_conn * sizeof (dg_inform_list));
      vect->local_bit_vect_facts =
	(int *) calloc (gnum_ft_block, sizeof (int));
      memset (vect->local_bit_vect_facts, 0, gnum_ft_block * sizeof (int));
    }

  return (vect);
}




def_level_list
create_vectlevel (int fixpoint)
{
  static int time = 0;
  def_level_list vect;

  GpG.max_plan_length++;

  if (GpG.max_plan_length >= MAX_PLAN_LENGTH)
    {
      exit (1);
    }


  vect = allocate_level ();

  vectlevel[time] = vect;
  vectlevel[time]->level = time;

  allocate_data_level (time);

  time++;

  if (time >= MAX_PLAN_LENGTH + 1)
    {
      exit (1);
    }


  if (time == 0)
    {
      tot_alloc_mem_size = 0;
    }

  tot_alloc_mem_size += sizeof (struct LEVEL);
  tot_alloc_mem_size += (1 + 2 * GpG.max_num_facts) * sizeof (inform);
  tot_alloc_mem_size += (sizeof (int) * (gnum_ft_block + 1)) * 14;
  if (GpG.lm_multilevel)
    tot_alloc_mem_size += (gnum_ef_conn * sizeof (float)) * 2;

  return vect;
}








void
reset_level (int time)
{
  inform_list infp;
  int i;


  if (vectlevel[time] == NULL)
    return;


  memset (vectlevel[time]->numeric->false_position, -1,
	  gnum_comp_var * sizeof (int));
  memset (vectlevel[time]->numeric->w_is_goal, 0,
	  gnum_comp_var * sizeof (short));

  if (time != 0)
    copy_num_values_from_level_to_level (0, time, FALSE);
  vectlevel[time]->level = time;
  vectlevel[time]->num_actions = 0;
  vectlevel[time]->modified = 0;

  reset_bitarray (vectlevel[time]->prec_vect, gnum_ft_block);
  vectlevel[time]->num_prec = 0;
  reset_bitarray (vectlevel[time]->fact_vect, gnum_ft_block);

  vectlevel[time]->num_fact = 0;
  reset_bitarray (vectlevel[time]->true_crit_vect, gnum_ft_block);
  vectlevel[time]->num_true_crit = 0;
  reset_bitarray (vectlevel[time]->false_crit_vect, gnum_ft_block);
  vectlevel[time]->num_false_crit = 0;

  reset_bitarray (vectlevel[time]->noop_act_vect, gnum_ft_block);
  reset_bitarray (vectlevel[time]->noop_prec_act_vect, gnum_ft_block);

  for (i = 0; i < GpG.max_num_facts; i++)
    {
      infp = &vectlevel[time]->fact[i];
      infp->w_is_goal = 0;
      infp->w_is_used = 0;
      infp->w_is_true = 0;
      infp->false_position = -1;
      CONVERT_FACT_TO_VERTEX (i)->lamda_prec =
	CONVERT_FACT_TO_VERTEX (i)->lamda_me = 1.0;
      CONVERT_FACT_TO_VERTEX (i)->last_lm_me =
	CONVERT_FACT_TO_VERTEX (i)->last_lm_prec = 0;

      infp->time_f = -2.0;
      infp->action_f = NULL;
      infp->ord_pos = 0;


      infp = &vectlevel[time]->noop_act[i];
      infp->w_is_goal = 0;
      infp->w_is_used = 0;
      infp->w_is_true = 0;
      infp->w_is_overall = 0;
      infp->false_position = -1;

      infp->time_f = -2.0;
      infp->action_f = NULL;
      infp->ord_pos = 0;
    }

  infp = &vectlevel[time]->action;
  infp->step = -2000;

  infp->w_is_goal = 0;
  infp->w_is_used = 0;
  infp->w_is_true = 0;
  infp->false_position = -1;

  infp->position = -1;
  infp->being_removed = 0;
  free_noop_not_in (infp);
  vectlevel[time]->max_action_time = 0.0;

  infp->time_f = 0.0;
  infp->action_f = NULL;
  infp->ord_pos = 0;

  if ((GpG.accurate_cost == COMPUTE_MAX_COST
       && GpG.inc_choice_type >= MIN_COST_INC)
      || GpG.accurate_cost >= COMPUTE_DG_SUM_COST)
    reset_computed_dg_costs (time);
}



void
compress_vectlevel ()
{
  int i, j, level, last_used, last_free = -1;
  int start_free = 0;

  printf ("\n Start compress: GpG.curr_plan_length %d  Action %d",
	  GpG.curr_plan_length, GpG.num_actions);
  print_actions_in_subgraph ();
  check_plan (GpG.curr_plan_length);
  printf ("\n -> Compress...\n");

  last_used = GpG.fixpoint_plan_length - 1;

  for (level = GpG.fixpoint_plan_length; level < GpG.curr_plan_length;
       level++)
    {

      if (GET_ACTION_POSITION_OF_LEVEL (level) >= 0)
	{



	  if (last_used == GpG.fixpoint_plan_length - 1)
	    {


	      if (level > ++last_used)
		{

		  vectlevel[last_used] = vectlevel[level];
		  vectlevel[last_used]->level = last_used;

		  if (GpG.max_temp_vect > 0)
		    {
		      for (i = 0; i < GpG.max_num_facts; i++)
			{
			  if ((vectlevel[last_used - 1]->noop_act[i].
			       w_is_goal == 1)
			      && (temp_vectlevel[GpG.max_temp_vect - 1]->
				  noop_act[i].w_is_goal == 2))
			    vectlevel[last_used - 1]->noop_act[i].w_is_goal =
			      2;

			  if ((vectlevel[last_used]->fact[i].w_is_true == 1)
			      && (temp_vectlevel[start_free]->fact[i].
				  w_is_true == 2))
			    vectlevel[last_used]->fact[i].w_is_true = 2;

			  if (GpG.temporal_plan)
			    if (vectlevel[last_used]->fact[i].w_is_true > 0)
			      vectlevel[last_used]->fact[i].action_f =
				temp_vectlevel[start_free]->fact[i].action_f;

			}
		    }
		  else
		    {
		      MSG_ERROR (" Error ");
		      printf ("\n level %d  last_used %d  fixpoint %d", level,
			      last_used, GpG.fixpoint_plan_length);
		    }

		}

	    }

	  else
	    {


	      if (last_free < level - 1)
		{

		  vectlevel[last_used + 1] = vectlevel[level];
		  vectlevel[last_used + 1]->level = last_used + 1;

		}

	      else
		{

		  vectlevel[last_used + 1] = vectlevel[level];
		  vectlevel[last_used + 1]->level = last_used + 1;



		  for (i = 0; i < GpG.max_num_facts; i++)
		    {

		      if ((vectlevel[last_used + 1]->fact[i].w_is_true == 1)
			  && (temp_vectlevel[start_free]->fact[i].w_is_true ==
			      2))
			vectlevel[last_used + 1]->fact[i].w_is_true = 2;

		      if (GpG.temporal_plan)
			if (vectlevel[last_used + 1]->fact[i].w_is_true > 0)
			  vectlevel[last_used + 1]->fact[i].action_f =
			    temp_vectlevel[start_free]->fact[i].action_f;

		      if ((vectlevel[last_used]->noop_act[i].w_is_goal == 1)
			  && (temp_vectlevel[GpG.max_temp_vect - 1]->
			      noop_act[i].w_is_goal == 2))
			vectlevel[last_used]->noop_act[i].w_is_goal = 2;

		    }

		  memcpy (vectlevel[last_used + 1]->true_crit_vect,
			  temp_vectlevel[start_free]->true_crit_vect,
			  sizeof (int) * gnum_ft_block);





		}

	      last_used++;
	    }

	}
      else
	{


	  if (last_free < level - 1)
	    start_free = GpG.max_temp_vect;

	  temp_vectlevel[GpG.max_temp_vect++] = vectlevel[level];

	  last_free = level;

	}

    }



  vectlevel[last_used + 1] = vectlevel[level];
  vectlevel[last_used + 1]->level = last_used + 1;


  if (last_free == level - 1)
    {

      for (i = 0; i < GpG.max_num_facts; i++)
	{

	  if ((vectlevel[last_used + 1]->fact[i].w_is_true == 1)
	      && (temp_vectlevel[start_free]->fact[i].w_is_true == 2))
	    vectlevel[last_used + 1]->fact[i].w_is_true = 2;

	  if (GpG.temporal_plan)
	    if (vectlevel[last_used + 1]->fact[i].w_is_true > 0)
	      vectlevel[last_used + 1]->fact[i].action_f =
		temp_vectlevel[start_free]->fact[i].action_f;

	}

      memcpy (vectlevel[last_used + 1]->true_crit_vect,
	      temp_vectlevel[start_free]->true_crit_vect,
	      sizeof (int) * gnum_ft_block);

    }



  for (i = GpG.curr_plan_length + 1; i < GpG.max_plan_length; i++)
    temp_vectlevel[GpG.max_temp_vect++] = vectlevel[i];


  GpG.curr_plan_length = last_used + 1;
  GpG.max_plan_length = GpG.curr_plan_length + 1;

  for (i = 0; i < GpG.max_plan_length; i++)
    for (j = i; j < GpG.max_plan_length; j++)
      if (vectlevel[i] == vectlevel[j] && i != j)
	printf ("\n\n XXXX vectlevel %d  temp_vect %d  plan %d  max %d", i, j,
		GpG.curr_plan_length, GpG.max_plan_length);


}



void
up_vectlevel (int level)
{
  int i, k, j, noop_pos, l1;
  def_level_list temp_level, temp_level_2, max_temp_level;

  if (GpG.curr_plan_length + 1 >= MAX_PLAN_LENGTH)
    compress_vectlevel ();


  if (GpG.curr_plan_length >= (GpG.max_plan_length - 1))
    {
      if (GpG.max_temp_vect > 0)
	{
	  GpG.curr_plan_length++;
	  vectlevel[GpG.curr_plan_length] =
	    temp_vectlevel[GpG.max_temp_vect - 1];
	  reset_level (GpG.curr_plan_length);
	  temp_vectlevel[GpG.max_temp_vect - 1] = NULL;
	  GpG.curr_plan_length--;
	  GpG.max_temp_vect--;
	  GpG.max_plan_length++;
	}
      else
	{
	  create_vectlevel (1);
	}

    }
  GpG.curr_plan_length++;


  if (DEBUG2)
    printf ("\n    insert action => increase level %d ", level);

  max_temp_level = vectlevel[GpG.curr_plan_length];

  temp_level = vectlevel[level];
  for (i = level; i < GpG.curr_plan_length; i++)
    {
      temp_level_2 = vectlevel[i + 1];
      vectlevel[i + 1] = temp_level;
      temp_level->level += 1;
      temp_level = temp_level_2;
    }

  vectlevel[level] = max_temp_level;
  vectlevel[level]->level = level;
  vectlevel[level]->modified = 0;




  l1 = level + 1;

  memcpy (vectlevel[level]->prec_vect, vectlevel[l1]->prec_vect,
	  sizeof (int) * gnum_ft_block);
  memcpy (vectlevel[level]->fact_vect, vectlevel[l1]->fact_vect,
	  sizeof (int) * gnum_ft_block);
  memcpy (vectlevel[level]->true_crit_vect, vectlevel[l1]->true_crit_vect,
	  sizeof (int) * gnum_ft_block);
  memcpy (vectlevel[level]->false_crit_vect, vectlevel[l1]->false_crit_vect,
	  sizeof (int) * gnum_ft_block);
  memset (vectlevel[level]->noop_prec_act_vect, 0,
	  sizeof (int) * gnum_ft_block);
  memset (vectlevel[level]->noop_act_vect, 0, sizeof (int) * gnum_ft_block);

  for (k = 0; k < gnum_ft_block; k++)
    {
      vectlevel[l1]->true_crit_vect[k] =
	vectlevel[l1]->prec_vect[k] & vectlevel[l1]->fact_vect[k];
      vectlevel[l1]->false_crit_vect[k] =
	vectlevel[l1]->prec_vect[k] & (~vectlevel[l1]->fact_vect[k]);
    }

  vectlevel[level]->action.step = 0;
  vectlevel[level]->action.w_is_used = 0;
  vectlevel[level]->action.w_is_goal = 0;
  vectlevel[level]->action.false_position = -1;


  memcpy (vectlevel[level]->numeric->values, vectlevel[l1]->numeric->values,
	  sizeof (float) * gnum_comp_var);
  memcpy (vectlevel[level]->numeric->values_after_start,
	  vectlevel[l1]->numeric->values_after_start,
	  sizeof (int) * gnum_comp_var);
  memcpy (vectlevel[level]->numeric->w_is_goal,
	  vectlevel[l1]->numeric->w_is_goal, sizeof (short) * gnum_comp_var);
  memcpy (vectlevel[level]->numeric->w_is_used,
	  vectlevel[l1]->numeric->w_is_used, sizeof (short) * gnum_comp_var);
  memset (vectlevel[level]->numeric->false_position, -1,
	  sizeof (int) * gnum_comp_var);
  memset (vectlevel[level]->numeric->w_is_goal, 0,
	  sizeof (short) * gnum_comp_var);
  memcpy (vectlevel[level]->numeric->modified_vars_start,
	  vectlevel[l1]->numeric->modified_vars_start,
	  sizeof (int) * gnum_block_compvar);
  memcpy (vectlevel[level]->numeric->modified_vars_end,
	  vectlevel[l1]->numeric->modified_vars_end,
	  sizeof (int) * gnum_block_compvar);
  memcpy (vectlevel[level]->numeric->used_vars,
	  vectlevel[l1]->numeric->used_vars,
	  sizeof (int) * gnum_block_compvar);

  for (i = 0; i < GpG.max_num_facts; i++)
    {

      if (vectlevel[l1]->fact[i].w_is_true >= 1)
	vectlevel[level]->fact[i].time_f = vectlevel[l1]->fact[i].time_f;
      else
	vectlevel[level]->fact[i].time_f = -2.0;


      if (vectlevel[l1]->fact[i].w_is_goal && CHECK_NOOP_POS (i, level))
	{
	  noop_pos = i;

	  vectlevel[level]->noop_act[noop_pos].w_is_goal = TRUE;
	  vectlevel[level]->noop_prec_act_vect[GUID_BLOCK (noop_pos)] |=
	    GUID_MASK (noop_pos);
	  vectlevel[level]->fact[i].w_is_goal = TRUE;
	}
      else
	vectlevel[level]->fact[i].w_is_goal = FALSE;

      if (vectlevel[l1]->fact[i].w_is_true)
	{
	  noop_pos = i;

	  vectlevel[level]->noop_act[noop_pos].w_is_used = TRUE;

	  if (GpG.temporal_plan)
	    {
	      vectlevel[level]->noop_act[noop_pos].time_f =
		vectlevel[l1]->fact[i].time_f;


	      if (level > 0)
		vectlevel[level]->fact[i].action_f =
		  vectlevel[l1]->fact[i].action_f;
	      vectlevel[l1]->fact[i].action_f =
		&vectlevel[level]->noop_act[noop_pos];
	    }

	  vectlevel[level]->noop_act_vect[GUID_BLOCK (noop_pos)] |=
	    GUID_MASK (noop_pos);
	  vectlevel[level]->fact[i].w_is_true =
	    vectlevel[l1]->fact[i].w_is_true;
	  vectlevel[l1]->fact[i].w_is_true = TRUE;
	}

      else
	{
	  vectlevel[level]->fact[i].w_is_true = FALSE;
	  vectlevel[level]->noop_act[i].time_f = -2.0;
	}
      vectlevel[level]->fact[i].w_is_used = FALSE;
      vectlevel[level]->fact[i].false_position = -1;
    }



  vectlevel[level]->num_prec = vectlevel[l1]->num_prec;
  vectlevel[level]->num_fact = vectlevel[l1]->num_fact;
  vectlevel[level]->num_true_crit = vectlevel[l1]->num_true_crit;
  vectlevel[level]->num_false_crit = vectlevel[l1]->num_false_crit;


  for (i = level; i < GpG.curr_plan_length; i++)
    {

      for (j = 0; j < GpG.max_num_facts; j++)
	if (gft_conn[j].level == i && vectlevel[i + 1]->fact[j].w_is_goal)
	  backward_precond_propagation (&vectlevel[i + 1]->fact[j]);
    }




}












void
insert_prec_act (inform_list infAction)
{
  inform_list infEl, infNoop = NULL;
  int el;
  EfConn *act;
  int level, j;



  if (infAction->w_is_used <= 0 || infAction->w_is_goal > 0)
    return;
  level = *infAction->level;
  act = CONVERT_ACTION_TO_VERTEX (infAction->position);

  infAction->w_is_goal++;



  for (j = 0; j < gef_conn[act->position].num_PC; j++)
    {
      el = gef_conn[act->position].PC[j];
      if (el < 0)
	continue;

      if (CHECK_FACT_POS (el, level))
	{

	  infEl = CONVERT_FACT_TO_INFORM (el, level);

	  if (infEl->w_is_goal <= 0)
	    {
	      vectlevel[level]->prec_vect[GUID_BLOCK (el)] |= GUID_MASK (el);

	      if (!infEl->w_is_true)
		vectlevel[level]->false_crit_vect[GUID_BLOCK (el)] |=
		  GUID_MASK (el);
	      else if (infEl->w_is_true == 1)
		vectlevel[level]->true_crit_vect[GUID_BLOCK (el)] |=
		  GUID_MASK (el);

	    }
	  infEl->w_is_goal++;
	  backward_precond_propagation (infEl);


	}
    }

  if (gef_conn[act->position].sf != NULL)
    {
      for (j = 0; j < gef_conn[act->position].sf->num_PC_overall; j++)
	{
	  el = gef_conn[act->position].sf->PC_overall[j];

	  if (el < 0)
	    continue;

	  if (CHECK_FACT_POS (el, level))
	    {

	      infEl = CONVERT_FACT_TO_INFORM (el, level);

	      infNoop = CONVERT_NOOP_TO_INFORM (el, level);

	      if (infNoop->w_is_overall != ADD_DEL
		  && infNoop->w_is_overall != ADD_NDEL)
		{
		  if (infEl->w_is_goal <= 0)
		    {
		      vectlevel[level]->prec_vect[GUID_BLOCK (el)] |=
			GUID_MASK (el);

		      if (!infNoop->w_is_used)
			vectlevel[level]->false_crit_vect[GUID_BLOCK (el)] |=
			  GUID_MASK (el);
		      else if (infEl->w_is_true == 1)
			vectlevel[level]->true_crit_vect[GUID_BLOCK (el)] |=
			  GUID_MASK (el);

		    }

		  infEl->w_is_goal++;
		  backward_precond_propagation (infEl);

		}
	    }

	}


      for (j = 0; j < gef_conn[act->position].sf->num_PC_end; j++)
	{
	  el = gef_conn[act->position].sf->PC_end[j];
	  if (el < 0)
	    continue;



	  if (CHECK_FACT_POS (el, level))
	    {




	      infEl = CONVERT_FACT_TO_INFORM (el, level);

	      if (infNoop->w_is_overall != ADD_DEL
		  && infNoop->w_is_overall != ADD_NDEL)


		{
		  if (infEl->w_is_goal <= 0)
		    {


		      vectlevel[level]->prec_vect[GUID_BLOCK (el)] |=
			GUID_MASK (el);

		      if (!infEl->w_is_true)


			vectlevel[level]->false_crit_vect[GUID_BLOCK (el)] |=
			  GUID_MASK (el);

		      else if (infEl->w_is_true == 1)


			vectlevel[level]->true_crit_vect[GUID_BLOCK (el)] |=
			  GUID_MASK (el);
		    }

		  infEl->w_is_goal++;
		  backward_precond_propagation (infEl);
		}

	    }
	}
    }
}


void
remove_prec_act (inform_list infAction)
{
  inform_list infEl;
  int el;
  EfConn *act;
  int level, j, i;



  if (infAction->w_is_used <= 0)
    return;

  for (i = 0; i < ind_remove_act_chain; i++)
    if (remove_act_chain[i] == infAction)
      break;

  if (i == ind_remove_act_chain)
    {
      remove_act_chain[ind_remove_act_chain] = infAction;
      ind_remove_act_chain++;

      if (ind_remove_act_chain >= MAX_PLAN_LENGTH)
	{
	  exit (1);
	}
    }


  return;

  infAction->w_is_goal = 0;
  level = *infAction->level;
  act = CONVERT_ACTION_TO_VERTEX (infAction->position);

  for (j = 0; j < gef_conn[act->position].num_PC; j++)
    {
      el = gef_conn[act->position].PC[j];

      if (el < 0)
	continue;

      if (CHECK_FACT_POS (el, level))
	{

	  infEl = CONVERT_FACT_TO_INFORM (el, level);
	  if (infEl->w_is_goal)
	    infEl->w_is_goal--;

	  if (infEl->w_is_goal < 1)
	    {

	      vectlevel[level]->prec_vect[GUID_BLOCK (el)] &=
		~(GUID_MASK (el));



	      if (!infEl->w_is_true)
		vectlevel[level]->false_crit_vect[GUID_BLOCK (el)] &=
		  ~(GUID_MASK (el));
	      else if (infEl->w_is_true == 1)
		vectlevel[level]->true_crit_vect[GUID_BLOCK (el)] &=
		  ~(GUID_MASK (el));

	      backward_precond_remotion (infEl, GpG.approximation_level);

	    }




	}
    }
}







void
noop_add_not_in_add (inform * act, unsigned int pos)
{
  noop_not_in *new;

  if (noop_free_list == NULL)
    new = (noop_not_in *) calloc (1, sizeof (noop_not_in));

  else
    {
      new = noop_free_list;
      noop_free_list = noop_free_list->next;
    }
  new->position = pos;
  if (act->add == NULL)
    new->next = NULL;

  else
    new->next = act->add;
  act->add = new;

}


void
noop_prec_not_in_add (inform * act, unsigned int pos)
{
  noop_not_in *new;

  if (noop_free_list == NULL)
    new = (noop_not_in *) calloc (1, sizeof (noop_not_in));

  else
    {
      new = noop_free_list;
      noop_free_list = noop_free_list->next;
    }
  new->position = pos;
  if (act->preco == NULL)
    new->next = NULL;

  else
    new->next = act->preco;
  act->preco = new;
}


void
free_noop_not_in (inform * act)
{
  register noop_not_in *temp;
  noop_not_in *temp_noop;
  temp_noop = noop_free_list;
  temp = noop_free_list = act->preco;

  while (act->preco != NULL)
    {
      temp = act->preco;
      act->preco = act->preco->next;
    }
  if (temp != NULL)
    temp->next = act->add;

  while (act->add != NULL)
    {
      temp = act->add;
      act->add = act->add->next;
    }
  if (temp != NULL)
    temp->next = act->treated;

  while (act->treated != NULL)
    {
      temp = act->treated;
      act->treated = act->treated->next;
    }
  if (temp != NULL)
    temp->next = temp_noop;
}




void
forward_noop_propagation (int position_fact, int curr_level)
{
  int position_noop, position_me_action, i, cel;
  short save_action;
  inform *infnoop;
  if (curr_level >= GpG.curr_plan_length)
    return;
  position_noop = position_fact;
  infnoop = &vectlevel[curr_level]->noop_act[position_fact];

  if (infnoop->w_is_used)
    return;
  position_me_action = check_mutex_noop_durative (position_noop, curr_level);


  if (position_me_action != -1)
    {


      if (vectlevel[curr_level]->noop_act[position_fact].w_is_overall == 0)
	noop_add_not_in_add (&vectlevel[curr_level]->action, position_noop);

      if (GpG.lm_multilevel)
	update_mutex_multilevel (curr_level, position_me_action);
      else
	update_mutex (position_me_action);

      return;
    }


  vectlevel[curr_level]->noop_act[position_noop].w_is_used++;


  vectlevel[curr_level]->noop_act_vect[GUID_BLOCK (position_noop)] |=
    GUID_MASK (position_noop);


  if (vectlevel[curr_level]->noop_act[position_noop].w_is_overall ==
      NADD_DEL
      || vectlevel[curr_level]->noop_act[position_noop].w_is_overall ==
      ADD_DEL)
    return;


  while (curr_level < GpG.curr_plan_length)
    {
      curr_level++;
      vectlevel[curr_level]->fact[position_fact].w_is_true++;
      if (vectlevel[curr_level]->fact[position_fact].w_is_true >= 2
	  && GET_ACTION_OF_LEVEL (curr_level - 1))
	{


	  save_action = FALSE;


	  for (i = 0;
	       i < gef_conn[vectlevel[curr_level - 1]->action.position].num_A;
	       i++)
	    {
	      cel = gef_conn[vectlevel[curr_level - 1]->action.position].A[i];
	      if (cel < 0)
		{
		  save_action = TRUE;
		  break;
		}

	      if (cel == position_fact)
		continue;

	      if (vectlevel[curr_level]->fact[cel].w_is_goal >= 1
		  && vectlevel[curr_level]->fact[cel].w_is_true == 1)
		{
		  save_action = TRUE;
		  break;
		}
	    }


	  if (gef_conn[vectlevel[curr_level - 1]->action.position].sf != NULL)
	    {
	      for (i = 0;
		   i <
		   gef_conn[vectlevel[curr_level - 1]->action.position].sf->
		   num_A_start; i++)
		{
		  cel =
		    gef_conn[vectlevel[curr_level - 1]->action.position].sf->
		    A_start[i];
		  if (cel < 0)
		    {
		      save_action = TRUE;
		      break;
		    }

		  if (is_fact_in_delete_effects
		      (GET_ACTION_POSITION_OF_LEVEL (curr_level - 1), cel))
		    continue;

		  if (cel == position_fact)
		    continue;

		  if (vectlevel[curr_level]->fact[cel].w_is_goal >= 1
		      && vectlevel[curr_level]->fact[cel].w_is_true == 1)
		    {
		      save_action = TRUE;
		      break;
		    }
		}
	    }

	  if (save_action == FALSE)
	    {
	      for (i = 0; i < ind_remove_act_chain; i++)
		if (remove_act_chain[i] == &vectlevel[curr_level - 1]->action)
		  break;
	      if (i == ind_remove_act_chain)
		{
		  remove_act_chain[ind_remove_act_chain] =
		    &vectlevel[curr_level - 1]->action;
		  ind_remove_act_chain++;


		  if (ind_remove_act_chain >= MAX_PLAN_LENGTH)
		    {
		      exit (1);
		    }

		}
	    }
	}



      vectlevel[curr_level]->fact_vect[GUID_BLOCK (position_fact)] |=
	GUID_MASK (position_fact);
      vectlevel[curr_level]->false_crit_vect[GUID_BLOCK (position_fact)] &=
	~(GUID_MASK (position_fact));
      if (vectlevel[curr_level]->fact[position_fact].w_is_goal)
	{
	  if (vectlevel[curr_level]->fact[position_fact].w_is_true == 1)
	    vectlevel[curr_level]->
	      true_crit_vect[GUID_BLOCK (position_fact)] |=
	      GUID_MASK (position_fact);

	  else if (vectlevel[curr_level]->fact[position_fact].w_is_true > 1)
	    vectlevel[curr_level]->
	      true_crit_vect[GUID_BLOCK (position_fact)] &=
	      ~(GUID_MASK (position_fact));
	}


      if (vectlevel[curr_level]->fact[position_fact].false_position >= 0)
	remove_false_fact ((inform_list) & vectlevel[curr_level]->
			   fact[position_fact]);


      vectlevel[curr_level]->num_fact++;
      if (curr_level >= GpG.curr_plan_length
	  || vectlevel[curr_level]->noop_act[position_noop].w_is_used)
	return;
      position_me_action =
	check_mutex_noop_durative (position_noop, curr_level);


      if (position_me_action != -1)
	{
	  if (vectlevel[curr_level]->noop_act[position_fact].w_is_overall ==
	      0)
	    noop_add_not_in_add (&vectlevel[curr_level]->action,
				 position_noop);
	  if (GpG.lm_multilevel)
	    update_mutex_multilevel (curr_level, position_me_action);
	  else
	    update_mutex (position_me_action);


	  return;
	}
      vectlevel[curr_level]->noop_act[position_noop].w_is_used++;


      vectlevel[curr_level]->noop_act_vect[GUID_BLOCK (position_noop)] |=
	GUID_MASK (position_noop);


      if (vectlevel[curr_level]->noop_act[position_noop].w_is_overall ==
	  NADD_DEL
	  || vectlevel[curr_level]->noop_act[position_noop].w_is_overall ==
	  ADD_DEL)
	return;
    }
}


int
forward_noop_remotion (int position_fact, int curr_level)
{
  int position_noop;
  inform *infnoop;
  inform *fact;
  if (curr_level < GpG.curr_plan_length)
    {
      position_noop = position_fact;
      fact = CONVERT_FACT_TO_INFORM (position_fact, curr_level);
      infnoop = &vectlevel[curr_level]->noop_act[position_fact];


      if (infnoop->w_is_used <= 0)
	return (0);
      if (GpG.temporal_plan && GpG.forward_time)
	forward_noop_remotion_time (infnoop);
      while (curr_level < GpG.curr_plan_length)
	{


	  if (vectlevel[curr_level]->noop_act[position_noop].w_is_overall ==
	      ADD_DEL
	      || vectlevel[curr_level]->noop_act[position_noop].
	      w_is_overall == ADD_NDEL)
	    return (0);


	  vectlevel[curr_level]->noop_act[position_noop].w_is_used--;
	  if (vectlevel[curr_level]->noop_act[position_noop].w_is_used <= 0)
	    vectlevel[curr_level]->
	      noop_act_vect[GUID_BLOCK (position_noop)] &=
	      ~(GUID_MASK (position_noop));


	  if (vectlevel[curr_level]->noop_act[position_noop].w_is_overall ==
	      NADD_DEL)
	    return (0);


	  curr_level++;



	  vectlevel[curr_level]->fact[position_fact].w_is_true--;
	  if (vectlevel[curr_level]->fact[position_fact].w_is_true == 1)
	    {


	      if (vectlevel[curr_level]->fact[position_fact].w_is_goal)
		vectlevel[curr_level]->
		  true_crit_vect[GUID_BLOCK (position_fact)] |=
		  (GUID_MASK (position_fact));
	    }
	  if (vectlevel[curr_level]->fact[position_fact].w_is_true > 0)
	    return (0);

	  else
	    {


	      vectlevel[curr_level]->fact_vect[GUID_BLOCK (position_fact)] &=
		~(GUID_MASK (position_fact));
	      vectlevel[curr_level]->num_fact--;
	      vectlevel[curr_level]->
		true_crit_vect[GUID_BLOCK (position_fact)] &=
		~(GUID_MASK (position_fact));
	      if (vectlevel[curr_level]->fact[position_fact].w_is_used)
		insert_unsup_fact (&vectlevel[curr_level]->
				   fact[position_fact]);
	      if (vectlevel[curr_level]->fact[position_fact].w_is_goal > 0)
		{
		  vectlevel[curr_level]->
		    false_crit_vect[GUID_BLOCK (position_fact)] |=
		    GUID_MASK (position_fact);
		}
	    }


	  if (vectlevel[curr_level]->noop_act[position_noop].w_is_used == 0)
	    return (0);
	}
    }
  return (0);
}


int
remove_noop (inform * act, unsigned int pos)
{


  inform *fatto;
  int curr_level;
  unsigned int f_uid_block, f_uid_mask;
  curr_level = *act->level;


  vectlevel[curr_level]->noop_act[pos].w_is_used--;
  vectlevel[curr_level]->noop_act_vect[GUID_BLOCK (pos)] &=
    ~(GUID_MASK (pos));


  noop_add_not_in_add (act, pos);


  fatto = &vectlevel[curr_level + 1]->fact[pos];


  fatto->w_is_true--;
  if (GpG.temporal_plan)
    noop_remotion_time (&vectlevel[curr_level]->noop_act[pos]);


  if (fatto->w_is_true == 0)
    {
      f_uid_block = GUID_BLOCK (pos);
      f_uid_mask = GUID_MASK (pos);
      vectlevel[curr_level + 1]->fact_vect[f_uid_block] &= ~(f_uid_mask);
      vectlevel[curr_level + 1]->true_crit_vect[f_uid_block] &= ~f_uid_mask;
      if (fatto->w_is_goal <= 0)
	vectlevel[curr_level + 1]->false_crit_vect[f_uid_block] &=
	  ~f_uid_mask;

      else
	vectlevel[curr_level + 1]->false_crit_vect[f_uid_block] |= f_uid_mask;
      vectlevel[curr_level + 1]->num_fact--;

      if (fatto->w_is_used)
	insert_unsup_fact (&vectlevel[curr_level + 1]->fact[pos]);
      forward_noop_remotion (pos, curr_level + 1);
    }

  else if (fatto->w_is_true == 1)
    {
      f_uid_block = GUID_BLOCK (pos);
      f_uid_mask = GUID_MASK (pos);
      vectlevel[curr_level + 1]->false_crit_vect[f_uid_block] &= ~f_uid_mask;
      if (fatto->w_is_goal > 0)
	vectlevel[curr_level + 1]->true_crit_vect[f_uid_block] |= f_uid_mask;
    }

  if (GpG.temporal_plan)
    GpG.forward_time = 1;
  return (0);
}


void
remove_backward_noop_chain (inform * act, int noop_pos)
{
  register inform *noop, *fact;
  int level, fact_pos, j;
  unsigned int uid_block, uid_mask, fact_uid_block, fact_uid_mask;
  int el;
  inform_list inf_act;
  level = *act->level;
  if (vectlevel[level]->noop_act[noop_pos].w_is_overall == 0)
    noop_prec_not_in_add (act, noop_pos);
  noop = &vectlevel[level]->noop_act[noop_pos];
  fact_uid_block = uid_block = GUID_BLOCK (noop_pos);
  fact_uid_mask = uid_mask = GUID_MASK (noop_pos);

  vectlevel[level]->noop_prec_act_vect[uid_block] &= ~uid_mask;
  noop->w_is_goal--;
  fact = &vectlevel[level]->fact[noop_pos];
  fact_pos = noop_pos;
  if (fact->w_is_goal)
    fact->w_is_goal--;


  while (fact->w_is_goal == 0)
    {


      vectlevel[level]->prec_vect[fact_uid_block] &= ~fact_uid_mask;
      vectlevel[level]->true_crit_vect[fact_uid_block] &= ~fact_uid_mask;
      vectlevel[level]->false_crit_vect[fact_uid_block] &= ~fact_uid_mask;
      level--;

      if (level < 0)
	{
	  exit (0);
	}

      inf_act = GET_ACTION_OF_LEVEL (level);
      if (inf_act->position >= 0)
	{


	  for (j = 0; j < gef_conn[inf_act->position].num_A; j++)
	    {
	      el = gef_conn[inf_act->position].A[j];
	      if (el < 0)
		break;
	      if (el == fact_pos && action_eff_cost (inf_act) <= 0.0)
		{


		  remove_prec_act (inf_act);
		  break;
		}
	    }


	  if (gef_conn[inf_act->position].sf != NULL)
	    {
	      for (j = 0; j < gef_conn[inf_act->position].sf->num_A_start;
		   j++)
		{
		  el = gef_conn[inf_act->position].sf->A_start[j];
		  if (el < 0)
		    break;
		  if (is_fact_in_delete_effects (inf_act->position, el))
		    continue;
		  if (el == fact_pos && action_eff_cost (inf_act) <= 0.0)
		    {


		      remove_prec_act (inf_act);
		      break;
		    }
		}
	    }
	}
      if (level <= 0 || CHECK_FACT_POS (fact->position, level) == FALSE
	  || (vectlevel[level]->noop_prec_act_vect[uid_block] & uid_mask) ==
	  0)
	break;

      vectlevel[level]->noop_prec_act_vect[uid_block] &= ~uid_mask;
      vectlevel[level]->noop_act[noop_pos].w_is_goal--;
      fact = &vectlevel[level]->fact[fact_pos];
      if (fact->w_is_goal)
	fact->w_is_goal--;

    }
}


int
backward_precond_propagation (inform * fact)
{
  int curr_level, position_fact, position_noop, position_me_action, back_prop;
  unsigned int f_uid_block, f_uid_mask, uid_block, uid_mask;
  inform_list inf_act;
  position_fact = fact->position;
  curr_level = *fact->level;
  if (curr_level > GpG.curr_plan_length)
    return 1;
  back_prop = 0;
  position_noop = position_fact;
  if (position_noop < 0)
    return 1;
  f_uid_block = uid_block = GUID_BLOCK (position_noop);
  f_uid_mask = uid_mask = GUID_MASK (position_noop);
  while (curr_level > 0)
    {
      curr_level--;
      inf_act = GET_ACTION_OF_LEVEL (curr_level);
      position_me_action =
	check_mutex_noop_durative (position_noop, curr_level);
      if (inf_act->w_is_used)
	{


	  if (is_fact_in_additive_effects (inf_act->position, position_fact))
	    {
	      insert_prec_act (inf_act);
	      if (position_me_action >= 0)
		noop_prec_not_in_add (&vectlevel[curr_level]->action,
				      position_noop);
	      break;
	    }
	  if (is_fact_in_additive_effects_start
	      (inf_act->position, position_fact)
	      && !is_fact_in_delete_effects (inf_act->position,
					     position_fact))
	    {
	      insert_prec_act (inf_act);
	      if (position_me_action >= 0)
		noop_prec_not_in_add (&vectlevel[curr_level]->action,
				      position_noop);
	      break;
	    }
	}
      if (!CHECK_NOOP_POS (position_noop, curr_level)
	  || vectlevel[curr_level]->noop_act[position_noop].w_is_goal)
	return 0;

      if ((vectlevel[curr_level]->noop_act[position_noop].w_is_overall ==
	   DEL_ADD
	   || vectlevel[curr_level]->noop_act[position_noop].
	   w_is_overall == DEL_NADD
	   || vectlevel[curr_level]->noop_act[position_noop].
	   w_is_overall == ADD_DEL
	   || vectlevel[curr_level]->noop_act[position_noop].
	   w_is_overall == NADD_DEL) && back_prop)
	return (0);


      if (vectlevel[curr_level]->noop_act[position_noop].w_is_used == 1
	  || position_me_action < 0)
	{
	  vectlevel[curr_level]->noop_act[position_noop].w_is_goal++;
	  vectlevel[curr_level]->noop_prec_act_vect[uid_block] |= uid_mask;
	  vectlevel[curr_level]->fact[position_fact].w_is_goal++;
	  vectlevel[curr_level]->prec_vect[f_uid_block] |= f_uid_mask;
	  if (vectlevel[curr_level]->fact[position_fact].w_is_true == 1)
	    {


	      vectlevel[curr_level]->true_crit_vect[f_uid_block] |=
		f_uid_mask;
	      vectlevel[curr_level]->false_crit_vect[f_uid_block] &=
		~f_uid_mask;
	    }

	  else if (vectlevel[curr_level]->fact[position_fact].w_is_true <= 0)
	    {


	      vectlevel[curr_level]->true_crit_vect[f_uid_block] &=
		~f_uid_mask;
	      vectlevel[curr_level]->false_crit_vect[f_uid_block] |=
		f_uid_mask;
	    }

	  else
	    {

	      vectlevel[curr_level]->true_crit_vect[f_uid_block] &=
		~f_uid_mask;
	      vectlevel[curr_level]->false_crit_vect[f_uid_block] &=
		~f_uid_mask;
	    }
	}

      else
	{


	  if (position_me_action >= 0)
	    {
	      noop_prec_not_in_add (&vectlevel[curr_level]->action,
				    position_noop);
	      return (0);
	    }


	  else
	    {

	      exit (0);
	    }

	}
      back_prop = 1;
    }
  if (curr_level <= 0
      || vectlevel[curr_level - 1]->fact[position_fact].w_is_goal > 1)
    return (1);
  return 0;
}


int
backward_precond_remotion (inform * fact, int propagation)
{
  int curr_level, position_fact, noop_position, j;
  unsigned int uid_block, uid_mask, f_uid_block, f_uid_mask;
  int el;
  inform_list inf_act;

  curr_level = *fact->level;
  position_fact = fact->position;
  if (curr_level > 0 && CHECK_FACT_POS (position_fact, curr_level - 1))
    noop_position = position_fact;

  else
    return 1;
  f_uid_block = uid_block = GUID_BLOCK (noop_position);
  f_uid_mask = uid_mask = GUID_MASK (noop_position);
  while (curr_level > 0)
    {
      curr_level--;

      if (propagation)
	{
	  inf_act = &vectlevel[curr_level]->action;
	  if (inf_act->w_is_used)
	    {
	      for (j = 0; j < gef_conn[inf_act->position].num_A; j++)
		if ((el = gef_conn[inf_act->position].A[j]) == position_fact
		    && action_eff_cost (inf_act) <= 0.0)
		  {

		    remove_prec_act (inf_act);
		    break;
		  }


	      if (gef_conn[inf_act->position].sf != NULL)
		{
		  for (j = 0;
		       j < gef_conn[inf_act->position].sf->num_A_start; j++)
		    {
		      el = gef_conn[inf_act->position].sf->A_start[j];
		      if (el < 0)
			continue;
		      if (is_fact_in_delete_effects (inf_act->position, el))
			continue;
		      if (el == position_fact
			  && action_eff_cost (inf_act) <= 0.0)
			{
			  remove_prec_act (inf_act);
			  break;
			}
		    }
		}
	    }
	}

      if (propagation == 0)
	propagation = 1;
      if (!CHECK_NOOP_POS (noop_position, curr_level))
	return 0;
      if (vectlevel[curr_level]->noop_prec_act_vect[uid_block] & uid_mask)
	{
	  vectlevel[curr_level]->noop_prec_act_vect[uid_block] &= ~uid_mask;
	  vectlevel[curr_level]->noop_act[noop_position].w_is_goal--;
	  vectlevel[curr_level]->fact[position_fact].w_is_goal--;
	}

      else
	return 0;


      if (vectlevel[curr_level]->fact[position_fact].w_is_goal != 0)
	return (0);


      vectlevel[curr_level]->true_crit_vect[f_uid_block] &= ~f_uid_mask;
      vectlevel[curr_level]->false_crit_vect[f_uid_block] &= ~f_uid_mask;
      vectlevel[curr_level]->prec_vect[f_uid_block] &= ~f_uid_mask;



    }
  return (0);
}


void
check_prec_add_list (inform * act)
{
  noop_not_in *temp;
  int level;
  inform_list inf_noop;
  level = *act->level;

  temp = act->preco;
  while (temp != NULL)
    {
      inf_noop = &vectlevel[level]->noop_act[temp->position];
      if (CHECK_NOOP_POS (temp->position, level)
	  && vectlevel[level + 1]->fact[temp->position].w_is_goal
	  && vectlevel[level]->fact[temp->position].w_is_goal == 0)
	backward_precond_propagation (&vectlevel[level + 1]->
				      fact[temp->position]);
      if (vectlevel[level + 1]->fact[temp->position].w_is_goal > 0
	  && vectlevel[level]->noop_act[temp->position].w_is_goal == 0
	  && vectlevel[level]->fact[temp->position].w_is_goal > 0)
	{
	  vectlevel[level]->noop_act[temp->position].w_is_goal++;
	  vectlevel[level]->
	    noop_prec_act_vect[GUID_BLOCK (temp->position)] |=
	    GUID_MASK (temp->position);
	}
      temp = temp->next;
    }

  temp = act->add;
  while (temp != NULL)
    {
      inf_noop = &vectlevel[level]->noop_act[temp->position];
      if (CHECK_NOOP_POS (temp->position, level)
	  && vectlevel[level]->fact[temp->position].w_is_true)
	{
	  forward_noop_propagation (temp->position, level);
	  if (GpG.temporal_plan)
	    forward_noop_propagation_time (&vectlevel[level]->
					   noop_act[temp->position]);
	}
      temp = temp->next;
    }

  temp = act->treated;
  while (temp != NULL)
    {
      inf_noop = &vectlevel[level]->noop_act[temp->position];
      if (CHECK_NOOP_POS (temp->position, level)
	  && vectlevel[level + 1]->noop_act[temp->position].false_position)
	remove_treated_noop (inf_noop);
      temp = temp->next;
    }
  free_noop_not_in (act);
}



void
remove_false_fact (register inform_list inf_address)
{
  constraints_list unsup_f;
  register int temp, level, j, el;
  FtConn *go_address = NULL;

  if (inf_address->action_fact == IS_FACT)
    {
      go_address = CONVERT_FACT_TO_VERTEX (inf_address->position);

    }

  else
    {
      exit (1);
    }

  temp = 0;
  level = *inf_address->level;
  if (inf_address->false_position < 0)
    {

      return;
    }

  if (inf_address->w_is_used && level > 0)
    {
      temp = 0;



      if (GET_ACTION_OF_LEVEL (level - 1)->w_is_used)
	for (j = 0;
	     j < gef_conn[GET_ACTION_OF_LEVEL (level - 1)->position].num_A;
	     j++)
	  if ((el =
	       gef_conn[GET_ACTION_OF_LEVEL (level - 1)->position].A[j]) ==
	      inf_address->position)
	    {
	      temp++;
	      break;
	    }
      if (CONVERT_NOOP_TO_INFORM (inf_address->position, level - 1)->
	  w_is_used)
	temp++;
      if (temp == 0)
	{

	  return;
	}
    }

  GpG.num_false_fa--;

  unsup_f = unsup_fact[inf_address->false_position];
  unsup_fact[inf_address->false_position] = unsup_fact[GpG.num_false_fa];
  CONVERT_FACT_TO_INFORM (unsup_fact[GpG.num_false_fa]->fact,
			  *unsup_fact[GpG.num_false_fa]->level)->
    false_position = inf_address->false_position;
  inf_address->false_position = -1;
  unsup_fact[GpG.num_false_fa] = unsup_f;
  unsup_fact[GpG.num_false_fa]->fact = unsup_fact[GpG.num_false_fa]->action =
    unsup_fact[GpG.num_false_fa]->constraint_type = -1;
  if (DEBUG3 && num_try > 0)
    printf ("\n New True Fact: %s   Level %d ",
	    print_ft_name_string (inf_address->position, temp_name),
	    *inf_address->level);

}


void
remove_treated_noop (register inform_list inf_address)
{
  constraints_list t_c_l;
  register int temp, level;
  FtConn *go_address;

  go_address = CONVERT_NOOP_TO_VERTEX (inf_address->position);

  temp = 0;
  level = *inf_address->level;

  if (inf_address->false_position < 0)
    {
      return;
    }

  if (GpG.num_false_act <= 0)
    {

      return;
    }

  GpG.num_false_act--;
  if (DEBUG3 && num_try > 0)
    printf ("\n New Not Treated Noop: %s   Level %d ",
	    print_noop_name_string (treated_c_l[GpG.num_false_act]->fact,
				    temp_name),
	    *treated_c_l[GpG.num_false_act]->level);
  t_c_l = treated_c_l[inf_address->false_position];
  treated_c_l[inf_address->false_position] = treated_c_l[GpG.num_false_act];

  CONVERT_NOOP_TO_INFORM (treated_c_l[GpG.num_false_act]->fact,
			  *treated_c_l[GpG.num_false_act]->level)->
    false_position = inf_address->false_position;
  inf_address->false_position = -1;
  treated_c_l[GpG.num_false_act] = t_c_l;
  treated_c_l[GpG.num_false_act]->fact =
    treated_c_l[GpG.num_false_act]->action =
    treated_c_l[GpG.num_false_act]->constraint_type = -1;

}


void
insert_unsup_fact (register inform_list inf_address)
{
  FtConn *go_address;
  int level;

  if (inf_address->action_fact == IS_FACT)
    {
      go_address = CONVERT_FACT_TO_VERTEX (inf_address->position);

    }
  level = *inf_address->level;
  if (inf_address->false_position >= 0)
    {

      return;
    }

  if (inf_address->action_fact != IS_FACT)
    {


      return;
    }

  if (unsup_fact[GpG.num_false_fa] == NULL)
    unsup_fact[GpG.num_false_fa] =
      (constraints_list) malloc (sizeof (constraints));

  if (GpG.num_false_fa >= MAX_FALSE)
    {
      exit (1);
    }

  unsup_fact[GpG.num_false_fa]->fact = inf_address->position;
  unsup_fact[GpG.num_false_fa]->action = -1;
  unsup_fact[GpG.num_false_fa]->constraint_type = C_T_UNSUP_FACT;
  unsup_fact[GpG.num_false_fa]->level = inf_address->level;
  inf_address->false_position = GpG.num_false_fa++;
  if (DEBUG3 && num_try > 0)
    printf ("\n New False Fact: %s   Level %d ",
	    print_ft_name_string (inf_address->position, temp_name),
	    *inf_address->level);

  if (GpG.num_false_fa >= MAX_FALSE)
    {
      exit (1);
    }

}

void
insert_treated_fact (register inform_list act_address, int noop_pos)
{
  EfConn *op_address;
  int level;
  inform_list inf_address;

  op_address = CONVERT_ACTION_TO_VERTEX (act_address->position);

  level = *act_address->level;
  inf_address = CONVERT_NOOP_TO_INFORM (noop_pos, level);
  if (inf_address->false_position >= 0)
    {

      return;
    }

  if (treated_c_l[GpG.num_false_act] == NULL)
    treated_c_l[GpG.num_false_act] =
      (constraints_list) malloc (sizeof (constraints));
  treated_c_l[GpG.num_false_act]->action = act_address->position;
  treated_c_l[GpG.num_false_act]->fact = noop_pos;
  treated_c_l[GpG.num_false_act]->constraint_type = C_T_TREATED_CL;
  treated_c_l[GpG.num_false_act]->level = act_address->level;
  act_address->false_position = GpG.num_false_act;
  CONVERT_NOOP_TO_INFORM (noop_pos, *inf_address->level)->false_position =
    GpG.num_false_act++;
  if (DEBUG3 && num_try > 0)
    printf ("\n New Treated Noop: %s   Level %d ",
	    print_noop_name_string (noop_pos, temp_name), level);

  if (GpG.num_false_act >= MAX_FALSE)
    {
      exit (1);
    }


}









int
insert_action_in_vectlevel (int act_pos, int level)
{
  register int temp, k, count;
  int el, cel, next_level, i, j, jk;
  inform_list infEl, infAction, add_effect, infNoop = NULL;
  EfConn *act;
  float temp_cost;
  if (DEBUG6 && num_try > -2000)
    print_actions_in_subgraph ();
  if (DEBUG6 && num_try > -2000)
    {
      print_unsup_fact_vect ();
      print_unsup_num_facts ();
    }
  if (level > 0 && vectlevel[level]->action.position >= 0
      && vectlevel[level - 1]->action.position < 0
      && CHECK_ACTION_POS (act_pos, level - 1)
      && (level - 1) > gef_conn[act_pos].level)
    level--;
  else
    if (vectlevel[level]->action.position >= 0
	|| level == GpG.curr_plan_length)
    {
      if (level >= MAX_PLAN_LENGTH)
	{

	  exit (0);
	}

      up_vectlevel (level);
    }

  vectlevel[level]->action.being_removed = TRUE;
  vectlevel[level]->action.position = act_pos;
  act = CONVERT_ACTION_TO_VERTEX (act_pos);
  infAction = GET_ACTION_OF_LEVEL (level);
  if (level >= GpG.curr_plan_length)
    {

    }



  if ((DEBUG1 && num_try > -2000) || DEBUG2)
    {

      printf ("\n\n*** INSERT ACTION: level %d   action : ", level);
      print_op_name (act->position);
      if (DEBUG1 && !DEBUG2 && num_try > 0)
	printf ("  num_try %d", num_try);

    }

  GpG.num_actions++;
  vectlevel[level]->num_actions++;

  temp_cost = get_action_cost (infAction->position);
  GpG.total_cost += temp_cost;
  temp_cost = get_action_time (infAction->position, *infAction->level);
  act->step = num_try;
  infAction->w_is_used++;
  infAction->w_is_goal++;

  next_level = level + 1;
  if (gef_conn[act_pos].sf != NULL)
    {

      for (j = 0; j < gef_conn[act_pos].sf->num_A_start; j++)
	{
	  cel = gef_conn[act_pos].sf->A_start[j];
	  if (cel < 0)
	    continue;
	  add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);
	  if (is_fact_in_delete_effects (act_pos, cel))
	    {

	      vectlevel[level]->noop_act[cel].w_is_overall = ADD_DEL;

	      if (vectlevel[level]->
		  fact_vect[GUID_BLOCK (cel)] & vectlevel[level]->
		  noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		insert_treated_noop_chain (infAction, cel);


	      if (vectlevel[level]->
		  noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		{
		  if (!is_fact_in_preconditions
		      (GET_ACTION_POSITION_OF_LEVEL (level), cel)
		      &&
		      !is_fact_in_preconditions_overall
		      (GET_ACTION_POSITION_OF_LEVEL (level), cel)
		      &&
		      !is_fact_in_preconditions_end
		      (GET_ACTION_POSITION_OF_LEVEL (level), cel))
		    backward_precond_remotion (add_effect, 0);

		  else
		    {


		      vectlevel[level]->
			noop_prec_act_vect[GUID_BLOCK (cel)] &=
			~GUID_MASK (cel);
		      vectlevel[level]->noop_act[cel].w_is_goal--;
		      vectlevel[level]->fact[cel].w_is_overall = PREC_DEL;
		    }
		}

	      if (!vectlevel[level]->noop_act[cel].w_is_used)
		{
		  vectlevel[level]->noop_act[cel].w_is_used++;
		  vectlevel[level]->noop_act_vect[GUID_BLOCK (cel)] |=
		    GUID_MASK (cel);
		}

	      else
		{


		  add_effect->w_is_true--;

		  if (add_effect->w_is_true == 0)
		    {
		      vectlevel[level + 1]->fact_vect[GUID_BLOCK (cel)] &=
			~(GUID_MASK (cel));
		      vectlevel[level +
				1]->true_crit_vect[GUID_BLOCK (cel)] &=
			~GUID_MASK (cel);
		      if (add_effect->w_is_goal <= 0)
			vectlevel[level +
				  1]->false_crit_vect[GUID_BLOCK (cel)] &=
			  ~GUID_MASK (cel);

		      else
			vectlevel[level +
				  1]->false_crit_vect[GUID_BLOCK (cel)] |=
			  GUID_MASK (cel);
		      vectlevel[level + 1]->num_fact--;
		      if (add_effect->w_is_used)
			insert_unsup_fact (add_effect);
		    }
		  noop_remotion_time (CONVERT_NOOP_TO_INFORM (cel, level));
		  forward_noop_remotion (cel, level + 1);
		}
	    }

	  else
	    {

	      vectlevel[level]->noop_act[cel].w_is_overall = ADD_NDEL;
	      if (check_mutex_noop (cel, level) >= 0
		  && vectlevel[level]->
		  noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		{

		  if (vectlevel[level]->
		      fact_vect[GUID_BLOCK (cel)] & vectlevel[level]->
		      noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		    insert_treated_noop_chain (infAction, cel);
		  if (vectlevel[level]->
		      noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		    {

		      if (!is_fact_in_preconditions
			  (GET_ACTION_POSITION_OF_LEVEL (level), cel)
			  &&
			  !is_fact_in_preconditions_overall
			  (GET_ACTION_POSITION_OF_LEVEL (level), cel)
			  &&
			  !is_fact_in_preconditions_end
			  (GET_ACTION_POSITION_OF_LEVEL (level), cel))
			backward_precond_remotion (add_effect, 0);

		      else
			{


			  vectlevel[level]->
			    noop_prec_act_vect[GUID_BLOCK (cel)] &=
			    ~GUID_MASK (cel);
			  vectlevel[level]->noop_act[cel].w_is_goal--;
			  vectlevel[level]->fact[cel].w_is_overall = PREC_DEL;
			}
		    }
		}

	      if (!vectlevel[level]->noop_act[cel].w_is_used)
		{
		  vectlevel[level]->noop_act[cel].w_is_used++;
		  vectlevel[level]->noop_act_vect[GUID_BLOCK (cel)] |=
		    GUID_MASK (cel);
		  add_effect->w_is_true++;
		}
	      if (add_effect->w_is_true <= 2)
		{
		  vectlevel[next_level]->fact_vect[GUID_BLOCK (cel)] |=
		    GUID_MASK (cel);
		  vectlevel[next_level]->num_fact++;
		  if (add_effect->w_is_goal)
		    {

		      if (add_effect->w_is_true == 1)
			{
			  remove_false_fact (add_effect);
			  GpG.num_prec -= add_effect->w_is_goal;
			  vectlevel[next_level]->
			    false_crit_vect[GUID_BLOCK (cel)] &=
			    ~(GUID_MASK (cel));
			  vectlevel[next_level]->
			    true_crit_vect[GUID_BLOCK (cel)] |=
			    GUID_MASK (cel);
			}

		      else
			vectlevel[next_level]->
			  true_crit_vect[GUID_BLOCK (cel)] &=
			  ~(GUID_MASK (cel));
		    }
		}
	      if (add_effect->w_is_true == 1)


		forward_noop_propagation (cel, next_level);
	    }
	}

      for (j = 0; j < gef_conn[act_pos].sf->num_D_start; j++)
	{
	  cel = gef_conn[act_pos].sf->D_start[j];
	  if (cel < 0)
	    continue;
	  add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);
	  if (is_fact_in_additive_effects (act_pos, cel))
	    {

	      vectlevel[level]->noop_act[cel].w_is_overall = DEL_ADD;

	      if (vectlevel[level]->
		  noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		{
		  if (!is_fact_in_preconditions
		      (GET_ACTION_POSITION_OF_LEVEL (level), cel))
		    backward_precond_remotion (add_effect, 0);

		  else
		    {


		      vectlevel[level]->
			noop_prec_act_vect[GUID_BLOCK (cel)] &=
			~GUID_MASK (cel);
		      vectlevel[level]->noop_act[cel].w_is_goal--;
		      vectlevel[level]->fact[cel].w_is_overall = PREC_DEL;
		    }
		}

	      if (vectlevel[level]->noop_act[cel].w_is_used > 0)
		{
		  vectlevel[level]->noop_act[cel].w_is_used--;
		  vectlevel[level]->noop_act_vect[GUID_BLOCK (cel)] &=
		    ~(GUID_MASK (cel));
		}

	      else
		{
		  if (add_effect->w_is_true++ <= 1)
		    {



		      vectlevel[next_level]->fact_vect[GUID_BLOCK (cel)] |=
			GUID_MASK (cel);


		      vectlevel[next_level]->num_fact++;
		      if (add_effect->w_is_goal)
			{

			  if (add_effect->w_is_true == 1)
			    {
			      remove_false_fact (add_effect);
			      GpG.num_prec -= add_effect->w_is_goal;
			      vectlevel[next_level]->
				false_crit_vect[GUID_BLOCK (cel)] &=
				~(GUID_MASK (cel));
			      vectlevel[next_level]->
				true_crit_vect[GUID_BLOCK (cel)] |=
				GUID_MASK (cel);
			    }

			  else
			    vectlevel[next_level]->
			      true_crit_vect[GUID_BLOCK (cel)] &=
			      ~(GUID_MASK (cel));
			}


		      forward_noop_propagation (cel, next_level);
		    }
		}
	    }

	  else
	    {

	      vectlevel[level]->noop_act[cel].w_is_overall = DEL_NADD;

	      if (vectlevel[level]->
		  fact_vect[GUID_BLOCK (cel)] & vectlevel[level]->
		  noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		insert_treated_noop_chain (infAction, cel);
	      if (vectlevel[level]->
		  noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		{

		  if (!is_fact_in_preconditions
		      (GET_ACTION_POSITION_OF_LEVEL (level), cel))
		    backward_precond_remotion (add_effect, 0);

		  else
		    {


		      vectlevel[level]->
			noop_prec_act_vect[GUID_BLOCK (cel)] &=
			~GUID_MASK (cel);
		      vectlevel[level]->noop_act[cel].w_is_goal--;
		      vectlevel[level]->fact[cel].w_is_overall = PREC_DEL;
		    }
		}


	      if (vectlevel[level]->noop_act[cel].w_is_used)
		{
		  vectlevel[level]->noop_act[cel].w_is_used--;
		  vectlevel[level]->noop_act_vect[GUID_BLOCK (cel)] &=
		    ~(GUID_MASK (cel));


		  add_effect->w_is_true--;
		  if (add_effect->w_is_true == 0)
		    {
		      vectlevel[level + 1]->fact_vect[GUID_BLOCK (cel)] &=
			~(GUID_MASK (cel));
		      vectlevel[level +
				1]->true_crit_vect[GUID_BLOCK (cel)] &=
			~GUID_MASK (cel);
		      if (add_effect->w_is_goal <= 0)
			vectlevel[level +
				  1]->false_crit_vect[GUID_BLOCK (cel)] &=
			  ~GUID_MASK (cel);

		      else
			vectlevel[level +
				  1]->false_crit_vect[GUID_BLOCK (cel)] |=
			  GUID_MASK (cel);
		      vectlevel[level + 1]->num_fact--;
		      if (add_effect->w_is_used)
			insert_unsup_fact (&vectlevel[level + 1]->fact[cel]);
		    }
		  noop_remotion_time (CONVERT_NOOP_TO_INFORM (cel, level));
		  forward_noop_remotion (cel, level + 1);
		}
	    }
	}

      for (j = 0; j < gef_conn[act_pos].num_D; j++)
	{
	  cel = gef_conn[act_pos].D[j];
	  if (cel < 0)
	    continue;
	  add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);
	  if (!is_fact_in_additive_effects_start (act_pos, cel))
	    {

	      vectlevel[level]->noop_act[cel].w_is_overall = NADD_DEL;

	      if (vectlevel[level]->
		  fact_vect[GUID_BLOCK (cel)] & vectlevel[level]->
		  noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		insert_treated_noop_chain (infAction, cel);
	      if (vectlevel[next_level]->
		  prec_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		{

		  if (!is_fact_in_preconditions
		      (GET_ACTION_POSITION_OF_LEVEL (level), cel)
		      &&
		      !is_fact_in_preconditions_overall
		      (GET_ACTION_POSITION_OF_LEVEL (level), cel)
		      &&
		      !is_fact_in_preconditions_end
		      (GET_ACTION_POSITION_OF_LEVEL (level), cel))
		    backward_precond_remotion (add_effect, 0);

		  else
		    {


		      vectlevel[level]->
			noop_prec_act_vect[GUID_BLOCK (cel)] &=
			~GUID_MASK (cel);
		      vectlevel[level]->noop_act[cel].w_is_goal--;
		      vectlevel[level]->fact[cel].w_is_overall = PREC_DEL;
		    }
		}
	      if (vectlevel[level]->noop_act[cel].w_is_used)


		if (add_effect->w_is_true)
		  {


		    add_effect->w_is_true--;
		    if (add_effect->w_is_true == 0)
		      {
			vectlevel[level + 1]->fact_vect[GUID_BLOCK (cel)] &=
			  ~(GUID_MASK (cel));
			vectlevel[level +
				  1]->true_crit_vect[GUID_BLOCK (cel)] &=
			  ~GUID_MASK (cel);
			if (add_effect->w_is_goal <= 0)
			  vectlevel[level +
				    1]->false_crit_vect[GUID_BLOCK (cel)] &=
			    ~GUID_MASK (cel);

			else
			  vectlevel[level +
				    1]->false_crit_vect[GUID_BLOCK (cel)] |=
			    GUID_MASK (cel);
			vectlevel[level + 1]->num_fact--;
			if (add_effect->w_is_used)
			  insert_unsup_fact (&vectlevel[level + 1]->
					     fact[cel]);
		      }
		    noop_remotion_time (CONVERT_NOOP_TO_INFORM (cel, level));
		    forward_noop_remotion (cel, level + 1);
		  }
	    }
	}
    }

  for (j = 0; j < gef_conn[act_pos].num_A; j++)
    {
      cel = gef_conn[act_pos].A[j];


      if (gef_conn[act_pos].sf
	  && is_fact_in_delete_effects_start (act_pos, cel))
	continue;
      if (cel < 0)
	continue;

      {
	add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);
	if (add_effect->w_is_true++ <= 1)
	  {



	    vectlevel[next_level]->fact_vect[GUID_BLOCK (cel)] |=
	      GUID_MASK (cel);


	    vectlevel[next_level]->num_fact++;
	    if (add_effect->w_is_goal)
	      {

		if (add_effect->w_is_true == 1)
		  {
		    remove_false_fact (add_effect);
		    GpG.num_prec -= add_effect->w_is_goal;
		    vectlevel[next_level]->
		      false_crit_vect[GUID_BLOCK (cel)] &= ~(GUID_MASK (cel));
		    vectlevel[next_level]->
		      true_crit_vect[GUID_BLOCK (cel)] |= GUID_MASK (cel);
		  }

		else
		  {

		    vectlevel[next_level]->
		      true_crit_vect[GUID_BLOCK (cel)] &= ~(GUID_MASK (cel));
		  }
	      }


	    if (add_effect->w_is_true == 1)

	      forward_noop_propagation (cel, next_level);


	  }
      }
    }
  count = 0;


  for (i = 0, j = 0, count = 0; j < gnum_ft_conn; i++, j += 32)
    {

      temp =
	act->ft_exclusive_vect[i] & vectlevel[level]->
	noop_prec_act_vect[i] & vectlevel[level]->noop_act_vect[i];
      k = 32;
      while (temp)
	{
	  k--;
	  if (temp & FIRST_1
	      && !vectlevel[level]->noop_act[j + k].w_is_overall)
	    {
	      insert_treated_noop_chain (infAction, j + k);

	      if (GpG.lm_multilevel)
		update_mutex_multilevel (level, act_pos);
	      else
		update_mutex (act_pos);

	    }
	  temp <<= 1;
	}

      temp = act->ft_exclusive_vect[i] & vectlevel[level]->noop_act_vect[i];
      k = 32;
      while (temp)
	{
	  k--;
	  if (temp & FIRST_1)
	    {



	      if (!vectlevel[level]->noop_act[j + k].w_is_overall)
		remove_noop (infAction, j + k);
	    }
	  temp <<= 1;
	}

      temp =
	act->ft_exclusive_vect[i] & vectlevel[level]->noop_prec_act_vect[i];
      k = 32;
      while (temp)
	{
	  k--;
	  if (temp & FIRST_1
	      && !vectlevel[level]->noop_act[j + k].w_is_overall)
	    {
	      if (!is_fact_in_preconditions
		  (GET_ACTION_POSITION_OF_LEVEL (level), j + k)
		  &&
		  !is_fact_in_preconditions_overall
		  (GET_ACTION_POSITION_OF_LEVEL (level), j + k)
		  &&
		  !is_fact_in_preconditions_end (GET_ACTION_POSITION_OF_LEVEL
						 (level), j + k))
		remove_backward_noop_chain (infAction, j + k);

	      else
		{
		  jk = j + k;
		  noop_prec_not_in_add (infAction, jk);
		  vectlevel[level]->noop_prec_act_vect[GUID_BLOCK (jk)] &=
		    ~GUID_MASK (jk);
		  vectlevel[level]->noop_act[jk].w_is_goal--;
		  vectlevel[level]->fact[jk].w_is_overall = PREC_DEL;
		}
	    }
	  temp <<= 1;
	}
    }


  GpG.num_m_e += count;


  for (j = 0; j < gef_conn[act_pos].num_PC; j++)
    {
      el = gef_conn[act_pos].PC[j];
      if (el < 0)
	{


	  vectlevel[level]->numeric->w_is_goal[-el] = 1;


	  if (!is_num_prec_satisfied (-el, level))
	    {

	      insert_unsup_numeric_fact (-el, level);
	    }
	  continue;
	}

      if (CHECK_FACT_POS (el, level) == FALSE)
	printf ("\n Error fact_pos %d fact_lev %d, act_pos %d level %d ", el,
		gft_conn[el].level, act_pos, level);







      infEl = CONVERT_FACT_TO_INFORM (el, level);



      if (infEl->w_is_overall != PREC_DEL)
	infEl->w_is_goal++;

      if (infEl->w_is_used++ == 0 || infEl->w_is_goal == 1)
	{





	  vectlevel[level]->prec_vect[GUID_BLOCK (el)] |= GUID_MASK (el);


	  if (!infEl->w_is_true)
	    {


	      vectlevel[level]->false_crit_vect[GUID_BLOCK (el)] |=
		GUID_MASK (el);


	      insert_unsup_fact (CONVERT_FACT_TO_INFORM (el, level));
	    }

	  else if (infEl->w_is_true == 1)


	    vectlevel[level]->true_crit_vect[GUID_BLOCK (el)] |=
	      GUID_MASK (el);
	}
      if (infEl->w_is_overall != PREC_DEL)
	backward_precond_propagation (infEl);

      else
	infEl->w_is_overall = 0;



      if (!infEl->w_is_true)
	{
	  count++;
	  GpG.num_prec++;
	}


      CONVERT_FACT_TO_VERTEX (el)->lamda_prec =
	check_value (CONVERT_FACT_TO_VERTEX (el)->lamda_prec +
		     CONVERT_ACTION_TO_VERTEX (act_pos)->lamda_prec);
    }

  if (gef_conn[act_pos].sf != NULL)
    {


      for (j = 0; j < gef_conn[act_pos].sf->num_PC_overall; j++)
	{
	  el = gef_conn[act_pos].sf->PC_overall[j];
	  if (el < 0)
	    {


	      vectlevel[level]->numeric->w_is_goal[-el] = 1;


	      if (!is_num_prec_satisfied (-el, level))
		{

		  insert_unsup_numeric_fact (-el, level);
		}
	      continue;
	    }
	  if (CHECK_FACT_POS (el, level) == FALSE)
	    printf
	      ("\n Error fact_OVERALL_pos %d fact_lev %d, act_pos %d level %d ",
	       el, gft_conn[el].level, act_pos, level);

	  infEl = CONVERT_FACT_TO_INFORM (el, level);
	  infNoop = CONVERT_NOOP_TO_INFORM (el, level);

	  if (infNoop->w_is_overall != ADD_DEL
	      && infNoop->w_is_overall != ADD_NDEL)
	    {
	      if (infEl->w_is_overall != PREC_DEL)
		infEl->w_is_goal++;
	      if (infEl->w_is_used++ == 0 || infEl->w_is_goal == 1)
		{


		  vectlevel[level]->prec_vect[GUID_BLOCK (el)] |=
		    GUID_MASK (el);


		  if (!infNoop->w_is_used)
		    {


		      vectlevel[level]->false_crit_vect[GUID_BLOCK (el)] |=
			GUID_MASK (el);


		      insert_unsup_fact (CONVERT_FACT_TO_INFORM (el, level));
		    }

		  else if (infEl->w_is_true == 1)


		    vectlevel[level]->true_crit_vect[GUID_BLOCK (el)] |=
		      GUID_MASK (el);
		}
	      if (infEl->w_is_overall != PREC_DEL)
		backward_precond_propagation (infEl);

	      else
		infEl->w_is_overall = 0;
	      if (!infEl->w_is_true)
		{
		  count++;
		  GpG.num_prec++;
		}


	      CONVERT_FACT_TO_VERTEX (el)->lamda_prec =
		check_value (CONVERT_FACT_TO_VERTEX (el)->lamda_prec +
			     CONVERT_ACTION_TO_VERTEX (act_pos)->lamda_prec);
	    }
	}


      for (j = 0; j < gef_conn[act_pos].sf->num_PC_end; j++)
	{
	  el = gef_conn[act_pos].sf->PC_end[j];
	  if (el < 0)
	    {


	      vectlevel[level]->numeric->w_is_goal[-el] = 1;


	      if (!is_num_prec_satisfied (-el, level))
		{

		  insert_unsup_numeric_fact (-el, level);
		}
	      continue;
	    }





	  if (CHECK_FACT_POS (el, level) == FALSE)
	    printf
	      ("\n Error fact_AT_END__pos %d fact_lev %d, act_pos %d level %d ",
	       el, gft_conn[el].level, act_pos, level);







	  infEl = CONVERT_FACT_TO_INFORM (el, level);
	  infNoop = CONVERT_NOOP_TO_INFORM (el, level);




	  if (infNoop->w_is_overall != ADD_DEL
	      && infNoop->w_is_overall != ADD_NDEL)
	    {
	      if (infEl->w_is_overall != PREC_DEL)
		infEl->w_is_goal++;
	      if (infEl->w_is_used++ == 0 || infEl->w_is_goal == 1)
		{







		  vectlevel[level]->prec_vect[GUID_BLOCK (el)] |=
		    GUID_MASK (el);


		  if (!infEl->w_is_true)
		    {







		      vectlevel[level]->false_crit_vect[GUID_BLOCK (el)] |=
			GUID_MASK (el);







		      insert_unsup_fact (CONVERT_FACT_TO_INFORM (el, level));
		    }

		  else if (infEl->w_is_true == 1)







		    vectlevel[level]->true_crit_vect[GUID_BLOCK (el)] |=
		      GUID_MASK (el);

		}
	      if (infEl->w_is_overall != PREC_DEL)
		backward_precond_propagation (infEl);

	      else
		infEl->w_is_overall = 0;
	      if (!infEl->w_is_true)
		{
		  count++;
		  GpG.num_prec++;
		}


	      CONVERT_FACT_TO_VERTEX (el)->lamda_prec =
		check_value (CONVERT_FACT_TO_VERTEX (el)->lamda_prec +
			     CONVERT_ACTION_TO_VERTEX (act_pos)->lamda_prec);
	    }
	}
    }


  vectlevel[level]->action.being_removed = FALSE;
  apply_numeric_effects_of_action (act_pos, level);
  vectlevel[level]->action.being_removed = TRUE;

  if (GpG.temporal_plan)
    {
      if (DEBUG4)
	printf ("\n\n -+- TEMPORAL -+-\n");
      insert_time (infAction);
    }
  if ((GpG.accurate_cost == COMPUTE_MAX_COST
       && GpG.inc_choice_type >= MIN_COST_INC)
      || GpG.accurate_cost >= COMPUTE_DG_SUM_COST)
    {
      if (GpG.orig_weight_time)
	get_total_time_plan ();
      cri_heuristic_for_action (act_pos, level);
      set_computed_dg_costs (level);
    }

  vectlevel[level]->action.being_removed = FALSE;



  clean_unsup_num_fact ();

  if (DEBUG5 && num_try > -2000)
    {
      print_num_levels_and_actions ();
      print_actions_in_subgraph ();
    }

  return (TRUE);
}




int
remove_action_from_vectlevel (int act_pos, int level, int propagation)
{
  int el, cel, next_level, j;
  inform_list infAction, add_effect, precond_fact, infNoop = NULL;
  EfConn *act;
  float temp_cost;
  if (DEBUG6 && num_try > 1)
    print_actions_in_subgraph ();
  if (DEBUG6 && num_try > 1)
    {
      print_unsup_fact_vect ();
      print_unsup_num_facts ();
    }

  if (vectlevel[level]->action.position != act_pos)
    {

      return 0;
    }

  act = CONVERT_ACTION_TO_VERTEX (act_pos);
  infAction = GET_ACTION_OF_LEVEL (level);

  vectlevel[level]->action.being_removed = TRUE;



  if ((DEBUG1 && num_try > 1) || DEBUG2)
    {

      printf ("\n\n*** REMOVE ACTION: level %d   action: ", level);
      print_op_name (act->position);
      if (DEBUG1 && !DEBUG2 && num_try > 0)
	printf ("  num_try %d", num_try);

    }

  vectlevel[*infAction->level]->num_actions--;
  GpG.total_cost -= get_action_cost (infAction->position);
  GpG.num_actions--;
  temp_cost = get_action_time (infAction->position, *infAction->level);





  next_level = level + 1;

  for (j = 0; j < gef_conn[infAction->position].num_PC; j++)
    {
      el = gef_conn[infAction->position].PC[j];
      if (el < 0)
	{
	  vectlevel[level]->numeric->w_is_goal[-el] = 0;


	  remove_unsup_num_fact (vectlevel[level]->numeric->
				 false_position[-el]);
	  continue;
	}

      {
	precond_fact = CONVERT_FACT_TO_INFORM (el, level);
	precond_fact->w_is_used--;
	if (!vectlevel[next_level]->fact[el].w_is_goal > 0
	    || !vectlevel[level]->noop_act[el].w_is_goal == 0)



	  if (precond_fact->w_is_goal)
	    precond_fact->w_is_goal--;

	if (!precond_fact->w_is_used)
	  remove_false_fact (precond_fact);

	if (precond_fact->w_is_goal < 1)
	  {
	    vectlevel[level]->prec_vect[GUID_BLOCK (el)] &= ~(GUID_MASK (el));
	    if (!precond_fact->w_is_true)
	      {
		vectlevel[level]->false_crit_vect[GUID_BLOCK (el)] &=
		  ~(GUID_MASK (el));

	      }

	    else if (precond_fact->w_is_true == 1)
	      vectlevel[level]->true_crit_vect[GUID_BLOCK (el)] &=
		~(GUID_MASK (el));


	    if (!vectlevel[next_level]->fact[el].w_is_goal > 0
		|| !vectlevel[level]->noop_act[el].w_is_goal == 0)
	      backward_precond_remotion (precond_fact, propagation);
	  }
	if (!precond_fact->w_is_true)
	  GpG.num_prec--;


      }
    }

  if (gef_conn[act_pos].sf != NULL)
    {


      for (j = 0; j < gef_conn[infAction->position].sf->num_PC_overall; j++)
	{
	  el = gef_conn[infAction->position].sf->PC_overall[j];
	  infNoop = CONVERT_NOOP_TO_INFORM (el, level);
	  if (el < 0)
	    {
	      vectlevel[level]->numeric->w_is_goal[-el] = 0;


	      remove_unsup_num_fact (vectlevel[level]->numeric->
				     false_position[-el]);
	      continue;
	    }

	  if (infNoop->w_is_overall != ADD_DEL
	      && infNoop->w_is_overall != ADD_NDEL)
	    {
	      precond_fact = CONVERT_FACT_TO_INFORM (el, level);
	      precond_fact->w_is_used--;
	      if (!vectlevel[next_level]->fact[el].w_is_goal > 0
		  || !vectlevel[level]->noop_act[el].w_is_goal == 0)
		if (precond_fact->w_is_goal)
		  precond_fact->w_is_goal--;

	      if (!precond_fact->w_is_used)
		remove_false_fact (precond_fact);

	      if (precond_fact->w_is_goal < 1)
		{


		  vectlevel[level]->prec_vect[GUID_BLOCK (el)] &=
		    ~(GUID_MASK (el));
		  if (!precond_fact->w_is_true)
		    {
		      vectlevel[level]->false_crit_vect[GUID_BLOCK (el)] &=
			~(GUID_MASK (el));

		      remove_false_fact (precond_fact);
		    }

		  else if (precond_fact->w_is_true == 1)
		    vectlevel[level]->true_crit_vect[GUID_BLOCK (el)] &=
		      ~(GUID_MASK (el));

		  if (!vectlevel[next_level]->fact[el].w_is_goal > 0
		      || !vectlevel[level]->noop_act[el].w_is_goal == 0)
		    backward_precond_remotion (precond_fact, propagation);
		}
	      if (!precond_fact->w_is_true)
		GpG.num_prec--;


	    }
	}


      for (j = 0; j < gef_conn[infAction->position].sf->num_PC_end; j++)
	{
	  el = gef_conn[infAction->position].sf->PC_end[j];
	  infNoop = CONVERT_NOOP_TO_INFORM (el, level);

	  if (el < 0)
	    {
	      vectlevel[level]->numeric->w_is_goal[-el] = 0;


	      remove_unsup_num_fact (vectlevel[level]->numeric->
				     false_position[-el]);
	      continue;
	    }

	  if (infNoop->w_is_overall != ADD_DEL
	      && infNoop->w_is_overall != ADD_NDEL)
	    {



	      precond_fact = CONVERT_FACT_TO_INFORM (el, level);
	      precond_fact->w_is_used--;

	      if (!vectlevel[next_level]->fact[el].w_is_goal > 0
		  || !vectlevel[level]->noop_act[el].w_is_goal == 0)


		if (!precond_fact->w_is_used)
		  remove_false_fact (precond_fact);


	      if (precond_fact->w_is_goal)


		precond_fact->w_is_goal--;


	      if (precond_fact->w_is_goal < 1)
		{


		  vectlevel[level]->prec_vect[GUID_BLOCK (el)] &=
		    ~(GUID_MASK (el));


		  if (!precond_fact->w_is_true)
		    {




		      vectlevel[level]->false_crit_vect[GUID_BLOCK (el)] &=
			~(GUID_MASK (el));

		      remove_false_fact (precond_fact);
		    }

		  else if (precond_fact->w_is_true == 1)


		    vectlevel[level]->true_crit_vect[GUID_BLOCK (el)] &=
		      ~(GUID_MASK (el));

		  if (!vectlevel[next_level]->fact[el].w_is_goal > 0
		      || !vectlevel[level]->noop_act[el].w_is_goal == 0)
		    backward_precond_remotion (precond_fact, propagation);
		}
	      if (!precond_fact->w_is_true)
		GpG.num_prec--;


	    }
	}
    }

  infAction->w_is_goal = 0;


  vectlevel[level]->action.w_is_used = 0;

  if (gef_conn[act_pos].sf != NULL)
    {

      for (j = 0; j < gef_conn[act_pos].sf->num_A_start; j++)
	{
	  cel = gef_conn[act_pos].sf->A_start[j];
	  if (cel < 0)
	    continue;
	  add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);
	  vectlevel[level]->noop_act[cel].w_is_overall = 0;
	  if (is_fact_in_delete_effects (act_pos, cel))
	    {

	      if (vectlevel[next_level]->fact[cel].w_is_goal > 0
		  && vectlevel[level]->noop_act[cel].w_is_goal == 0
		  && vectlevel[level]->fact[cel].w_is_goal > 0)
		{


		  vectlevel[level]->noop_act[cel].w_is_goal++;
		  vectlevel[level]->noop_prec_act_vect[GUID_BLOCK (cel)] |=
		    GUID_MASK (cel);
		}

	      if (vectlevel[next_level]->fact[cel].w_is_goal
		  && vectlevel[level]->fact[cel].w_is_goal == 0)
		backward_precond_propagation (add_effect);

	      if (vectlevel[level]->
		  noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		remove_treated_noop (CONVERT_NOOP_TO_INFORM (cel, level));
	      if (!vectlevel[level]->fact[cel].w_is_true)
		{
		  vectlevel[level]->noop_act[cel].w_is_used--;
		  vectlevel[level]->noop_act_vect[GUID_BLOCK (cel)] &=
		    ~(GUID_MASK (cel));
		}

	      else
		{


		  vectlevel[next_level]->fact[cel].w_is_true++;



		  vectlevel[next_level]->fact_vect[GUID_BLOCK (cel)] |=
		    GUID_MASK (cel);
		  vectlevel[next_level]->
		    false_crit_vect[GUID_BLOCK (cel)] &= ~(GUID_MASK (cel));
		  if (vectlevel[next_level]->fact[cel].w_is_goal)
		    {
		      if (vectlevel[next_level]->fact[cel].w_is_true == 1)
			vectlevel[next_level]->
			  true_crit_vect[GUID_BLOCK (cel)] |= GUID_MASK (cel);

		      else if (vectlevel[next_level]->fact[cel].w_is_true > 1)
			vectlevel[next_level]->
			  true_crit_vect[GUID_BLOCK (cel)] &=
			  ~(GUID_MASK (cel));
		    }


		  if (vectlevel[next_level]->fact[cel].false_position >= 0)
		    remove_false_fact ((inform_list) &
				       vectlevel[next_level]->fact[cel]);


		  vectlevel[next_level]->num_fact++;
		  forward_noop_propagation (cel, next_level);
		}
	    }

	  else
	    {

	      if (check_mutex_noop (cel, level) >= 0
		  && vectlevel[next_level]->fact[cel].w_is_goal
		  && vectlevel[level]->fact[cel].w_is_goal == 0)
		{
		  if (vectlevel[next_level]->fact[cel].w_is_goal > 0
		      && vectlevel[level]->noop_act[cel].w_is_goal == 0
		      && vectlevel[level]->fact[cel].w_is_goal > 0)
		    {
		      vectlevel[level]->noop_act[cel].w_is_goal++;
		      vectlevel[level]->
			noop_prec_act_vect[GUID_BLOCK (cel)] |=
			GUID_MASK (cel);
		    }

		  backward_precond_propagation (add_effect);

		  remove_treated_noop (CONVERT_NOOP_TO_INFORM (cel, level));
		}
	      if (!vectlevel[level]->fact[cel].w_is_true)
		{
		  vectlevel[level]->noop_act[cel].w_is_used--;
		  vectlevel[level]->noop_act_vect[GUID_BLOCK (cel)] &=
		    ~(GUID_MASK (cel));
		  add_effect->w_is_true--;
		  GpG.num_prec -= add_effect->w_is_goal;
		  vectlevel[next_level]->fact_vect[GUID_BLOCK (cel)] &=
		    ~(GUID_MASK (cel));
		  vectlevel[next_level]->num_fact--;
		  if (add_effect->w_is_goal)
		    {
		      if (add_effect->w_is_used)
			insert_unsup_fact (add_effect);
		      vectlevel[next_level]->
			false_crit_vect[GUID_BLOCK (cel)] |= GUID_MASK (cel);
		      vectlevel[next_level]->
			true_crit_vect[GUID_BLOCK (cel)] &=
			~(GUID_MASK (cel));
		    }

		  forward_noop_remotion (cel, next_level);
		}
	    }
	}

      for (j = 0; j < gef_conn[act_pos].sf->num_D_start; j++)
	{
	  cel = gef_conn[act_pos].sf->D_start[j];
	  if (cel < 0)
	    continue;
	  add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);
	  vectlevel[level]->noop_act[cel].w_is_overall = 0;
	  if (is_fact_in_additive_effects (act_pos, cel))
	    {

	      if (vectlevel[next_level]->fact[cel].w_is_goal > 0
		  && vectlevel[level]->noop_act[cel].w_is_goal == 0
		  && vectlevel[level]->fact[cel].w_is_goal > 0)
		{


		  vectlevel[level]->noop_act[cel].w_is_goal++;
		  vectlevel[level]->noop_prec_act_vect[GUID_BLOCK (cel)] |=
		    GUID_MASK (cel);
		}

	      if (!vectlevel[next_level]->fact[cel].w_is_goal
		  && vectlevel[level]->fact[cel].w_is_goal == 0)
		backward_precond_propagation (add_effect);

	      if (vectlevel[level]->
		  noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		remove_treated_noop (CONVERT_NOOP_TO_INFORM (cel, level));
	      if (vectlevel[level]->fact[cel].w_is_true)
		{
		  vectlevel[level]->noop_act[cel].w_is_used++;
		  vectlevel[level]->noop_act_vect[GUID_BLOCK (cel)] |=
		    GUID_MASK (cel);
		}

	      else
		{
		  add_effect->w_is_true--;
		  GpG.num_prec -= add_effect->w_is_goal;
		  vectlevel[next_level]->fact_vect[GUID_BLOCK (cel)] &=
		    ~(GUID_MASK (cel));
		  vectlevel[next_level]->num_fact--;
		  if (add_effect->w_is_goal)
		    {
		      if (add_effect->w_is_used)
			insert_unsup_fact (add_effect);
		      vectlevel[next_level]->
			false_crit_vect[GUID_BLOCK (cel)] |= GUID_MASK (cel);
		      vectlevel[next_level]->
			true_crit_vect[GUID_BLOCK (cel)] &=
			~(GUID_MASK (cel));
		    }
		  forward_noop_remotion (cel, next_level);
		}
	    }

	  else
	    {

	      if (vectlevel[next_level]->fact[cel].w_is_goal > 0
		  && vectlevel[level]->noop_act[cel].w_is_goal == 0
		  && vectlevel[level]->fact[cel].w_is_goal > 0)
		{


		  vectlevel[level]->noop_act[cel].w_is_goal++;
		  vectlevel[level]->noop_prec_act_vect[GUID_BLOCK (cel)] |=
		    GUID_MASK (cel);
		}

	      if (vectlevel[next_level]->fact[cel].w_is_goal
		  && vectlevel[level]->fact[cel].w_is_goal == 0)
		backward_precond_propagation (add_effect);

	      if (vectlevel[level]->
		  noop_prec_act_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		remove_treated_noop (CONVERT_NOOP_TO_INFORM (cel, level));

	      if (vectlevel[level]->fact[cel].w_is_true)
		forward_noop_propagation (cel, level);
	    }
	}

      for (j = 0; j < gef_conn[act_pos].num_D; j++)
	{
	  cel = gef_conn[act_pos].D[j];
	  if (cel < 0)
	    continue;
	  add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);
	  if (!is_fact_in_additive_effects_start (act_pos, cel))
	    {

	      vectlevel[level]->noop_act[cel].w_is_overall = 0;
	      if (vectlevel[next_level]->fact[cel].w_is_goal > 0
		  && vectlevel[level]->noop_act[cel].w_is_goal == 0
		  && vectlevel[level]->fact[cel].w_is_goal > 0)
		{


		  vectlevel[level]->noop_act[cel].w_is_goal++;
		  vectlevel[level]->noop_prec_act_vect[GUID_BLOCK (cel)] |=
		    GUID_MASK (cel);
		}

	      if (vectlevel[next_level]->fact[cel].w_is_goal
		  && vectlevel[level]->fact[cel].w_is_goal == 0)
		backward_precond_propagation (add_effect);

	      if (vectlevel[next_level]->
		  prec_vect[GUID_BLOCK (cel)] & GUID_MASK (cel))
		remove_treated_noop (CONVERT_NOOP_TO_INFORM (cel, level));


	      if (vectlevel[level]->noop_act[cel].w_is_used)
		{
		  vectlevel[next_level]->fact[cel].w_is_true++;



		  vectlevel[next_level]->fact_vect[GUID_BLOCK (cel)] |=
		    GUID_MASK (cel);
		  vectlevel[next_level]->false_crit_vect[GUID_BLOCK (cel)] &=
		    ~(GUID_MASK (cel));
		  if (vectlevel[next_level]->fact[cel].w_is_goal)
		    {
		      if (vectlevel[next_level]->fact[cel].w_is_true == 1)
			vectlevel[next_level]->
			  true_crit_vect[GUID_BLOCK (cel)] |= GUID_MASK (cel);

		      else if (vectlevel[next_level]->fact[cel].w_is_true > 1)
			vectlevel[next_level]->
			  true_crit_vect[GUID_BLOCK (cel)] &=
			  ~(GUID_MASK (cel));
		    }


		  if (vectlevel[next_level]->fact[cel].false_position >= 0)
		    remove_false_fact ((inform_list) &
				       vectlevel[next_level]->fact[cel]);


		  vectlevel[next_level]->num_fact++;
		  forward_noop_propagation (cel, next_level);
		}
	    }
	}
    }

  for (j = 0; j < gef_conn[act->position].num_A; j++)
    {
      cel = gef_conn[act->position].A[j];


      if (gef_conn[act_pos].sf
	  && is_fact_in_delete_effects_start (act_pos, cel))
	continue;
      if (cel < 0)
	continue;
      if (CHECK_FACT_POS (cel, next_level))
	{
	  add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);
	  if (--add_effect->w_is_true <= 1)
	    {
	      if (add_effect->w_is_true < 1)
		{
		  GpG.num_prec -= add_effect->w_is_goal;
		  vectlevel[next_level]->fact_vect[GUID_BLOCK (cel)] &=
		    ~(GUID_MASK (cel));
		  vectlevel[next_level]->num_fact--;
		  forward_noop_remotion (cel, next_level);
		  if (add_effect->w_is_goal)
		    {
		      if (add_effect->w_is_used)
			insert_unsup_fact (add_effect);
		      vectlevel[next_level]->
			false_crit_vect[GUID_BLOCK (cel)] |= GUID_MASK (cel);
		      vectlevel[next_level]->
			true_crit_vect[GUID_BLOCK (cel)] &=
			~(GUID_MASK (cel));
		    }
		}

	      else if (add_effect->w_is_goal)
		vectlevel[next_level]->true_crit_vect[GUID_BLOCK (cel)] |=
		  GUID_MASK (cel);
	    }
	}

      else
	printf ("\n Error fact position %s level %d",
		print_ft_name_string (cel, temp_name), next_level);
    }
  j = vectlevel[level]->action.position;
  vectlevel[level]->action.position = -1;


  check_prec_add_list (infAction);
  vectlevel[level]->action.being_removed = FALSE;
  remove_numeric_effects_of_action (act_pos, level);



  clean_unsup_num_fact ();

  if (GpG.temporal_plan)
    {
      vectlevel[level]->action.position = j;
      if (DEBUG4)
	printf ("\n\n -+- TEMPORAL -+-\n");
      update_time (infAction);

    }
  vectlevel[level]->action.position = -1;


  act->step = num_try;
  if ((GpG.accurate_cost == COMPUTE_MAX_COST
       && GpG.inc_choice_type >= MIN_COST_INC)
      || GpG.accurate_cost >= COMPUTE_DG_SUM_COST)
    reset_computed_dg_costs (level);

  if (DEBUG5 && num_try > 1)
    {
      print_num_levels_and_actions ();
      print_actions_in_subgraph ();
    }
  return (TRUE);
}


int
insert_remove_action (int act_pos, int act_level, int ins_rem,
		      int propagation)
{
  int i;
  ind_remove_act_chain = 0;

  if (ins_rem == C_T_REMOVE_ACTION)
    remove_action_from_vectlevel (act_pos, act_level, propagation);

  else
    insert_action_in_vectlevel (act_pos, act_level);

  if (DEBUG2 && ind_remove_act_chain > 0 && num_try > 1)
    printf ("\nxXx Remove action in precondition chain:");

  for (i = 0; i < ind_remove_act_chain; i++)
    {

      if (num_try > 1)
	remove_action_from_vectlevel (remove_act_chain[i]->position,
				      *remove_act_chain[i]->level, 1);

      remove_act_chain[i] = NULL;
    }


  if (DEBUG5)
    {
      printf ("\nUNSUP FACTS");
      print_unsup_fact_vect ();
      print_unsup_num_facts ();
    }

  return 0;
}






void
reset_plan (int max_time)
{
  inform_list infp;
  int i, time;
  max_time = GpG.max_plan_length;
  GpG.num_false_fa = 0;
  GpG.num_false_act = 0;


  for (i = 0; i < GpG.num_false_num_fa; i++)

    {
      vectlevel[*(unsup_num_fact[i]->level)]->numeric->
	false_position[unsup_num_fact[i]->fact] = -1;
    }
  GpG.num_false_num_fa = 0;
  GpG.num_false_tot = 0;
  GpG.num_prec = 0;
  GpG.num_m_e = 0;
  GpG.num_actions = 0;
  GpG.total_cost = GpG.total_time = 0.0;

  GpG.weight_cost = GpG.orig_weight_cost;
  GpG.weight_time = GpG.orig_weight_time;
  GpG.current_lm = 0;




  for (i = 0; i < gnum_comp_var; i++)
    {
      vectlevel[0]->numeric->values[i] = ginitial_state.V[i];
      vectlevel[0]->numeric->values_after_start[i] = ginitial_state.V[i];
    }



  for (time = 0; time < max_time; time++)

    {

      if (vectlevel[time] == NULL)
	continue;


      memset (vectlevel[time]->numeric->false_position, -1,
	      gnum_comp_var * sizeof (int));
      memset (vectlevel[time]->numeric->w_is_goal, 0,
	      gnum_comp_var * sizeof (short));


      if (time != 0)
	copy_num_values_from_level_to_level (0, time, FALSE);
      vectlevel[time]->level = time;
      vectlevel[time]->num_actions = 0;
      vectlevel[time]->modified = 0;

      reset_bitarray (vectlevel[time]->prec_vect, gnum_ft_block);
      vectlevel[time]->num_prec = 0;
      reset_bitarray (vectlevel[time]->fact_vect, gnum_ft_block);

      vectlevel[time]->num_fact = 0;
      reset_bitarray (vectlevel[time]->true_crit_vect, gnum_ft_block);
      vectlevel[time]->num_true_crit = 0;
      reset_bitarray (vectlevel[time]->false_crit_vect, gnum_ft_block);
      vectlevel[time]->num_false_crit = 0;

      reset_bitarray (vectlevel[time]->noop_act_vect, gnum_ft_block);
      reset_bitarray (vectlevel[time]->noop_prec_act_vect, gnum_ft_block);
      for (i = 0; i < GpG.max_num_facts; i++)

	{
	  infp = &vectlevel[time]->fact[i];
	  infp->w_is_goal = 0;
	  infp->w_is_used = 0;
	  infp->w_is_true = 0;
	  infp->false_position = -1;


	  if (!GpG.lm_multilevel)
	    {
	      CONVERT_FACT_TO_VERTEX (i)->lamda_prec = 1.0;
	      CONVERT_FACT_TO_VERTEX (i)->lamda_me = 1.0;
	      CONVERT_FACT_TO_VERTEX (i)->last_lm_me = 0;
	      CONVERT_FACT_TO_VERTEX (i)->last_lm_prec = 0;
	    }

	  infp->time_f = -2.0;
	  infp->action_f = NULL;
	  infp->ord_pos = 0;


	  infp = &vectlevel[time]->noop_act[i];
	  infp->w_is_goal = 0;
	  infp->w_is_used = 0;
	  infp->w_is_true = 0;
	  infp->w_is_overall = 0;
	  infp->false_position = -1;

	  infp->time_f = -2.0;
	  infp->action_f = NULL;
	  infp->ord_pos = 0;

	}

      infp = &vectlevel[time]->action;
      infp->step = -2000;

      infp->w_is_goal = 0;
      infp->w_is_used = 0;
      infp->w_is_true = 0;
      infp->false_position = -1;

      infp->position = -1;
      infp->being_removed = 0;
      free_noop_not_in (infp);
      vectlevel[time]->max_action_time = 0.0;

      infp->time_f = 0.0;
      infp->action_f = NULL;
      infp->ord_pos = 0;

      if ((GpG.accurate_cost == COMPUTE_MAX_COST
	   && GpG.inc_choice_type >= MIN_COST_INC)
	  || GpG.accurate_cost >= COMPUTE_DG_SUM_COST)
	reset_computed_dg_costs (time);


      if (GpG.lm_multilevel)
	for (i = 0; i < GpG.max_num_actions; i++)
	  {
	    vectlevel[time]->lambda_prec[i] = 1.0;
	    vectlevel[time]->lambda_me[i] = 1.0;
	    vectlevel[time]->flag_decr_lm = 0;
	  }

    }


  if (!GpG.lm_multilevel)
    for (i = 0; i < GpG.max_num_actions; i++)
      {
	CONVERT_ACTION_TO_VERTEX (i)->lamda_prec = 1.0;
	CONVERT_ACTION_TO_VERTEX (i)->lamda_me = 1.0;
	CONVERT_ACTION_TO_VERTEX (i)->last_lm_me = 0;
	CONVERT_ACTION_TO_VERTEX (i)->last_lm_prec = 0;
	CONVERT_ACTION_TO_VERTEX (i)->step = -2000;
	CONVERT_ACTION_TO_VERTEX (i)->flag_decr_lm = 0;;
      }

  for (i = 0; i < MAX_PLAN_LENGTH; i++)
    remove_act_chain[i] = NULL;
  reset_constraint_matrix ();
  reset_propagation_vect ();
}



void
iniz_random (int max_time)
{
  int i, time;
  for (time = 0; time < max_time; time++)
    for (i = 0; i < GpG.max_num_actions; i++)
      if (CHECK_ACTION_POS (i, time) && (MY_RANDOM % 200) == 0)

	{

	  insert_remove_action (i, time, C_T_INSERT_ACTION,
				GpG.approximation_level);
	}
}

int
initialize (State * start_state, State * end_state, int num_run)
{
  int time, i, num, num_act, num_false_act, num_unsupported, num_min,
    num_neg, choice, j, el;
  FtConn *vertex_ft;
  inform_list fa, inf_fact, infEl, inform_tofix;
  inform_list false_init_facts[MAX_FALSE];
  PlanAction *temp_act;
  if (DEBUG1)
    printf ("\n INITIALIZE: ");
  num_run++;
  time = GpG.curr_plan_length = GpG.fixpoint_plan_length;
  num_try = INITIALIZE_STEP;

  if (GpG.num_solutions && (num_run % 2))
    {
      time = GpG.input_plan_lenght;
      GpG.curr_plan_length = GpG.input_plan_lenght;
    }

  if (time + 1 < GpG.max_plan_length)
    for (i = time + 1; i < GpG.max_plan_length; i++)
      temp_vectlevel[GpG.max_temp_vect++] = vectlevel[i];

  if (time >= GpG.max_plan_length)
    for (i = GpG.max_plan_length; i <= time; i++)
      vectlevel[i] = temp_vectlevel[--GpG.max_temp_vect];


  GpG.max_plan_length = time + 1;
  reset_plan (GpG.max_plan_length);

  if (DEBUG5)
    {
      printf ("\n After Reset plan Lev %d", GpG.curr_plan_length);
      print_num_levels_and_actions ();
      if (GpG.temporal_plan)
	print_temporal_plan (GpG.curr_plan_length);
    }

  num_unsupported = 0;
  for (i = 0; i < end_state->num_F; i++)
    if (end_state->F[i] >= 0)
      {
	vertex_ft = &gft_conn[end_state->F[i]];
	CONVERT_FACT_TO_VERTEX (end_state->F[i])->lamda_prec =
	  CONVERT_FACT_TO_VERTEX (end_state->F[i])->lamda_me = 1.0;
	CONVERT_FACT_TO_INFORM (end_state->F[i], time)->w_is_goal = TRUE;
	CONVERT_FACT_TO_INFORM (end_state->F[i], time)->w_is_used = TRUE;

	insert_unsup_fact (CONVERT_FACT_TO_INFORM (end_state->F[i], time));
	false_init_facts[num_unsupported] =
	  CONVERT_FACT_TO_INFORM (end_state->F[i], time);
	num_unsupported++;
	vectlevel[time]->prec_vect[GUID_BLOCK (vertex_ft->position)] |=
	  GUID_MASK (vertex_ft->position);
	vectlevel[time]->false_crit_vect[GUID_BLOCK (vertex_ft->position)] |=
	  GUID_MASK (vertex_ft->position);


	backward_precond_propagation (CONVERT_FACT_TO_INFORM
				      (end_state->F[i], time));
	if (num_unsupported > MAX_GOALS)

	  {
	    printf ("\n\nipp-d: increase MAX_GOALS( preset value: %d )",
		    MAX_GOALS);
	    exit (1);
	  }
      }
    else
      {
	j = -end_state->F[i];
	vectlevel[time]->numeric->w_is_goal[j]++;

	if (!is_num_prec_satisfied (j, time))
	  insert_unsup_numeric_fact (j, time);

      }

  vectlevel[time]->num_prec = num_unsupported;
  GpG.num_prec = num_unsupported;
  if (GpG.temporal_plan)
    GpG.forward_time = 1;

  for (num = 0, i = 0; i < start_state->num_F; i++, num++)

    {
      vertex_ft = &gft_conn[start_state->F[i]];
      (fa = CONVERT_FACT_TO_INFORM (start_state->F[i], 0))->w_is_true = TRUE;
      vectlevel[0]->fact_vect[GUID_BLOCK (vertex_ft->position)] |=
	GUID_MASK (vertex_ft->position);
      if (fa->w_is_goal)

	{
	  vectlevel[0]->true_crit_vect[GUID_BLOCK (vertex_ft->position)] |=
	    (GUID_MASK (vertex_ft->position));
	  vectlevel[0]->false_crit_vect[GUID_BLOCK (vertex_ft->position)] &=
	    ~(GUID_MASK (vertex_ft->position));
	}


      forward_noop_propagation (start_state->F[i], 0);

      {
	vectlevel[0]->fact[start_state->F[i]].time_f = 0.0;
	forward_noop_propagation_time (&vectlevel[0]->
				       noop_act[start_state->F[i]]);
      }
    }


  if (DEBUG5)
    {
      printf ("\n After propagation Lev %d", GpG.curr_plan_length);
      print_num_levels_and_actions ();
      if (GpG.temporal_plan)
	print_temporal_plan (GpG.curr_plan_length);
    }

  vectlevel[0]->num_fact = num;
  num = 0;
  if (GpG.initialize == PLAN_ADAPT || (GpG.num_solutions && (num_run % 3)))
    {
      if (DEBUG0)
	if (num_run > 1)
	  printf ("using stored plan\n");
      if (DEBUG2)
	printf ("\n   ==> Insert action from stored plan in present plan\n ");
      GpG.initialize_from = PLAN_ADAPT;
      if (GpG.num_solutions)

	{
	  time = GpG.input_plan_lenght;
	}
      for (temp_act = GpG.gplan_actions, i = 0; temp_act;
	   temp_act = temp_act->next, i++)

	{

	  if (DEBUG2)
	    printf ("\nInitialize->insert action %s  in level %d",
		    print_op_name_string (temp_act->act_pos, temp_name), i);

	  GpG.num_false_tot =
	    (GpG.num_false_act + GpG.num_false_fa + GpG.num_false_num_fa);


	  if (i >= gef_conn[temp_act->act_pos].level)
	    insert_remove_action (temp_act->act_pos, i, C_T_INSERT_ACTION,
				  GpG.approximation_level);



	  GpG.num_false_tot =
	    (GpG.num_false_act + GpG.num_false_fa + GpG.num_false_num_fa);
	}
      while (GpG.num_false_act > 0)

	{
	  inform_tofix =
	    CONVERT_NOOP_TO_INFORM (treated_c_l[0]->fact,
				    *treated_c_l[0]->level);
	  num_false_act =
	    choose_actions_treated_fact (inform_tofix, GpG.curr_plan_length);
	  if (num_false_act <= 0)
	    remove_treated_noop (inform_tofix);
	}
      GpG.num_false_tot =
	(GpG.num_false_act + GpG.num_false_fa + GpG.num_false_num_fa);
    }

  else if (GpG.initialize == INIT_EMPTY_PLAN)

    {
      if (DEBUG0)
	if (num_run > 1)
	  printf ("using null plan\n");
      GpG.initialize_from = INIT_EMPTY_PLAN;
      return TRUE;
    }

  else

    {
      if (DEBUG1)
	printf ("\nInitialize random:  ");
      if (GpG.initialize == INIT_RANDOM)
	iniz_random (time);

      else
	while (num != num_unsupported)

	  {


	    inf_fact = false_init_facts[num];
	    num_act = choose_actions (inf_fact, -1);
	    if (num_act <= 0 || inf_fact->w_is_true)

	      {
		num++;
		continue;
	      }

	    if (GpG.initialize == INIT_GOAL)

	      {
		if (DEBUG1)
		  printf ("\nInitialize initial goal:  ");
		if (num_act == 1)
		  choice = 0;

		else
		  choice = MY_RANDOM % num_act;
		choice = pos_temp_vect[choice];
	      }

	    else

	      {


		printf ("\nWARNING: shouldnt get here!!!exiting..\n\n");
		exit (0);
		find_min (NULL, pos_temp_vect, num_act, &num_min, &num_neg);
		if (GpG.initialize == INIT_MIN_GOAL)
		  {
		    if (num_min == 1)
		      choice = 0;

		    else
		      choice = MY_RANDOM % num_min;
		    choice = pos_temp_vect[choice];
		  }

		else

		  {

		    choice = 0;
		    choice = pos_temp_vect[choice];
		  }
	      }


	    for (j = 0; j < gef_conn[neighb_vect[choice]->act_pos].num_PC;
		 j++)

	      {
		el = gef_conn[neighb_vect[choice]->act_pos].PC[j];
		if (el < 0)
		  continue;
		if (CHECK_ACTION_POS (el, neighb_vect[choice]->act_level))

		  {
		    infEl =
		      CONVERT_FACT_TO_INFORM (el,
					      neighb_vect[choice]->act_level);
		    if (infEl->w_is_true == 0)
		      false_init_facts[num_unsupported++] = infEl;
		  }
	      }
	    num++;
	    insert_remove_action (neighb_vect[choice]->act_pos,
				  neighb_vect[choice]->act_level,
				  neighb_vect[choice]->constraint_type,
				  GpG.approximation_level);
	  }
    }

  if (DEBUG2)
    {

      printf ("\n END INITIALIZE");
    }
  return (TRUE);
}








void
choose_random_act_time ()
{
  int ind, i, j, level = 0, posGoal, posPrecond =
    0, indlevel, indDelete, save_level, pos_act, numgoal = 0;
  int delete_action_level[MAX_PLAN_LENGTH];
  float num_act, sum, time, timeStart, timeGoal;
  inform_list infGoal, infAction;

  if (GpG.num_false_tot == 0)
    {

      if (GpG.init_tabu_length == 0)
	{

	  GpG.local = 2;
	  GpG.init_tabu_length = 20;
	}
    }

  memset (delete_action_level, -1, MAX_PLAN_LENGTH * sizeof (int));

  indDelete = 0;

  timeGoal = 0.0;

  for (i = 0; i < GpG.curr_goal_state->num_F; i++)
    {
      infGoal =
	CONVERT_FACT_TO_INFORM (GpG.curr_goal_state->F[i],
				GpG.curr_plan_length);

      if (infGoal->w_is_true)
	{

	  if (infGoal->time_f > timeGoal)
	    {
	      numgoal = 1;
	      timeGoal = infGoal->time_f;
	    }
	  else if (infGoal->time_f == timeGoal)
	    numgoal++;
	}
    }

  for (i = 0; i < GpG.curr_goal_state->num_F; i++)
    {
      infGoal =
	CONVERT_FACT_TO_INFORM (GpG.curr_goal_state->F[i],
				GpG.curr_plan_length);

      if (infGoal->w_is_true)
	{

	  if (infGoal->time_f == timeGoal)
	    {
	      posGoal = infGoal->position;

	      level = GpG.curr_plan_length;

	      time = timeGoal;

	      indlevel = level;

	      while (CHECK_FACT_POS (posGoal, indlevel) && indlevel > 0)
		{
		  if (vectlevel[indlevel]->fact[posGoal].action_f->
		      action_fact == IS_ACTION
		      || (vectlevel[indlevel]->fact[posGoal].action_f->
			  action_fact == IS_NOOP
			  &&
			  is_fact_in_additive_effects_start
			  (GET_ACTION_POSITION_OF_LEVEL (indlevel - 1),
			   posGoal)))
		    break;

		  indlevel--;
		}

	      level = indlevel - 1;

	      save_level = level;

	      do
		{
		  if (time == 0.0)
		    time = infGoal->time_f;

		  time = time / GpG.min_action_time;


		  if (time < 1.0)
		    time = 1.0;

		  num_act = MY_RANDOM % ((int) time);

		  num_act = num_act * GpG.min_action_time;

		  for (sum = 0.0; level >= 0;)
		    {
		      sum +=
			get_action_time (vectlevel[level]->action.position,
					 level);

		      if (sum >= num_act)
			{
			  delete_action_level[level] = level;
			  indDelete++;

			  time =
			    vectlevel[level]->action.time_f -
			    get_action_time (vectlevel[level]->action.
					     position, level);
			}

		      timeStart = 0.0;
		      indlevel = level;


		      for (j = 0;
			   j <
			   gef_conn[vectlevel[level]->action.position].num_PC;
			   j++)
			{
			  if (gef_conn[vectlevel[level]->action.position].
			      PC[j] < 0)
			    continue;

			  if (timeStart <=
			      vectlevel[level]->
			      fact[gef_conn
				   [vectlevel[level]->action.position].PC[j]].
			      time_f)
			    {

			      timeStart =
				vectlevel[level]->
				fact[gef_conn
				     [vectlevel[level]->action.position].
				     PC[j]].time_f;
			      posPrecond =
				gef_conn[vectlevel[level]->action.position].
				PC[j];
			    }
			}

		      if (gef_conn[vectlevel[level]->action.position].sf)
			{

			  for (j = 0;
			       j <
			       gef_conn[vectlevel[level]->action.position].
			       sf->num_PC_overall; j++)
			    {
			      if (gef_conn[vectlevel[level]->action.position].
				  sf->PC_overall[j] < 0)
				continue;

			      if (is_fact_in_additive_effects_start
				  (vectlevel[level]->action.position,
				   gef_conn[vectlevel[level]->action.
					    position].sf->PC_overall[j]))
				continue;

			      if (timeStart <=
				  vectlevel[level]->
				  fact[gef_conn
				       [vectlevel[level]->action.position].
				       sf->PC_overall[j]].time_f)
				{
				  timeStart =
				    vectlevel[level]->
				    fact[gef_conn
					 [vectlevel[level]->action.position].
					 sf->PC_overall[j]].time_f;
				  posPrecond =
				    gef_conn[vectlevel[level]->action.
					     position].sf->PC_overall[j];
				}
			    }


			  for (j = 0;
			       j <
			       gef_conn[vectlevel[level]->action.position].
			       sf->num_PC_end; j++)
			    {
			      if (gef_conn[vectlevel[level]->action.position].
				  sf->PC_end[j] < 0)
				continue;

			      if (is_fact_in_additive_effects_start
				  (vectlevel[level]->action.position,
				   gef_conn[vectlevel[level]->action.
					    position].sf->PC_end[j]))
				continue;

			      if (timeStart <=
				  (vectlevel[level]->
				   fact[gef_conn
					[vectlevel[level]->action.position].
					sf->PC_end[j]].time_f -
				   get_action_time (vectlevel[level]->action.
						    position, level)))
				{
				  timeStart =
				    vectlevel[level]->
				    fact[gef_conn
					 [vectlevel[level]->action.position].
					 sf->PC_end[j]].time_f -
				    get_action_time (vectlevel[level]->action.
						     position, level);
				  posPrecond =
				    gef_conn[vectlevel[level]->action.
					     position].sf->PC_end[j];
				}
			    }
			}

		      infAction =
			CONVERT_ACTION_TO_INFORM (vectlevel[level]->action.
						  position, level);

		      for (ind = 0; ind < num_act_ord; ind++)
			{
			  if (GpG.constraint_type == 0)
			    {
			      if (mat_ord[ind][infAction->ord_pos] != 0)
				{
				  if (timeStart <= act_ord_vect[ind]->time_f)
				    {
				      timeStart = act_ord_vect[ind]->time_f;
				      level = *act_ord_vect[ind]->level;
				    }
				}
			    }
			  else
			    {
			      if (mat_ord[ind][infAction->ord_pos] == EA_SB)
				{
				  if (timeStart <= act_ord_vect[ind]->time_f)
				    {
				      timeStart = act_ord_vect[ind]->time_f;
				      level = *act_ord_vect[ind]->level;
				    }
				}
			      else
				{
				  if (mat_ord[ind][infAction->ord_pos] ==
				      EA_EB)
				    {
				      if (timeStart <=
					  act_ord_vect[ind]->time_f -
					  get_action_time (infAction->
							   position, level))
					{
					  timeStart =
					    act_ord_vect[ind]->time_f -
					    get_action_time (infAction->
							     position, level);
					  level = *act_ord_vect[ind]->level;
					}
				    }
				  else
				    if (mat_ord[ind][infAction->ord_pos] ==
					SA_SB)
				    {
				      if (timeStart <=
					  act_ord_vect[ind]->time_f -
					  get_action_time (act_ord_vect[ind]->
							   position,
							   *act_ord_vect
							   [ind]->level))
					{
					  timeStart =
					    act_ord_vect[ind]->time_f -
					    get_action_time (act_ord_vect
							     [ind]->position,
							     *act_ord_vect
							     [ind]->level);
					  level = *act_ord_vect[ind]->level;
					}
				    }
				  else
				    {
				      if (mat_ord[ind][infAction->ord_pos] ==
					  SA_EB)
					{
					  if (timeStart <=
					      act_ord_vect[ind]->time_f -
					      get_action_time (act_ord_vect
							       [ind]->
							       position,
							       *act_ord_vect
							       [ind]->level) -
					      get_action_time (infAction->
							       position,
							       level))
					    {
					      timeStart =
						act_ord_vect[ind]->time_f -
						get_action_time (act_ord_vect
								 [ind]->
								 position,
								 *act_ord_vect
								 [ind]->
								 level) -
						get_action_time (infAction->
								 position,
								 level);
					      level =
						*act_ord_vect[ind]->level;
					    }
					}
				      else
					{
					  if (mat_ord[ind][infAction->ord_pos]
					      == EA_EB__SA_SB)
					    {
					      if (get_action_time
						  (infAction->position,
						   level) <=
						  get_action_time
						  (act_ord_vect[ind]->
						   position,
						   *act_ord_vect[ind]->level))
						{

						  if (timeStart <
						      act_ord_vect[ind]->
						      time_f -
						      get_action_time
						      (infAction->position,
						       level))
						    {
						      timeStart =
							act_ord_vect[ind]->
							time_f -
							get_action_time
							(infAction->position,
							 level);
						      level =
							*act_ord_vect[ind]->
							level;
						    }
						}
					      else
						{
						  if (timeStart <
						      act_ord_vect[ind]->
						      time_f -
						      get_action_time
						      (act_ord_vect[ind]->
						       position,
						       *act_ord_vect[ind]->
						       level))
						    {
						      timeStart =
							act_ord_vect[ind]->
							time_f -
							get_action_time
							(act_ord_vect[ind]->
							 position,
							 *act_ord_vect[ind]->
							 level);
						      level =
							*act_ord_vect[ind]->
							level;
						    }
						}
					    }

					}

				    }
				}


			    }
			}




		      if (indlevel == level)
			{
			  while (CHECK_FACT_POS (posPrecond, indlevel)
				 && indlevel > 0)
			    {
			      if (vectlevel[indlevel]->fact[posPrecond].
				  action_f->action_fact == IS_ACTION
				  || (vectlevel[indlevel]->fact[posPrecond].
				      action_f->action_fact == IS_NOOP
				      &&
				      is_fact_in_additive_effects_start
				      (GET_ACTION_POSITION_OF_LEVEL
				       (indlevel - 1), posPrecond)))
				break;

			      indlevel--;
			    }
			  level = indlevel - 1;
			}

		      if (level < 0)
			level = save_level;

		      if (sum >= num_act)
			break;

		    }
		}
	      while (indDelete < ((GpG.num_actions / 5) / numgoal));
	    }
	}
    }

  for (level = 0; level < GpG.curr_plan_length; level++)
    {
      if (delete_action_level[level] >= 0)
	continue;

      for (indlevel = level + 1; indlevel < GpG.curr_plan_length; indlevel++)
	{

	  if (GET_ACTION_POSITION_OF_LEVEL (level) > 0
	      && GET_ACTION_POSITION_OF_LEVEL (level) ==
	      GET_ACTION_POSITION_OF_LEVEL (indlevel))
	    {
	      delete_action_level[indlevel] = indlevel;
	      delete_action_level[level] = level;
	      break;
	    }

	}
    }


  for (i = 0; i < GpG.curr_plan_length; i++)
    {
      if (delete_action_level[i] < 0)
	continue;

      pos_act = vectlevel[delete_action_level[i]]->action.position;
      if (pos_act < 0)
	continue;


      for (j = 0; j < gef_conn[pos_act].num_A; j++)
	{

	  if (gef_conn[pos_act].A[j] < 0)
	    continue;

	  indlevel = delete_action_level[i] + 1;
	  while (indlevel < GpG.curr_plan_length)
	    {

	      if (GET_ACTION_OF_LEVEL (indlevel))
		{
		  if ((is_fact_in_preconditions
		       (vectlevel[indlevel]->action.position,
			gef_conn[pos_act].A[j])
		       ||
		       (is_fact_in_preconditions_overall
			(vectlevel[indlevel]->action.position,
			 gef_conn[pos_act].A[j])
			&&
			!is_fact_in_additive_effects_start (vectlevel
							    [indlevel]->
							    action.position,
							    gef_conn[pos_act].
							    A[j])))
		      && vectlevel[indlevel]->action.position > 0)
		    insert_remove_action (vectlevel[indlevel]->action.
					  position, indlevel,
					  C_T_REMOVE_ACTION,
					  GpG.approximation_level);
		}

	      if (!vectlevel[indlevel]->noop_act[gef_conn[pos_act].A[j]].
		  w_is_used)
		break;

	      indlevel++;
	    }

	}


      if (gef_conn[pos_act].sf)
	for (j = 0; j < gef_conn[pos_act].sf->num_A_start; j++)
	  {
	    if (gef_conn[pos_act].sf->A_start[j] < 0)
	      continue;

	    indlevel = delete_action_level[i] + 1;
	    while (indlevel < GpG.curr_plan_length)
	      {

		if (GET_ACTION_OF_LEVEL (indlevel))
		  {
		    if ((is_fact_in_preconditions
			 (vectlevel[indlevel]->action.position,
			  gef_conn[pos_act].sf->A_start[j])
			 ||
			 (is_fact_in_preconditions_overall
			  (vectlevel[indlevel]->action.position,
			   gef_conn[pos_act].sf->A_start[j])
			  &&
			  !is_fact_in_additive_effects_start (vectlevel
							      [indlevel]->
							      action.position,
							      gef_conn
							      [pos_act].sf->
							      A_start[j])))
			&& vectlevel[indlevel]->action.position > 0)
		      insert_remove_action (vectlevel[indlevel]->action.
					    position, indlevel,
					    C_T_REMOVE_ACTION,
					    GpG.approximation_level);
		  }

		if (!vectlevel[indlevel]->
		    noop_act[gef_conn[pos_act].sf->A_start[j]].w_is_used)
		  break;

		indlevel++;
	      }

	  }



      for (j = 0; j < gef_conn[pos_act].num_PC; j++)
	{
	  if (gef_conn[pos_act].PC[j] < 0)
	    continue;

	  indlevel = delete_action_level[i];
	  while (indlevel > 0)
	    {

	      if (CHECK_FACT_POS (gef_conn[pos_act].PC[j], indlevel) == FALSE)
		break;

	      if (GET_ACTION_OF_LEVEL (indlevel - 1)
		  &&
		  (is_fact_in_additive_effects
		   (vectlevel[indlevel - 1]->action.position,
		    gef_conn[pos_act].PC[j])
		   ||
		   is_fact_in_additive_effects_start (vectlevel
						      [indlevel -
						       1]->action.position,
						      gef_conn[pos_act].
						      PC[j]))
		  && vectlevel[indlevel - 1]->action.position > 0)
		insert_remove_action (vectlevel[indlevel - 1]->action.
				      position, indlevel - 1,
				      C_T_REMOVE_ACTION,
				      GpG.approximation_level);


	      if (CHECK_NOOP_POS (gef_conn[pos_act].PC[j], indlevel - 1) ==
		  FALSE)
		break;

	      if (!vectlevel[indlevel - 1]->noop_act[gef_conn[pos_act].PC[j]].
		  w_is_used)
		break;

	      indlevel--;
	    }
	}


      if (gef_conn[pos_act].sf)
	for (j = 0; j < gef_conn[pos_act].sf->num_PC_overall; j++)
	  {

	    if (gef_conn[pos_act].sf->PC_overall[j] < 0)
	      continue;

	    indlevel = delete_action_level[i];
	    while (indlevel > 0)
	      {

		if (CHECK_FACT_POS
		    (gef_conn[pos_act].sf->PC_overall[j], indlevel) == FALSE)
		  break;

		if (GET_ACTION_OF_LEVEL (indlevel - 1)
		    &&
		    (is_fact_in_additive_effects
		     (vectlevel[indlevel - 1]->action.position,
		      gef_conn[pos_act].sf->PC_overall[j])
		     ||
		     is_fact_in_additive_effects_start (vectlevel
							[indlevel -
							 1]->action.position,
							gef_conn[pos_act].sf->
							PC_overall[j]))
		    && vectlevel[indlevel - 1]->action.position > 0)
		  insert_remove_action (vectlevel[indlevel - 1]->action.
					position, indlevel - 1,
					C_T_REMOVE_ACTION,
					GpG.approximation_level);


		if (CHECK_NOOP_POS
		    (gef_conn[pos_act].sf->PC_overall[j],
		     indlevel - 1) == FALSE)
		  break;

		if (!vectlevel[indlevel - 1]->
		    noop_act[gef_conn[pos_act].sf->PC_overall[j]].w_is_used)
		  break;

		indlevel--;
	      }
	  }

      if (vectlevel[delete_action_level[i]]->action.position > 0)

	insert_remove_action (pos_act, delete_action_level[i],
			      C_T_REMOVE_ACTION, GpG.approximation_level);

    }

  return;

}




void
choose_random_act_cost ()
{
  int i, j, level = 0, pos = -1, num_removed =
    0, indlevel, pos_act, delete_action_level[MAX_PLAN_LENGTH], num_act;
  float sum, num;

  memset (delete_action_level, -1, MAX_PLAN_LENGTH * sizeof (int));

  if (GpG.num_false_tot == 0)
    {

      if (GpG.init_tabu_length == 0)
	{

	  GpG.local = 2;
	  GpG.init_tabu_length = 20;
	}
    }

  do
    {

      num_act = MY_RANDOM % ((int) ceil (GpG.best_cost));

      for (sum = 0.0, num = 0, level = 0;
	   level < GpG.curr_plan_length && sum <= num_act; level++)
	if (vectlevel[level]->num_actions)
	  {

	    pos_act = vectlevel[level]->action.position;
	    sum += get_action_cost (pos_act);

	    if (sum >= num_act)
	      {

		pos = vectlevel[level]->action.position;


		if (vectlevel[level] == NULL || pos == -1
		    || vectlevel[level]->action.w_is_used == 0)
		  break;
		else
		  {

		    if (DEBUG6)
		      {

			printf
			  ("\n__________ RANDOM CHOICE %s level %d pos %d cost %.2f time %.2f",
			   print_op_name_string (pos, temp_name), level, pos,
			   get_action_cost (pos), get_action_time (pos,
								   level));
		      }

		    if (vectlevel[level]->action.position >= 0)
		      {
			delete_action_level[level] = level;

			num_removed++;
		      }
		  }
	      }

	  }
    }
  while (num_removed < 3);

  for (level = 0; level < GpG.curr_plan_length; level++)
    {
      if (delete_action_level[level] >= 0)
	continue;

      for (indlevel = level + 1; indlevel < GpG.curr_plan_length; indlevel++)
	{

	  if (GET_ACTION_POSITION_OF_LEVEL (level) > 0
	      && GET_ACTION_POSITION_OF_LEVEL (level) ==
	      GET_ACTION_POSITION_OF_LEVEL (indlevel))
	    {
	      delete_action_level[indlevel] = indlevel;
	      delete_action_level[level] = level;
	      break;
	    }

	}
    }


  for (i = 0; i < GpG.curr_plan_length; i++)
    {
      if (delete_action_level[i] < 0)
	continue;

      pos_act = vectlevel[delete_action_level[i]]->action.position;
      if (pos_act < 0)
	continue;


      for (j = 0; j < gef_conn[pos_act].num_A; j++)
	{

	  if (gef_conn[pos_act].A[j] < 0)
	    continue;

	  indlevel = delete_action_level[i] + 1;
	  while (indlevel < GpG.curr_plan_length)
	    {

	      if (GET_ACTION_OF_LEVEL (indlevel))
		{
		  if ((is_fact_in_preconditions
		       (vectlevel[indlevel]->action.position,
			gef_conn[pos_act].A[j])
		       ||
		       (is_fact_in_preconditions_overall
			(vectlevel[indlevel]->action.position,
			 gef_conn[pos_act].A[j])
			&&
			!is_fact_in_additive_effects_start (vectlevel
							    [indlevel]->
							    action.position,
							    gef_conn[pos_act].
							    A[j])))
		      && vectlevel[indlevel]->action.position > 0)
		    insert_remove_action (vectlevel[indlevel]->action.
					  position, indlevel,
					  C_T_REMOVE_ACTION,
					  GpG.approximation_level);

		}

	      if (!vectlevel[indlevel]->noop_act[gef_conn[pos_act].A[j]].
		  w_is_used)
		break;

	      indlevel++;
	    }
	}


      if (gef_conn[pos_act].sf)
	for (j = 0; j < gef_conn[pos_act].sf->num_A_start; j++)
	  {
	    if (gef_conn[pos_act].sf->A_start[j] < 0)
	      continue;

	    indlevel = delete_action_level[i] + 1;
	    while (indlevel < GpG.curr_plan_length)
	      {

		if (GET_ACTION_OF_LEVEL (indlevel))
		  {
		    if ((is_fact_in_preconditions
			 (vectlevel[indlevel]->action.position,
			  gef_conn[pos_act].sf->A_start[j])
			 ||
			 (is_fact_in_preconditions_overall
			  (vectlevel[indlevel]->action.position,
			   gef_conn[pos_act].sf->A_start[j])
			  &&
			  !is_fact_in_additive_effects_start (vectlevel
							      [indlevel]->
							      action.position,
							      gef_conn
							      [pos_act].sf->
							      A_start[j])))
			&& vectlevel[indlevel]->action.position > 0)
		      insert_remove_action (vectlevel[indlevel]->action.
					    position, indlevel,
					    C_T_REMOVE_ACTION,
					    GpG.approximation_level);

		  }

		if (!vectlevel[indlevel]->
		    noop_act[gef_conn[pos_act].sf->A_start[j]].w_is_used)
		  break;
		indlevel++;
	      }
	  }



      for (j = 0; j < gef_conn[pos_act].num_PC; j++)
	{

	  if (gef_conn[pos_act].PC[j] < 0)
	    continue;

	  indlevel = delete_action_level[i];
	  while (indlevel > 0)
	    {

	      if (CHECK_FACT_POS (gef_conn[pos_act].PC[j], indlevel) == FALSE)
		break;

	      if (GET_ACTION_OF_LEVEL (indlevel - 1)
		  &&
		  (is_fact_in_additive_effects
		   (vectlevel[indlevel - 1]->action.position,
		    gef_conn[pos_act].PC[j])
		   ||
		   is_fact_in_additive_effects_start (vectlevel
						      [indlevel -
						       1]->action.position,
						      gef_conn[pos_act].
						      PC[j]))
		  && vectlevel[indlevel - 1]->action.position > 0)
		insert_remove_action (vectlevel[indlevel - 1]->action.
				      position, indlevel - 1,
				      C_T_REMOVE_ACTION,
				      GpG.approximation_level);

	      if (CHECK_NOOP_POS (gef_conn[pos_act].PC[j], indlevel - 1) ==
		  FALSE)
		break;

	      if (!vectlevel[indlevel - 1]->noop_act[gef_conn[pos_act].PC[j]].
		  w_is_used)
		break;

	      indlevel--;
	    }
	}


      if (gef_conn[pos_act].sf)
	for (j = 0; j < gef_conn[pos_act].sf->num_PC_overall; j++)
	  {

	    if (gef_conn[pos_act].sf->PC_overall[j] < 0)
	      continue;

	    indlevel = delete_action_level[i];

	    while (indlevel > 0)
	      {

		if (CHECK_FACT_POS
		    (gef_conn[pos_act].sf->PC_overall[j], indlevel) == FALSE)
		  break;

		if (GET_ACTION_OF_LEVEL (indlevel - 1)
		    &&
		    (is_fact_in_additive_effects
		     (vectlevel[indlevel - 1]->action.position,
		      gef_conn[pos_act].sf->PC_overall[j])
		     ||
		     is_fact_in_additive_effects_start (vectlevel
							[indlevel -
							 1]->action.position,
							gef_conn[pos_act].sf->
							PC_overall[j]))
		    && vectlevel[indlevel - 1]->action.position > 0)
		  insert_remove_action (vectlevel[indlevel - 1]->action.
					position, indlevel - 1,
					C_T_REMOVE_ACTION,
					GpG.approximation_level);

		if (CHECK_NOOP_POS
		    (gef_conn[pos_act].sf->PC_overall[j],
		     indlevel - 1) == FALSE)
		  break;

		if (!vectlevel[indlevel - 1]->
		    noop_act[gef_conn[pos_act].sf->PC_overall[j]].w_is_used)
		  break;

		indlevel--;
	      }
	  }

      if (vectlevel[delete_action_level[i]]->action.position > 0)

	insert_remove_action (pos_act, delete_action_level[i],
			      C_T_REMOVE_ACTION, GpG.approximation_level);
    }


  return;

}

void
choose_maximizing_actions ()
{
  static action_set neighbors;

  int i, choice;
  static Bool initialized = FALSE;
  int num_ins = 0;
  int inserted[MAX_NUM_ACTIONS / 32 + 1];

  if (goptimization_exp == -1)
    return;


  memset (inserted, 0, (MAX_NUM_ACTIONS / 32 + 1) * sizeof (int));
  if (!initialized)
    {
      initialized = TRUE;

      neighbors.num_A = 0;
      memset (neighbors.A, 0, MAX_NUM_ACTIONS * sizeof (int));
      create_neighborhood_for_compvar (goptimization_exp, 1, 0, &neighbors, 1,
				       -1);
    }
  if (neighbors.num_A == 0)
    {
      printf ("\nWarning: there are no maximizing actions.\n\n");
      return;
    }


  if (neighbors.num_A < 10)
    {
      num_ins = neighbors.num_A;
    }
  else if ((neighbors.num_A >= 10) && (neighbors.num_A < 30))
    {
      num_ins = 10;
    }
  else if ((neighbors.num_A >= 30) && (neighbors.num_A < 90))
    {
      num_ins = neighbors.num_A / 3;
    }
  else if (neighbors.num_A >= 90)
    {
      num_ins = 30;
    }

  if (DEBUG1)
    printf
      ("\n\n----- Start inizialization: Insert Action for maximize plan quality");

  for (i = 0; i < num_ins; i++)
    {
      choice = MY_RANDOM % neighbors.num_A;
      insert_remove_action (neighbors.A[choice], GpG.curr_plan_length,
			    C_T_INSERT_ACTION, GpG.approximation_level);
    }

  if (DEBUG1)
    printf ("\n\n----- End Inizialization -----");


}


void
choose_random_act ()
{

  if (DEBUG2)
    printf
      ("\n\n^^^^CHOOSE_RANDOM_ACT: Remove some action to make inconsitence");

  if (DEBUG3)
    print_actions_in_subgraph ();


  if (GpG.maximize_plan)
    choose_maximizing_actions ();
  else
    {
      if (GpG.weight_cost <= GpG.weight_time)
	choose_random_act_time ();
      else
	choose_random_act_cost ();
    }

  if (DEBUG3)
    print_actions_in_subgraph ();

  if (DEBUG2)
    printf ("\n^^^^END CHOOSE_RANDOM_ACT\n");

}
