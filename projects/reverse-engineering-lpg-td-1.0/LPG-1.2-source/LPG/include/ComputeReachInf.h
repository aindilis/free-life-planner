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
 * Description: header file for computing reachability information
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/ 



#ifndef _COMPUTEREACHINFO_H
#define _COMPUTEREACHINFO_H




void allocate_reachability_information_data();

void reset_computed_dg_costs (int level);
void set_computed_dg_costs (int level);

void  cri_insert_fact_inlist (int pos);
int cri_insert_action_inlist (int pos);

int cri_get_Action_for_fct (int fact_pos);

void reset_cri_list();

void cri_insert_ftcost (int fact_pos, float cost, float duration, int num_actions,int best_ef);
void cri_insert_list_fact_additive (int index);
void cri_insert_list_action_preconditions (int index);

float cri_predict_cost_relaxed (int action, float *duration_act, int *num_actions);
float cri_predict_cost_sum (int action, int *num_actions);
float cri_predict_cost_max (int action, int *num_actions);
float cri_predict_duration (int action, float *duration_act);

void cri_activate_distgraph_ef (int index);

inline void cri_heuristic_for_action (int action, int level);

void set_init_computed_dg_costs ();

void remove_mutex_facts_in_bitvect_and_update_num_actions (int fact, int *bit_vect);

#endif
