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
 * File: numeric.h
 * Description: header file for numerical quantity management
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/ 





#ifndef _NUMERIC_H
#define _NUMERIC_H

void set_numeric_flag ();

void create_descnumeff_of_efconns (void);

void make_false_all_checks_on_not_init (void);

int choose_num_actions (constraints_list unsup_numeric_fact, int initialize);
int choose_min_cost_unsup_num_fact ();

void eval_comp_var_non_recursive (int cv_index, float *in_vect, float *out_vect, int in_level, int out_level);

void refresh_cvars (int level);
void propagate_cvars (int level_from, int level_to);

void copy_num_values_from_level_to_level (int level_from, int level_to, Bool check_variations);
void insert_values_unsup_num_fact (int status, int fact, int level);

IntList *free_intlist (IntList * ilist);

float try_num_eff_in_level (int cv_index, float *in_vect, float *out_vect);

Bool is_num_prec_satisfied_in_common_level (int cvar);
Bool is_num_prec_satisfied (int cvar, int level);

void apply_numeric_effect_in_common_level (int num_eff);
void apply_numeric_effects_of_action (int act_pos, int level);

void insert_unsup_numeric_fact (int cv_index, int level);

int fix_unsup_num_fact (constraints_list unsup_numeric_fact);

void clean_unsup_num_fact ();

void remove_unsup_num_fact (int position);
void remove_numeric_effects_of_action (int act_pos, int level);

void remove_unappliable_actions ();

void sub_affects_list (int cv_index, int *bitarray);

#endif
