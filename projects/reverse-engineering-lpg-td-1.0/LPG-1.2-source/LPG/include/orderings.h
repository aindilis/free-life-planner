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
 * File: orderings.h
 *
 * Description: headers for GAM computation
 *
 * Author: Joerg Hoffmann 1999
 *
 *********************************************************************/ 







#ifndef _ORDERINGS_H
#define _ORDERINGS_H



void compute_goal_agenda( void );



void build_False_set( int ft );



void detect_ordering_constraints( void );
void setup_E( int ft );
void unsetup_E( int ft );
Bool possibly_achievable( int ft );



void build_goal_agenda( void );




#endif /* _ORDERINGS_H */
