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
 * File: LpgTime.h
 * Description: header file for temporal information management
 *
 *   PDDL 2.1 version without conditional and quantified effects 
 *
 * Authors: Alfonso Gerevini, Marco Lazzaroni, Alessandro Saetti, 
 *          Ivan Serina, Sergio Spinoni
 *
 *********************************************************************/ 




#ifndef __TIME_H 
#define __TIME_H  

void insert_time (inform_list infAction);
void update_time (inform_list infAction);

void compress_plan ();

int insert_propagation_list (inform_list infAction);

void forward_noop_propagation_time (inform_list infNoop);
void forward_noop_remotion_time (inform_list infNoop);
void noop_remotion_time (inform_list infNoop);

void temporal_constraints (inform_list infAction);
void remove_temporal_constraints (int posAction);
int constraint_type(int posA, int posB);

void get_total_time_plan ();

void reset_constraint_matrix ();
void reset_propagation_vect ();


#endif
