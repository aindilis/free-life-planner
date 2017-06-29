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
 * File: mutex.c
 * Description: computing mutex relations.
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/





#include "lpg.h"
#include "ComputeReachInf.h"
#include "inst_utils.h"
#include "utilities.h"
#include "output.h"






int
are_mutex_ops (int a, int b)
{
  int i, j, k;
  static Bool first_call = TRUE;



  if (!GpG.lowmemory)
    {
      i = GET_BIT (EF_EF_mutex[a], b);
      return i;
    }
  else
    {


      for (i = 0; i < gef_conn[a].num_PC; i++)
	{
	  if (gef_conn[a].PC[i] < 0)
	    continue;

	  for (j = 0; j < gef_conn[b].num_PC; j++)
	    {
	      if (gef_conn[b].PC[j] < 0)
		continue;
	      if (GET_BIT (FT_FT_mutex[gef_conn[a].PC[i]], gef_conn[b].PC[j]))
		return TRUE;
	    }
	  if (gef_conn[b].sf)
	    {

	      for (j = 0; j < gef_conn[b].sf->num_PC_overall; j++)
		{
		  if (gef_conn[b].sf->PC_overall[j] < 0)
		    continue;
		  if (GET_BIT
		      (FT_FT_mutex[gef_conn[a].PC[i]],
		       gef_conn[b].sf->PC_overall[j]))
		    return TRUE;
		}

	      for (j = 0; j < gef_conn[b].sf->num_PC_end; j++)
		{
		  if (gef_conn[b].sf->PC_end[j] < 0)
		    continue;
		  if (GET_BIT
		      (FT_FT_mutex[gef_conn[a].PC[i]],
		       gef_conn[b].sf->PC_end[j]))
		    return TRUE;
		}


	      for (j = 0; j < gef_conn[b].num_D; j++)
		{
		  if (gef_conn[b].D[j] < 0)
		    continue;
		  if (gef_conn[a].PC[i] == gef_conn[b].D[j])
		    return TRUE;
		}


	      if (gef_conn[b].sf)
		{
		  for (j = 0; j < gef_conn[b].sf->num_D_start; j++)
		    {
		      if (gef_conn[b].sf->D_start[j] < 0)
			continue;
		      if (gef_conn[a].PC[i] == gef_conn[b].sf->D_start[j])
			return TRUE;
		    }
		}
	    }
	}
      if (gef_conn[a].sf)
	{

	  for (i = 0; i < gef_conn[a].sf->num_PC_overall; i++)
	    {
	      if (gef_conn[a].sf->PC_overall[i] < 0)
		continue;

	      for (j = 0; j < gef_conn[b].num_PC; j++)
		{
		  if (gef_conn[b].PC[j] < 0)
		    continue;
		  if (GET_BIT
		      (FT_FT_mutex[gef_conn[a].sf->PC_overall[i]],
		       gef_conn[b].PC[j]))
		    return TRUE;
		}
	      if (gef_conn[b].sf)
		{

		  for (j = 0; j < gef_conn[b].sf->num_PC_overall; j++)
		    {
		      if (gef_conn[b].sf->PC_overall[j] < 0)
			continue;
		      if (GET_BIT
			  (FT_FT_mutex[gef_conn[a].sf->PC_overall[i]],
			   gef_conn[b].sf->PC_overall[j]))
			return TRUE;
		    }

		  for (j = 0; j < gef_conn[b].sf->num_PC_end; j++)
		    {
		      if (gef_conn[b].sf->PC_end[j] < 0)
			continue;
		      if (GET_BIT
			  (FT_FT_mutex[gef_conn[a].sf->PC_overall[i]],
			   gef_conn[b].sf->PC_end[j]))
			return TRUE;
		    }


		  for (j = 0; j < gef_conn[b].num_D; j++)
		    {
		      if (gef_conn[b].D[j] < 0)
			continue;
		      if (gef_conn[a].sf->PC_overall[i] == gef_conn[b].D[j])
			return TRUE;
		    }


		  if (gef_conn[b].sf)
		    {
		      for (j = 0; j < gef_conn[b].sf->num_D_start; j++)
			{
			  if (gef_conn[b].sf->D_start[j] < 0)
			    continue;
			  if (gef_conn[a].sf->PC_overall[i] ==
			      gef_conn[b].sf->D_start[j])
			    return TRUE;
			}
		    }
		}
	    }


	  for (i = 0; i < gef_conn[a].sf->num_PC_end; i++)
	    {
	      if (gef_conn[a].sf->PC_end[i] < 0)
		continue;

	      for (j = 0; j < gef_conn[b].num_PC; j++)
		{
		  if (gef_conn[b].PC[j] < 0)
		    continue;
		  if (GET_BIT
		      (FT_FT_mutex[gef_conn[a].sf->PC_end[i]],
		       gef_conn[b].PC[j]))
		    return TRUE;
		}
	      if (gef_conn[b].sf)
		{

		  for (j = 0; j < gef_conn[b].sf->num_PC_overall; j++)
		    {
		      if (gef_conn[b].sf->PC_overall[j] < 0)
			continue;
		      if (GET_BIT
			  (FT_FT_mutex[gef_conn[a].sf->PC_end[i]],
			   gef_conn[b].sf->PC_overall[j]))
			return TRUE;
		    }

		  for (j = 0; j < gef_conn[b].sf->num_PC_end; j++)
		    {
		      if (gef_conn[b].sf->PC_end[j] < 0)
			continue;
		      if (GET_BIT
			  (FT_FT_mutex[gef_conn[a].sf->PC_end[i]],
			   gef_conn[b].sf->PC_end[j]))
			return TRUE;
		    }


		  for (j = 0; j < gef_conn[b].num_D; j++)
		    {
		      if (gef_conn[b].D[j] < 0)
			continue;
		      if (gef_conn[a].sf->PC_end[i] == gef_conn[b].D[j])
			return TRUE;
		    }


		  if (gef_conn[b].sf)
		    {
		      for (j = 0; j < gef_conn[b].sf->num_D_start; j++)
			{
			  if (gef_conn[b].sf->D_start[j] < 0)
			    continue;
			  if (gef_conn[a].sf->PC_end[i] ==
			      gef_conn[b].sf->D_start[j])
			    return TRUE;
			}
		    }

		}
	    }
	}



      for (i = 0; i < gef_conn[a].num_D; i++)
	{
	  if (gef_conn[a].D[i] < 0)
	    continue;


	  for (j = 0; j < gef_conn[b].num_PC; j++)
	    {
	      if (gef_conn[b].PC[j] < 0)
		continue;
	      if (gef_conn[a].D[i] == gef_conn[b].PC[j])
		return TRUE;
	    }

	  if (gef_conn[b].sf)
	    {

	      for (j = 0; j < gef_conn[b].sf->num_PC_overall; j++)
		{
		  if (gef_conn[b].sf->PC_overall[j] < 0)
		    continue;
		  if (gef_conn[a].D[i] == gef_conn[b].sf->PC_overall[j])
		    return TRUE;
		}


	      for (j = 0; j < gef_conn[b].sf->num_PC_end; j++)
		{
		  if (gef_conn[b].sf->PC_end[j] < 0)
		    continue;
		  if (gef_conn[a].D[i] == gef_conn[b].sf->PC_end[j])
		    return TRUE;

		}
	    }
	}


      if (gef_conn[a].sf)
	for (i = 0; i < gef_conn[a].sf->num_D_start; i++)
	  {
	    if (gef_conn[a].sf->D_start[i] < 0)
	      continue;

	    for (j = 0; j < gef_conn[b].num_PC; j++)
	      {
		if (gef_conn[b].PC[j] < 0)
		  continue;
		if (gef_conn[a].sf->D_start[i] == gef_conn[b].PC[j])
		  return TRUE;
	      }
	    if (gef_conn[b].sf)
	      {

		for (j = 0; j < gef_conn[b].sf->num_PC_overall; j++)
		  {
		    if (gef_conn[b].sf->PC_overall[j] < 0)
		      continue;
		    if (gef_conn[a].sf->D_start[i] ==
			gef_conn[b].sf->PC_overall[j])
		      return TRUE;
		  }

		for (j = 0; j < gef_conn[b].sf->num_PC_end; j++)
		  {
		    if (gef_conn[b].sf->PC_end[j] < 0)
		      continue;
		    if (gef_conn[a].sf->D_start[i] ==
			gef_conn[b].sf->PC_end[j])
		      return TRUE;


		  }
	      }
	  }



      if (first_call)
	{
	  first_call = FALSE;
	  if (!l_vals)
	    l_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
	  if (!lstar_vals)
	    lstar_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
	  if (!r_vals)
	    r_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
	  if (!tested_vars)
	    tested_vars = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
	  set_lvals_and_rvals ();
	}
      for (k = 0; k < gnum_fullnum_blocks; k++)
	{

	  if (l_vals[a][k] & r_vals[b][k])
	    return TRUE;

	  if (l_vals[a][k] & tested_vars[b][k])
	    return TRUE;

	  if ((l_vals[a][k] & l_vals[b][k]) !=
	      (lstar_vals[a][k] & lstar_vals[b][k]))
	    return TRUE;
	}
    }

  return FALSE;
}



int
are_mutex_ops_online (int a, int b)
{
  int i, j, k;
  static Bool first_call = TRUE;



  for (i = 0; i < gef_conn[a].num_PC; i++)
    {
      if (gef_conn[a].PC[i] < 0)
	continue;

      for (j = 0; j < gef_conn[b].num_PC; j++)
	{
	  if (gef_conn[b].PC[j] < 0)
	    continue;
	  if (GET_BIT (FT_FT_mutex[gef_conn[a].PC[i]], gef_conn[b].PC[j]))
	    return TRUE;
	}
      if (gef_conn[b].sf)
	{

	  for (j = 0; j < gef_conn[b].sf->num_PC_overall; j++)
	    {
	      if (gef_conn[b].sf->PC_overall[j] < 0)
		continue;
	      if (GET_BIT
		  (FT_FT_mutex[gef_conn[a].PC[i]],
		   gef_conn[b].sf->PC_overall[j]))
		return TRUE;
	    }

	  for (j = 0; j < gef_conn[b].sf->num_PC_end; j++)
	    {
	      if (gef_conn[b].sf->PC_end[j] < 0)
		continue;
	      if (GET_BIT
		  (FT_FT_mutex[gef_conn[a].PC[i]], gef_conn[b].sf->PC_end[j]))
		return TRUE;
	    }
	}
    }
  if (gef_conn[a].sf)
    {

      for (i = 0; i < gef_conn[a].sf->num_PC_overall; i++)
	{
	  if (gef_conn[a].sf->PC_overall[i] < 0)
	    continue;

	  for (j = 0; j < gef_conn[b].num_PC; j++)
	    {
	      if (gef_conn[b].PC[j] < 0)
		continue;
	      if (GET_BIT
		  (FT_FT_mutex[gef_conn[a].sf->PC_overall[i]],
		   gef_conn[b].PC[j]))
		return TRUE;
	    }
	  if (gef_conn[b].sf)
	    {

	      for (j = 0; j < gef_conn[b].sf->num_PC_overall; j++)
		{
		  if (gef_conn[b].sf->PC_overall[j] < 0)
		    continue;
		  if (GET_BIT
		      (FT_FT_mutex[gef_conn[a].sf->PC_overall[i]],
		       gef_conn[b].sf->PC_overall[j]))
		    return TRUE;
		}

	      for (j = 0; j < gef_conn[b].sf->num_PC_end; j++)
		{
		  if (gef_conn[b].sf->PC_end[j] < 0)
		    continue;
		  if (GET_BIT
		      (FT_FT_mutex[gef_conn[a].sf->PC_overall[i]],
		       gef_conn[b].sf->PC_end[j]))
		    return TRUE;
		}
	    }
	}

      for (i = 0; i < gef_conn[a].sf->num_PC_end; i++)
	{
	  if (gef_conn[a].sf->PC_end[i] < 0)
	    continue;

	  for (j = 0; j < gef_conn[b].num_PC; j++)
	    {
	      if (gef_conn[b].PC[j] < 0)
		continue;
	      if (GET_BIT
		  (FT_FT_mutex[gef_conn[a].sf->PC_end[i]], gef_conn[b].PC[j]))
		return TRUE;
	    }
	  if (gef_conn[b].sf)
	    {

	      for (j = 0; j < gef_conn[b].sf->num_PC_overall; j++)
		{
		  if (gef_conn[b].sf->PC_overall[j] < 0)
		    continue;
		  if (GET_BIT
		      (FT_FT_mutex[gef_conn[a].sf->PC_end[i]],
		       gef_conn[b].sf->PC_overall[j]))
		    return TRUE;
		}

	      for (j = 0; j < gef_conn[b].sf->num_PC_end; j++)
		{
		  if (gef_conn[b].sf->PC_end[j] < 0)
		    continue;
		  if (GET_BIT
		      (FT_FT_mutex[gef_conn[a].sf->PC_end[i]],
		       gef_conn[b].sf->PC_end[j]))
		    return TRUE;
		}
	    }
	}
    }

  if (first_call)
    {
      first_call = FALSE;
      l_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
      lstar_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
      r_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
      set_lvals_and_rvals ();
    }
  for (k = 0; k < gnum_fullnum_blocks; k++)
    {
      if (l_vals[a][k] & r_vals[b][k])
	return TRUE;

      if (l_vals[a][k] & tested_vars[b][k])
	return TRUE;

      if ((l_vals[a][k] & l_vals[b][k]) !=
	  (lstar_vals[a][k] | lstar_vals[b][k]))
	return TRUE;
    }

  return FALSE;
}




void
calc_mutex_num_efs (void)
{
  int i, j, k;

  if (!l_vals)
    l_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
  if (!lstar_vals)
    lstar_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
  if (!r_vals)
    r_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
  if (!tested_vars)
    tested_vars = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);

  set_lvals_and_rvals ();

  for (i = 0; i < gnum_ef_conn; i++)
    {
      if (!gef_conn[i].is_numeric)
	continue;

      for (j = i; j < gnum_ef_conn; j++)
	{
	  if (GET_BIT (EF_EF_mutex[i], j))
	    continue;

	  for (k = 0; k < gnum_fullnum_blocks; k++)
	    {
	      if (l_vals[i][k] & r_vals[j][k])
		{
		  SET_BIT (EF_EF_mutex[i], j);
		  SET_BIT (EF_EF_mutex[j], i);

		  break;
		}

	      if (l_vals[i][k] & tested_vars[j][k])
		{
		  SET_BIT (EF_EF_mutex[i], j);
		  SET_BIT (EF_EF_mutex[j], i);
		  break;
		}
	      if (l_vals[j][k] & tested_vars[i][k])
		{
		  SET_BIT (EF_EF_mutex[i], j);
		  SET_BIT (EF_EF_mutex[j], i);
		  break;
		}




	      if ((l_vals[i][k] & l_vals[j][k]) !=
		  (lstar_vals[i][k] & lstar_vals[j][k]))
		{
		  SET_BIT (EF_EF_mutex[i], j);
		  SET_BIT (EF_EF_mutex[j], i);

		  break;
		}
	    }
	}
    }


}



Bool
are_goal_reachable_and_non_mutex (void)
{
  int i, j;

  for (i = 0; i < ggoal_state.num_F; i++)
    if (ggoal_state.F[i] >= 0 && gft_conn[ggoal_state.F[i]].level < 0)
      {
	printf ("Goal unreachable %d", ggoal_state.F[i]);
	print_ft_name (ggoal_state.F[i]);
	return FALSE;
      }
  for (i = 0; i < ggoal_state.num_F; i++)
    for (j = i + 1; j < ggoal_state.num_F; j++)
      if (GET_BIT (FT_FT_mutex[ggoal_state.F[i]], ggoal_state.F[j]))
	{
	  printf ("Mutex Goals: ");
	  print_ft_name (ggoal_state.F[i]);
	  printf ("--");
	  print_ft_name (ggoal_state.F[j]);
	  return FALSE;
	}

  return TRUE;
}





int
count_unsatisfied_preconds (int action_number, int *fact_vector)
{
  int i = 0;
  int num_unsatisfied_precond = 0;
  EfConn action_considered = gef_conn[action_number];


  for (i = 0; i < action_considered.num_PC; i++)
    {
      if (action_considered.PC[i] >= 0
	  && !(GET_BIT (fact_vector, action_considered.PC[i])))
	num_unsatisfied_precond++;
    }



  if (action_considered.sf != NULL)
    {

      for (i = 0; i < action_considered.sf->num_PC_overall; i++)
	{
	  if (action_considered.sf->PC_overall[i] < 0)
	    continue;
	  if (GET_BIT (fact_vector, action_considered.sf->PC_overall[i]))
	    continue;

	  if (is_fact_in_preconditions
	      (action_number, action_considered.sf->PC_overall[i]))
	    continue;

	  if (is_fact_in_additive_effects_start
	      (action_number, action_considered.sf->PC_overall[i]))
	    continue;

	  num_unsatisfied_precond++;

	}


      for (i = 0; i < action_considered.sf->num_PC_end; i++)

	{
	  if (action_considered.sf->PC_end[i] < 0)
	    continue;

	  if (GET_BIT (fact_vector, action_considered.sf->PC_end[i]))
	    continue;

	  if (is_fact_in_preconditions
	      (action_number, action_considered.sf->PC_end[i]))
	    continue;

	  if (is_fact_in_preconditions_overall
	      (action_number, action_considered.sf->PC_end[i]))
	    continue;

	  if (is_fact_in_additive_effects_start
	      (action_number, action_considered.sf->PC_end[i]))
	    continue;


	  num_unsatisfied_precond++;
	}
    }
  return num_unsatisfied_precond;
}


int
is_action_prec_mutex (int action_number, int **FT_FT_mutex_pass)
{
  int i, j;
  EfConn action_considered = gef_conn[action_number];

  for (i = 0; i < action_considered.num_PC; i++)

    {
      for (j = i + 1; j < action_considered.num_PC - 1; j++)
	if (action_considered.PC[i] >= 0 && action_considered.PC[j] >= 0 &&
	    GET_BIT (FT_FT_mutex_pass[action_considered.PC[i]],
		     action_considered.PC[j]))
	  return TRUE;

    }
  if (action_considered.sf != NULL)
    {
      for (i = 0; i < action_considered.sf->num_PC_overall - 1; i++)

	{
	  for (j = i + 1; j < action_considered.sf->num_PC_overall; j++)
	    if (action_considered.sf->PC_overall[i] >= 0
		&& action_considered.sf->PC_overall[j] >= 0
		&&
		GET_BIT (FT_FT_mutex_pass
			 [action_considered.sf->PC_overall[i]],
			 action_considered.sf->PC_overall[j]))
	      return TRUE;

	}

      for (i = 0; i < action_considered.sf->num_PC_end - 1; i++)

	{
	  for (j = i + 1; j < action_considered.sf->num_PC_end; j++)
	    if (action_considered.sf->PC_end[i] >= 0
		&& action_considered.sf->PC_end[j]
		&& GET_BIT (FT_FT_mutex_pass[action_considered.sf->PC_end[i]],
			    action_considered.sf->PC_end[j]))
	      return TRUE;

	}

    }

  return FALSE;
}

void
make_actions_num_prec_vector (int *action_num_prec_vector, int *fact_vector)
{
  int i = 0;

  for (i = 0; i < gnum_ef_conn; i++)
    action_num_prec_vector[i] = count_unsatisfied_preconds (i, fact_vector);


}






void
calc_mutex (State * initial_state)
{
  Bool changed;


  State A, C;

  register int i, j, k, l, m, n, p, pp, ppp, ll, aus_counter;
  register int y;
  register int temp;
  int num_facts = 0;
  int num_ops = 0;


  int *applied;

  int *To_be_Executed;




  int *num_prec_vector = NULL;

  num_prec_vector = alloc_vect (gnum_ef_conn);
  assert (num_prec_vector);

  applied = alloc_vect (gnum_ef_block);






  if (!FT_FT_mutex)
    FT_FT_mutex = alloc_matrix (gnum_ft_conn, gnum_ft_block);
  if (!l_vals)
    l_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
  if (!lstar_vals)
    lstar_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
  if (!r_vals)
    r_vals = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);
  if (!tested_vars)
    tested_vars = alloc_matrix (gnum_ef_conn, gnum_fullnum_blocks);


  if (GpG.accurate_cost >= 1 && Hvar.init_facts_array == NULL)
    {

      Hvar.init_facts_array =
	(dg_inform_list *) calloc (gnum_ft_conn, sizeof (dg_inform_list));
      Hvar.dg_facts_min_duration =
	(float *) calloc (gnum_ft_conn, sizeof (float));
      Hvar.bit_vect_facts = alloc_vect (gnum_ft_block);
      Hvar.bit_vect_actions = alloc_vect (gnum_ef_block);

    }


  assert (FT_FT_mutex);

  To_be_Executed = alloc_vect (gnum_ef_block);
  assert (To_be_Executed);



  if (F == NULL)
    {
      F = alloc_vect (gnum_ft_block);
      assert (F);
    }

  else
    reset_bitarray (F, gnum_ft_block);


  gmutex_level = 0;

  for (i = 0; i < gnum_ef_conn; i++)
    {
      gef_conn[i].in_E = FALSE;
      gef_conn[i].level = -1;
    }

  for (i = 0; i < gnum_ft_conn; i++)
    {
      gft_conn[i].in_F = FALSE;
      gft_conn[i].level = -1;
    }




  for (i = 0; i < initial_state->num_F; i++)
    {
      SET_BIT (F, initial_state->F[i]);


      gft_conn[initial_state->F[i]].in_F = TRUE;
      gft_conn[initial_state->F[i]].level = gmutex_level;

      num_facts++;

      if (GpG.accurate_cost >= 1)
	cri_insert_ftcost (initial_state->F[i], 0, 0, 0, INITIAL_ACTION);

    }

  if (FALSE)
    {


      for (i = 0; i < gnum_ef_conn; i++)
	{
	  for (j = 0; j < gef_conn[i].num_A; j++)
	    for (k = 0; k < gef_conn[i].num_D; k++)
	      {
		if (gef_conn[i].A[j] < 0 || gef_conn[i].D[k] < 0)
		  continue;
		if (GET_BIT (F, gef_conn[i].A[j]))
		  if (GET_BIT (F, gef_conn[i].D[k]))
		    continue;

		SET_BIT (FT_FT_mutex[gef_conn[i].A[j]], gef_conn[i].D[k]);
		SET_BIT (FT_FT_mutex[gef_conn[i].D[k]], gef_conn[i].A[j]);
	      }


	  if (gef_conn[i].sf != NULL)
	    for (j = 0; j < gef_conn[i].sf->num_A_start; j++)
	      for (k = 0; k < gef_conn[i].sf->num_D_start; k++)
		{
		  if (gef_conn[i].sf->A_start[j] < 0
		      || gef_conn[i].sf->D_start[k] < 0)
		    continue;
		  if (GET_BIT (F, gef_conn[i].sf->A_start[j]))
		    if (GET_BIT (F, gef_conn[i].sf->D_start[k]))
		      continue;

		  SET_BIT (FT_FT_mutex[gef_conn[i].sf->A_start[j]],
			   gef_conn[i].sf->D_start[k]);
		  SET_BIT (FT_FT_mutex[gef_conn[i].sf->D_start[k]],
			   gef_conn[i].sf->A_start[j]);
		}

	}
    }








  make_actions_num_prec_vector (num_prec_vector, F);


  do
    {



      changed = FALSE;
      reset_bitarray (To_be_Executed, gnum_ef_block);
      for (i = 0; i < gnum_ef_conn; i++)
	if (!GET_BIT (applied, i))
	  if (num_prec_vector[i] == 0)
	    SET_BIT (To_be_Executed, i);



      for (i = 0; i < gnum_ef_conn; i++)
	{
	  if (!GET_BIT (applied, i))
	    {


	      if (!GET_BIT (To_be_Executed, i))
		continue;
	      if (is_action_prec_mutex (i, FT_FT_mutex))
		continue;
	      if (GpG.accurate_cost >= 1)
		cri_activate_distgraph_ef (i);

	      SET_BIT (applied, i);
	      A.num_F = 0;
	      for (j = 0; j < gef_conn[i].num_A; j++)
		{
		  if (gef_conn[i].A[j] >= 0)
		    if (!GET_BIT (F, gef_conn[i].A[j]))
		      {
			gft_conn[gef_conn[i].A[j]].in_F = TRUE;
			gft_conn[gef_conn[i].A[j]].level = gmutex_level + 1;
			SET_BIT (F, gef_conn[i].A[j]);


			for (aus_counter = 0;
			     aus_counter < gft_conn[gef_conn[i].A[j]].num_PC;
			     aus_counter++)
			  {
			    if (GpG.temporal_plan)
			      {
				if (is_fact_in_preconditions_end
				    (gft_conn[gef_conn[i].A[j]].
				     PC[aus_counter], gef_conn[i].A[j])
				    &&
				    is_fact_in_additive_effects_start
				    (gft_conn[gef_conn[i].A[j]].
				     PC[aus_counter], gef_conn[i].A[j]))
				  continue;

				if (is_fact_in_preconditions_overall
				    (gft_conn[gef_conn[i].A[j]].
				     PC[aus_counter], gef_conn[i].A[j])
				    &&
				    is_fact_in_additive_effects_start
				    (gft_conn[gef_conn[i].A[j]].
				     PC[aus_counter], gef_conn[i].A[j]))
				  continue;
			      }
			    num_prec_vector[gft_conn[gef_conn[i].A[j]].
					    PC[aus_counter]]--;
			    if (num_prec_vector
				[gft_conn[gef_conn[i].A[j]].
				 PC[aus_counter]] == 0)
			      changed = TRUE;

			  }
			A.F[A.num_F++] = gef_conn[i].A[j];
			num_facts++;
		      }
		}

	      if (gef_conn[i].sf)
		{
		  for (j = 0; j < gef_conn[i].sf->num_A_start; j++)
		    {
		      if (gef_conn[i].sf->A_start[j] >= 0)
			if (!GET_BIT (F, gef_conn[i].sf->A_start[j]))
			  {
			    gft_conn[gef_conn[i].sf->A_start[j]].in_F = TRUE;
			    gft_conn[gef_conn[i].sf->A_start[j]].level =
			      gmutex_level + 1;
			    SET_BIT (F, gef_conn[i].sf->A_start[j]);


			    for (aus_counter = 0;
				 aus_counter <
				 gft_conn[gef_conn[i].sf->A_start[j]].num_PC;
				 aus_counter++)
			      {
				if (GpG.temporal_plan)
				  {
				    if (is_fact_in_preconditions_end
					(gft_conn[gef_conn[i].sf->A_start[j]].
					 PC[aus_counter],
					 gef_conn[i].sf->A_start[j])
					&&
					is_fact_in_additive_effects_start
					(gft_conn[gef_conn[i].sf->A_start[j]].
					 PC[aus_counter], gef_conn[i].A[j]))
				      continue;

				    if (is_fact_in_preconditions_overall
					(gft_conn[gef_conn[i].sf->A_start[j]].
					 PC[aus_counter],
					 gef_conn[i].sf->A_start[j])
					&&
					is_fact_in_additive_effects_start
					(gft_conn[gef_conn[i].sf->A_start[j]].
					 PC[aus_counter],
					 gef_conn[i].sf->A_start[j]))
				      continue;
				  }
				num_prec_vector[gft_conn
						[gef_conn[i].sf->A_start[j]].
						PC[aus_counter]]--;
				if (num_prec_vector
				    [gft_conn[gef_conn[i].sf->A_start[j]].
				     PC[aus_counter]] == 0)
				  changed = TRUE;
			      }
			    A.F[A.num_F++] = gef_conn[i].sf->A_start[j];
			    num_facts++;
			  }
		    }
		}
	      if (!gef_conn[i].in_E)
		{
		  gef_conn[i].in_E = TRUE;
		  gef_conn[i].level = gmutex_level;
		  num_ops++;
		}
	      for (j = 0; j < A.num_F; j++)
		{
		  for (k = 0; k < gef_conn[i].num_D; k++)
		    {

		      if (A.F[j] == gef_conn[i].D[k])
			continue;




		      if (!GET_BIT (FT_FT_mutex[A.F[j]], gef_conn[i].D[k]))
			gnum_mutex++;
		      else
			continue;
		      SET_BIT (FT_FT_mutex[A.F[j]], gef_conn[i].D[k]);
		      SET_BIT (FT_FT_mutex[gef_conn[i].D[k]], A.F[j]);

		      changed = TRUE;
		    }

		  if (gef_conn[i].sf)
		    {
		      for (k = 0; k < gef_conn[i].sf->num_D_start; k++)
			{
			  if (A.F[j] == gef_conn[i].sf->D_start[k])
			    continue;

			  if (!GET_BIT
			      (FT_FT_mutex[A.F[j]],
			       gef_conn[i].sf->D_start[k]))
			    gnum_mutex++;
			  else
			    continue;
			  SET_BIT (FT_FT_mutex[A.F[j]],
				   gef_conn[i].sf->D_start[k]);
			  SET_BIT (FT_FT_mutex[gef_conn[i].sf->D_start[k]],
				   A.F[j]);

			  changed = TRUE;
			}
		    }
		  for (k = 0; k < gef_conn[i].num_PC; k++)
		    if ((gef_conn[i].PC[k] >= 0))
		      {
			for (ll = 0, l = 0; l < gnum_ft_block; l++, ll += 32)
			  {
			    temp = FT_FT_mutex[gef_conn[i].PC[k]][l];
			    y = 32;
			    while (temp)
			      {
				y--;
				if (temp & FIRST_1)
				  {
				    m = ll + y;
				    if (A.F[j] != m)
				      {

					if (!GET_BIT (FT_FT_mutex[A.F[j]], m))
					  {
					    gnum_mutex++;

					    SET_BIT (FT_FT_mutex[A.F[j]], m);
					    SET_BIT (FT_FT_mutex[m], A.F[j]);
					    changed = TRUE;
					  }
				      }
				  }
				temp <<= 1;
			      }
			  }
		      }
		  if (gef_conn[i].sf)
		    {

		      for (k = 0; k < gef_conn[i].sf->num_PC_overall; k++)
			if ((gef_conn[i].sf->PC_overall[k] >= 0))
			  {
			    for (ll = 0, l = 0; l < gnum_ft_block;
				 l++, ll += 32)
			      {
				temp =
				  FT_FT_mutex[gef_conn[i].sf->
					      PC_overall[k]][l];
				y = 32;
				while (temp)
				  {
				    y--;
				    if (temp & FIRST_1)
				      {
					m = ll + y;
					if (A.F[j] != m)
					  {
					    if (!GET_BIT
						(FT_FT_mutex[A.F[j]], m))
					      {
						gnum_mutex++;

						SET_BIT (FT_FT_mutex[A.F[j]],
							 m);
						SET_BIT (FT_FT_mutex[m],
							 A.F[j]);
						changed = TRUE;
					      }
					  }
				      }
				    temp <<= 1;
				  }
			      }
			  }

		      for (k = 0; k < gef_conn[i].sf->num_PC_end; k++)
			if ((gef_conn[i].sf->PC_end[k] >= 0))
			  {
			    for (ll = 0, l = 0; l < gnum_ft_block;
				 l++, ll += 32)
			      {
				temp =
				  FT_FT_mutex[gef_conn[i].sf->PC_end[k]][l];
				y = 32;
				while (temp)
				  {
				    y--;
				    if (temp & FIRST_1)
				      {
					m = ll + y;
					if (A.F[j] != m)
					  {

					    if (!GET_BIT
						(FT_FT_mutex[A.F[j]], m))
					      {
						gnum_mutex++;

						SET_BIT (FT_FT_mutex[A.F[j]],
							 m);
						SET_BIT (FT_FT_mutex[m],
							 A.F[j]);
						changed = TRUE;
					      }
					  }
				      }
				    temp <<= 1;
				  }
			      }
			  }
		    }
		}



	      for (j = 0; j < gef_conn[i].num_A; j++)
		for (k = j + 1; k < gef_conn[i].num_A; k++)
		  if ((gef_conn[i].A[j] >= 0) && (gef_conn[i].A[k] >= 0))
		    {
		      if (GET_BIT
			  (FT_FT_mutex[gef_conn[i].A[j]], gef_conn[i].A[k]))
			gnum_mutex--;
		      else
			continue;

		      RESET_BIT (FT_FT_mutex[gef_conn[i].A[j]],
				 gef_conn[i].A[k]);
		      RESET_BIT (FT_FT_mutex[gef_conn[i].A[k]],
				 gef_conn[i].A[j]);
		    }
	      if (gef_conn[i].sf)
		{



		  for (j = 0; j < gef_conn[i].sf->num_A_start; j++)
		    for (k = j + 1; k < gef_conn[i].sf->num_A_start; k++)
		      if ((gef_conn[i].sf->A_start[j] >= 0)
			  && (gef_conn[i].sf->A_start[k] >= 0))
			{
			  if (GET_BIT
			      (FT_FT_mutex[gef_conn[i].sf->A_start[j]],
			       gef_conn[i].sf->A_start[k]))
			    gnum_mutex--;
			  else
			    continue;
			  RESET_BIT (FT_FT_mutex[gef_conn[i].sf->A_start[j]],
				     gef_conn[i].sf->A_start[k]);
			  RESET_BIT (FT_FT_mutex[gef_conn[i].sf->A_start[k]],
				     gef_conn[i].sf->A_start[j]);
			}

		  for (j = 0; j < gef_conn[i].sf->num_A_start; j++)
		    for (k = 0; k < gef_conn[i].num_A; k++)
		      if ((gef_conn[i].sf->A_start[j] >= 0)
			  && (gef_conn[i].A[k] >= 0))
			{
			  if (GET_BIT
			      (FT_FT_mutex[gef_conn[i].sf->A_start[j]],
			       gef_conn[i].A[k]))
			    gnum_mutex--;
			  else
			    continue;
			  RESET_BIT (FT_FT_mutex[gef_conn[i].sf->A_start[j]],
				     gef_conn[i].A[k]);
			  RESET_BIT (FT_FT_mutex[gef_conn[i].A[k]],
				     gef_conn[i].sf->A_start[j]);
			}







		  if (FALSE)
		    {
		      for (j = 0; j < gef_conn[i].sf->num_A_start; j++)
			for (k = 0; k < gef_conn[i].num_D; k++)
			  if ((gef_conn[i].sf->A_start[j] >= 0)
			      && (gef_conn[i].sf->A_start[j] ==
				  gef_conn[i].D[k]))
			    {
			      if (GET_BIT
				  (FT_FT_mutex[gef_conn[i].sf->A_start[j]],
				   gef_conn[i].D[k]))
				gnum_mutex--;
			      else
				continue;
			      RESET_BIT (FT_FT_mutex
					 [gef_conn[i].sf->A_start[j]],
					 gef_conn[i].D[k]);
			      RESET_BIT (FT_FT_mutex[gef_conn[i].D[k]],
					 gef_conn[i].sf->A_start[j]);
			    }


		      for (j = 0; j < gef_conn[i].sf->num_D_start; j++)
			for (k = 0; k < gef_conn[i].num_A; k++)
			  if ((gef_conn[i].sf->D_start[j] >= 0)
			      && (gef_conn[i].sf->D_start[j] ==
				  gef_conn[i].A[k]))
			    {
			      if (GET_BIT
				  (FT_FT_mutex[gef_conn[i].sf->D_start[j]],
				   gef_conn[i].A[k]))
				gnum_mutex--;
			      else
				continue;
			      RESET_BIT (FT_FT_mutex
					 [gef_conn[i].sf->D_start[j]],
					 gef_conn[i].A[k]);
			      RESET_BIT (FT_FT_mutex[gef_conn[i].A[k]],
					 gef_conn[i].sf->D_start[j]);
			    }
		    }
		}
	    }
	  else
	    A.num_F = 0;



	  C.num_F = 0;
	  for (j = 0; j < gef_conn[i].num_A; j++)
	    {
	      if (gef_conn[i].A[j] >= 0)
		{
		  for (k = 0; k < A.num_F; k++)
		    if (A.F[k] == gef_conn[i].A[j])
		      break;
		  if (k != A.num_F)
		    continue;
		  C.F[C.num_F++] = gef_conn[i].A[j];
		}
	    }

	  if (gef_conn[i].sf)
	    {
	      for (j = 0; j < gef_conn[i].sf->num_A_start; j++)
		{
		  if (gef_conn[i].sf->A_start[j] >= 0)
		    {
		      for (k = 0; k < A.num_F; k++)
			if (A.F[k] == gef_conn[i].sf->A_start[j])
			  break;
		      if (k != A.num_F)
			continue;

		      C.F[C.num_F++] = gef_conn[i].sf->A_start[j];
		    }
		}
	    }

	  for (j = 0; j < C.num_F; j++)
	    {

	      for (m = 0; m < gnum_ft_block; m++)
		{
		  temp = FT_FT_mutex[C.F[j]][m];
		  y = 32;
		  while (temp)
		    {
		      y--;

		      if (temp & FIRST_1)
			{
			  n = m * 32 + y;

			  for (p = 0; p < gef_conn[i].num_D; p++)
			    if (n == gef_conn[i].D[p])
			      break;

			  if (gef_conn[i].sf)
			    {
			      for (pp = 0;
				   pp < gef_conn[i].sf->num_D_start; pp++)
				if (n == gef_conn[i].sf->D_start[pp])
				  break;
			      if ((p != gef_conn[i].num_D)
				  || (pp != gef_conn[i].sf->num_D_start))
				{

				  temp <<= 1;
				  continue;
				}
			    }
			  else
			    {
			      if (p != gef_conn[i].num_D)
				{

				  temp <<= 1;
				  continue;
				}
			    }

			  for (p = 0; p < gef_conn[i].num_PC; p++)
			    if (gef_conn[i].PC[p] >= 0)
			      {
				if (GET_BIT
				    (FT_FT_mutex[n], gef_conn[i].PC[p]))
				  break;
			      }


			  if (gef_conn[i].sf)
			    {
			      for (pp = 0;
				   pp < gef_conn[i].sf->num_PC_overall; pp++)
				if (gef_conn[i].sf->PC_overall[pp] >= 0)
				  {
				    if (GET_BIT
					(FT_FT_mutex[n],
					 gef_conn[i].sf->PC_overall[pp]))
				      break;
				  }
			      for (ppp = 0;
				   ppp < gef_conn[i].sf->num_PC_end; ppp++)
				if (gef_conn[i].sf->PC_end[ppp] >= 0)
				  {
				    if (GET_BIT
					(FT_FT_mutex[n],
					 gef_conn[i].sf->PC_end[ppp]))
				      break;
				  }
			      if ((p != gef_conn[i].num_PC)
				  || (pp !=
				      gef_conn[i].sf->num_PC_overall)
				  || (ppp != gef_conn[i].sf->num_PC_end))
				{
				  temp <<= 1;
				  continue;
				}
			    }
			  else
			    {
			      if (p != gef_conn[i].num_PC)
				{
				  temp <<= 1;
				  continue;
				}
			    }

			  changed = TRUE;

			  RESET_BIT (FT_FT_mutex[C.F[j]], n);
			  RESET_BIT (FT_FT_mutex[n], C.F[j]);

			  gnum_mutex--;


			}
		      temp <<= 1;
		    }
		}
	    }


	}
      gmutex_level++;
      if (DEBUG2)
	printf ("\n Level %3d:%5d facts, %5d ops and %5d mutex relations",
		gmutex_level, num_facts, num_ops, gnum_mutex);


    }
  while (changed);

  total_ft_ft_mutex = gnum_mutex * 2;

  free (applied);
  free (num_prec_vector);

  free (To_be_Executed);

  set_init_computed_dg_costs ();

}


void
calc_mutex_ops (void)
{
  register int i, j, k, l;
  register int temp, x, y, xx;


  if (!FT_EF_mutex)
    FT_EF_mutex = alloc_matrix (gnum_ft_conn, gnum_ef_block);
  if (!EF_FT_mutex)
    EF_FT_mutex = alloc_matrix (gnum_ef_conn, gnum_ft_block);
  if ((!GpG.lowmemory) && (!EF_EF_mutex))
    EF_EF_mutex = alloc_matrix (gnum_ef_conn, gnum_ef_block);

  for (i = 0; i < gnum_ft_conn; i++)
    {

      gft_conn[i].ft_exclusive_vect = FT_FT_mutex[i];
      gft_conn[i].ef_exclusive_vect = FT_EF_mutex[i];
    }

  for (i = 0; i < gnum_ef_conn; i++)
    {

      gef_conn[i].ft_exclusive_vect = EF_FT_mutex[i];
      if (!GpG.lowmemory)
	gef_conn[i].ef_exclusive_vect = EF_EF_mutex[i];

    }

  for (i = 0; i < gnum_ft_conn; i++)
    {

      for (x = GUID_BLOCK (i), xx = x << 5; x < gnum_ft_block; x++, xx += 32)
	{
	  temp = FT_FT_mutex[i][x];
	  y = 32;
	  while (temp)
	    {
	      y--;
	      if (temp & FIRST_1)
		{
		  j = xx + y;
		  if (j <= i)
		    {
		      temp <<= 1;
		      continue;
		    }


		  if (!GpG.lowmemory)
		    {
		      for (k = 0; k < gft_conn[i].num_PC; k++)
			{
			  for (l = 0; l < gft_conn[j].num_PC; l++)
			    {
			      if (GET_BIT
				  (EF_EF_mutex[gft_conn[i].PC[k]],
				   gft_conn[j].PC[l]))
				continue;

			      if (DEBUG2)
				total_ef_ef_mutex++;

			      SET_BIT (EF_EF_mutex[gft_conn[i].PC[k]],
				       gft_conn[j].PC[l]);
			      SET_BIT (EF_EF_mutex[gft_conn[j].PC[l]],
				       gft_conn[i].PC[k]);
			    }
			}
		    }

		  for (k = 0; k < gft_conn[j].num_PC; k++)
		    {

		      if (GET_BIT (FT_EF_mutex[i], gft_conn[j].PC[k]))
			continue;
		      if (DEBUG2)
			{
			  total_ft_ef_mutex++;
			  total_ef_ft_mutex++;
			}

		      SET_BIT (FT_EF_mutex[i], gft_conn[j].PC[k]);


		      SET_BIT (EF_FT_mutex[gft_conn[j].PC[k]], i);
		    }

		  for (k = 0; k < gft_conn[i].num_PC; k++)
		    {
		      if (GET_BIT (FT_EF_mutex[j], gft_conn[i].PC[k]))
			continue;
		      if (DEBUG2)
			{
			  total_ft_ef_mutex++;
			  total_ef_ft_mutex++;
			}
		      SET_BIT (FT_EF_mutex[j], gft_conn[i].PC[k]);


		      SET_BIT (EF_FT_mutex[gft_conn[i].PC[k]], j);
		    }
		}
	      temp <<= 1;
	    }
	}



      for (j = 0; j < gft_conn[i].num_D; j++)
	{
	  if (GET_BIT (FT_EF_mutex[i], gft_conn[i].D[j]))
	    continue;

	  if (DEBUG2)
	    {
	      total_ft_ef_mutex++;
	      total_ef_ft_mutex++;
	    }

	  SET_BIT (FT_EF_mutex[i], gft_conn[i].D[j]);


	  SET_BIT (EF_FT_mutex[gft_conn[i].D[j]], i);
	}

    }
  if (!GpG.lowmemory)
    {
      for (i = 0; i < gnum_ef_conn; i++)
	{
	  for (j = 0; j < gef_conn[i].num_PC; j++)
	    {
	      if (gef_conn[i].PC[j] >= 0)
		for (k = 0; k < gft_conn[gef_conn[i].PC[j]].num_D; k++)
		  {
		    if (gft_conn[gef_conn[i].PC[j]].D[k] >= 0)
		      {
			if (GET_BIT
			    (EF_EF_mutex[i],
			     gft_conn[gef_conn[i].PC[j]].D[k]))
			  continue;
			if (DEBUG2)
			  total_ef_ef_mutex++;

			SET_BIT (EF_EF_mutex[i],
				 gft_conn[gef_conn[i].PC[j]].D[k]);
			SET_BIT (EF_EF_mutex
				 [gft_conn[gef_conn[i].PC[j]].D[k]], i);
		      }

		  }

	    }
	  if (gef_conn[i].sf != NULL)
	    {
	      for (j = 0; j < gef_conn[i].sf->num_PC_end; j++)
		{
		  if (gef_conn[i].sf->PC_end[j] >= 0)
		    for (k = 0; k < gft_conn[gef_conn[i].sf->PC_end[j]].num_D;
			 k++)
		      {
			if (gft_conn[gef_conn[i].sf->PC_end[j]].D[k] >= 0)
			  {
			    if (GET_BIT
				(EF_EF_mutex[i],
				 gft_conn[gef_conn[i].sf->PC_end[j]].D[k]))
			      continue;
			    if (DEBUG2)
			      total_ef_ef_mutex++;

			    SET_BIT (EF_EF_mutex[i],
				     gft_conn[gef_conn[i].sf->PC_end[j]].
				     D[k]);
			    SET_BIT (EF_EF_mutex
				     [gft_conn[gef_conn[i].sf->PC_end[j]].
				      D[k]], i);
			  }

		      }

		}


	      for (j = 0; j < gef_conn[i].sf->num_PC_overall; j++)
		{
		  if (gef_conn[i].sf->PC_overall[j] >= 0)
		    for (k = 0;
			 k < gft_conn[gef_conn[i].sf->PC_overall[j]].num_D;
			 k++)
		      {
			if (gft_conn[gef_conn[i].sf->PC_overall[j]].D[k] >= 0)
			  {
			    if (GET_BIT
				(EF_EF_mutex[i],
				 gft_conn[gef_conn[i].sf->PC_overall[j]].
				 D[k]))
			      continue;
			    if (DEBUG2)
			      total_ef_ef_mutex++;

			    SET_BIT (EF_EF_mutex[i],
				     gft_conn[gef_conn[i].sf->PC_overall[j]].
				     D[k]);
			    SET_BIT (EF_EF_mutex
				     [gft_conn[gef_conn[i].sf->PC_overall[j]].
				      D[k]], i);
			  }

		      }

		}


	      for (j = 0; j < gef_conn[i].sf->num_A_start; j++)
		{
		  if (gef_conn[i].sf->A_start[j] >= 0)
		    for (k = 0;
			 k < gft_conn[gef_conn[i].sf->A_start[j]].num_D; k++)
		      {
			if (gft_conn[gef_conn[i].sf->A_start[j]].D[k] >= 0)
			  {
			    if (GET_BIT
				(EF_EF_mutex[i],
				 gft_conn[gef_conn[i].sf->A_start[j]].D[k]))
			      continue;
			    if (DEBUG2)
			      total_ef_ef_mutex++;

			    SET_BIT (EF_EF_mutex[i],
				     gft_conn[gef_conn[i].sf->A_start[j]].
				     D[k]);
			    SET_BIT (EF_EF_mutex
				     [gft_conn[gef_conn[i].sf->A_start[j]].
				      D[k]], i);
			  }

		      }

		}

	    }


	  for (j = 0; j < gef_conn[i].num_A; j++)
	    {
	      if (gef_conn[i].A[j] >= 0)
		for (k = 0; k < gft_conn[gef_conn[i].A[j]].num_D; k++)
		  {
		    if (gft_conn[gef_conn[i].A[j]].D[k] >= 0)
		      {
			if (GET_BIT
			    (EF_EF_mutex[i], gft_conn[gef_conn[i].A[j]].D[k]))
			  continue;
			if (DEBUG2)
			  total_ef_ef_mutex++;

			SET_BIT (EF_EF_mutex[i],
				 gft_conn[gef_conn[i].A[j]].D[k]);
			SET_BIT (EF_EF_mutex[gft_conn[gef_conn[i].A[j]].D[k]],
				 i);
		      }

		  }


	    }

	}
    }

}

void
remove_mutex_facts_in_bitvect (int fact, int *bit_vect)
{
  int i;

  for (i = 0; i < gnum_ft_block; i++)
    {
      bit_vect[i] &= ~gft_conn[fact].ft_exclusive_vect[i];

    }

}
