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


/**
   The following parts of this file have been modified by the 
   LPG developers (DEA - University of Brescia):

   - Added prototypes:
   - reduce_pddl2_to_pddl1
   - reduce_PlOperator
   - reduce_PlNode
   - reduce_PlGoal
   - remove_true_nodes
   - count_num_preconds_and_effects
**/









/*********************************************************************
 * File: parse.h
 * Description: Functions for the pddl parser
 *
 * Author: Frank Rittinger 1998 / Joerg Hoffmann 1999
 *
 *********************************************************************/ 





#ifndef _PARSE_H
#define _PARSE_H



char *copy_Token( char *s );
TokenList *copy_TokenList( TokenList *source );
void strupcase( char *from );
char *rmdash( char *s );



void build_orig_constant_list( void );
void collect_type_names_in_pl( PlNode *n );
int get_type( char *str );
void make_either_ty( TypedList *tyl );
void make_either_ty_in_pl( PlNode *n );
void normalize_tyl_in_pl( PlNode **n );



Bool make_adl_domain( void );
Bool make_conjunction_of_atoms( PlNode **n );
Bool is_wff( PlNode *n );
Bool make_effects( PlNode **n );
Bool is_eff_literal( PlNode *n );
Bool make_conjunction_of_literals( PlNode **n );

/*
 * DEA - University of Brescia
 */

void reduce_pddl2_to_pddl1 (void);
void reduce_PlOperator (PlOperator * plop);
void reduce_PlNode (PlNode * pln);
void reduce_PlGoal (PlNode * pln);
int remove_true_nodes (PlNode * pln);
void count_num_preconds_and_effects ();

/*
 * End of DEA
 */

#endif /* PARSE */
