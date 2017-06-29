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
 * File: memory.h
 * Description: Creation / Deletion functions for all data structures.
 *
 * Author: Joerg Hoffmann / Frank Rittinger
 *
 *********************************************************************/ 






#ifndef _MEMORY_H
#define _MEMORY_H





char *new_Token( int len );
TokenList *new_TokenList( void );
FactList *new_FactList( void );
TypedList *new_TypedList( void );
TypedListList *new_TypedListList( void );
PlNode *new_PlNode( Connective c );
PlOperator *new_PlOperator( char *name );
PlOperator *new_axiom_op_list( void );



Fact *new_Fact( void );
Facts *new_Facts( void );
WffNode *new_WffNode( Connective c );
Literal *new_Literal( void );
Effect *new_Effect( void );
Operator *new_Operator( char *name, int norp );
NormEffect *new_NormEffect1( Effect *e );
NormEffect *new_NormEffect2( NormEffect *e );
NormOperator *new_NormOperator( Operator *op );
EasyTemplate *new_EasyTemplate( NormOperator *op );
MixedOperator *new_MixedOperator( Operator *op );
PseudoActionEffect *new_PseudoActionEffect( void );
PseudoAction *new_PseudoAction( MixedOperator *op );
Action *new_Action( void );
EhcNode *new_EhcNode( void );
EhcHashEntry *new_EhcHashEntry( void );
PlanHashEntry *new_PlanHashEntry( void );
BfsNode *new_BfsNode( void );
BfsHashEntry *new_BfsHashEntry( void );







void free_TokenList( TokenList *source );
void free_FactList( FactList *source );
void free_PlNode( PlNode *node );
void free_PlOperator( PlOperator *o );
void free_Operator( Operator *o );
void free_WffNode( WffNode *w );
void free_NormEffect( NormEffect *e );
void free_partial_Effect( Effect *e );
void free_NormOperator( NormOperator *o );
void free_single_NormEffect( NormEffect *e );
void free_single_EasyTemplate( EasyTemplate *t );
void free_TypedList( TypedList *t );
void free_TypedListList( TypedListList *t );





#endif /* _MEMORY_H */
