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
 * File: LpgOutput.c
 * Description: printing and storing  output info.
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/





#include "lpg.h"
#include "output.h"
#include "LpgOutput.h"
#include "check.h"
#include "utilities.h"
#include "H_relaxed.h"





void
print_NumVar (NumVar * f, int cv_index, int level)
{
  int j = 0;

  if (f == NULL)
    {
      printf ("NULL");
      return;
    }

  if (f->function == -3)
    {
      printf ("GOAL-REACHED");
      return;
    }

  if (f->function == -1)
    {
      printf ("=(");
      for (j = 0; j < 2; j++)
	{
	  if (f->args[j] >= 0)
	    {
	      printf ("%s", gconstants[(f->args)[j]]);
	    }
	  else
	    {
	      printf ("x%d", DECODE_VAR (f->args[j]));
	    }
	  if (j < 1)
	    {
	      printf (" ");
	    }
	}
      printf (")");
      return;
    }

  if (f->function == -2)
    {
      printf ("!=(");
      for (j = 0; j < 2; j++)
	{
	  if (f->args[j] >= 0)
	    {
	      printf ("%s", gconstants[(f->args)[j]]);
	    }
	  else
	    {
	      printf ("x%d", DECODE_VAR (f->args[j]));
	    }
	  if (j < 1)
	    {
	      printf (" ");
	    }
	}
      printf (")");
      return;
    }

  printf ("%s(", gfunctions[f->function]);
  for (j = 0; j < gfunarity[f->function]; j++)
    {
      if (f->args[j] >= 0)
	{
	  printf ("%s", gconstants[(f->args)[j]]);
	}
      else
	{
	  printf ("x%d", DECODE_VAR (f->args[j]));
	}
      if (j < gfunarity[f->function] - 1)
	{
	  printf (" ");
	}
    }
  if (level < 0)
    printf (") = %f", f->value);
  else
    printf (") = %f", vectlevel[level]->numeric->values[cv_index]);

}












void
print_parser_info_for_debug ()
{
  int i, j;
  if (gcmd_line.display_info == 141)
    {
      for (i = 0; i < gnum_fullnum_initial; i++)
	{
	  printf
	    ("\n------------------------------------------------------------\nNumVar %d:",
	     i);
	  print_NumVar (gfullnum_initial[i], i, -1);
	  printf
	    ("\n------------------------------------------------------------");
	}

      for (i = 0; i < gnum_comp_var; i++)
	{
	  IntList *il;
	  printf
	    ("\n------------------------------------------------------------\nCompVar %d:\n------------------------------------------------------------",
	     i);
	  if (GET_BIT (gis_inertial, i))
	    printf ("\n******INERTIAL******");
	  else
	    printf ("\n******DYNAMIC*******");
	  printf ("\nOPERATOR     :%s",
		  goperator_table[gcomp_var[i].operator]);
	  printf ("\nfirst_op     :%d\n", gcomp_var[i].first_op);

	  print_cvar_tree (gcomp_var[i].first_op, -1);
	  printf ("\nsecond_op    :%d\n", gcomp_var[i].second_op);
	  print_cvar_tree (gcomp_var[i].second_op, -1);
	  printf ("\nvalue        :%f\n", GCOMP_VAR_VALUE (i));
	  printf ("\nAffects vars :");
	  for (il = gcomp_var[i].affects; il; il = il->next)
	    printf ("%d%s", il->item, il->next ? ", " : " ");
	  printf ("\n");
	}
    }
  if (gcmd_line.display_info == 142)
    {
      printf ("\n\ncreated connectivity graph as follows:");

      printf ("\n\n------------------OP ARRAY:-----------------------");
      for (i = 0; i < gnum_op_conn; i++)
	{
	  printf ("\n\nOP: ");
	  print_op_name (i);
	  printf ("\n----------EFFS:");
	  for (j = 0; j < gop_conn[i].num_E; j++)
	    {
	      printf ("\neffect %d", gop_conn[i].E[j]);
	    }
	  printf ("\nSIZE = %d",
		  sizeof (gop_conn[i]) + sizeof (int) * (gop_conn[i].num_E -
							 1));
	}

      printf ("\n\n-------------------EFFECT ARRAY:----------------------");
      for (i = 0; i < gnum_ef_conn; i++)
	{
	  printf ("\n\neffect %d of op %d: ", i, gef_conn[i].op);
	  print_op_name (gef_conn[i].op);
	  printf ("\ncost     :%f", gef_conn[i].cost);
	  printf ("\nduration :%f - ", gef_conn[i].duration);
	  print_cvar_tree (gef_conn[i].dur_var_index, -1);
	  printf ("\n----------PCS START:");
	  for (j = 0; j < gef_conn[i].num_PC; j++)
	    {
	      printf ("\n");
	      print_ft_name (gef_conn[i].PC[j]);
	    }
	  if (gef_conn[i].sf)
	    {
	      printf ("\n----------PCS OVERALL:");
	      for (j = 0; j < gef_conn[i].sf->num_PC_overall; j++)
		{
		  printf ("\n");
		  print_ft_name (gef_conn[i].sf->PC_overall[j]);
		}
	      printf ("\n----------PCS END:");
	      for (j = 0; j < gef_conn[i].sf->num_PC_end; j++)
		{
		  printf ("\n");
		  print_ft_name (gef_conn[i].sf->PC_end[j]);
		}
	      printf ("\n----------ADDS START:");
	      for (j = 0; j < gef_conn[i].sf->num_A_start; j++)
		{
		  printf ("\n");
		  print_ft_name (gef_conn[i].sf->A_start[j]);
		}
	    }
	  printf ("\n----------ADDS END:");
	  for (j = 0; j < gef_conn[i].num_A; j++)
	    {
	      printf ("\n");
	      print_ft_name (gef_conn[i].A[j]);
	    }
	  if (gef_conn[i].sf)
	    {
	      printf ("\n----------DELS START:");
	      for (j = 0; j < gef_conn[i].sf->num_D_start; j++)
		{
		  printf ("\n");
		  print_ft_name (gef_conn[i].sf->D_start[j]);
		}
	    }
	  printf ("\n----------DELS END:");
	  for (j = 0; j < gef_conn[i].num_D; j++)
	    {
	      printf ("\n");
	      print_ft_name (gef_conn[i].D[j]);
	    }
	  printf ("\n----------IMPLIEDS:");
	  for (j = 0; j < gef_conn[i].num_I; j++)
	    {
	      printf ("\nimplied effect %d of op %d: ",
		      gef_conn[i].I[j], gef_conn[gef_conn[i].I[j]].op);
	      print_op_name (gef_conn[gef_conn[i].I[j]].op);
	    }
	}

      printf
	("\n\n----------------------FT ARRAY:-----------------------------");
      for (i = 0; i < gnum_ft_conn; i++)
	{

	  printf ("\n -------------------\n\n %d FT: ", i);
	  print_ft_name (i);
	  printf (" rand: %d", gft_conn[i].rand);
	  printf ("\n----------PRE COND OF:");
	  for (j = 0; j < gft_conn[i].num_PC; j++)
	    {
	      printf ("\neffect %d", gft_conn[i].PC[j]);
	    }
	  printf ("\n----------ADD BY:");
	  for (j = 0; j < gft_conn[i].num_A; j++)
	    {
	      printf ("\neffect %d", gft_conn[i].A[j]);
	    }
	  printf ("\n----------DEL BY:");
	  for (j = 0; j < gft_conn[i].num_D; j++)
	    {
	      printf ("\neffect %d", gft_conn[i].D[j]);
	    }

	}
    }

}









void
print_matrs ()
{
  int i, j;
  int total, total_mutex = 0;

  total_mutex = 0;
  for (i = 0; i < gnum_ft_conn; i++)
    {
      total = 0;
      for (j = 0; j < gnum_ef_conn; j++)
	if (GET_BIT (FT_EF_mutex[i], j))
	  {
	    total++;
	  }
      total_mutex += total;
    }

  total_ft_ef_mutex = total_mutex;

  total_mutex = 0;
  for (i = 0; i < gnum_ef_conn; i++)
    {
      total = 0;
      for (j = 0; j < gnum_ef_conn; j++)
	if (GET_BIT (EF_EF_mutex[i], j))
	  {
	    total++;
	  }
      total_mutex += total;
    }
  total_ef_ef_mutex = total_mutex;

  total_mutex = 0;
  for (i = 0; i < gnum_ef_conn; i++)
    {
      total = 0;
      for (j = 0; j < gnum_ft_conn; j++)
	if (GET_BIT (EF_FT_mutex[i], j))
	  {
	    total++;
	  }
      total_mutex += total;
    }
  total_ef_ft_mutex = total_mutex;

  if (total_ft_ef_mutex != total_ef_ft_mutex)
    {
      printf ("\n\nWARNING num_ft_ef!=num_ef_ft\n\n");

    }

  printf ("\n Total mutex pairs between facts: %d", total_ft_ft_mutex / 2);
  printf ("\n Total mutex pairs between facts and actions: %d",
	  total_ft_ef_mutex / 2);
  printf ("\n Total mutex pairs between actions: %d", total_ef_ef_mutex / 2);
  printf ("\n Total mutex pairs between actions and facts: %d",
	  total_ef_ft_mutex / 2);

  printf ("\n Number of facts  : %d", gnum_ft_conn);
  printf ("\n Number of actions: %d", gnum_ef_conn);
}



void
print_mutex_result (void)
{

  printf ("\n\n Total mutex pairs between facts: %d", total_ft_ft_mutex / 2);
  printf ("\n Total mutex pairs between facts and actions: %d",
	  total_ft_ef_mutex / 2);
  printf ("\n Total mutex pairs between actions: %d", total_ef_ef_mutex);
  printf ("\n Total mutex pairs between actions and facts: %d",
	  total_ef_ft_mutex / 2);

  printf ("\n Number of facts  : %d", gnum_ft_conn);
  printf ("\n Number of actions: %d", gnum_ef_conn);

}



void
print_mutex_table ()
{

  int i, j;
  printf ("\n");
  for (i = 0; i < gnum_ft_conn; i++)
    {
      printf ("\t\t%2d ", i);
      print_ft_name (i);
    }

  printf ("\n\nMutex table:\n   ");
  for (i = 0; i < gnum_ft_conn; i++)
    printf ("%d", i % 10);
  printf ("\n\n");
  for (i = 0; i < gnum_ft_conn; i++)
    {
      printf ("%2d ", i);
      for (j = 0; j < gnum_ft_conn; j++)
	if (GET_BIT (FT_FT_mutex[i], j))
	  printf ("1");
	else
	  printf ("0");
      printf ("\n");
    }

}







void
print_cvar_tree (int cv_index, int level)
{
  CompositeNumVar *cv;

  if (cv_index < 0)
    return;
  cv = &gcomp_var[cv_index];
  switch (cv->operator)
    {
    case MUL_OP:
    case DIV_OP:
    case PLUS_OP:
    case MINUS_OP:
    case LESS_THAN_OP:
    case LESS_THAN_OR_EQUAL_OP:
    case EQUAL_OP:
    case GREATER_THAN_OP:
    case GREATER_OR_EQUAL_OP:
    case INCREASE_OP:
    case DECREASE_OP:
    case SCALE_UP_OP:
    case SCALE_DOWN_OP:
    case ASSIGN_OP:
      printf ("( %s ", goperator_table[cv->operator]);
      print_cvar_tree (cv->first_op, level);
      printf (" ");
      print_cvar_tree (cv->second_op, level);
      printf (" )");
      break;
    case UMINUS_OP:
    case MINIMIZE_OP:
    case MAXIMIZE_OP:
      printf ("( %s ", goperator_table[cv->operator]);
      print_cvar_tree (cv->first_op, level);
      printf (" )");
      break;
      break;
    case FIX_NUMBER:
      printf (" %f ", GCOMP_VAR_VALUE (cv_index));
      break;
    case VARIABLE_OP:
      printf ("( ");
      print_NumVar (gfullnum_initial[cv_index], cv_index, level);
      printf (" )");
      break;
    default:
      break;
    }
}



void
print_unsup_num_facts ()
{
  int i;

  printf ("\n<<< UNSUP NUM FACT: %d", GpG.num_false_num_fa);


  for (i = 0; i < GpG.num_false_num_fa; i++)
    {
      printf ("\nFalse num pos %d", i + 1);
      printf ("   Level %d", *unsup_num_fact[i]->level);
      printf ("   Fact %d\n", unsup_num_fact[i]->fact);



      if (unsup_num_fact[i]->fact != -1)
	print_cvar_tree (unsup_num_fact[i]->fact, *unsup_num_fact[i]->level);

      printf ("\n->action=%4d\t", unsup_num_fact[i]->action);
      if (unsup_num_fact[i]->action != -1)
	print_op_name (unsup_num_fact[i]->action);
      assert (i ==
	      vectlevel[*unsup_num_fact[i]->level]->numeric->
	      false_position[unsup_num_fact[i]->fact]);
    }


}


void
print_cri_computed_costs (int level)
{
  int i;

  dg_inform **loc_dg_facts_array;


  if (level < 0)
    loc_dg_facts_array = Hvar.init_facts_array;
  else
    {
      printf ("\n;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n\n\nLevel %d action ",
	      level);
      print_op_name (GET_ACTION_POSITION_OF_LEVEL (level));
      loc_dg_facts_array = vectlevel[level]->dg_facts_array;

    }
  for (i = 0; i < gnum_ft_conn; i++)
    {
      if (loc_dg_facts_array[i] == NULL)
	continue;

      printf
	("\nFact %d numact %d cost %.2f dur %.2f best_act %d tot %.2f name ",
	 i, loc_dg_facts_array[i]->num_actions, loc_dg_facts_array[i]->cost,
	 loc_dg_facts_array[i]->duration, loc_dg_facts_array[i]->best_act,
	 loc_dg_facts_array[i]->totcost);
      print_ft_name (i);

    }
}




char *
print_op_name_string (int pos, char *out_string)
{
  int i;
  Action *a = gop_conn[pos].action;


  if (pos == INITIAL_ACTION)
    {
      strcat (out_string, "A_start");
      return out_string;
    }

  assert (pos >= 0);
  assert (pos < gnum_op_conn);

  if (!a->norm_operator && !a->pseudo_action)
    strcat (out_string, "REACH-GOAL");

  else
    {
      strcpy (out_string, "(");
      strcat (out_string, a->name);
      for (i = 0; i < a->num_name_vars; i++)
	{
	  strcat (out_string, CONN_PLAN);
	  strcat (out_string, gconstants[a->name_inst_table[i]]);
	}
      strcat (out_string, ")");
    }

  return out_string;
}



char *
print_ft_name_string (int pos, char *out_string)
{
  int j;
  Fact *f;
  char *temp = NULL;

  if (pos < 0)
    return ("Derived NUM fact  ");

  assert (pos >= 0);
  assert (pos < gnum_ft_conn);

  f = &grelevant_facts[pos];

  sprintf (out_string, "(%s ", gpredicates[f->predicate]);
  for (j = 0; j < garity[f->predicate]; j++)
    {
      if (f->args[j] >= 0)
	strcat (out_string, gconstants[(f->args)[j]]);
      else
	{
	  sprintf (temp, "x%d", DECODE_VAR (f->args[j]));
	  strcat (out_string, temp);
	}
      if (j < garity[f->predicate] - 1)
	strcat (out_string, " ");
    }
  strcat (out_string, ")");
  return out_string;
}




char *
print_noop_name_string (int pos, char *out_string)
{

  int j;
  Fact *f;
  char *temp = NULL;

  assert (pos >= 0);

  sprintf (out_string, "NOOP_");

  f = &grelevant_facts[pos];
  strcat (out_string, gpredicates[f->predicate]);
  strcat (out_string, "(");
  for (j = 0; j < garity[f->predicate]; j++)
    {
      if (f->args[j] >= 0)
	strcat (out_string, gconstants[(f->args)[j]]);
      else
	{
	  sprintf (temp, "x%d", DECODE_VAR (f->args[j]));
	  strcat (out_string, temp);
	}
      if (j < garity[f->predicate] - 1)
	strcat (out_string, " ");
    }
  strcat (out_string, ")");


  return out_string;
}


void
print_cost_of_fact (int fact_pos, int level)
{
  dg_inform_list loc_dg_cost;


  get_dg_fact_cost (fact_pos, level - 1, &loc_dg_cost);

  printf ("\n \tFact pos %d   level %d : ", fact_pos, level);
  print_ft_name (fact_pos);

  printf
    (" totcost %.2f    cost  %.2f duration %.2f num_actions %d, best_act %d  : ",
     loc_dg_cost->totcost, loc_dg_cost->cost, loc_dg_cost->duration,
     loc_dg_cost->num_actions, loc_dg_cost->best_act);
  if (loc_dg_cost->best_act >= 0)
    print_op_name (loc_dg_cost->best_act);

}

void
print_cost_of_unsupported_facts ()
{
  int i;
  if (GpG.accurate_cost < COMPUTE_DG_SUM_COST)
    return;


  printf ("\n COST OF UNSUPPORTED FACTS num %d ", GpG.num_false_fa);
  for (i = 0; i < GpG.num_false_fa; i++)
    print_cost_of_fact (unsup_fact[i]->fact, *unsup_fact[i]->level);

  printf ("\n\n");
}







void
print_actions_in_subgraph ()
{
  int i;

  printf ("\n\n>>> ACTIONS in subgraph");
  for (i = GpG.curr_plan_length - 1; i >= 0; i--)
    if (GET_ACTION_POSITION_OF_LEVEL (i) >= 0)
      {
	printf ("\nLevel %d: %s", i,
		print_op_name_string (GET_ACTION_POSITION_OF_LEVEL (i),
				      temp_name));
	if (GpG.temporal_plan)
	  printf (", start_time %.2f, end_time %.2f",
		  GET_ACTION_OF_LEVEL (i)->time_f -
		  get_action_time (GET_ACTION_POSITION_OF_LEVEL (i), i),
		  GET_ACTION_OF_LEVEL (i)->time_f);

	printf ("  pos %d", GET_ACTION_POSITION_OF_LEVEL (i));
      }
  printf ("\n");

}



void
print_actions_in_plan ()
{
  int i;
  PlanAction *temp_act;


  printf ("\n\nLast plan computed:");

  if (GpG.gplan_actions)
    {
      printf ("\n   Time: (ACTION) [action Duration; action Cost]");
      for (temp_act = GpG.gplan_actions, i = 0; temp_act;
	   temp_act = temp_act->next, i++)
	{
	  printf ("\n %.3f: %s", temp_act->start_time,
		  print_op_name_string (temp_act->act_pos, temp_name));

	  printf (" [D:%.3f; C:%.3f]", temp_act->duration, temp_act->cost);

	}
    }
  else
    printf ("\n No action in solution.");

}



void
print_temporal_plan (int levels)
{

  int lev, i;
  inform_list infTemp;


  printf ("\n\n -+- TEMPORAL PLAN -+-\n");

  for (lev = 0; lev < levels; lev++)
    {

      printf ("\n %2d: Facts\n", lev);

      for (i = 0; i < gnum_ft_conn; i++)
	{
	  infTemp = &vectlevel[lev]->fact[i];


	  if (grelevant_facts[i].predicate == GpG.dummy_pos)
	    continue;

	  if (infTemp->w_is_true)
	    {

	      printf ("\t%s,", print_ft_name_string (i, temp_name));
	      printf (" true %d, goal %d, time %.2f \n", infTemp->w_is_true,
		      infTemp->w_is_goal, infTemp->time_f);
	    }
	}

      printf ("\n     Action");

      infTemp = GET_ACTION_OF_LEVEL (lev);

      if (infTemp->w_is_used)
	{
	  printf ("\t%s,",
		  print_op_name_string (GET_ACTION_POSITION_OF_LEVEL (lev),
					temp_name));
	  printf (" used %d, end_time %.2f \n", infTemp->w_is_used,
		  infTemp->time_f);
	}
    }

  printf ("\n %2d: Facts \t--- GOAL LEVEL ---\n", lev);

  for (i = 0; i < gnum_ft_conn; i++)
    {
      infTemp = &vectlevel[lev]->fact[i];

      if (grelevant_facts[i].predicate == GpG.dummy_pos)
	continue;

      if (infTemp->w_is_true)
	{
	  printf ("\t%s,", print_ft_name_string (i, temp_name));
	  printf (" true %d, goal %d, time %.2f \n", infTemp->w_is_true,
		  infTemp->w_is_goal, infTemp->time_f);
	}
    }
}




void
my_print_plan_level (int level)
{
  int j, k, temp, pos;

  printf ("\n LEVEL %d Fact:", level);
  for (j = 0; j < gnum_ft_block; j++)

    {
      temp = vectlevel[level]->fact_vect[j];
      k = 32;
      while (temp)

	{
	  k--;
	  if (temp & FIRST_1)
	    printf ("\n\t %s [%d] time_f: %.2f w_is_true %d",
		    print_ft_name_string (j * 32 + k, temp_name),
		    j * 32 + k,
		    vectlevel[level]->fact[j * 32 + k].time_f,
		    vectlevel[level]->fact[j * 32 + k].w_is_true);
	  temp <<= 1;
	}
    }
  printf ("\n LEVEL %d True-crit-vect:", level);
  for (j = 0; j < gnum_ft_block; j++)

    {
      temp = vectlevel[level]->true_crit_vect[j];
      k = 32;
      while (temp)

	{
	  k--;
	  if (temp & FIRST_1)
	    printf ("\n\t %s ", print_ft_name_string (j * 32 + k, temp_name));
	  temp <<= 1;
	}
    }
  printf ("\n LEVEL %d False-crit-vect:", level);
  for (j = 0; j < gnum_ft_block; j++)

    {
      temp = vectlevel[level]->false_crit_vect[j];
      k = 32;
      while (temp)

	{
	  k--;
	  if (temp & FIRST_1)
	    printf ("\n\t %s ", print_ft_name_string (j * 32 + k, temp_name));
	  temp <<= 1;
	}
    }
  printf ("\n LEVEL %d prec-vect:", level);
  for (j = 0; j < gnum_ft_block; j++)

    {
      temp = vectlevel[level]->prec_vect[j];
      k = 32;
      while (temp)

	{
	  k--;
	  if (temp & FIRST_1)
	    printf ("\n\t %s ", print_ft_name_string (j * 32 + k, temp_name));
	  temp <<= 1;
	}
    }
  printf ("\n LEVEL %d check w_is_used - w_is_goal - w_is_true:", level);
  for (pos = 0; pos < GpG.max_num_facts; pos++)

    {
      if (vectlevel[level]->fact[pos].w_is_goal >= 1
	  || vectlevel[level]->fact[pos].w_is_used >= 1
	  || vectlevel[level]->fact[pos].w_is_true >= 1)
	printf ("\n\t %s \tw_is_goal: %d \t w_is_used: %d \t w_is_true: %d",
		print_ft_name_string (pos, temp_name),
		vectlevel[level]->fact[pos].w_is_goal,
		vectlevel[level]->fact[pos].w_is_used,
		vectlevel[level]->fact[pos].w_is_true);
    }
  printf ("\n LEVEL %d NOOP:", level);
  for (j = 0; j < gnum_ft_conn; j++)

    {
      if (vectlevel[level]->noop_act[j].w_is_overall != 0)
	printf
	  ("\nnoop overall: %s w_is_used %d w_is_overall %d w_is_goal %d level %d",
	   print_noop_name_string (j, temp_name),
	   vectlevel[level]->noop_act[j].w_is_used,
	   vectlevel[level]->noop_act[j].w_is_overall,
	   vectlevel[level]->noop_act[j].w_is_goal, level);

      else
	if (vectlevel[level]->noop_act[j].w_is_used
	    || vectlevel[level]->noop_act[j].w_is_goal)
	printf
	  ("\n\tnoop: %s w_is_used %d w_is_overall %d w_is_goal %d level %d",
	   print_noop_name_string (j, temp_name),
	   vectlevel[level]->noop_act[j].w_is_used,
	   vectlevel[level]->noop_act[j].w_is_overall,
	   vectlevel[level]->noop_act[j].w_is_goal, level);
    }
  if (level >= GpG.curr_plan_length)

    {
      printf ("\n\n");
      return;
    }
  printf ("\n LEVEL %d Action:", level);
  if (vectlevel[level]->action.position >= 0)
    printf (" %s [%d] time_f %.2f time_start %.2f",
	    print_op_name_string (vectlevel[level]->action.position,
				  temp_name),
	    vectlevel[level]->action.position,
	    vectlevel[level]->action.time_f,
	    vectlevel[level]->action.time_f -
	    get_action_time (vectlevel[level]->action.position, level));
  printf ("\n\n");

}




void
my_print_plan_all (int max_time)
{
  int i;

  for (i = 0; i <= max_time; i++)
    my_print_plan_level (i);
}



void
my_print_plan (int level)
{
  my_print_plan_level (level);
  if (level >= GpG.curr_plan_length)
    return;
  my_print_plan_level (level + 1);
}



void
print_unsup_fact_vect ()
{
  int i;

  printf ("\n\n<<< UNSUP FACT: %d", GpG.num_false_fa);

  for (i = 0; i < GpG.num_false_fa; i++)
    printf ("\nFalse pos %d  Level %d  Unsup fact %s ",
	    CONVERT_FACT_TO_INFORM (unsup_fact[i]->fact,
				    *unsup_fact[i]->level)->false_position,
	    *unsup_fact[i]->level, print_ft_name_string (unsup_fact[i]->fact,
							 temp_name));

  printf ("\n<<< TREATED FACT: %d", GpG.num_false_act);

  for (i = 0; i < GpG.num_false_act; i++)
    printf ("\nTreated pos %d  Level %d  Treated noop %s ",
	    CONVERT_NOOP_TO_INFORM (treated_c_l[i]->fact,
				    *treated_c_l[i]->level)->false_position,
	    *treated_c_l[i]->level,
	    print_noop_name_string (treated_c_l[i]->fact, temp_name));
}


void
print_list_resources (int index)
{
  dg_inform *tmp;
  int num;
  num = 0;
  tmp = Hvar.init_facts_array[index];

  return;
  printf ("\n +++++ FACT %d numA=%d  ", index, gft_conn[index].num_A);
  print_ft_name (index);

  while (tmp)
    {
      num = num + 1;
      printf ("\n Num. Risorsa %d \n", num);
      if (tmp->best_act == INFINITY)
	{
	  printf ("\n FATTO INIZIALE\n");
	}
      else
	{
	  print_op_name (tmp->best_act);
	  printf ("\n Costo %3f : Durata %3f \n", tmp->cost, tmp->duration);
	}
      tmp = tmp->next;
    }
}




void
print_num_levels_and_actions ()
{
  int j, k, cnt = 0;
  static int progress = 0;

  printf ("\n\n -x- NUMERIC -x- %d", progress++);
  for (j = 0; j <= GpG.curr_plan_length; j++)
    {
      cnt = 0;
      printf ("\n----------------------------------------------------------");
      printf ("\n Num value in level %d:\n", j);
      if (vectlevel[j]->action.being_removed)
	{
	  printf ("\n Act in insert/remove => BREAK\n");
	  break;
	}
      for (k = 0; k < gnum_comp_var; k++)
	if (!GET_BIT (gis_inertial, k))
	  {

	    if ((j > 0)
		&&
		(abs
		 (vectlevel[j]->numeric->values[k] -
		  vectlevel[j - 1]->numeric->values[k]) > 0.10))
	      {
		printf ("%5d:", k);
		printf ("%9.2f->%9.2f", vectlevel[j]->numeric->values[k],
			vectlevel[j]->numeric->values_after_start[k]);
		if ((++cnt % 3) == 0)
		  printf ("\n");
	      }
	  }
      check_consistency (j);
      if (j != GpG.curr_plan_length)
	{
	  printf ("\n\n  Action %d: ", vectlevel[j]->action.position);
	  if (vectlevel[j]->action.position >= 0)
	    {
	      print_op_name (vectlevel[j]->action.position);
	    }
	  else
	    {


	      for (k = 0; k < gnum_comp_var; k++)
		if (abs
		    (vectlevel[j]->numeric->values[k] -
		     vectlevel[j + 1]->numeric->values[k]) > 0.1)
		  {
		    printf ("\nERRRR2: livello %d-%d: cvar %d\n %f != %f\n\n",
			    j, j + 1, k, vectlevel[j]->numeric->values[k],
			    vectlevel[j + 1]->numeric->values[k]);
		  }
		else
		  printf ("cvar %d= %.2f \t", k,
			  vectlevel[j]->numeric->values[k]);

	    }
	}

      if (vectlevel[j]->action.position == 452)
	{
	  printf ("\n VARTEST lev %d :<>: ", j);
	  k = 5;
	  printf ("cvar %d= %.2f \t", k, vectlevel[j]->numeric->values[k]);
	  k = 68;
	  printf ("cvar %d= %.2f \t", k, vectlevel[j]->numeric->values[k]);
	  k = 69;
	  printf ("cvar %d= %.2f \t", k, vectlevel[j]->numeric->values[k]);
	  print_unsup_num_facts ();
	}

    }

  printf ("\n\n");
}




void
print_file_action_name (FILE * outfile, int index)
{

  int i;
  Action *a = gop_conn[index].action;

  if (a->norm_operator || a->pseudo_action)
    {
      fprintf (outfile, " (%s", a->name);

      for (i = 0; i < a->num_name_vars; i++)
	{
	  fprintf (outfile, " %s", gconstants[a->name_inst_table[i]]);
	}
    }

  fprintf (outfile, ")");
}


void
print_file_fact_name (FILE * outfile, int index)
{


  int j;
  Fact *f;

  f = &(grelevant_facts[index]);

  if (f->predicate == -3)
    {
      fprintf (outfile, "GOAL-REACHED");
      return;
    }

  if (f->predicate == -1)
    {
      fprintf (outfile, "=(");

      for (j = 0; j < 2; j++)
	{

	  if (f->args[j] >= 0)
	    {
	      fprintf (outfile, "%s", gconstants[(f->args)[j]]);
	    }
	  else
	    {
	      fprintf (outfile, "x%d", DECODE_VAR (f->args[j]));
	    }

	  if (j < 1)
	    {
	      fprintf (outfile, " ");
	    }
	}

      fprintf (outfile, ")");
      return;
    }

  if (f->predicate == -2)
    {
      fprintf (outfile, "!=(");

      for (j = 0; j < 2; j++)
	{

	  if (f->args[j] >= 0)
	    {
	      fprintf (outfile, "%s", gconstants[(f->args)[j]]);
	    }
	  else
	    {
	      fprintf (outfile, "x%d", DECODE_VAR (f->args[j]));

	    }

	  if (j < 1)
	    {
	      fprintf (outfile, " ");
	    }
	}

      fprintf (outfile, ")");
      return;
    }

  fprintf (outfile, "%s(", gpredicates[f->predicate]);

  for (j = 0; j < garity[f->predicate]; j++)
    {

      if (f->args[j] >= 0)
	{
	  fprintf (outfile, "%s", gconstants[(f->args)[j]]);
	}
      else
	{
	  fprintf (outfile, "x%d", DECODE_VAR (f->args[j]));
	}

      if (j < garity[f->predicate] - 1)
	{
	  fprintf (outfile, " ");
	}
    }

  fprintf (outfile, ")");

}










void
store_adapted_temporal_plan_ff (char *fact_file_name)
{

  int i, curr_plan_length = 0, out;
  char cNameFile[256];
  char validate_string[MAX_LENGTH];
  FILE *fp;
  float temp_time;

  curr_plan_length = 0;

  GpG.total_cost = 0.0;
  GpG.num_actions = gnum_plan_ops;
  temp_time = 0.0;


  sprintf (cNameFile, "plan_bestfirst_%s.SOL", fact_file_name);

  if ((fp = fopen (cNameFile, "w")) == NULL)
    {
      MSG_ERROR (WAR_OPEN_FILE);
      exit (1);
    }

  fprintf (fp, "\n;; Version %s", VERSION);
  fprintf (fp, "\n;; Seed %d", seed);
  fprintf (fp, "\n;; Command line: %s", gcomm_line);
  fprintf (fp, "\n\n;;Problem:\t%s\t actions:\t%d\t \n", fact_file_name,
	   gnum_plan_ops);
  fprintf (fp, ";;Parsing time: %f\n\n",
	   gtempl_time + greach_time + grelev_time + gconn_time + gnum_time);




  for (i = 0; i < gnum_plan_ops; i++)
    {

      fprintf (fp, "\t %.3f:  ", temp_time + MIN_DELTA_TIME * i);
      print_file_action_name (fp, gplan_ops[i]);
      fprintf (fp, "[%.3f] ;; cost %.3f \n",
	       get_action_time (gplan_ops[i], i),
	       get_action_cost (gplan_ops[i]));

      temp_time += get_action_time (gplan_ops[i], i);
      GpG.total_cost += get_action_cost (gplan_ops[i]);
    }
  GpG.total_time = temp_time;




  fprintf (fp, "\nTime %d\n\n", (int) (gtotal_time * 1000.0 + 0.5));
  fclose (fp);

  if (GpG.validate)
    {
      strcpy (validate_string, VALIDATOR);
      strcat (validate_string, gops_file);
      strcat (validate_string, " ");
      strcat (validate_string, gfct_file);
      strcat (validate_string, " ");
      strcat (validate_string, cNameFile);
      printf ("\n\n%s\n", validate_string);
      fflush (stdout);
      out = system (validate_string);
    }

}








void
store_adapted_temporal_plan (int levels, char *fact_file_name, double time)
{
  int i, j, curr_plan_length = 0, num_actions = 0, out;
  char cNameFile[256];
  char validate_string[MAX_LENGTH];
  static int num_plan = 0;
  FILE *fp;
  inform_list infAction;
  float start_time;

  curr_plan_length = 0;

  num_actions = 0;
  num_plan++;
  if (GpG.out_file_name)
    sprintf (cNameFile, "%s_%d.SOL", gcmd_line.out_file_name, num_plan);
  else
    sprintf (cNameFile, "plan_%s_%d.SOL", fact_file_name, num_plan);

  if ((fp = fopen (cNameFile, "w")) == NULL)
    {
      printf ("\n\n\nError opening output file: %s", cNameFile);
      MSG_ERROR (WAR_OPEN_FILE);
      return;
    }

  fprintf (fp, "\n;; Version %s", VERSION);
  fprintf (fp, "\n;; Seed %d", seed);
  fprintf (fp, "\n;; Command line: %s", gcomm_line);
  fprintf (fp, "\n\n;;Problem:\t%s", fact_file_name);
  fprintf (fp,
	   "\n;;Search time: %.3f    Parsing time: %.3f    Mutex time: %.3f",
	   time,
	   gtempl_time + greach_time + grelev_time + gconn_time + gnum_time,
	   gmutex_total_time);
  if (GpG.maximize_plan && GpG.total_cost < 0)
    fprintf (fp,
	     "\n;;Actions: %d    Execution cost: %.3f    Duration: %.3f\n\n",
	     GpG.num_actions, GpG.total_cost * (-1), GpG.total_time);
  else
    fprintf (fp,
	     "\n;;Actions: %d    Execution cost: %.3f    Duration: %.3f\n\n",
	     GpG.num_actions, GpG.total_cost, GpG.total_time);


  for (j = 0, i = 0; i < levels; i++)

    if (GET_ACTION_POSITION_OF_LEVEL (i) >= 0)
      {
	curr_plan_length++;

	infAction = GET_ACTION_OF_LEVEL (i);

	if (get_action_time (infAction->position, *infAction->level) <
	    MAX_APPROX)
	  continue;

	num_actions++;

	start_time =
	  infAction->time_f - get_action_time (infAction->position,
					       *infAction->level) +
	  MIN_DELTA_TIME * curr_plan_length;
	fprintf (fp, "\t %.3f:  ", start_time);

	print_file_action_name (fp, infAction->position);
	if (GpG.maximize_plan && get_action_cost (infAction->position) < 0)
	  fprintf (fp, "[%.3f] ;; cost %.3f \n",
		   get_action_time (infAction->position, *infAction->level),
		   get_action_cost (infAction->position) * (-1));
	else
	  fprintf (fp, "[%.3f] ;; cost %.3f \n",
		   get_action_time (infAction->position, *infAction->level),
		   get_action_cost (infAction->position));


      }

  fprintf (fp, "\nTime %d\n\n", (int) (gtotal_time * 1000.0 + 0.5));
  fclose (fp);

  if (GpG.validate)
    {
      strcpy (validate_string, VALIDATOR_T);
      strcat (validate_string, gops_file);
      strcat (validate_string, " ");
      strcat (validate_string, gfct_file);
      strcat (validate_string, " ");
      strcat (validate_string, cNameFile);
      printf ("\n\n%s\n", validate_string);
      fflush (stdout);
      out = system (validate_string);
    }

}










void
store_adapted_temporal_plan_for_debug (int levels, char *fact_file_name,
				       double time)
{
  int i, j, curr_plan_length = 0, num_actions = 0, ind, pos;
  char cNameFile[256];
  static int num_plan = 0;
  FILE *fp;
  inform_list infAction, infTemp;
  FtConn *factTemp;
  float start_time;

  curr_plan_length = 0;

  num_actions = 0;
  num_plan++;

  sprintf (cNameFile, "plan_%s_%d.SOL", fact_file_name, num_plan);

  if ((fp = fopen (cNameFile, "w")) == NULL)
    {
      MSG_ERROR (WAR_OPEN_FILE);
      exit (1);
    }

  fprintf (fp,
	   ";;Problem:\t%s\t time:\t%f\t actions:\t%d total cost \t %f \t total time %.2f\n",
	   fact_file_name, time, GpG.num_actions, GpG.total_cost,
	   GpG.total_time);


  for (j = 0, i = 0; i < levels; i++)

    if (GET_ACTION_POSITION_OF_LEVEL (i) >= 0)
      {
	curr_plan_length++;
	{
	  fprintf (fp, ";; \n;;Fatti lev %d: \n", i);
	  for (ind = 0; ind < gnum_ft_conn; ind++)

	    if ((infTemp = &vectlevel[i]->fact[ind])->w_is_true)
	      {
		factTemp = CONVERT_FACT_TO_VERTEX (ind);

		fprintf (fp, ";;\t");
		print_file_fact_name (fp, ind);
		fprintf (fp, "   time %.2f \n", infTemp->time_f);
	      }
	}
	fprintf (fp, "\n;; check w_is_used - w_is_goal:\n");

	for (pos = 0; pos < GpG.max_num_facts; pos++)
	  {

	    if (vectlevel[i]->fact[pos].w_is_goal >= 1
		|| vectlevel[i]->fact[pos].w_is_used >= 1)
	      {

		fprintf (fp, ";;\t");
		print_file_fact_name (fp, pos);
		fprintf (fp, "\tw_is_goal: %d \t w_is_used: %d\n",
			 vectlevel[i]->fact[pos].w_is_goal,
			 vectlevel[i]->fact[pos].w_is_used);
	      }
	  }

	fprintf (fp, "\n;; check NOOP w_is_used - w_is_goal:\n");

	for (pos = 0; pos < GpG.max_num_facts; pos++)
	  {

	    if (vectlevel[i]->noop_act[pos].w_is_goal >= 1
		|| vectlevel[i]->noop_act[pos].w_is_used >= 1)
	      {
		fprintf (fp, ";;\t NOOP: ");
		print_file_fact_name (fp, pos);
		fprintf (fp, "\tw_is_goal: %d \t w_is_used: %d\n",
			 vectlevel[i]->noop_act[pos].w_is_goal,
			 vectlevel[i]->noop_act[pos].w_is_used);
	      }
	  }

	num_actions++;
	infAction = GET_ACTION_OF_LEVEL (i);

	if (GpG.temporal_plan)
	  {
	    start_time =
	      infAction->time_f - get_action_time (infAction->position,
						   *infAction->level) +
	      START_TIME + MIN_DELTA_TIME * i;
	    fprintf (fp, "\t %.3f:  ", start_time);
	  }
	else
	  fprintf (fp, "\t %d: ", curr_plan_length);

	print_file_action_name (fp, infAction->position);
	fprintf (fp, "[%.2f] ;; cost %.2f \n",
		 get_action_time (infAction->position, *infAction->level),
		 get_action_cost (infAction->position));
      }

  fprintf (fp, ";; \n;;Fatti lev GOAL: \n");

  for (ind = 0; ind < gnum_ft_conn; ind++)

    if ((infTemp = &vectlevel[i]->fact[ind])->w_is_true)
      {
	factTemp = CONVERT_FACT_TO_VERTEX (ind);
	fprintf (fp, ";;\t");
	print_file_fact_name (fp, ind);
	fprintf (fp, "   time %.2f \n", infTemp->time_f);
      }

  fclose (fp);

}




int
store_curr_plan (int max_time, PlanAction ** plan_actions)
{
  int level, i;
  static int max_level = 0;

  if (plan_actions != NULL)
    {
      free_gplan_actions (*plan_actions);
      *plan_actions = NULL;
    }
  else
    {
      printf ("\nplanact not initialized\n");
      return (0);
    }

  GpG.num_false_tot =
    GpG.num_false_act + GpG.num_false_fa + GpG.num_false_num_fa;

  for (i = level = 0; level <= max_time; level++)
    if (vectlevel[level]->action.w_is_used)
      {
	store_action_vect (plan_actions, vectlevel[level]->action.position,
			   vectlevel[level]->action.time_f -
			   get_action_time (vectlevel[level]->action.position,
					    level),
			   get_action_time (vectlevel[level]->action.position,
					    level));

	i++;
      }
  if (level > max_level)
    max_level = level;
  GpG.min_inc = GpG.num_false_tot;
  GpG.min_num_actions = GpG.num_actions;
  GpG.found_plan = STORED;
  if (GpG.num_false_tot == 0)
    {
      GpG.num_solutions++;
      GpG.best_cost = GpG.total_cost;
      GpG.best_time = GpG.total_time;
      GpG.input_plan_lenght = GpG.curr_plan_length;
      if (GpG.init_tabu_length == 0)
	{

	  GpG.local = 1;
	  GpG.init_tabu_length = 10;
	}
    }
  GpG.curr_plan_length = GpG.input_plan_lenght = level - 1;
  GpG.num_actions = i;
  GpG.gplan_actions = *plan_actions;

  return (0);
}
