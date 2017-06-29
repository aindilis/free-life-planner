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
 * File: relax.c
 * Description: this file handles the relaxed planning problem, i.e.,
 *              the code is responsible for the heuristic evaluation
 *              of states during search.
 *
 *              --- THE HEART PEACE OF THE FF PLANNER ! ---
 *
 *              - fast (time critical) computation of the relaxed fixpoint
 *              - extraction of as short as possible plans, without search
 *
 *
 *
 *  ----- UPDATED VERSION TO HANDLE NORMALIZED ADL OPERATORS -----
 *
 *
 *
 * Author: Joerg Hoffmann 2000
 *
 *********************************************************************/ 









#include "ff.h"

#include "output.h"
#include "memory.h"

#include "relax.h"
#include "search.h"













/* local globals
 */








/* in agenda driven algorithm, the current set of goals is this
 */
State lgoals;



/* fixpoint
 */
int *lF;
int lnum_F;
int *lE;
int lnum_E;

int *lch_E;
int lnum_ch_E;

int *l0P_E;
int lnum_0P_E;





/* 1P extraction
 */
int **lgoals_at;
int *lnum_goals_at;

int *lch_F;
int lnum_ch_F;

int *lused_O;
int lnum_used_O;











/*************************************
 * helper, for -1 == INFINITY method *
 *************************************/












Bool LESS( int a, int b )

{

  if ( a == INFINITY ) {
    return FALSE;
  }

  if ( b == INFINITY ) {
    return TRUE;
  }

  return ( a < b ? TRUE : FALSE );

}












/***********************************
 * FUNCTIONS ACCESSED FROM OUTSIDE *
 ***********************************/






Bool lnew_goal;






int get_1P_and_H( State *S, State *current_goals, int new_goal )

{

  int h, max;

  source_to_dest( &lgoals, current_goals );  
  if ( new_goal != -1 ) {
    lnew_goal = TRUE;
  } else {
    lnew_goal = FALSE;
  }
  
  gevaluated_states++;

  max = build_fixpoint( S );
  h = extract_1P( max, TRUE );

  if ( gcmd_line.display_info == 122 ) {
    print_fixpoint_result();
  }

  reset_fixpoint();

  return h;

}



int get_1P( State *S, State *current_goals )

{

  int h, max;

  source_to_dest( &lgoals, current_goals );  

  gevaluated_states++;

  max = build_fixpoint( S );
  h = extract_1P( max, FALSE );

  if ( gcmd_line.display_info == 122 ) {
    print_fixpoint_result();
  }

  reset_fixpoint();

  return h;

}



void get_A( State *S )

{

  int i;

  initialize_fixpoint( S );
  
  for ( i = 0; i < lnum_F; i++ ) {
    activate_ft( lF[i], 0 );
  }

  for ( i = 0; i < lnum_0P_E; i++ ) {
    if ( gef_conn[l0P_E[i]].in_E ) {
      continue;
    }
    new_ef( l0P_E[i] );
  }
  collect_A_info();

  reset_fixpoint();

}



void collect_A_info( void )

{

  static Bool first_call = TRUE;

  int i;

  if ( first_call ) {
    gA = ( int * ) calloc( gnum_op_conn, sizeof( int ) );
    gnum_A = 0;
    first_call = FALSE;
  }

  for ( i = 0; i < gnum_A; i++ ) {
    gop_conn[gA[i]].is_in_A = FALSE;
  }
  gnum_A = 0;

  for ( i = 0; i < lnum_E; i++ ) {
    if ( gop_conn[gef_conn[lE[i]].op].is_in_A ) {
      continue;
    }
    gop_conn[gef_conn[lE[i]].op].is_in_A = TRUE;
    gA[gnum_A++] = gef_conn[lE[i]].op;
  }

}















/*******************************
 * RELAXED FIXPOINT ON A STATE *
 *******************************/
















int build_fixpoint( State *S )

{

  int start_ft, stop_ft, start_ef, stop_ef, i, time = 0;

  static Bool first_call = TRUE;

  if ( first_call ) {
    /* get memory for local globals
     */
    lF = ( int * ) calloc( gnum_ft_conn, sizeof( int ) );
    lE = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );
    lch_E = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );
    l0P_E = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );
    
    /* initialize connectivity graph members for
     * relaxed planning
     */
    lnum_0P_E = 0;
    for ( i = 0; i < gnum_ef_conn; i++ ) {      
      gef_conn[i].level = INFINITY;    
      gef_conn[i].in_E = FALSE;
      gef_conn[i].num_active_PCs = 0;
      gef_conn[i].ch = FALSE;
      
      if ( gef_conn[i].num_PC == 0 ) {
	l0P_E[lnum_0P_E++] = i;
      }
    }
    for ( i = 0; i < gnum_op_conn; i++ ) {      
      gop_conn[i].is_in_A = FALSE;
      gop_conn[i].is_in_H = FALSE;
    }
    for ( i = 0; i < gnum_ft_conn; i++ ) {
      gft_conn[i].level = INFINITY;
      gft_conn[i].in_F = FALSE;
    }
    first_call = FALSE;
  }

  initialize_fixpoint( S );

  start_ft = 0;
  start_ef = 0;
  while ( TRUE ) {
    if ( all_goals_activated( time ) ) {
      break;
    }

    stop_ft = lnum_F;
    for ( i = start_ft; i < stop_ft; i++ ) {
      activate_ft( lF[i], time );
    }

    if ( time == 0 ) {
      for ( i = 0; i < lnum_0P_E; i++ ) {
	if ( gef_conn[l0P_E[i]].in_E ) {
	  continue;
	}
	new_ef( l0P_E[i] );
      }
    }

    stop_ef = lnum_E;
    for ( i = start_ef; i < stop_ef; i++ ) {
      activate_ef( lE[i], time );
    }

    if ( stop_ft == lnum_F ) {
      break;
    }

    start_ft = stop_ft;
    start_ef = stop_ef;
    time++;
  }

  return time;

}    



void initialize_fixpoint( State *S )

{

  int i;

  lnum_E = 0;
  lnum_ch_E = 0;

  lnum_F = 0;
  for ( i = 0; i < S->num_F; i++ ) {
    if ( gft_conn[S->F[i]].in_F ) {
      continue;
    }
    new_fact( S->F[i] );
  }

}
   


void activate_ft( int index, int time )

{

  int i;

  gft_conn[index].level = time;

  for ( i = 0; i < gft_conn[index].num_PC; i++ ) {
    gef_conn[gft_conn[index].PC[i]].num_active_PCs++;
    if ( !gef_conn[gft_conn[index].PC[i]].ch ) {
      gef_conn[gft_conn[index].PC[i]].ch = TRUE;
      lch_E[lnum_ch_E++] = gft_conn[index].PC[i];
    }
    if ( gef_conn[gft_conn[index].PC[i]].num_active_PCs ==
	 gef_conn[gft_conn[index].PC[i]].num_PC ) {
      new_ef( gft_conn[index].PC[i] );
    }
  }

}



void activate_ef( int index, int time )

{

  int i;

  gef_conn[index].level = time;

  for ( i = 0; i < gef_conn[index].num_A; i++ ) {
    if ( gft_conn[gef_conn[index].A[i]].in_F ) {
      continue;
    }
    new_fact( gef_conn[index].A[i] );
  }

}



void new_fact( int index )

{

  lF[lnum_F++] = index;
  gft_conn[index].in_F = TRUE;

}



void new_ef( int index )

{

  lE[lnum_E++] = index;
  gef_conn[index].in_E = TRUE;

}



void reset_fixpoint( void )

{

  int i;

  for ( i = 0; i < lnum_F; i++ ) {
    gft_conn[lF[i]].level = INFINITY;
    gft_conn[lF[i]].in_F = FALSE;
  }

  for ( i = 0; i < lnum_E; i++ ) {
    gef_conn[lE[i]].level = INFINITY;
    gef_conn[lE[i]].in_E = FALSE;
  }

  for ( i = 0; i < lnum_ch_E; i++ ) {
    gef_conn[lch_E[i]].num_active_PCs = 0;
    gef_conn[lch_E[i]].ch = FALSE;
  }

}



Bool all_goals_activated( int time ) 

{

  int i;

  if ( lnew_goal ) {
    for ( i = 0; i < ggoal_state.num_F; i++ ) {
      if ( !gft_conn[ggoal_state.F[i]].in_F ) {
	return FALSE;
      }
    }
    for ( i = 0; i < ggoal_state.num_F; i++ ) {
      if ( gft_conn[ggoal_state.F[i]].level == INFINITY ) {
	gft_conn[ggoal_state.F[i]].level = time;
      }
    }
  } else {
    for ( i = 0; i < lgoals.num_F; i++ ) {
      if ( !gft_conn[lgoals.F[i]].in_F ) {
	return FALSE;
      }
    }
    for ( i = 0; i < lgoals.num_F; i++ ) {
      if ( gft_conn[lgoals.F[i]].level == INFINITY ) {
	gft_conn[lgoals.F[i]].level = time;
      }
    }
  }

  return TRUE;

}



void print_fixpoint_result( void )

{

  int time, i;
  Bool hit, hit_F, hit_E;

  time = 0;
  while ( 1 ) {
    hit = FALSE;
    hit_F = FALSE;
    hit_E = FALSE;
    for ( i = 0; i < gnum_ft_conn; i++ ) {
      if ( gft_conn[i].level == time ) {
	hit = TRUE;
	hit_F = TRUE;
	break;
      }
    }
    for ( i = 0; i < gnum_ef_conn; i++ ) {
      if ( gef_conn[i].level == time ) {
	hit = TRUE;
	hit_E = TRUE;
	break;
      }
    }
    if ( !hit ) {
      break;
    }
 
    printf("\n\nLEVEL %d:", time);
    if ( hit_F ) {
      printf("\n\nFACTS:");
      for ( i = 0; i < gnum_ft_conn; i++ ) {
	if ( gft_conn[i].level == time ) {
	  printf("\n");
	  print_ft_name( i );
	}
      }
    }
    if ( hit_E ) {
      printf("\n\nEFS:");
      for ( i = 0; i < gnum_ef_conn; i++ ) {
	if ( gef_conn[i].level == time ) {
	  printf("\neffect %d to ", i);
	  print_op_name( gef_conn[i].op );
	}
      }
    }

    time++;
  }
  fflush( stdout );

}
    












/**************************************
 * FIRST RELAXED PLAN (1P) EXTRACTION *
 **************************************/













int extract_1P( int max, Bool H_info )

{

  static Bool first_call = TRUE;
  int i, max_goal_level, time, r;

  if ( first_call ) {
    for ( i = 0; i < gnum_ft_conn; i++ ) {
      gft_conn[i].is_true = INFINITY;
      gft_conn[i].is_goal = FALSE;
      gft_conn[i].ch = FALSE;
    }
    for ( i = 0; i < gnum_op_conn; i++ ) {
      gop_conn[i].is_used = INFINITY;
    }
    for ( i = 0; i < gnum_ef_conn; i++ ) {
      gef_conn[i].in_plan = FALSE;
    }
    lch_F = ( int * ) calloc( gnum_ft_conn, sizeof( int ) );
    lnum_ch_F = 0;
    lused_O = ( int * ) calloc( gnum_op_conn, sizeof( int ) );
    lnum_used_O = 0;
    gin_plan_E = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );
    gnum_in_plan_E = 0;
    first_call = FALSE;
  }

  reset_search_info();

  if ( (max_goal_level = initialize_goals( max )) == INFINITY ) {
    return INFINITY;
  }

  for ( time = max_goal_level; time > 0; time-- ) {
    achieve_goals( time );
  }
  if ( H_info ) {
    collect_H_info();
  }
  r = lnum_used_O;

  if ( lnew_goal ) {
    reset_search_info();
    source_to_dest( &lgoals, &ggoal_state );  
    if ( (max_goal_level = initialize_goals( max )) == INFINITY ) {
      return INFINITY;
    }
    for ( time = max_goal_level; time > 0; time-- ) {
      achieve_goals( time );
    }
  }

  return r;

}



int initialize_goals( int max )

{

  static Bool first_call = TRUE;
  static int highest_seen;

  int i, max_goal_level, ft;

  if ( first_call ) {
    lgoals_at = ( int ** ) calloc( RELAXED_STEPS_DEFAULT, sizeof( int * ) );
    lnum_goals_at = ( int * ) calloc( RELAXED_STEPS_DEFAULT, sizeof( int ) );
    for ( i = 0; i < RELAXED_STEPS_DEFAULT; i++ ) {
      lgoals_at[i] = ( int * ) calloc( gnum_ft_conn, sizeof( int ) );
    }
    highest_seen = RELAXED_STEPS_DEFAULT;
    first_call = FALSE;
  }

  if ( max + 1 > highest_seen ) {
    for ( i = 0; i < highest_seen; i++ ) {
      free( lgoals_at[i] );
    }
    free( lgoals_at );
    free( lnum_goals_at );
    highest_seen = max + 1;
    lgoals_at = ( int ** ) calloc( highest_seen, sizeof( int * ) );
    lnum_goals_at = ( int * ) calloc( highest_seen, sizeof( int ) );
    for ( i = 0; i < highest_seen; i++ ) {
      lgoals_at[i] = ( int * ) calloc( gnum_ft_conn, sizeof( int ) );
    }
  }

  for ( i = 0; i < max + 1; i++ ) {
    lnum_goals_at[i] = 0;
  }

  max_goal_level = 0;
  for ( i = 0; i < lgoals.num_F; i++ ) {
    ft = lgoals.F[i];
    if ( gft_conn[ft].level == INFINITY ) {
      return INFINITY;
    }
    if ( gft_conn[ft].level > max_goal_level ) {
      max_goal_level = gft_conn[ft].level;
    }
    lgoals_at[gft_conn[ft].level][lnum_goals_at[gft_conn[ft].level]++] = ft;
    gft_conn[ft].is_goal = TRUE;
    if ( !gft_conn[ft].ch ) {
      lch_F[lnum_ch_F++] = ft;
      gft_conn[ft].ch = TRUE;
    }
  }

  return max_goal_level;

}



void achieve_goals( int time )

{

  int i, j, k, ft, min_p, min_e, ef, p, op;

  if ( gcmd_line.display_info == 123 ) {
    printf("\nselecting at step %3d: ", time-1);
  }

  for ( i = 0; i < lnum_goals_at[time]; i++ ) {
    ft = lgoals_at[time][i];

    if ( gft_conn[ft].is_true == time ) {
      /* fact already added by prev now selected op
       */
      continue;
    }

    min_p = INFINITY;
    min_e = -1;
    for ( j = 0; j < gft_conn[ft].num_A; j++ ) {
      ef = gft_conn[ft].A[j];
      if ( gef_conn[ef].level != time - 1 ) continue; 
      p = 0;
      for ( k = 0; k < gef_conn[ef].num_PC; k++ ) {
	p += gft_conn[gef_conn[ef].PC[k]].level;
      }
      if ( LESS( p, min_p ) ) {
	min_p = p;
	min_e = ef;
      }
    }
    ef = min_e;
    if ( !gef_conn[ef].in_plan ) {
      gef_conn[ef].in_plan = TRUE;
      gin_plan_E[gnum_in_plan_E++] = ef;
    }

    op = gef_conn[ef].op;
    if ( gop_conn[op].is_used != time ) {
      gop_conn[op].is_used = time;
      lused_O[lnum_used_O++] = op;
      if ( gcmd_line.display_info == 123 ) {
	print_op_name( op );
	printf("\n                       ");
      }
    }

    for ( j = 0; j < gef_conn[ef].num_PC; j++ ) {
      ft = gef_conn[ef].PC[j];
      if ( gft_conn[ft].is_true == time ) {
	/* a prev at this level selected op accidently adds this precond, 
	 * so we can order that op before this one and get the precond added for free.
	 */
	continue;
      }
      if ( gft_conn[ft].is_goal ) {
	/* this fact already is a goal
	 */
	continue;
      }
      lgoals_at[gft_conn[ft].level][lnum_goals_at[gft_conn[ft].level]++] = ft;
      gft_conn[ft].is_goal = TRUE;
      if ( !gft_conn[ft].ch ) {
	lch_F[lnum_ch_F++] = ft;
	gft_conn[ft].ch = TRUE;
      }
    }

    for ( j = 0; j < gef_conn[ef].num_A; j++ ) {
      ft = gef_conn[ef].A[j];
      gft_conn[ft].is_true = time;
      if ( !gft_conn[ft].ch ) {
	lch_F[lnum_ch_F++] = ft;
	gft_conn[ft].ch = TRUE;
      }
    }
    for ( j = 0; j < gef_conn[ef].num_I; j++ ) {
      for ( k = 0; k < gef_conn[gef_conn[ef].I[j]].num_A; k++ ) {
	ft = gef_conn[gef_conn[ef].I[j]].A[k];
	gft_conn[ft].is_true = time;
	if ( !gft_conn[ft].ch ) {
	  lch_F[lnum_ch_F++] = ft;
	  gft_conn[ft].ch = TRUE;
	}
      }
    }
  }

}



void collect_H_info( void )

{

  static Bool first_call = TRUE;
  static int *H, num_H, *D;
  int i, j, k, ft, ef, op, d;

  if ( first_call ) {
    gH = ( int * ) calloc( gnum_op_conn, sizeof( int ) );
    H = ( int * ) calloc( gnum_op_conn, sizeof( int ) );
    D = ( int * ) calloc( gnum_op_conn, sizeof( int ) );
    gnum_H = 0;
    num_H = 0;
    first_call = FALSE;
  }

  for ( i = 0; i < gnum_H; i++ ) {
    gop_conn[gH[i]].is_in_H = FALSE;
  }

  num_H = 0;
  for ( i = 0; i < lnum_goals_at[1]; i++ ) {
    ft = lgoals_at[1][i];

    for ( j = 0; j < gft_conn[ft].num_A; j++ ) {
      ef = gft_conn[ft].A[j];
      if ( gef_conn[ef].level != 0 ) {
	continue;
      }
      op = gef_conn[ef].op;

      if ( gop_conn[op].is_in_H ) {
	continue;
      }
      gop_conn[op].is_in_H = TRUE;
      H[num_H++] = op;
    }
  }

  /* H collected; now order it
   * here: count number of goal- and subgoal facts that
   *       op deletes (with level 0 effects). order less deletes
   *       before more deletes.
   *       start from back of H, to prefer down under
   *       goals to upper goals.
   */
  gnum_H = 0;
  for ( i = num_H - 1; i > -1; i-- ) {
    d = 0;
    for ( j = 0; j < gop_conn[H[i]].num_E; j++ ) {
      ef = gop_conn[H[i]].E[j];
      if ( gef_conn[ef].level != 0 ) continue;
      for ( k = 0; k < gef_conn[ef].num_D; k++ ) {
	if ( gft_conn[gef_conn[ef].D[k]].is_goal ) d++;
      }
    }
    for ( j = 0; j < gnum_H; j++ ) {
      if ( D[j] > d ) break;
    }
    for ( k = gnum_H; k > j; k-- ) {
      gH[k] = gH[k-1];
      D[k] = D[k-1];
    }
    gH[j] = H[i];
    D[j] = d;
    gnum_H++;
  }
  if ( gcmd_line.display_info == 124 ) {
    printf("\ncollected H: ");
    for ( i = 0; i < gnum_H; i++ ) {
      print_op_name( gH[i] );
      printf("\n              ");
    }
  }

}



void reset_search_info( void )

{

  int i;

  for ( i = 0; i < lnum_ch_F; i++ ) {
    gft_conn[lch_F[i]].is_true = INFINITY;
    gft_conn[lch_F[i]].is_goal = FALSE;
    gft_conn[lch_F[i]].ch = FALSE;
  }
  lnum_ch_F = 0;

  for ( i = 0; i < lnum_used_O; i++ ) {
    gop_conn[lused_O[i]].is_used = INFINITY;
  }
  lnum_used_O = 0;

  for ( i = 0; i < gnum_in_plan_E; i++ ) {
    gef_conn[gin_plan_E[i]].in_plan = FALSE;
  }
  gnum_in_plan_E = 0;
  
}

