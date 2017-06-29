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
 * File: inst_hard.h
 * Description: headers for multiplying hard operators.
 *
 *
 * Author: Joerg Hoffmann 2000
 *
 *********************************************************************/ 








#ifndef _INST_HARD_H
#define _INST_HARD_H



void build_hard_action_templates( void );



void cleanup_hard_domain( void );
Bool var_used_in_literals( int code_var, Literal *ef );
void decrement_inferior_vars_in_literals( int var, Literal *ef );



void multiply_hard_op_parameters( void );
void create_hard_mixed_operators( Operator *o, int curr_var );
Effect *instantiate_Effect( Effect *e );
WffNode *instantiate_wff( WffNode *w );
Bool full_possibly_positive( Fact *f );
Bool full_possibly_negative( Fact *f );
int instantiated_fact_adress( Fact *f );



void multiply_hard_effect_parameters( void );
void create_hard_pseudo_effects( PseudoAction *a, Effect *e, int curr_var );
void make_instantiate_literals( PseudoActionEffect *e, Literal *ll );



#endif /* _INST_HARD_H */
