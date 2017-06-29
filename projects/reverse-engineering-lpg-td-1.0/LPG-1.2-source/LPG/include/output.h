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
 * File: output.h
 * Description: print headers
 *
 * Author: Joerg Hoffmann 1999
 *
 *********************************************************************/ 





#ifndef _OUTPUT_H
#define _OUTPUT_H



void print_FactList( FactList *list, char *sepf, char *sept );
void print_hidden_TokenList( TokenList *list, char *sep );
void print_indent( int indent );
void print_PlNode( PlNode *plnode, int indent );
void print_Wff( WffNode *n, int indent );
void print_plops( PlOperator *plop );
void print_Operator( Operator *o );
void print_NormOperator( NormOperator *o );
void print_MixedOperator( MixedOperator *o );
void print_PseudoAction( PseudoAction *o );
void print_Action( Action *a );
void print_type( int t );
void print_Fact( Fact *f ); 
void print_ft_name( int index );
void print_op_name( int index );



void print_plan( void );



#endif /* _OUTPUT_H */
