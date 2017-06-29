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
 * File: check.c
 * Description: check functions.
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/





#include "lpg.h"
#include "ActionSubgraph.h"
#include "numeric.h"
#include "utilities.h"
#include "LpgOutput.h"
#include "output.h"


extern int total_ft_ef_mutex, total_ef_ft_mutex, total_ef_ef_mutex,
  total_ft_ft_mutex;


int
check_time_and_length (int plan_ops_num)
{
  float elapsed_time = 0;
  struct tms end_time;

  if ((gcmd_line.max_plan_ops != 0)
      && (plan_ops_num >= gcmd_line.max_plan_ops))
    {
      printf ("\nError: max plan length reached\n\n");
      exit (1);
    }
  times (&end_time);
  elapsed_time = (float) ((end_time.tms_utime - glob_start_time.tms_utime +
			   end_time.tms_stime -
			   glob_start_time.tms_stime) / 100.0);

  if ((gcmd_line.max_cpu_time != 0)
      && (elapsed_time >= gcmd_line.max_cpu_time))
    {
      printf ("\nError: max cpu-time reached\n\n");
      exit (1);
    }
  return 1;
}









int
check_plan (int max_time)
{
  int level, pos, true, error, temp;
  unsigned int uid_block, uid_mask;
  inform_list fact, act;
  FtConn *vertex_noop;

  if (max_time != GpG.curr_plan_length)
    printf ("\nCheck Plan max_time error... %d %d", max_time,
	    GpG.curr_plan_length);
  max_time = GpG.curr_plan_length;

  printf ("\nCheck Plan... %s:%d, max %d", __FILE__, __LINE__,
	  GpG.curr_plan_length);
  for (level = 0; level <= max_time && level < GpG.max_plan_length; level++)

    {
      if (vectlevel[level]->modified || vectlevel[level]->level != level)
	printf ("\n MODIFIED %d, vectlevel %d, level %d",
		vectlevel[level]->modified, vectlevel[level]->level, level);
      if (vectlevel[level]->level != level)

	{
	  printf ("\n ERROR level %d\n", level);
	  exit (0);
	}
      for (pos = 0; pos < gnum_ft_conn; pos++)
	if (CHECK_FACT_POS (pos, level))

	  {
	    error = 0;
	    fact = &vectlevel[level]->fact[pos];
	    uid_block = GUID_BLOCK (pos);
	    uid_mask = GUID_MASK (pos);
	    if (fact->w_is_true && level > 0)
	      {
		temp = 0;


		if (is_fact_in_additive_effects
		    (GET_ACTION_POSITION_OF_LEVEL (level - 1), pos))
		  temp++;
		if (CHECK_NOOP_POS (pos, level - 1))

		  {
		    if (CONVERT_NOOP_TO_INFORM (pos, level - 1)
			&& CONVERT_NOOP_TO_INFORM (pos, level - 1)->w_is_used)
		      temp++;
		  }

		else
		  if (is_fact_in_additive_effects_start
		      (GET_ACTION_POSITION_OF_LEVEL (level - 1), pos)
		      &&
		      !is_fact_in_delete_effects
		      (GET_ACTION_POSITION_OF_LEVEL (level - 1), pos))
		  temp++;
		if (temp != fact->w_is_true)
		  printf ("\nFact error %s w_is_true %d count %d level %d ",
			  print_ft_name_string (pos, temp_name),
			  fact->w_is_true, temp, level);
	      }
	    if (fact->w_is_goal && level < max_time)

	      {
		temp = 0;


		if (GET_ACTION_OF_LEVEL (level)
		    &&
		    is_fact_in_preconditions (GET_ACTION_POSITION_OF_LEVEL
					      (level), pos)
		    && GET_ACTION_OF_LEVEL (level)->w_is_goal)
		  temp++;
		if (GET_ACTION_OF_LEVEL (level)
		    &&
		    is_fact_in_preconditions_overall
		    (GET_ACTION_POSITION_OF_LEVEL (level), pos)
		    && GET_ACTION_OF_LEVEL (level)->w_is_goal
		    && CONVERT_NOOP_TO_INFORM (pos,
					       level)->w_is_overall !=
		    ADD_DEL
		    && CONVERT_NOOP_TO_INFORM (pos,
					       level)->w_is_overall !=
		    ADD_NDEL)
		  temp++;
		if (CHECK_NOOP_POS (pos, level)
		    && CONVERT_NOOP_TO_INFORM (pos, level)->w_is_goal)
		  temp++;
		if (level > 1 && GET_ACTION_OF_LEVEL (level - 1)
		    &&
		    is_fact_in_preconditions_end
		    (GET_ACTION_POSITION_OF_LEVEL (level), pos)
		    && GET_ACTION_OF_LEVEL (level)->w_is_goal
		    && CONVERT_NOOP_TO_INFORM (pos,
					       level)->w_is_overall !=
		    DEL_ADD
		    && CONVERT_NOOP_TO_INFORM (pos,
					       level)->w_is_overall !=
		    ADD_NDEL
		    &&
		    !is_fact_in_additive_effects
		    (GET_ACTION_POSITION_OF_LEVEL (level - 1), pos))
		  temp++;
		if (level >= GpG.curr_plan_length)

		  {
		    if (fact->w_is_goal > 1
			&&
			!is_fact_in_preconditions_end
			(GET_ACTION_POSITION_OF_LEVEL (level), pos))
		      printf
			("\nFact error %s Goal  w_is_goal %d count %d level %d ",
			 print_ft_name_string (pos, temp_name),
			 fact->w_is_goal, temp, level);
		  }

		else if (temp != fact->w_is_goal)

		  {
		    printf ("\nFact error %s w_is_goal %d count %d level %d ",
			    print_ft_name_string (pos, temp_name),
			    fact->w_is_goal, temp, level);
		    if (GET_ACTION_OF_LEVEL (level)->w_is_goal
			&&
			(is_fact_in_preconditions
			 (GET_ACTION_POSITION_OF_LEVEL (level), pos)
			 ||
			 is_fact_in_preconditions_overall
			 (GET_ACTION_POSITION_OF_LEVEL (level), pos)))
		      printf
			("\nAction at level %d:   name %s w_is_goal %d pos %d",
			 level,
			 print_op_name_string (GET_ACTION_OF_LEVEL (level)->
					       position, temp_name),
			 GET_ACTION_OF_LEVEL (level)->w_is_goal,
			 GET_ACTION_OF_LEVEL (level)->position);
		    if (GET_ACTION_OF_LEVEL (level) && level > 1
			&&
			is_fact_in_preconditions_end
			(GET_ACTION_POSITION_OF_LEVEL (level), pos))
		      printf ("Action at level %d:   name %s %d %d", level,
			      print_op_name_string (GET_ACTION_OF_LEVEL
						    (level)->position,
						    temp_name),
			      GET_ACTION_OF_LEVEL (level)->w_is_goal,
			      GET_ACTION_OF_LEVEL (level)->position);
		    if (CONVERT_NOOP_TO_INFORM (pos, level)->w_is_goal)
		      printf
			("\nNoop:   name %s  w_is_goal %d pos %d min-level %d",
			 print_noop_name_string (pos, temp_name),
			 CONVERT_NOOP_TO_INFORM (pos, level)->w_is_goal,
			 CONVERT_NOOP_TO_INFORM (pos, level)->position,
			 gft_conn[pos].level);
		  }
	      }
	    true = (vectlevel[level]->fact_vect[uid_block] & uid_mask);
	    if ((fact->w_is_true != 0) != (true != 0))
	      printf
		("\nFact error %s w_is_true %d bit %d  level %d position %d",
		 print_ft_name_string (pos, temp_name), fact->w_is_true, true,
		 level, pos);
	    true = vectlevel[level]->prec_vect[uid_block] & uid_mask;
	    if ((fact->w_is_goal > 0) != (true != 0))
	      printf ("\nFact error %s w_is_goal %d bit %d level %d ",
		      print_ft_name_string (pos, temp_name), fact->w_is_goal,
		      true, level);
	    true = vectlevel[level]->false_crit_vect[uid_block] & uid_mask;
	    if ((fact->w_is_goal && fact->w_is_true == 0) != (true != 0))
	      printf
		("\nFact error False critic %s w_is_goal %d w_is_true %d bit %d  level %d",
		 print_ft_name_string (pos, temp_name), fact->w_is_goal,
		 fact->w_is_true, true, level);
	    true = vectlevel[level]->true_crit_vect[uid_block] & uid_mask;
	    if ((fact->w_is_goal && fact->w_is_true == 1) != (true != 0))
	      printf
		("\nFact error True critic %s w_is_goal %d w_is_true %d bit %d level %d ",
		 print_ft_name_string (pos, temp_name), fact->w_is_goal,
		 fact->w_is_true, true, level);
	  }
      if (level >= max_time)
	break;
      for (pos = 0; pos < gnum_ft_conn; pos++)
	if (CHECK_NOOP_POS (pos, level))

	  {
	    error = 0;
	    act = &vectlevel[level]->noop_act[pos];
	    vertex_noop = &gnoop_conn[pos];
	    uid_block = GUID_BLOCK (pos);
	    uid_mask = GUID_MASK (pos);
	    true = vectlevel[level]->noop_act_vect[uid_block] & uid_mask;
	    if ((act->w_is_used > 0) != (true != 0))
	      printf ("\nNoop error %s w_is_used %d bit %d  level %d",
		      print_noop_name_string (pos, temp_name), act->w_is_used,
		      true, level);
	    true = vectlevel[level]->noop_prec_act_vect[uid_block] & uid_mask;
	    if ((act->w_is_goal > 0) != (true != 0))
	      printf ("\nNoop error %s w_is_goal %d bit %d  level %d",
		      print_noop_name_string (pos, temp_name),
		      act->w_is_goal, true, level);
	    if (act->w_is_used
		&& (act->w_is_overall == DEL_ADD
		    || act->w_is_overall == DEL_NADD))
	      printf
		("\nNoop error: DEL_START noop used -  noop %s w_is_used %d w_is_overall %d level %d",
		 print_noop_name_string (pos, temp_name), act->w_is_used,
		 act->w_is_overall, level);
	    if (!act->w_is_used
		&& (act->w_is_overall == ADD_DEL
		    || act->w_is_overall == ADD_NDEL))
	      printf
		("\nNoop error: ADD_START noop not used -  noop %s w_is_used %d w_is_overall %d level %d",
		 print_noop_name_string (pos, temp_name), act->w_is_used,
		 act->w_is_overall, level);
	    if (!act->w_is_used && act->w_is_overall == NADD_DEL
		&& vectlevel[level]->fact[pos].w_is_true)
	      printf
		("\nNoop error: NADD_DEL noop used fact true -  noop %s w_is_used %d w_is_overall %d level %d",
		 print_noop_name_string (pos, temp_name), act->w_is_used,
		 act->w_is_overall, level);
	    if (act->w_is_used && act->w_is_overall == NADD_DEL
		&& !vectlevel[level]->fact[pos].w_is_true)
	      printf
		("\nNoop error: NADD_DEL noop used fact false -  noop %s w_is_used %d w_is_overall %d level %d",
		 print_noop_name_string (pos, temp_name), act->w_is_used,
		 act->w_is_overall, level);
	  }
    }
  printf ("\n");
  return 0;
}







void
check_temporal_plan ()
{
  int ind, level, indPrec, indlevel;
  inform_list infTemp;

  printf ("\nCheck temporal plan...");

  for (level = 0; level < GpG.curr_plan_length; level++)
    {
      for (ind = 0; ind < gnum_ft_conn; ind++)
	{
	  infTemp = &vectlevel[level]->fact[ind];

	  if (infTemp->w_is_true)
	    {

	      if (infTemp->time_f < 0)
		printf
		  ("\nERROR true fact1: time_f %s  level: %d time %.2f",
		   print_ft_name_string (ind, temp_name), level,
		   infTemp->time_f);

	      if (infTemp->time_f == 0.0 && gft_conn[ind].level > 0)
		{

		  for (indlevel = level - 1; indlevel > 0; indlevel--)
		    {

		      if (CHECK_FACT_POS (ind, indlevel) < 0)
			break;

		      if (vectlevel[indlevel]->fact[ind].w_is_true <= 0)
			break;

		      if (vectlevel[indlevel]->fact[ind].time_f > 0)
			break;

		      if (is_fact_in_delete_effects_start
			  (GET_ACTION_POSITION_OF_LEVEL (indlevel), ind)
			  ||
			  is_fact_in_delete_effects
			  (GET_ACTION_POSITION_OF_LEVEL (indlevel), ind))
			break;

		    }


		  if (!is_fact_in_additive_effects_start
		      (GET_ACTION_POSITION_OF_LEVEL (indlevel), ind)
		      || (GET_ACTION_OF_LEVEL (indlevel)->time_f -
			  get_action_time (GET_ACTION_POSITION_OF_LEVEL
					   (indlevel), indlevel)) > 0)
		    printf
		      ("\nERROR true fact2: time_f %s  level: %d time %.2f",
		       print_ft_name_string (ind, temp_name), level,
		       infTemp->time_f);
		}

	      if (infTemp->action_f == NULL && level != 0)
		printf
		  ("\nERRORE true fact: action_f %s  level: %d time %.2f",
		   print_ft_name_string (ind, temp_name), *infTemp->level,
		   infTemp->time_f);

	      if (level != 0 && *infTemp->action_f->level != level - 1)
		printf
		  ("\nERROR true fact: action_f %s  level: %d time %.2f act_level %d",
		   print_ft_name_string (ind, temp_name), *infTemp->level,
		   infTemp->time_f, *infTemp->action_f->level);
	    }

	  else
	    {
	      if (infTemp->time_f > 0.0
		  &&
		  !(is_fact_in_preconditions
		    (GET_ACTION_POSITION_OF_LEVEL (level), ind)
		    ||
		    is_fact_in_preconditions_overall
		    (GET_ACTION_POSITION_OF_LEVEL (level), ind)
		    ||
		    is_fact_in_preconditions_end (GET_ACTION_POSITION_OF_LEVEL
						  (level), ind)))

		printf
		  ("\nERROR false fact3: time_f %s [%d] level: %d time %.2f",
		   print_ft_name_string (ind, temp_name), ind,
		   *infTemp->level, infTemp->time_f);

	      if (infTemp->action_f != NULL)
		printf
		  ("\nERROR false fact4: action_f %s  level: %d time %.2f",
		   print_ft_name_string (ind, temp_name), *infTemp->level,
		   infTemp->time_f);
	    }

	  if (infTemp->w_is_true == 1 && infTemp->action_f != NULL
	      && infTemp->time_f != infTemp->action_f->time_f)
	    {
	      printf
		("\nERROR 3 fact %s   level: %d time %.2f action_f pos %d action_f time  %.2f  ",
		 print_ft_name_string (ind, temp_name), *infTemp->level,
		 infTemp->time_f, infTemp->action_f->position,
		 infTemp->action_f->time_f);

	      if (infTemp->action_f->action_fact == IS_ACTION)
		print_op_name_string (infTemp->action_f->position, temp_name);
	      else
		printf (" \n%s level %d w_is_used %d ",
			print_noop_name_string (infTemp->action_f->position,
						temp_name),
			*infTemp->action_f->level,
			infTemp->action_f->w_is_used);
	    }

	}

      ind = GET_ACTION_POSITION_OF_LEVEL (level);

      if (ind >= 0)
	{

	  if ((infTemp = GET_ACTION_OF_LEVEL (level))->w_is_used)
	    {

	      if (infTemp->time_f <= 0.0)
		printf ("\nERROR action used: %s  level: %d time %.2f",
			print_op_name_string (ind, temp_name),
			*infTemp->level, infTemp->time_f);
	    }

	  else
	    {
	      if (infTemp->time_f > 0.0)
		printf
		  ("\nERROR action not used: %s  level: %d time %.2f",
		   print_op_name_string (ind, temp_name), *infTemp->level,
		   infTemp->time_f);
	    }

	  for (indPrec = 0; indPrec < gef_conn[ind].num_PC; indPrec++)

	    if (infTemp->time_f <
		vectlevel[level]->fact[gef_conn[ind].PC[indPrec]].time_f +
		get_action_time (ind, level) - TIME_TOLERANCE)
	      {
		printf ("\nERROR action used: %s end-time %.2f",
			print_op_name_string (ind, temp_name),
			infTemp->time_f);
		printf ("  with precondition %s at time %.2f in level %d",
			print_ft_name_string (gef_conn[ind].PC[indPrec],
					      temp_name),
			vectlevel[level]->fact[gef_conn[ind].PC[indPrec]].
			time_f, *infTemp->level);
	      }

	  if (gef_conn[ind].sf)
	    for (indPrec = 0; indPrec < gef_conn[ind].sf->num_PC_overall;
		 indPrec++)

	      if (infTemp->time_f <
		  vectlevel[level]->fact[gef_conn[ind].sf->
					 PC_overall[indPrec]].time_f +
		  get_action_time (ind, level) - TIME_TOLERANCE)
		{
		  printf ("\nERROR action used: %s end-time %.2f",
			  print_op_name_string (ind, temp_name),
			  infTemp->time_f);
		  printf
		    ("  with precondition %s at time %.2f in level %d",
		     print_ft_name_string (gef_conn[ind].sf->
					   PC_overall[indPrec], temp_name),
		     vectlevel[level]->fact[gef_conn[ind].sf->
					    PC_overall[indPrec]].time_f,
		     *infTemp->level);
		}

	}

      for (ind = 0; ind < GpG.max_num_facts; ind++)
	if (CHECK_NOOP_POS (ind, level))
	  if (vectlevel[level]->noop_act[ind].w_is_used
	      && vectlevel[level]->noop_act[ind].time_f < 0.0)
	    printf ("\nERROR noop %s %d  level %d  time_f %.3f",
		    print_noop_name_string (ind, temp_name), ind, level,
		    vectlevel[level]->noop_act[ind].time_f);

    }

}



void
check_act_ord_vect ()
{

  int ind;


  for (ind = 0; ind < num_act_ord; ind++)
    if (act_ord_vect[ind] != NULL)
      if (act_ord_vect[ind]->ord_pos != ind)
	printf ("\nERROR ACT_ORD_VECT");

}



void
check_matrix ()
{

  int i, j;


  for (i = 0; i < num_act_ord; i++)

    for (j = 0; j < num_act_ord; j++)
      {
	if (mat_ord[i][j] != 0
	    && *act_ord_vect[i]->level >= *act_ord_vect[j]->level)
	  printf ("\nERROR MATRIX CONSTRAINT MANAGEMENT");
      }

}



void
check_prop_level_index ()
{

  int ind;


  for (ind = 0; ind <= GpG.curr_plan_length; ind++)
    if (prop_level_index[ind] != -1)
      printf ("\nERROR PROP_LEVEL_INDEX");


}





void
check_num_prec ()
{

  int level, i, el, position;



  for (level = 0; level < GpG.curr_plan_length; level++)
    {


      if (GET_ACTION_POSITION_OF_LEVEL (level) >= 0)
	{

	  position = GET_ACTION_POSITION_OF_LEVEL (level);

	  for (i = 0; i < gef_conn[position].num_PC; i++)
	    {
	      el = gef_conn[position].PC[i];
	      if (el < 0)
		{
		  if (!is_num_prec_satisfied (-el, level)
		      && vectlevel[level]->numeric->false_position[-el] < 0)
		    {
		      insert_unsup_numeric_fact (-el, level);
		    }
		}
	      else
		if (!fact_is_supported (el, level)
		    && vectlevel[level]->fact[el].false_position < 0)
		{
		  insert_unsup_fact (&vectlevel[level]->fact[el]);
		}
	    }
	  if (gef_conn[position].sf != NULL)
	    {
	      for (i = 0; i < gef_conn[position].sf->num_PC_overall; i++)
		{
		  el = gef_conn[position].sf->PC_overall[i];
		  if (el < 0)
		    {
		      if (!is_num_prec_satisfied (-el, level)
			  && vectlevel[level]->numeric->false_position[-el] <
			  0)
			{
			  insert_unsup_numeric_fact (-el, level);
			}
		    }
		  else
		    if (!fact_is_supported (el, level)
			&& vectlevel[level]->fact[el].false_position < 0)
		    {
		      insert_unsup_fact (&vectlevel[level]->fact[el]);
		    }
		}
	      for (i = 0; i < gef_conn[position].sf->num_PC_end; i++)
		{
		  el = gef_conn[position].sf->PC_end[i];
		  if (el < 0)
		    {
		      if (!is_num_prec_satisfied (-el, level)
			  && vectlevel[level +
				       1]->numeric->false_position[-el] < 0)
			{
			  insert_unsup_numeric_fact (-el, level);
			}

		    }
		  else
		    if (!fact_is_supported (el, level)
			&& vectlevel[level]->fact[el].false_position < 0)
		    {
		      insert_unsup_fact (&vectlevel[level]->fact[el]);
		    }

		}
	    }
	}


    }
}


void
check_consistency (int level)
{
  int i;
  float *values;

  CompositeNumVar *cv;
  values = vectlevel[level]->numeric->values;

  for (i = 0; i < gnum_comp_var; i++)
    {
      cv = &gcomp_var[i];
      switch (cv->operator)
	{
	case MUL_OP:
	  if ((values[i] - (values[cv->first_op] * values[cv->second_op])) >
	      MAX_APPROX)
	    {
	      printf ("\n\n inconsistency in cvars array");
	      printf ("\nop: *");
	      printf ("\nfirst   : %5d:%8f", cv->first_op,
		      values[cv->first_op]);
	      printf ("\nsecond  : %5d:%8f", cv->second_op,
		      values[cv->second_op]);
	      printf ("\ncorrect : %f",
		      values[cv->first_op] * values[cv->second_op]);
	      printf ("\nreported: %f\n\n", values[i]);
	      assert (0);
	      exit (1);
	    }
	  break;
	case DIV_OP:
	  if (values[cv->second_op] == 0)
	    {
	      printf ("\n\n check_consistency: div by 0\n\n");
	      exit (1);
	    }
	  if ((values[i] - (values[cv->first_op] / values[cv->second_op])) >
	      MAX_APPROX)
	    {
	      printf ("\n\n inconsistency in cvars array\n\n");
	      printf ("\nop: /");
	      printf ("\nfirst   : %5d:%8f", cv->first_op,
		      values[cv->first_op]);
	      printf ("\nsecond  : %5d:%8f", cv->second_op,
		      values[cv->second_op]);
	      printf ("\ncorrect : %f",
		      values[cv->first_op] / values[cv->second_op]);
	      printf ("\nreported: %f\n\n", values[i]);
	      assert (0);
	      exit (1);
	    }
	  break;
	case PLUS_OP:
	  if ((values[i] - (values[cv->first_op] + values[cv->second_op])) >
	      MAX_APPROX)
	    {
	      printf ("\n\n inconsistency in cvars array\n\n");
	      printf ("\nop: +");
	      printf ("\nfirst   : %5d:%8f", cv->first_op,
		      values[cv->first_op]);
	      printf ("\nsecond  : %5d:%8f", cv->second_op,
		      values[cv->second_op]);
	      printf ("\ncorrect : %5f",
		      values[cv->first_op] + values[cv->second_op]);
	      printf ("\nreported: %5f\n\n", values[i]);
	      assert (0);
	      exit (1);
	    }
	  break;
	case MINUS_OP:
	  if ((values[i] - (values[cv->first_op] - values[cv->second_op])) >
	      MAX_APPROX)
	    {
	      printf ("\n\n inconsistency in cvars array\n\n");
	      printf ("\nop: -");
	      printf ("\nfirst   : %5d:%8f", cv->first_op,
		      values[cv->first_op]);
	      printf ("\nsecond  : %5d:%8f", cv->second_op,
		      values[cv->second_op]);
	      printf ("\ncorrect : %f",
		      values[cv->first_op] - values[cv->second_op]);
	      printf ("\nreported: %f\n\n", values[i]);
	      assert (0);
	      exit (1);
	    }
	  break;
	case UMINUS_OP:
	  if ((values[i] + values[cv->first_op]) > MAX_APPROX)
	    {
	      printf ("\n\n inconsistency in cvars array\n\n");
	      printf ("\nop: unary -");
	      printf ("\nfirst   : %5d:%8f", cv->first_op,
		      values[cv->first_op]);
	      printf ("\nsecond  : %5d:%8f", cv->second_op,
		      values[cv->second_op]);
	      printf ("\ncorrect : %f", -values[cv->first_op]);
	      printf ("\nreported: %f\n\n", values[i]);
	      assert (0);
	      exit (1);
	    }
	  break;
	case LESS_THAN_OP:
	  if (((values[i] < 0.5)
	       && (values[cv->first_op] < values[cv->second_op]))
	      || ((values[i] > 0.5)
		  && (values[cv->first_op] >= values[cv->second_op])))
	    {
	      printf ("\n\n inconsistency in cvars array\n\n");
	      printf ("\nop: <");
	      printf ("\nfirst   : %5d:%8f", cv->first_op,
		      values[cv->first_op]);
	      printf ("\nsecond  : %5d:%8f", cv->second_op,
		      values[cv->second_op]);
	      printf ("\ncorrect : %f",
		      (float) (values[cv->first_op] < values[cv->second_op]));
	      printf ("\nreported: %f\n\n", values[i]);
	      assert (0);
	      exit (1);
	    }
	  break;
	case LESS_THAN_OR_EQUAL_OP:
	  if (((values[i] < 0.5)
	       && (values[cv->first_op] <= values[cv->second_op]))
	      || ((values[i] > 0.5)
		  && (values[cv->first_op] > values[cv->second_op])))
	    {
	      printf ("\n\n inconsistency in cvars array\n\n");
	      printf ("\nop: <=");
	      printf ("\nfirst   : %5d:%8f", cv->first_op,
		      values[cv->first_op]);
	      printf ("\nsecond  : %5d:%8f", cv->second_op,
		      values[cv->second_op]);
	      printf ("\ncorrect : %d",
		      (values[cv->first_op] <= values[cv->second_op]));
	      printf ("\nreported: %f\n\n", values[i]);
	      assert (0);
	      exit (1);
	    }
	  break;
	case EQUAL_OP:
	  if (((values[i] < 0.5)
	       && ((values[cv->first_op] - values[cv->second_op]) <
		   MAX_APPROX)) || ((values[i] > 0.5)
				    &&
				    ((values[cv->first_op] -
				      values[cv->second_op]) > MAX_APPROX)))
	    {
	      printf ("\n\n inconsistency in cvars array\n\n");
	      printf ("\nop: =");
	      printf ("\nfirst   : %5d:%8f", cv->first_op,
		      values[cv->first_op]);
	      printf ("\nsecond  : %5d:%8f", cv->second_op,
		      values[cv->second_op]);
	      printf ("\ncorrect : %d",
		      ((values[cv->first_op] - values[cv->second_op]) <
		       MAX_APPROX));
	      printf ("\nreported: %f\n\n", values[i]);
	      assert (0);
	      exit (1);
	    }
	  break;
	case GREATER_THAN_OP:
	  if (((values[i] < 0.5)
	       && (values[cv->first_op] > values[cv->second_op]))
	      || ((values[i] > 0.5)
		  && (values[cv->first_op] <= values[cv->second_op])))
	    {
	      printf ("\n\n inconsistency in cvars array\n\n");
	      printf ("\nop: >");
	      printf ("\nfirst   : %5d:%8f", cv->first_op,
		      values[cv->first_op]);
	      printf ("\nsecond  : %5d:%8f", cv->second_op,
		      values[cv->second_op]);
	      printf ("\ncorrect : %d",
		      (values[cv->first_op] > values[cv->second_op]));
	      printf ("\nreported: %f\n\n", values[i]);
	      assert (0);
	      exit (1);
	    }
	  break;
	case GREATER_OR_EQUAL_OP:
	  if (((values[i] < 0.5)
	       && (values[cv->first_op] >= values[cv->second_op]))
	      || ((values[i] > 0.5)
		  && (values[cv->first_op] < values[cv->second_op])))
	    {
	      printf ("\n\n inconsistency in cvars array, pos %d\n\n", i);
	      printf ("\nop: >");
	      printf ("\nfirst   : %5d:%8f", cv->first_op,
		      values[cv->first_op]);
	      printf ("\nsecond  : %5d:%8f", cv->second_op,
		      values[cv->second_op]);
	      printf ("\ncorrect : %d",
		      (values[cv->first_op] >= values[cv->second_op]));
	      printf ("\nreported: %f\n\n", values[i]);
	      assert (0);
	      exit (1);
	    }
	  break;

	case INCREASE_OP:
	case DECREASE_OP:
	case SCALE_UP_OP:
	case SCALE_DOWN_OP:
	case ASSIGN_OP:
	case MINIMIZE_OP:
	case MAXIMIZE_OP:
	case FIX_NUMBER:
	case VARIABLE_OP:

	  break;
	default:
	  printf ("\nOperator %d not yet supported in consistency check\n\n",
		  cv->operator);

	  break;
	}
    }
}
