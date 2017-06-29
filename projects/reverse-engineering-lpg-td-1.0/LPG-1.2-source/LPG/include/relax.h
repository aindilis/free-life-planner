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
 * File: relax.h
 * Description: headers for relaxed ADL planning
 *
 * Author: Joerg Hoffmann 2000
 *
 *********************************************************************/ 






#ifndef _RELAX_H
#define _RELAX_H



Bool LESS( int a, int b );



int get_1P_and_H( State *S, State *current_goals, int new_goal );
int get_1P( State *S, State *current_goals );
void get_A( State *S );
void collect_A_info( void );



int build_fixpoint( State *S );
void initialize_fixpoint( State *S );
void activate_ft( int index, int time );
void activate_ef( int index, int time );
void new_fact( int index );
void new_ef( int index );
void reset_fixpoint( void );
Bool all_goals_activated( int time ); 
void print_fixpoint_result( void );



int extract_1P( int max, Bool H_info );
int initialize_goals( int max );
void achieve_goals( int time );
void collect_H_info( void );
void reset_search_info( void );



#endif /* _RELAX_H */


