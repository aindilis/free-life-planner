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
 * File: LpgOutput.h
 * Description: header file for the output functions.
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/ 



#ifndef _LPGOUT_H
#define _LPGOUT_H

char *print_op_name_string (int pos, char *out_string);
char *print_ft_name_string (int pos, char *out_string);
char *print_noop_name_string (int pos, char *out_string);

void print_cvar_tree (int cv_index, int level);
void print_num_levels_and_actions ();
void print_NumVar (NumVar * f, int cv_index, int level);

void print_mutex_table (void);
void print_matrs (void);
void print_mutex_result (void);

void print_cri_computed_costs (int level);

void print_cost_of_unsupported_facts ();
void print_unsup_fact_vect ();
void print_unsup_num_facts ();

void print_actions_in_plan ();
void print_actions_in_subgraph ();
void print_temporal_plan (int levels);
void my_print_plan_level (int level);
void my_print_plan_all (int max_time);
void my_print_plan (int level);

void print_parser_info_for_debug();

void store_adapted_temporal_plan (int levels, char *fact_file_name, double time);
int store_curr_plan (int max_time, PlanAction ** plan_actions);
void store_adapted_temporal_plan_ff (char *fact_file_name);

void store_action_vect (PlanAction ** plan_actions, int act_pos, float start_time, float duration);

#endif
