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
 * File: LocalSearch.h
 * Description: header file for local-search strategies
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/ 

 


#ifndef __LOCSEARCH_H 
#define __LOCSEARCH_H  

int LocalSearch (State * start_state, State * end_state, PlanAction ** plan_actions);

int not_tabu (int tofix);

int action_eff_cost (register inform_list infAction);

float find_min (constraints_list inf_tofix, int *pos_temp_vect, int num, int *num_min, int *num_neg);

float check_value (float new);

int choose_actions (register inform_list inform_tofix, int initialize);

void update_mutex_multilevel (int level,int pos_action);
void update_mutex (int pos_action);

#endif
