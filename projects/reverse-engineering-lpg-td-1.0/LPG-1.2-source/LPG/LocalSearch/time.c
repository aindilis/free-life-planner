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
 * File: time.c
 * Description: Time manager for the LPG planner.
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/




#include "lpg.h"
#include "LpgTime.h"
#include "LpgOutput.h"
#include "utilities.h"








void
reset_constraint_matrix ()
{

  int i;

  num_act_ord = 0;


  for (i = 0; i < MAX_NUM_ACTIONS; i++)
    {
      act_ord_vect[i] = NULL;
    }

  memset (mat_ord[0], 0, MAX_NUM_ACTIONS * MAX_NUM_ACTIONS * sizeof (char));

}



void
reset_propagation_vect ()
{
  memset (prop_level_index, -1, MAX_PLAN_LENGTH * sizeof (short int));
}







int
insert_propagation_list (inform_list infAction)
{

  if (*infAction->level >= MAX_PLAN_LENGTH)
    {
      exit (1);
    }

  if (prop_level_index[*infAction->level] == -1)
    prop_level_index[*infAction->level] = *infAction->level;
  else
    return FALSE;

  return TRUE;

}


float
get_fact_time (int pos)
{
  return (Hvar.dg_facts_min_duration[pos]);
}


int
propagation_time (inform_list infAction)
{

  inform_list infEl;
  register inform_list add_effect = NULL, infNoop;
  register int el, cel = 0, j;
  float max_time, savetime;
  int level, next_level, ind;
  EfConn *act;


  act = CONVERT_ACTION_TO_VERTEX (infAction->position);

  level = *infAction->level;
  next_level = level + 1;


  max_time = 0.0;

  for (j = 0; j < gef_conn[infAction->position].num_PC; j++)
    {
      el = gef_conn[infAction->position].PC[j];

      if (el < 0)
	continue;

      infEl = CONVERT_FACT_TO_INFORM (el, level);
      if (max_time < infEl->time_f)
	max_time = infEl->time_f;
    }

  if (gef_conn[infAction->position].sf != NULL)
    {
      for (j = 0; j < gef_conn[infAction->position].sf->num_PC_overall; j++)
	{
	  el = gef_conn[infAction->position].sf->PC_overall[j];

	  if (el < 0)
	    continue;

	  if (is_fact_in_additive_effects_start (infAction->position, el))
	    continue;

	  infEl = CONVERT_FACT_TO_INFORM (el, level);
	  if (max_time < infEl->time_f)
	    max_time = infEl->time_f;
	}

      for (j = 0; j < gef_conn[infAction->position].sf->num_PC_end; j++)
	{
	  el = gef_conn[infAction->position].sf->PC_end[j];

	  if (el < 0)
	    continue;

	  if (is_fact_in_additive_effects_start (infAction->position, el))
	    continue;

	  infEl = CONVERT_FACT_TO_INFORM (el, level);

	  if (max_time <
	      infEl->time_f - get_action_time (infAction->position, level))
	    max_time =
	      infEl->time_f - get_action_time (infAction->position, level);
	}

    }


  if (infAction->action_fact == IS_ACTION)
    for (ind = 0; ind < num_act_ord; ind++)
      {
	if (GpG.constraint_type == 0)
	  {
	    if (mat_ord[ind][infAction->ord_pos] == 1)
	      if (max_time < act_ord_vect[ind]->time_f)
		max_time = act_ord_vect[ind]->time_f;
	  }
	else
	  {
	    if (mat_ord[ind][infAction->ord_pos] == EA_SB)
	      {
		if (max_time < act_ord_vect[ind]->time_f)
		  max_time = act_ord_vect[ind]->time_f;
	      }
	    else
	      {
		if (mat_ord[ind][infAction->ord_pos] == EA_EB)
		  {
		    if (max_time <
			act_ord_vect[ind]->time_f -
			get_action_time (infAction->position, level))
		      max_time =
			act_ord_vect[ind]->time_f -
			get_action_time (infAction->position, level);
		  }
		else if (mat_ord[ind][infAction->ord_pos] == SA_SB)
		  {
		    if (max_time <
			act_ord_vect[ind]->time_f -
			get_action_time (act_ord_vect[ind]->position,
					 *act_ord_vect[ind]->level))
		      max_time =
			act_ord_vect[ind]->time_f -
			get_action_time (act_ord_vect[ind]->position,
					 *act_ord_vect[ind]->level);
		  }
		else
		  {
		    if (mat_ord[ind][infAction->ord_pos] == SA_EB)
		      {
			if (max_time <
			    act_ord_vect[ind]->time_f -
			    get_action_time (act_ord_vect[ind]->position,
					     *act_ord_vect[ind]->level) -
			    get_action_time (infAction->position, level))
			  max_time =
			    act_ord_vect[ind]->time_f -
			    get_action_time (act_ord_vect[ind]->position,
					     *act_ord_vect[ind]->level) -
			    get_action_time (infAction->position, level);
		      }
		    else
		      {
			if (mat_ord[ind][infAction->ord_pos] == EA_EB__SA_SB)
			  {
			    if (get_action_time (infAction->position, level) <
				get_action_time (act_ord_vect[ind]->position,
						 *act_ord_vect[ind]->level))
			      {

				if (max_time <
				    act_ord_vect[ind]->time_f -
				    get_action_time (infAction->position,
						     level))
				  max_time =
				    act_ord_vect[ind]->time_f -
				    get_action_time (infAction->position,
						     level);
			      }
			    else
			      {
				if (max_time <
				    act_ord_vect[ind]->time_f -
				    get_action_time (act_ord_vect[ind]->
						     position,
						     *act_ord_vect[ind]->
						     level))
				  max_time =
				    act_ord_vect[ind]->time_f -
				    get_action_time (act_ord_vect[ind]->
						     position,
						     *act_ord_vect[ind]->
						     level);
			      }
			  }

		      }

		  }
	      }

	  }
      }



  if (max_time < 0)
    max_time = 0.0;

  if (infAction->time_f ==
      max_time + get_action_time (infAction->position, level))
    return (0);
  else
    infAction->time_f =
      max_time + get_action_time (infAction->position, level);

  if (DEBUG4)
    printf
      ("\n  --Propagation Act: %s, level %d\n      start_time %.2f, end_time %.2f",
       print_op_name_string (infAction->position, temp_name), level, max_time,
       infAction->time_f);

  for (j = 0; j < gef_conn[infAction->position].num_A; j++)
    {
      cel = gef_conn[infAction->position].A[j];

      if (cel < 0)
	continue;

      add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);

      if (add_effect->time_f > infAction->time_f)
	{
	  add_effect->time_f = infAction->time_f;
	  add_effect->action_f = infAction;

	  if (GET_ACTION_OF_LEVEL (next_level)->w_is_used &&
	      (is_fact_in_preconditions
	       (vectlevel[next_level]->action.position, add_effect->position)
	       ||
	       (is_fact_in_preconditions_overall
		(vectlevel[next_level]->action.position, add_effect->position)
		&& !is_fact_in_additive_effects_start (vectlevel[next_level]->
						       action.position,
						       add_effect->
						       position))))
	    insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

	  if (CONVERT_NOOP_TO_INFORM (cel, next_level)->w_is_used)
	    forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
					   (cel, next_level));

	}

      else if (add_effect->action_f == infAction)
	{
	  savetime = add_effect->time_f;

	  add_effect->time_f = infAction->time_f;

	  if (CONVERT_NOOP_TO_INFORM (add_effect->position, level)->w_is_used)

	    if (add_effect->time_f >
		CONVERT_NOOP_TO_INFORM (add_effect->position, level)->time_f)
	      {
		add_effect->time_f =
		  CONVERT_NOOP_TO_INFORM (add_effect->position,
					  level)->time_f;
		add_effect->action_f =
		  CONVERT_NOOP_TO_INFORM (add_effect->position, level);
	      }

	  if (savetime != add_effect->time_f
	      || savetime == Hvar.dg_facts_min_duration[add_effect->position])
	    {

	      if (DEBUG4)
		{
		  printf
		    ("\n\t-Propagation end_effect: %s, level %d, time %.2f ",
		     print_ft_name_string (cel, temp_name), next_level,
		     add_effect->time_f);
		  printf ("\n\t  Act: %s",
			  print_op_name_string (act->position, temp_name));
		}

	      if (savetime != add_effect->time_f
		  && GET_ACTION_OF_LEVEL (next_level)->w_is_used
		  &&
		  (is_fact_in_preconditions
		   (vectlevel[next_level]->action.position,
		    add_effect->position)
		   ||
		   (is_fact_in_preconditions_overall
		    (vectlevel[next_level]->action.position,
		     add_effect->position)
		    &&
		    !is_fact_in_additive_effects_start (vectlevel
							[next_level]->action.
							position,
							add_effect->
							position))))
		insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

	      if (CONVERT_NOOP_TO_INFORM (add_effect->position, next_level)->
		  w_is_used)
		forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
					       (add_effect->position,
						next_level));
	    }
	}
    }


  if (gef_conn[infAction->position].sf != NULL)

    for (j = 0; j < gef_conn[infAction->position].sf->num_A_start; j++)
      {
	cel = gef_conn[infAction->position].sf->A_start[j];

	if (cel < 0)
	  continue;

	add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);
	infNoop = CONVERT_NOOP_TO_INFORM (cel, level);

	infNoop->time_f =
	  infAction->time_f - get_action_time (infAction->position, level);

	if (CONVERT_FACT_TO_INFORM (cel, level)->w_is_true > 0)
	  if (CONVERT_FACT_TO_INFORM (cel, level)->time_f < infNoop->time_f)
	    infNoop->time_f = CONVERT_FACT_TO_INFORM (cel, level)->time_f;

	if (!is_fact_in_delete_effects (infAction->position, cel))
	  {

	    if (add_effect->time_f > infNoop->time_f)
	      {
		add_effect->time_f = infNoop->time_f;
		add_effect->action_f = infNoop;

		if (GET_ACTION_OF_LEVEL (next_level)->w_is_used &&
		    (is_fact_in_preconditions
		     (vectlevel[next_level]->action.position,
		      add_effect->position)
		     ||
		     (is_fact_in_preconditions_overall
		      (vectlevel[next_level]->action.position,
		       add_effect->position)
		      &&
		      !is_fact_in_additive_effects_start (vectlevel
							  [next_level]->
							  action.position,
							  add_effect->
							  position))))
		  insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

		if (CONVERT_NOOP_TO_INFORM (cel, next_level)->w_is_used)
		  forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
						 (cel, next_level));

	      }

	    else if (add_effect->action_f == infNoop)
	      {
		savetime = add_effect->time_f;
		add_effect->time_f = infNoop->time_f;

		if (savetime != add_effect->time_f
		    || savetime ==
		    Hvar.dg_facts_min_duration[add_effect->position])
		  {

		    if (DEBUG4)
		      {
			printf
			  ("\n\t-Propagation Start_eff: %s, level %d, time %.2f ",
			   print_ft_name_string (cel, temp_name), next_level,
			   add_effect->time_f);
			printf ("\n\t  Act: %s",
				print_op_name_string (act->position,
						      temp_name));
		      }
		    if (savetime != add_effect->time_f
			&& GET_ACTION_OF_LEVEL (next_level)->w_is_used
			&&
			(is_fact_in_preconditions
			 (vectlevel[next_level]->action.position,
			  add_effect->position)
			 ||
			 (is_fact_in_preconditions_overall
			  (vectlevel[next_level]->action.position,
			   add_effect->position)
			  &&
			  !is_fact_in_additive_effects_start (vectlevel
							      [next_level]->
							      action.position,
							      add_effect->
							      position))))
		      insert_propagation_list (GET_ACTION_OF_LEVEL
					       (next_level));

		    if (CONVERT_NOOP_TO_INFORM
			(add_effect->position, next_level)->w_is_used)
		      forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
						     (add_effect->position,
						      next_level));
		  }
	      }
	  }
      }


  if (infAction->action_fact == IS_ACTION)
    {
      for (ind = 0; ind < num_act_ord; ind++)
	if (GpG.constraint_type == 0)
	  {
	    if (mat_ord[infAction->ord_pos][ind] == 1)
	      insert_propagation_list (act_ord_vect[ind]);
	  }
	else
	  {
	    if (mat_ord[infAction->ord_pos][ind] != 0)
	      insert_propagation_list (act_ord_vect[ind]);
	  }
    }
  return (1);

}





void
time_adj (inform_list infAction)
{

  int level, ind;

  level = *infAction->level;

  for (ind = level + 1; ind < GpG.curr_plan_length; ind++)
    {
      if (prop_level_index[ind] == -1)
	continue;

      propagation_time (GET_ACTION_OF_LEVEL (ind));
      prop_level_index[ind] = -1;

    }

}


void
insert_time (inform_list infAction)
{

  inform_list infEl, add_effect, infNoop;
  int el, cel, j;
  float max_time;
  int level, next_level, ind;

  level = *infAction->level;
  next_level = level + 1;


  if (infAction->action_fact == IS_ACTION && GpG.temporal_plan)
    temporal_constraints (infAction);

  max_time = 0.0;

  for (j = 0; j < gef_conn[infAction->position].num_PC; j++)
    {
      el = gef_conn[infAction->position].PC[j];

      if (el < 0)
	continue;

      infEl = CONVERT_FACT_TO_INFORM (el, level);

      if (infEl->w_is_true < 1)
	infEl->time_f = get_fact_time (el);

      if (max_time < infEl->time_f)
	max_time = infEl->time_f;
    }

  if (gef_conn[infAction->position].sf != NULL)
    {
      for (j = 0; j < gef_conn[infAction->position].sf->num_PC_overall; j++)
	{
	  el = gef_conn[infAction->position].sf->PC_overall[j];

	  if (el < 0)
	    continue;
	  if (is_fact_in_additive_effects_start (infAction->position, el))
	    continue;

	  infEl = CONVERT_FACT_TO_INFORM (el, level);

	  if (infEl->w_is_true < 1)
	    infEl->time_f = get_fact_time (el);

	  if (max_time < infEl->time_f)
	    max_time = infEl->time_f;
	}

      for (j = 0; j < gef_conn[infAction->position].sf->num_PC_end; j++)
	{
	  el = gef_conn[infAction->position].sf->PC_end[j];

	  if (el < 0)
	    continue;

	  if (is_fact_in_additive_effects_start (infAction->position, el))
	    continue;

	  infEl = CONVERT_FACT_TO_INFORM (el, level);

	  if (infEl->w_is_true < 1)
	    infEl->time_f = get_fact_time (el);

	  if (max_time <
	      infEl->time_f - get_action_time (infAction->position, level))
	    max_time =
	      infEl->time_f - get_action_time (infAction->position, level);
	}


    }

  if (infAction->action_fact == IS_ACTION)

    for (ind = 0; ind < num_act_ord; ind++)
      {
	if (GpG.constraint_type == 0)
	  {
	    if (mat_ord[ind][infAction->ord_pos] == 1)
	      if (max_time < act_ord_vect[ind]->time_f)
		max_time = act_ord_vect[ind]->time_f;
	  }
	else
	  {
	    if (mat_ord[ind][infAction->ord_pos] == EA_SB)
	      {
		if (max_time < act_ord_vect[ind]->time_f)
		  max_time = act_ord_vect[ind]->time_f;
	      }
	    else
	      {
		if (mat_ord[ind][infAction->ord_pos] == EA_EB)
		  {
		    if (max_time <
			act_ord_vect[ind]->time_f -
			get_action_time (infAction->position, level))
		      max_time =
			act_ord_vect[ind]->time_f -
			get_action_time (infAction->position, level);
		  }
		else if (mat_ord[ind][infAction->ord_pos] == SA_SB)
		  {
		    if (max_time <
			act_ord_vect[ind]->time_f -
			get_action_time (act_ord_vect[ind]->position,
					 *act_ord_vect[ind]->level))
		      max_time =
			act_ord_vect[ind]->time_f -
			get_action_time (act_ord_vect[ind]->position,
					 *act_ord_vect[ind]->level);
		  }
		else
		  {
		    if (mat_ord[ind][infAction->ord_pos] == SA_EB)
		      {
			if (max_time <
			    act_ord_vect[ind]->time_f -
			    get_action_time (act_ord_vect[ind]->position,
					     *act_ord_vect[ind]->level) -
			    get_action_time (infAction->position, level))
			  max_time =
			    act_ord_vect[ind]->time_f -
			    get_action_time (act_ord_vect[ind]->position,
					     *act_ord_vect[ind]->level) -
			    get_action_time (infAction->position, level);
		      }
		    else
		      {
			if (mat_ord[ind][infAction->ord_pos] == EA_EB__SA_SB)
			  {
			    if (get_action_time (infAction->position, level) <
				get_action_time (act_ord_vect[ind]->position,
						 *act_ord_vect[ind]->level))
			      {

				if (max_time <
				    act_ord_vect[ind]->time_f -
				    get_action_time (infAction->position,
						     level))
				  max_time =
				    act_ord_vect[ind]->time_f -
				    get_action_time (infAction->position,
						     level);
			      }
			    else
			      {
				if (max_time <
				    act_ord_vect[ind]->time_f -
				    get_action_time (act_ord_vect[ind]->
						     position,
						     *act_ord_vect[ind]->
						     level))
				  max_time =
				    act_ord_vect[ind]->time_f -
				    get_action_time (act_ord_vect[ind]->
						     position,
						     *act_ord_vect[ind]->
						     level);
			      }
			  }

		      }

		  }
	      }
	  }
      }

  infAction->time_f = max_time + get_action_time (infAction->position, level);

  if (DEBUG4)
    printf
      ("\n ---Compute Act: %s, level %d\n    start_time %.2f, end_time %.2f",
       print_op_name_string (infAction->position, temp_name), level, max_time,
       infAction->time_f);

  for (j = 0; j < gef_conn[infAction->position].num_A; j++)
    {
      cel = gef_conn[infAction->position].A[j];

      if (cel < 0)
	continue;

      add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);

      if (is_fact_in_delete_effects_start (infAction->position, cel))
	CONVERT_NOOP_TO_INFORM (cel, next_level)->time_f = -2.0;

      if (add_effect->w_is_true == 1
	  || add_effect->time_f > infAction->time_f)
	{
	  add_effect->time_f = infAction->time_f;
	  add_effect->action_f = infAction;

	  if (DEBUG4)
	    printf ("\n\t-Compute End_eff: %s, level %d, time %.2f",
		    print_ft_name_string (cel, temp_name), next_level,
		    add_effect->time_f);

	  if (GET_ACTION_OF_LEVEL (next_level)->w_is_used
	      &&
	      (is_fact_in_preconditions
	       (vectlevel[next_level]->action.position, add_effect->position)
	       ||
	       (is_fact_in_preconditions_overall
		(vectlevel[next_level]->action.position, add_effect->position)
		&& !is_fact_in_additive_effects_start (vectlevel[next_level]->
						       action.position,
						       add_effect->
						       position))))
	    insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

	  if (CONVERT_NOOP_TO_INFORM (cel, next_level)->w_is_used)
	    forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
					   (cel, next_level));
	}
    }

  if (gef_conn[infAction->position].sf != NULL)

    for (j = 0; j < gef_conn[infAction->position].sf->num_A_start; j++)
      {
	cel = gef_conn[infAction->position].sf->A_start[j];

	if (cel < 0)
	  continue;


	add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);
	infNoop = CONVERT_NOOP_TO_INFORM (cel, level);

	if (infNoop->time_f < 0
	    || infNoop->time_f >
	    (infAction->time_f -
	     get_action_time (infAction->position, level)))
	  infNoop->time_f =
	    infAction->time_f - get_action_time (infAction->position, level);

	if (!is_fact_in_delete_effects (infAction->position, cel))
	  {
	    add_effect->time_f = infNoop->time_f;
	    add_effect->action_f = infNoop;

	    if (DEBUG4)
	      printf ("\n\t-Compute Start_eff: %s, level %d, time %.2f",
		      print_ft_name_string (cel, temp_name), next_level,
		      add_effect->time_f);

	    if (GET_ACTION_OF_LEVEL (next_level)->w_is_used
		&&
		(is_fact_in_preconditions
		 (vectlevel[next_level]->action.position,
		  add_effect->position)
		 ||
		 (is_fact_in_preconditions_overall
		  (vectlevel[next_level]->action.position,
		   add_effect->position)
		  &&
		  !is_fact_in_additive_effects_start (vectlevel[next_level]->
						      action.position,
						      add_effect->position))))
	      insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

	    if (CONVERT_NOOP_TO_INFORM (cel, next_level)->w_is_used)
	      forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
					     (cel, next_level));

	  }

      }

  if (infAction->action_fact == IS_ACTION)
    {
      for (ind = 0; ind < num_act_ord; ind++)
	if (GpG.constraint_type == 0)
	  {
	    if (mat_ord[infAction->ord_pos][ind] == 1)
	      insert_propagation_list (act_ord_vect[ind]);
	  }
	else
	  {
	    if (mat_ord[infAction->ord_pos][ind] != 0)
	      insert_propagation_list (act_ord_vect[ind]);
	  }
    }


  time_adj (infAction);


}






void
update_time (inform_list infAction)
{

  float savetime = 0.0;
  int ind, level, next_level, cel, j;
  inform_list add_effect = NULL, infNoop;

  level = *infAction->level;
  next_level = level + 1;

  if (DEBUG4)
    printf
      ("\n ---Delete Act: %s, level %d\n     start_time %.2f, end_time %.2f",
       print_op_name_string (infAction->position, temp_name), level,
       infAction->time_f - get_action_time (infAction->position, level),
       infAction->time_f);

  infAction->time_f = 0.0;

  for (ind = 0; ind < num_act_ord; ind++)
    if (GpG.constraint_type == 0)
      {
	if (mat_ord[infAction->ord_pos][ind] == 1)
	  insert_propagation_list (act_ord_vect[ind]);
      }
    else
      {
	if (mat_ord[infAction->ord_pos][ind] != 0)
	  insert_propagation_list (act_ord_vect[ind]);
      }
  remove_temporal_constraints (infAction->ord_pos);

  for (j = 0; j < gef_conn[infAction->position].num_PC; j++)
    {
      cel = gef_conn[infAction->position].PC[j];

      if (cel < 0)
	continue;

      if (CONVERT_FACT_TO_INFORM (cel, level)->w_is_true <= 0)
	{
	  CONVERT_FACT_TO_INFORM (cel, level)->time_f = -2.0;
	  CONVERT_FACT_TO_INFORM (cel, level)->action_f = NULL;
	}
    }

  if (gef_conn[infAction->position].sf)
    for (j = 0; j < gef_conn[infAction->position].sf->num_PC_overall; j++)
      {
	cel = gef_conn[infAction->position].sf->PC_overall[j];

	if (cel < 0)
	  continue;

	if (CONVERT_FACT_TO_INFORM (cel, level)->w_is_true <= 0)
	  {
	    CONVERT_FACT_TO_INFORM (cel, level)->time_f = -2.0;
	    CONVERT_FACT_TO_INFORM (cel, level)->action_f = NULL;
	  }
      }

  if (gef_conn[infAction->position].sf)
    for (j = 0; j < gef_conn[infAction->position].sf->num_PC_end; j++)
      {
	cel = gef_conn[infAction->position].sf->PC_end[j];

	if (cel < 0)
	  continue;

	if (CONVERT_FACT_TO_INFORM (cel, level)->w_is_true <= 0)
	  {
	    CONVERT_FACT_TO_INFORM (cel, level)->time_f = -2.0;
	    CONVERT_FACT_TO_INFORM (cel, level)->action_f = NULL;
	  }
      }


  for (j = 0; j < gef_conn[infAction->position].num_A; j++)
    {
      cel = gef_conn[infAction->position].A[j];

      if (cel < 0)
	continue;

      add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);


      savetime = add_effect->time_f;

      infNoop = CONVERT_NOOP_TO_INFORM (cel, level);


      if (is_fact_in_delete_effects_start (infAction->position, cel))
	{
	  if (CONVERT_FACT_TO_INFORM (cel, level)->w_is_true)
	    {
	      infNoop->time_f = CONVERT_FACT_TO_INFORM (cel, level)->time_f;

	      add_effect->time_f = infNoop->time_f;
	      add_effect->action_f = infNoop;

	    }
	  else
	    {
	      add_effect->time_f = -2.0;
	      add_effect->action_f = NULL;
	    }
	}
      else
	{
	  if (infNoop->w_is_used > 0)
	    {
	      add_effect->time_f = infNoop->time_f;
	      add_effect->action_f = infNoop;

	    }
	  else
	    {
	      add_effect->time_f = -2.0;
	      add_effect->action_f = NULL;
	    }

	}


      if (savetime != add_effect->time_f
	  || savetime == Hvar.dg_facts_min_duration[add_effect->position])
	{
	  if (add_effect->time_f >= 0)
	    forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
					   (add_effect->position,
					    next_level));

	  if (savetime != add_effect->time_f
	      && GET_ACTION_OF_LEVEL (next_level)->w_is_used
	      &&
	      (is_fact_in_preconditions
	       (vectlevel[next_level]->action.position, add_effect->position)
	       ||
	       (is_fact_in_preconditions_overall
		(vectlevel[next_level]->action.position, add_effect->position)
		&& !is_fact_in_additive_effects_start (vectlevel[next_level]->
						       action.position,
						       add_effect->
						       position))))
	    insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));
	}

      if (DEBUG4)
	printf ("\n\t-Delete End_eff: %s, level %d, time %.2f, true %d",
		print_ft_name_string (cel, temp_name), next_level,
		add_effect->time_f, add_effect->w_is_true);
    }


  if (gef_conn[infAction->position].sf != NULL)

    for (j = 0; j < gef_conn[infAction->position].sf->num_A_start; j++)
      {
	cel = gef_conn[infAction->position].sf->A_start[j];

	if (cel < 0)
	  continue;

	add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);

	savetime = add_effect->time_f;

	infNoop = CONVERT_NOOP_TO_INFORM (cel, level);


	if (is_fact_in_delete_effects (infAction->position, cel))
	  {
	    if (CONVERT_FACT_TO_INFORM (cel, level)->w_is_true)
	      {
		infNoop->time_f = CONVERT_FACT_TO_INFORM (cel, level)->time_f;

		add_effect->time_f = infNoop->time_f;
		add_effect->action_f = infNoop;

	      }
	    else
	      infNoop->time_f = -2.0;

	  }
	else
	  {


	    if (CONVERT_FACT_TO_INFORM (cel, level)->w_is_true)
	      {
		infNoop->time_f = CONVERT_FACT_TO_INFORM (cel, level)->time_f;

		add_effect->time_f = infNoop->time_f;
		add_effect->action_f = infNoop;

	      }
	    else
	      {
		infNoop->time_f = -2.0;

		add_effect->time_f = -2.0;
		add_effect->action_f = NULL;
	      }

	  }

	if (savetime != add_effect->time_f
	    || savetime == Hvar.dg_facts_min_duration[add_effect->position])
	  {
	    if (add_effect->time_f >= 0)
	      forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
					     (add_effect->position,
					      next_level));

	    if (savetime != add_effect->time_f
		&& GET_ACTION_OF_LEVEL (next_level)->w_is_used
		&&
		(is_fact_in_preconditions
		 (vectlevel[next_level]->action.position,
		  add_effect->position)
		 ||
		 (is_fact_in_preconditions_overall
		  (vectlevel[next_level]->action.position,
		   add_effect->position)
		  &&
		  !is_fact_in_additive_effects_start (vectlevel[next_level]->
						      action.position,
						      add_effect->position))))
	      insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

	  }

	if (DEBUG4)
	  printf ("\n\t-Delete Start_eff: %s, level %d, time %.2f, true %d",
		  print_ft_name_string (cel, temp_name), next_level,
		  add_effect->time_f, add_effect->w_is_true);
      }

  if (gef_conn[infAction->position].sf != NULL)
    for (j = 0; j < gef_conn[infAction->position].sf->num_D_start; j++)
      {
	cel = gef_conn[infAction->position].sf->D_start[j];

	if (cel < 0)
	  continue;

	if (is_fact_in_additive_effects (infAction->position, cel))
	  continue;



	if (CONVERT_FACT_TO_INFORM (cel, level)->w_is_true)
	  {
	    add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);

	    savetime = add_effect->time_f;

	    infNoop = CONVERT_NOOP_TO_INFORM (cel, level);

	    infNoop->time_f = CONVERT_FACT_TO_INFORM (cel, level)->time_f;

	    add_effect->time_f = infNoop->time_f;
	    add_effect->action_f = infNoop;

	  }

	if (savetime != add_effect->time_f
	    || savetime == Hvar.dg_facts_min_duration[add_effect->position])
	  {
	    if (add_effect->time_f >= 0)
	      forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
					     (add_effect->position,
					      next_level));

	    if (savetime != add_effect->time_f
		&& GET_ACTION_OF_LEVEL (next_level)->w_is_used
		&&
		(is_fact_in_preconditions
		 (vectlevel[next_level]->action.position,
		  add_effect->position)
		 ||
		 (is_fact_in_preconditions_overall
		  (vectlevel[next_level]->action.position,
		   add_effect->position)
		  &&
		  !is_fact_in_additive_effects_start (vectlevel[next_level]->
						      action.position,
						      add_effect->position))))
	      insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

	  }

      }



  for (j = 0; j < gef_conn[infAction->position].num_D; j++)
    {
      cel = gef_conn[infAction->position].D[j];

      if (cel < 0)
	continue;


      if (is_fact_in_additive_effects_start (infAction->position, cel))
	continue;

      infNoop = CONVERT_NOOP_TO_INFORM (cel, level);

      if (infNoop->w_is_used)
	{
	  add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);

	  savetime = add_effect->time_f;

	  add_effect->time_f = infNoop->time_f;
	  add_effect->action_f = infNoop;

	}

      if (savetime != add_effect->time_f
	  || savetime == Hvar.dg_facts_min_duration[add_effect->position])
	{
	  if (add_effect->time_f >= 0)
	    forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
					   (add_effect->position,
					    next_level));

	  if (savetime != add_effect->time_f
	      && GET_ACTION_OF_LEVEL (next_level)->w_is_used
	      &&
	      (is_fact_in_preconditions
	       (vectlevel[next_level]->action.position, add_effect->position)
	       ||
	       (is_fact_in_preconditions_overall
		(vectlevel[next_level]->action.position, add_effect->position)
		&& !is_fact_in_additive_effects_start (vectlevel[next_level]->
						       action.position,
						       add_effect->
						       position))))
	    insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

	}

    }

  time_adj (infAction);

}



void
noop_remotion_time (inform_list infNoop)
{

  float savetime = 0.0;
  int level, next_level, cel;
  inform_list add_effect = NULL;


  level = *infNoop->level;
  next_level = level + 1;

  cel = infNoop->position;

  if (!is_fact_in_delete_effects (GET_ACTION_POSITION_OF_LEVEL (level), cel))
    infNoop->time_f = -2.0;

  add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);

  if (add_effect->action_f == infNoop)
    {
      savetime = add_effect->time_f;

      if (add_effect->w_is_true < 1)
	{
	  add_effect->time_f = -2.0;
	  add_effect->action_f = NULL;
	}
      else
	{
	  if (GET_ACTION_OF_LEVEL (level)
	      &&
	      is_fact_in_additive_effects (GET_ACTION_POSITION_OF_LEVEL
					   (level), cel))
	    {

	      add_effect->time_f = GET_ACTION_OF_LEVEL (level)->time_f;
	      add_effect->action_f = GET_ACTION_OF_LEVEL (level);

	    }

	}

      if (savetime != add_effect->time_f)
	{

	  if (GET_ACTION_OF_LEVEL (next_level)->w_is_used
	      &&
	      (is_fact_in_preconditions
	       (vectlevel[next_level]->action.position, add_effect->position)
	       ||
	       (is_fact_in_preconditions_overall
		(vectlevel[next_level]->action.position, add_effect->position)
		&& !is_fact_in_additive_effects_start (vectlevel[next_level]->
						       action.position,
						       add_effect->
						       position))))
	    insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

	  GpG.forward_time = 1;

	}
      else
	GpG.forward_time = 0;
    }
  else
    GpG.forward_time = 0;

}




void
forward_noop_propagation_time (inform_list infNoop)
{

  register inform_list add_effect, infTemp, infAction;
  register int cel, indbreak;
  int level, next_level, position_noop;
  float savetime;


  level = *infNoop->level;
  next_level = level + 1;
  position_noop = infNoop->position;

  if (position_noop < 0)
    return;


  for (infTemp = infNoop; infTemp->w_is_used > 0;
       infTemp = &vectlevel[level]->noop_act[position_noop])
    {

      infAction = GET_ACTION_OF_LEVEL (level);

      cel = infTemp->position;

      infTemp->time_f = CONVERT_FACT_TO_INFORM (cel, level)->time_f;

      if (is_fact_in_additive_effects_start (infAction->position, cel))
	infTemp->time_f =
	  infAction->time_f - get_action_time (infAction->position, level);

      if (CONVERT_FACT_TO_INFORM (cel, level)->w_is_true > 0
	  && CONVERT_FACT_TO_INFORM (cel, level)->time_f >= 0)
	if (CONVERT_FACT_TO_INFORM (cel, level)->time_f < infTemp->time_f)
	  infTemp->time_f = CONVERT_FACT_TO_INFORM (cel, level)->time_f;

      add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);

      indbreak = 1;

      if (infTemp->w_is_overall != ADD_DEL
	  && infTemp->w_is_overall != NADD_DEL)
	{

	  if ((add_effect->time_f > infTemp->time_f && infTemp->time_f >= 0.0)
	      || add_effect->w_is_true == 1)
	    {
	      add_effect->time_f = infTemp->time_f;
	      add_effect->action_f = infTemp;

	      if (GET_ACTION_OF_LEVEL (next_level)->w_is_used
		  &&
		  (is_fact_in_preconditions
		   (vectlevel[next_level]->action.position,
		    add_effect->position)
		   ||
		   (is_fact_in_preconditions_overall
		    (vectlevel[next_level]->action.position,
		     add_effect->position)
		    &&
		    !is_fact_in_additive_effects_start (vectlevel
							[next_level]->action.
							position,
							add_effect->
							position))))
		insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

	      indbreak = 0;
	    }
	  else if (add_effect->action_f == infTemp)
	    {
	      savetime = add_effect->time_f;

	      if (infTemp->time_f < 0.0)
		{
		  add_effect->time_f = infAction->time_f;
		  add_effect->action_f = infAction;
		}

	      else
		{

		  add_effect->time_f = infTemp->time_f;

		  if (add_effect->w_is_true > 1)
		    {
		      infAction = GET_ACTION_OF_LEVEL (level);
		      if (infAction && infAction->w_is_used
			  && is_fact_in_additive_effects (infAction->position,
							  cel))

			if (add_effect->time_f > infAction->time_f)
			  {
			    add_effect->time_f = infAction->time_f;
			    add_effect->action_f = infAction;
			  }

		    }
		}

	      if (savetime != add_effect->time_f)
		{

		  if (GET_ACTION_OF_LEVEL (next_level)->w_is_used
		      &&
		      (is_fact_in_preconditions
		       (vectlevel[next_level]->action.position,
			add_effect->position)
		       ||
		       (is_fact_in_preconditions_overall
			(vectlevel[next_level]->action.position,
			 add_effect->position)
			&&
			!is_fact_in_additive_effects_start (vectlevel
							    [next_level]->
							    action.position,
							    add_effect->
							    position))))
		    insert_propagation_list (GET_ACTION_OF_LEVEL
					     (next_level));

		  indbreak = 0;
		}
	    }
	}


      if (indbreak == 1)
	break;


      level++;
      next_level++;

    }

}



void
forward_noop_remotion_time (inform_list infNoop)
{

  float savetime = 0.0;
  int position_noop, level, next_level, cel;
  inform_list infTemp, infAction, add_effect = NULL;


  level = *infNoop->level;
  next_level = level + 1;
  position_noop = infNoop->position;

  if (position_noop < 0)
    return;

  if (GET_ACTION_OF_LEVEL (level)
      &&
      ((is_fact_in_additive_effects_start
	(vectlevel[level]->action.position, position_noop)
	&& !is_fact_in_delete_effects (vectlevel[level]->action.position,
				       position_noop))
       || is_fact_in_additive_effects (vectlevel[level]->action.position,
				       position_noop)))
    {
      forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
				     (position_noop, level));
      return;
    }


  for (infTemp = infNoop; infTemp->w_is_used > 0;
       infTemp = &vectlevel[level]->noop_act[position_noop])
    {

      cel = infTemp->position;

      add_effect = CONVERT_FACT_TO_INFORM (cel, next_level);

      savetime = add_effect->time_f;

      if (GET_ACTION_OF_LEVEL (level)
	  &&
	  is_fact_in_additive_effects_start (GET_ACTION_POSITION_OF_LEVEL
					     (level), cel))
	{
	  infTemp->time_f =
	    GET_ACTION_OF_LEVEL (level)->time_f -
	    get_action_time (GET_ACTION_POSITION_OF_LEVEL (level), level);

	  if (!is_fact_in_delete_effects
	      (GET_ACTION_POSITION_OF_LEVEL (level), cel))
	    {
	      add_effect->time_f = infTemp->time_f;
	      add_effect->action_f = infTemp;
	    }
	  else
	    break;

	}

      else
	{

	  infTemp->time_f = -2.0;

	  if (add_effect->action_f == infTemp)
	    {

	      if (add_effect->w_is_true <= 1)
		{
		  add_effect->time_f = -2.0;
		  add_effect->action_f = NULL;
		}
	      else
		{
		  infAction = GET_ACTION_OF_LEVEL (level);

		  if (infAction && infAction->w_is_used
		      && is_fact_in_additive_effects (infAction->position,
						      cel))
		    {
		      add_effect->time_f = infAction->time_f;
		      add_effect->action_f = infAction;
		    }
		}

	    }

	}

      if (savetime != add_effect->time_f)
	{

	  if (GET_ACTION_OF_LEVEL (next_level)->w_is_used
	      &&
	      (is_fact_in_preconditions
	       (vectlevel[next_level]->action.position, add_effect->position)
	       ||
	       (is_fact_in_preconditions_overall
		(vectlevel[next_level]->action.position, add_effect->position)
		&& !is_fact_in_additive_effects_start (vectlevel[next_level]->
						       action.position,
						       add_effect->
						       position))))
	    insert_propagation_list (GET_ACTION_OF_LEVEL (next_level));

	}
      else
	break;

      if ((is_fact_in_additive_effects_start
	   (vectlevel[next_level]->action.position, add_effect->position)
	   && !is_fact_in_delete_effects (vectlevel[next_level]->action.
					  position, add_effect->position))
	  || is_fact_in_additive_effects (vectlevel[next_level]->action.
					  position, add_effect->position))
	break;

      level++;
      next_level++;

    }

  if (savetime != add_effect->time_f && next_level < GpG.curr_plan_length)
    forward_noop_propagation_time (CONVERT_NOOP_TO_INFORM
				   (position_noop, next_level));
}




void
get_total_time_plan ()
{

  int i;
  inform_list infGoal;
  float max_time;


  max_time = 0.0;

  if (GpG.total_time_goal)
    {

      for (i = 0; i < GpG.curr_goal_state->num_F; i++)
	{
	  infGoal =
	    CONVERT_FACT_TO_INFORM (GpG.curr_goal_state->F[i],
				    GpG.curr_plan_length);

	  if (infGoal->w_is_true)
	    {

	      if (infGoal->time_f > max_time)
		max_time = infGoal->time_f;
	    }
	}
    }
  else
    {



      for (i = 0; i < gnum_ft_conn; i++)
	{

	  if (vectlevel[GpG.curr_plan_length]->fact[i].w_is_true)
	    {


	      if (vectlevel[GpG.curr_plan_length]->fact[i].time_f > max_time)
		max_time = vectlevel[GpG.curr_plan_length]->fact[i].time_f;
	    }

	}
    }

  GpG.total_time = max_time;

}





void
compress_plan ()
{
  int level, ind;

  if (DEBUG4)
    printf ("\n\n [o] COMPRESS PLAN => Delete empty levels \n");


  for (ind = 0, level = 0; level < GpG.curr_plan_length; level++)
    if (GET_ACTION_POSITION_OF_LEVEL (level) >= 0)
      {

	GET_ACTION_OF_LEVEL (level)->ord_pos = ind;
	act_ord_vect[ind] = GET_ACTION_OF_LEVEL (level);

	num_act_ord++;

	if (num_act_ord >= MAX_NUM_ACTIONS)
	  {
	    exit (1);
	  }

	ind++;
      }

  for (level = 0; level < GpG.curr_plan_length; level++)
    if (GET_ACTION_POSITION_OF_LEVEL (level) >= 0)
      temporal_constraints (GET_ACTION_OF_LEVEL (level));

  for (level = 0; level < GpG.curr_plan_length; level++)
    if (GET_ACTION_POSITION_OF_LEVEL (level) >= 0)
      insert_time (GET_ACTION_OF_LEVEL (level));

  get_total_time_plan ();


}







int
constraint_type_numeric (posA, posB)
{
  int k;

  for (k = 0; k < gnum_fullnum_blocks; k++)
    {

      if (l_vals[posA][k] & r_vals[posB][k])
	return TRUE;

      if (l_vals[posA][k] & tested_vars[posB][k])
	return TRUE;

      if ((l_vals[posA][k] & l_vals[posB][k]) !=
	  (lstar_vals[posA][k] & lstar_vals[posB][k]))
	return TRUE;
    }
  return FALSE;

}


int
constraint_type (int posA, int posB)
{
  int i, j, temp = 0;

  if (gef_conn[posA].is_numeric == 1 && gef_conn[posB].is_numeric == 1)
    {
      temp = constraint_type_numeric (posA, posB);

      if (temp == EA_SB)
	return temp;
    }

  if (gef_conn[posA].sf)
    for (i = 0; i < gef_conn[posA].sf->num_PC_end; i++)
      {
	if (gef_conn[posA].sf->PC_end[i] < 0)
	  continue;


	for (j = 0; j < gef_conn[posB].num_PC; j++)
	  {
	    if (gef_conn[posB].PC[j] < 0)
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].sf->PC_end[i], gef_conn[posB].PC[j]))
	      return (EA_SB);
	  }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_PC_overall; j++)
	    {
	      if (gef_conn[posB].sf->PC_overall[j] < 0)
		continue;

	      if (ARE_MUTEX_FT
		  (gef_conn[posA].sf->PC_end[i],
		   gef_conn[posB].sf->PC_overall[j]))
		return (EA_SB);
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_A_start; j++)
	    {
	      if (gef_conn[posB].sf->A_start[j] < 0)
		continue;

	      if (ARE_MUTEX_FT
		  (gef_conn[posA].sf->PC_end[i],
		   gef_conn[posB].sf->A_start[j]))
		return (EA_SB);
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_D_start; j++)
	    {
	      if (gef_conn[posA].sf->PC_end[i] ==
		  gef_conn[posB].sf->D_start[j])
		return (EA_SB);
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_PC_end; j++)
	    {
	      if (gef_conn[posB].sf->PC_end[j] < 0)
		continue;

	      if (ARE_MUTEX_FT
		  (gef_conn[posA].sf->PC_end[i],
		   gef_conn[posB].sf->PC_end[j]))
		{
		  temp = EA_EB;
		  break;
		}
	    }


	for (j = 0; j < gef_conn[posB].num_A; j++)
	  {
	    if (gef_conn[posB].A[j] < 0)
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].sf->PC_end[i], gef_conn[posB].A[j]))
	      {
		temp = EA_EB;
		break;
	      }
	  }


	for (j = 0; j < gef_conn[posB].num_D; j++)
	  {
	    if (gef_conn[posA].sf->PC_end[i] == gef_conn[posB].D[j])
	      {
		temp = EA_EB;
		break;
	      }
	  }


      }


  for (i = 0; i < gef_conn[posA].num_A; i++)
    {
      if (gef_conn[posA].A[i] < 0)
	continue;


      for (j = 0; j < gef_conn[posB].num_PC; j++)
	{
	  if (gef_conn[posB].PC[j] < 0)
	    continue;

	  if (ARE_MUTEX_FT (gef_conn[posA].A[i], gef_conn[posB].PC[j]))
	    return (EA_SB);
	}


      if (gef_conn[posB].sf)
	for (j = 0; j < gef_conn[posB].sf->num_PC_overall; j++)
	  {
	    if (gef_conn[posB].sf->PC_overall[j] < 0)
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].A[i], gef_conn[posB].sf->PC_overall[j]))
	      return (EA_SB);
	  }


      if (gef_conn[posB].sf)
	for (j = 0; j < gef_conn[posB].sf->num_A_start; j++)
	  {
	    if (gef_conn[posB].sf->A_start[j] < 0)
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].A[i], gef_conn[posB].sf->A_start[j]))
	      return (EA_SB);
	  }


      if (gef_conn[posB].sf)
	for (j = 0; j < gef_conn[posB].sf->num_D_start; j++)
	  {
	    if (gef_conn[posA].A[i] == gef_conn[posB].sf->D_start[j])
	      return (EA_SB);
	  }


      if (gef_conn[posB].sf)
	{
	  for (j = 0; j < gef_conn[posB].sf->num_PC_end; j++)
	    {
	      if (gef_conn[posB].sf->PC_end[j] < 0)
		continue;

	      if (ARE_MUTEX_FT
		  (gef_conn[posA].A[i], gef_conn[posB].sf->PC_end[j]))
		{
		  temp = EA_EB;
		  break;
		}
	    }
	}


      for (j = 0; j < gef_conn[posB].num_A; j++)
	{
	  if (gef_conn[posB].A[j] < 0)
	    continue;

	  if (ARE_MUTEX_FT (gef_conn[posA].A[i], gef_conn[posB].A[j]))
	    {
	      temp = EA_EB;
	      break;
	    }
	}


      for (j = 0; j < gef_conn[posB].num_D; j++)
	if (gef_conn[posA].A[i] == gef_conn[posB].D[j])
	  {
	    temp = EA_EB;
	    break;
	  }
    }



  for (i = 0; i < gef_conn[posA].num_D; i++)
    {

      if (gef_conn[posA].D[i] < 0)
	continue;


      for (j = 0; j < gef_conn[posB].num_PC; j++)
	{
	  if (gef_conn[posA].D[i] == gef_conn[posB].PC[j])
	    return (EA_SB);
	}


      if (gef_conn[posB].sf)
	for (j = 0; j < gef_conn[posB].sf->num_PC_overall; j++)
	  {
	    if (gef_conn[posA].D[i] == gef_conn[posB].sf->PC_overall[j])
	      return (EA_SB);
	  }


      if (gef_conn[posB].sf)
	for (j = 0; j < gef_conn[posB].sf->num_A_start; j++)
	  {
	    if (gef_conn[posA].D[i] == gef_conn[posB].sf->A_start[j])
	      return (EA_SB);
	  }


      if (gef_conn[posB].sf)
	for (j = 0; j < gef_conn[posB].sf->num_PC_end; j++)
	  {
	    if (gef_conn[posA].D[i] == gef_conn[posB].sf->PC_end[j])
	      {
		temp = EA_EB;
		break;
	      }
	  }


      for (j = 0; j < gef_conn[posB].num_A; j++)
	{
	  if (gef_conn[posA].D[i] == gef_conn[posB].A[j])
	    {
	      temp = EA_EB;
	      break;
	    }
	}

    }



  if (gef_conn[posA].sf)
    for (i = 0; i < gef_conn[posA].sf->num_PC_overall; i++)
      {

	if (gef_conn[posA].sf->PC_overall[i] < 0)
	  continue;


	for (j = 0; j < gef_conn[posB].num_PC; j++)
	  {
	    if (gef_conn[posB].PC[j] < 0)
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].sf->PC_overall[i], gef_conn[posB].PC[j]))
	      return (EA_SB);
	  }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_PC_overall; j++)
	    {
	      if (gef_conn[posB].sf->PC_overall[j] < 0)
		continue;

	      if (ARE_MUTEX_FT
		  (gef_conn[posA].sf->PC_overall[i],
		   gef_conn[posB].sf->PC_overall[j]))
		return (EA_SB);
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_A_start; j++)
	    {
	      if (gef_conn[posB].sf->A_start[j] < 0)
		continue;

	      if (ARE_MUTEX_FT
		  (gef_conn[posA].sf->PC_overall[i],
		   gef_conn[posB].sf->A_start[j]))
		return (EA_SB);
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_D_start; j++)
	    {
	      if (gef_conn[posA].sf->PC_overall[i] ==
		  gef_conn[posB].sf->D_start[j])
		return (EA_SB);
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_PC_end; j++)
	    {
	      if (gef_conn[posB].sf->PC_end[j] < 0)
		continue;

	      if (ARE_MUTEX_FT
		  (gef_conn[posA].sf->PC_overall[i],
		   gef_conn[posB].sf->PC_end[j]))
		{
		  temp = EA_EB;
		  break;
		}
	    }


	for (j = 0; j < gef_conn[posB].num_A; j++)
	  {
	    if (gef_conn[posB].A[j] < 0)
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].sf->PC_overall[i], gef_conn[posB].A[j]))
	      {
		temp = EA_EB;
		break;
	      }
	  }


	for (j = 0; j < gef_conn[posB].num_D; j++)
	  {
	    if (gef_conn[posA].sf->PC_overall[i] == gef_conn[posB].D[j])
	      {
		temp = EA_EB;
		break;
	      }
	  }
      }



  for (i = 0; i < gef_conn[posA].num_PC; i++)
    {

      if (gef_conn[posA].PC[i] < 0)
	continue;


      for (j = 0; j < gef_conn[posB].num_PC; j++)
	{
	  if (gef_conn[posB].PC[j] < 0)
	    continue;

	  if (ARE_MUTEX_FT (gef_conn[posA].PC[i], gef_conn[posB].PC[j]))
	    {
	      if (temp != EA_EB)
		temp = SA_SB;
	      else
		temp = EA_EB__SA_SB;
	      break;
	    }
	}


      if (gef_conn[posB].sf)
	for (j = 0; j < gef_conn[posB].sf->num_PC_overall; j++)
	  {
	    if (gef_conn[posB].sf->PC_overall[j] < 0)
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].PC[i], gef_conn[posB].sf->PC_overall[j]))
	      {
		if (temp != EA_EB)
		  temp = SA_SB;
		else
		  temp = EA_EB__SA_SB;
		break;
	      }
	  }

      if (gef_conn[posB].sf)
	for (j = 0; j < gef_conn[posB].sf->num_A_start; j++)
	  {
	    if (gef_conn[posB].sf->A_start[j] < 0)
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].PC[i], gef_conn[posB].sf->A_start[j]))
	      {
		if (temp != EA_EB)
		  temp = SA_SB;
		else
		  temp = EA_EB__SA_SB;
		break;
	      }
	  }

      if (gef_conn[posB].sf)
	for (j = 0; j < gef_conn[posB].sf->num_D_start; j++)
	  {
	    if (gef_conn[posA].PC[i] == gef_conn[posB].sf->D_start[j])
	      {
		if (temp != EA_EB)
		  temp = SA_SB;
		else
		  temp = EA_EB__SA_SB;
		break;
	      }
	  }


      if (gef_conn[posB].sf)
	for (j = 0; j < gef_conn[posB].sf->num_PC_end; j++)
	  {
	    if (gef_conn[posB].sf->PC_end[j] < 0)
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].PC[i], gef_conn[posB].sf->PC_end[j]))
	      if (temp != SA_SB && temp != EA_EB && temp != EA_EB__SA_SB)
		{
		  temp = SA_EB;
		  break;
		}
	  }


      for (j = 0; j < gef_conn[posB].num_A; j++)
	{
	  if (gef_conn[posB].A[j] < 0)
	    continue;

	  if (ARE_MUTEX_FT (gef_conn[posA].PC[i], gef_conn[posB].A[j]))
	    if (temp != SA_SB && temp != EA_EB && temp != EA_EB__SA_SB)
	      {
		temp = SA_EB;
		break;
	      }
	}


      for (j = 0; j < gef_conn[posB].num_D; j++)
	{
	  if (gef_conn[posA].PC[i] == gef_conn[posB].D[j])
	    if (temp != SA_SB && temp != EA_EB && temp != EA_EB__SA_SB)
	      {
		temp = SA_EB;
		break;
	      }
	}

    }




  if (gef_conn[posA].sf)
    for (i = 0; i < gef_conn[posA].sf->num_A_start; i++)
      {

	if (gef_conn[posA].sf->A_start[i])
	  continue;


	for (j = 0; j < gef_conn[posB].num_PC; j++)
	  {
	    if (gef_conn[posB].PC[j])
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].sf->A_start[i], gef_conn[posB].PC[j]))
	      {
		if (temp != EA_EB)
		  temp = SA_SB;
		else
		  temp = EA_EB__SA_SB;
		break;
	      }
	  }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_PC_overall; j++)
	    {
	      if (gef_conn[posB].sf->PC_overall[j])
		continue;

	      if (ARE_MUTEX_FT
		  (gef_conn[posA].sf->A_start[i],
		   gef_conn[posB].sf->PC_overall[j]))
		{
		  if (temp != EA_EB)
		    temp = SA_SB;
		  else
		    temp = EA_EB__SA_SB;
		  break;
		}
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_A_start; j++)
	    {
	      if (gef_conn[posB].sf->A_start[j])
		continue;

	      if (ARE_MUTEX_FT
		  (gef_conn[posA].sf->A_start[i],
		   gef_conn[posB].sf->A_start[j]))
		{
		  if (temp != EA_EB)
		    temp = SA_SB;
		  else
		    temp = EA_EB__SA_SB;
		  break;
		}
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_D_start; j++)
	    {



	      if (gef_conn[posA].sf->A_start[i] ==
		  gef_conn[posB].sf->D_start[j])
		{
		  if (temp != EA_EB)
		    temp = SA_SB;
		  else
		    temp = EA_EB__SA_SB;
		  break;
		}
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_PC_end; j++)
	    {
	      if (gef_conn[posB].sf->PC_end[j])
		continue;

	      if (ARE_MUTEX_FT
		  (gef_conn[posA].sf->A_start[i],
		   gef_conn[posB].sf->PC_end[j]))
		if (temp != SA_SB && temp != EA_EB && temp != EA_EB__SA_SB)
		  {
		    temp = SA_EB;
		    break;
		  }
	    }


	for (j = 0; j < gef_conn[posB].num_A; j++)
	  {
	    if (gef_conn[posB].A[j])
	      continue;

	    if (ARE_MUTEX_FT
		(gef_conn[posA].sf->A_start[i], gef_conn[posB].A[j]))
	      if (temp != SA_SB && temp != EA_EB && temp != EA_EB__SA_SB)
		{
		  temp = SA_EB;
		  break;
		}
	  }


	for (j = 0; j < gef_conn[posB].num_D; j++)
	  {
	    if (gef_conn[posA].sf->A_start[i] == gef_conn[posB].D[j])
	      if (temp != SA_SB && temp != EA_EB && temp != EA_EB__SA_SB)
		{
		  temp = SA_EB;
		  break;
		}
	  }

      }





  if (gef_conn[posA].sf)
    for (i = 0; i < gef_conn[posA].sf->num_D_start; i++)
      {
	if (gef_conn[posA].sf->D_start[i])
	  continue;


	for (j = 0; j < gef_conn[posB].num_PC; j++)
	  {
	    if (gef_conn[posA].sf->D_start[i] == gef_conn[posB].PC[j])
	      {
		if (temp != EA_EB)
		  temp = SA_SB;
		else
		  temp = EA_EB__SA_SB;
		break;
	      }
	  }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_PC_overall; j++)
	    {
	      if (gef_conn[posA].sf->D_start[i] ==
		  gef_conn[posB].sf->PC_overall[j])
		{
		  if (temp != EA_EB)
		    temp = SA_SB;
		  else
		    temp = EA_EB__SA_SB;
		  break;
		}
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_A_start; j++)
	    {



	      if (gef_conn[posA].sf->D_start[i] ==
		  gef_conn[posB].sf->A_start[j])
		{
		  if (temp != EA_EB)
		    temp = SA_SB;
		  else
		    temp = EA_EB__SA_SB;
		  break;
		}
	    }


	if (gef_conn[posB].sf)
	  for (j = 0; j < gef_conn[posB].sf->num_PC_end; j++)
	    {
	      if (gef_conn[posA].sf->D_start[i] ==
		  gef_conn[posB].sf->PC_end[j])
		if (temp != SA_SB && temp != EA_EB && temp != EA_EB__SA_SB)
		  {
		    temp = SA_EB;
		    break;
		  }
	    }

	for (j = 0; j < gef_conn[posB].num_A; j++)
	  {
	    if (gef_conn[posA].sf->D_start[i] == gef_conn[posB].A[j])
	      if (temp != SA_SB && temp != EA_EB && temp != EA_EB__SA_SB)
		{
		  temp = SA_EB;
		  break;
		}
	  }
      }



  return (temp);


}





void
temporal_constraints (inform_list infAction)
{

  int level, posAction, posB, ind_level;


  level = *infAction->level;

  if (GpG.temporal_plan)
    {
      for (posAction = 0; posAction < num_act_ord; posAction++)
	if (act_ord_vect[posAction] == NULL)
	  break;

      if (posAction == num_act_ord)
	num_act_ord++;

      if (num_act_ord >= MAX_NUM_ACTIONS)
	{
	  exit (1);
	}

      infAction->ord_pos = posAction;
      act_ord_vect[posAction] = infAction;
    }
  else
    {
      posAction = infAction->ord_pos;

    }

  for (ind_level = 0; ind_level < GpG.curr_plan_length; ind_level++)
    {
      if (&vectlevel[ind_level]->action == infAction)
	continue;

      posB = vectlevel[ind_level]->action.ord_pos;

      if (check_mutex_action (infAction->position, ind_level) >= 0)
	{

	  if (vectlevel[ind_level]->action.w_is_used > 0)
	    {
	      if (ind_level < level)
		{

		  if (GpG.constraint_type && GpG.durative_actions_in_domain)
		    mat_ord[posB][posAction] =
		      constraint_type (vectlevel[ind_level]->action.position,
				       infAction->position);
		  else
		    mat_ord[posB][posAction] = 1;
		}

	      if (ind_level > level)
		{
		  if (GpG.constraint_type && GpG.durative_actions_in_domain)
		    mat_ord[posAction][posB] =
		      constraint_type (infAction->position,
				       vectlevel[ind_level]->action.position);
		  else
		    mat_ord[posAction][posB] = 1;
		}
	    }
	}
    }


}


void
remove_temporal_constraints (int posAction)
{
  int i;

  for (i = 0; i < num_act_ord; i++)
    {
      mat_ord[i][posAction] = 0;
      mat_ord[posAction][i] = 0;
    }


  act_ord_vect[posAction] = NULL;

  if (posAction == (num_act_ord - 1))
    num_act_ord--;

}
