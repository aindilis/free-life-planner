/********************************************************************
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
 * File: H_relaxed.h
 * Description: header file for the "relaxed plan" heuristic (ICAPS-02)
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/ 




#ifndef _HRELAXED_H
#define _HRELAXED_H

dg_inform_list update_dg_fact_list(int fact_pos, int *level_ptr, int num_actions, int best_act, float duration, float cost);
void delete_dg_fact_list(int fact_pos, int *level);



int get_dg_fact_cost (register int fact_pos, register int level, dg_inform_list * loc_dg_cost);


float compute_constr_fact (int constraint, int fact_pos, int level, int initialize);

inline float compute_dg_facts_cost (int Fact_position, int level, node_cost_list n_cost, int action_level);

int insert_action_inlist (int pos);


int choose_best_action_for_unsup_num_fact (int Fact_position, int level,
					   int *best_action, int *best_level, int action_level);

float dg_action_cost (neighb_list neighb_act);

void ls_insert_fact_inlist (int pos);

float dg_insertion_action_cost (int act_pos, int level, int temp_removed_action,node_cost_list n_cost);
#endif
