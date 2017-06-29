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
 * File: mutex.h
 * Description: header file for computing mutex
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/ 





#ifndef _MUTEX_H
#define _MUTEX_H

void calc_mutex (State * initial_state);
void calc_mutex_ops (void);
void calc_mutex_num_efs (void);

int are_mutex_ops (int a, int b);
int are_mutex_ops_online (int a, int b);

Bool are_goal_reachable_and_non_mutex (void);

int  count_unsatisfied_preconds(int action_number,int *fact_vector);

int is_action_prec_mutex(int action_number, int **FT_FT_mutex_pass);

void  make_actions_num_prec_vector(int *action_num_prec_vector, int *fact_vector);

#endif /* _MUTEX_H */
