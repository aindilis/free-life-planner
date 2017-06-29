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
 *
 * File: search.h
 *
 * Description: headers of routines that search the state space
 *
 *              ADL version, Enforced Hill-climbing enhanced with
 *                           Goal-adders deletion heuristic
 *
 * Author: Joerg Hoffmann 2000
 *
 *********************************************************************/ 






#ifndef _SEARCH_H
#define _SEARCH_H



Bool do_enforced_hill_climbing( State *start, State *end );



Bool search_for_better_state( State *S, int h, State *S_, int *h_ );
void add_to_ehc_space( State *S, int op, EhcNode *father, int new_goal );
int expand_first_node( int h );



void hash_ehc_node( EhcNode *n );
Bool ehc_state_hashed( State *S );
Bool same_state( State *S1, State *S2 ) ;
int state_sum( State *S );
void reset_ehc_hash_entrys( void );



void extract_plan_fragment( State *S );
PlanHashEntry *hash_plan_state( State *S, int step );
PlanHashEntry *plan_state_hashed( State *S );



Bool new_goal_gets_deleted( EhcNode *n );



Bool do_best_first_search( void );
void add_to_bfs_space( State *S, int op, BfsNode *father );
void extract_plan( BfsNode *last );



void hash_bfs_node( BfsNode *n );
Bool bfs_state_hashed( State *S );



int result_to_dest( State *dest, State *source, int op );
void source_to_dest( State *dest, State *source );
void print_state( State S );



#endif /* _SEARCH_H */
