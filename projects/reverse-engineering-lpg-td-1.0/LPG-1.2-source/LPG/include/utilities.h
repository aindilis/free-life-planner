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
 * File: utilities.h
 * Description: header file for utility functions.
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/ 



#ifndef _UTILITIES_H
#define _UTILITIES_H

void forward_propagation (int from_level, int to_level);

void vectlevel_to_planops (int from_level, int to_level);

void add_planactions_to_planactions (PlanAction * plan_to_add, PlanAction * first_plan);

void create_all_min_array ();

void allocate_data_for_local_search();

int load_pddl2_plan (char *plan_file, PlanAction ** plan_actions, int start_level);

void reset_bitarray (register int *vector, register int dim);

Bool is_term_condition_reached();

int is_fact_in_additive_effects (int act_pos, int fact_pos);
int is_fact_in_delete_effects (int act_pos, int fact_pos);
int is_fact_in_additive_effects_start (int act_pos, int fact_pos);
int is_fact_in_delete_effects_start (int act_pos, int fact_pos);
int is_fact_in_preconditions (int act_pos, int fact_pos);
int is_fact_in_preconditions_overall (int act_pos, int fact_pos);
int is_fact_in_preconditions_end (int act_pos, int fact_pos);
int fact_is_supported (int Fact_position, int Fact_level);

int get_index_of_constant (char *arg);
float get_action_cost (int pos);
float get_action_time (int pos, int level);
float get_fact_cost (int Fact_pos, int level, node_cost_list n_cost);
void set_fact_cost (inform_list Fact, node_cost_list n_cost);
float weight_cost (node_cost_list n_cost);

inline int count_bit1 (register int mask);

int check_mutex_noop_durative (register int position, int level);
int check_mutex_action (register int act_position, int level);
int check_mutex_noop (register int position, int level);
int count_mutex_noop (int noop_pos, int level);
int count_mutex_noop_at_start (int act_pos, int level);
inline int count_mutex_action (int act_pos, int level);

void insert_element_in_neighb (neighb_list n_elem);
void insert_treated_noop_chain (inform * act, unsigned int pos);

int choose_actions_dg_list (register inform_list inform_tofix, int initialize);
int choose_actions_treated_fact (register inform_list inform_tofix, int initialize);

void reset_neighborhood ();

int continue_to_optimize ();

void set_heuristic_parameters (int num_run, int num_sol);
int set_param (int num_unsat);

float DeltaTime (struct tms start, struct tms end);

void free_gplan_actions (PlanAction * gplan_actions);

void get_path (char *full_file_name, char *result);
void initialize_preset_values (void);

void lpg_usage (void);
void allocate_after_parser();

void so_signal_management();

#endif
