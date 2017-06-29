
/*********************************************************************
 * (C) Copyright 2000 Albert Ludwigs University Freiburg
 *     Institute of Computer Science
 *
 * All rights reserved. Use of this software is permitted for 
 * non-commercial research purposes, and it may be copied only 
 * for that use.  All copies must include this copyright message.
 * This software is made available AS IS, and neither the authors
 * nor the  Albert Ludwigs University Freiburg make any warranty
 * about the software or its performance. 
 *********************************************************************/














/*********************************************************************
 * File: inst_easy.h
 * Description: headers for multiplying easy operators.
 *
 *
 * Author: Joerg Hoffmann 2000
 *
 *********************************************************************/ 








#ifndef _INST_EASY_H
#define _INST_EASY_H



void build_easy_action_templates( void );



void cleanup_easy_domain( void );
Bool identical_fact( Fact *f1, Fact *f2 );
void remove_unused_easy_parameters( void );
void replace_var_entries( NormOperator *o, int p0, int p1 );
void decrement_var_entries( NormOperator *o, int start );
void remove_unused_easy_effect_parameters( NormOperator *o, NormEffect *e );
void handle_empty_easy_parameters( void );



void encode_easy_unaries_as_types( void );
int create_intersected_type( TypeArray T, int num_T );
int find_intersected_type( TypeArray T, int num_T );



void multiply_easy_effect_parameters( void );
void unify_easy_inertia_conditions( int curr_inertia );
void multiply_easy_non_constrained_effect_parameters( int curr_parameter );



void multiply_easy_op_parameters( void );
void unify_easy_inertia_preconds( int curr_inertia );
void multiply_easy_non_constrained_op_parameters( int curr_parameter );



#endif /* _INST_EASY_H */
