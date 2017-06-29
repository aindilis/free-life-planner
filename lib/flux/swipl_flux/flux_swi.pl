%% $Id: flux.pl, v 2.0 2004/01/22 00:58:00 $
%%
%% FLUX: a Prolog library for high-level programming of cognitive agents
%% Copyright 2003, 2004  Michael Thielscher
%% This file belongs to the flux kernel package distributed at
%%   http://www.fluxagent.org
%%
%% This library is free software; you can redistribute it and/or modify it
%% under the terms of the GNU Library General Public License as published by
%% the Free Software Foundation; either version 2 of the License, or (at your
%% option) any later version.
%%
%% This library is distributed in the hope that it will be useful, but WITHOUT
%% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
%% FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
%% License for more details.
%%
%% You should have received a copy of the GNU Library General Public License
%% along with this library; if not, write to the Free Software Foundation,
%% Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
%%
%% Consult the file COPYING for license details.

:- prolog_use_module(library(clpfd)).

:- expects_dialect(sicstus).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Libraries
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%
%% term utilities (Sicstus library)
%%

:- prolog_use_module( library(terms)).

%%
%%  List operations (Sicstus library)
%%

:- prolog_use_module( library(lists)).

%%
%% finite domain constraint solver (Sicstus library)
%%

%%
%% constraint handling rules (Sicstus library)
%%

:- prolog_use_module( library(chr)).

%%
%% FLUX constraint handling rules
%%

:- ['fluent_swi.chr'].

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% State Specifications and Update
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%
%% holds(F,Z)
%%
%% asserts that fluent F holds in state Z
%%
holds(F, [F|_]).
holds(F, Z) :- nonvar(Z), Z=[F1|Z1], F\==F1, holds(F, Z1).

%%
%% holds(F,Z,Zp)
%%
%% asserts that fluent F holds in state Z
%%
%% state Zp is Z without F.
%%
holds(F, [F|Z], Z).
holds(F, Z, [F1|Zp]) :- nonvar(Z), Z=[F1|Z1], F\==F1, holds(F, Z1, Zp).

%%
%% cancel(F,Z1,Z2)
%%
%% state Z2 is state Z1 with all (positive, negative, disjunctive)
%% knowledge of fluent F canceled
%%
cancel(F,Z1,Z2) :-
   var(Z1)    -> cancel(F,Z1), cancelled(F,Z1), Z2=Z1 ;
   Z1 = [G|Z] -> ( F\=G -> cancel(F,Z,Z3), Z2=[G|Z3]
                         ; cancel(F,Z,Z2) ) ;
   Z1 = []    -> Z2 = [].

%%
%% minus(Z1,ThetaN,Z2)
%%
%% state Z2 is state Z1 minus the fluents in list ThetaN
%%
minus_(Z, [], Z).
minus_(Z, [F|Fs], Zp) :-
   ( \+ not_holds(F, Z) -> holds(F, Z, Z1) ;
     \+ holds(F, Z)     -> Z1 = Z
                         ; cancel(F, Z, Z1), not_holds(F, Z1) ),
   minus_(Z1, Fs, Zp).

%%
%% plus(Z1,ThetaP,Z2)
%%
%% state Z2 is state Z1 plus the fluents in list ThetaP
%%
plus_(Z, [], Z).
plus_(Z, [F|Fs], Zp) :-
   ( \+ holds(F, Z)     -> Z1=[F|Z] ;
     \+ not_holds(F, Z) -> Z1=Z
                         ; cancel(F, Z, Z2), not_holds(F, Z2), Z1=[F|Z2] ),
   plus_(Z1, Fs, Zp).

%%
%% update(Z1,ThetaP,ThetaN,Z2)
%%
%% state Z2 is state Z1 minus the fluents in list ThetaN
%% plus the fluents in list ThetaP
%%
update(Z1, ThetaP, ThetaN, Z2) :-
   minus_(Z1, ThetaN, Z), plus_(Z, ThetaP, Z2).


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% State Knowledge
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%
%% knows(F,Z)
%% 
%% ground fluent F is known to hold in state Z
%%
knows(F, Z) :- \+ not_holds(F, Z).

%%
%% knows_not(F,Z)
%% 
%% ground fluent F is known not to hold in state Z
%%
knows_not(F, Z) :- \+ holds(F, Z).

%%
%% knows_val(X,F,Z)
%% 
%% there is an of the variables in X for which
%% non-ground fluent F is known to hold in state Z
%%
%% Example:
%%
%% ?- knows_val([X], f(X,Y), [f(1,3),f(2,U),f(V,2)|_])
%%
%% X=1 More?
%% X=2 More?
%% No
%%
knows_val(X, F, Z) :- k_holds(F, Z), knows_val(X).

k_holds(F, Z) :- nonvar(Z), Z=[F1|Z1],
                 ( instance1(F1, F), F=F1 ; k_holds(F, Z1) ).

:- dynamic known_val/1.

knows_val(X) :- dom(X), \+ nonground(X), ambiguous(X) -> false.
knows_val(X) :- retract(known_val(X)).

dom([]).
dom([X|Xs]) :- dom(Xs), ( is_domain(X) -> indomain(X)
                                        ; true ).

ambiguous(X) :- retract(known_val(_)) -> true
                ;
                assertz(known_val(X)), false.


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Execution
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%
%% execute(A,Z1,Z2)
%%
%% perform A and update current state Z1 to state Z2
%%
%% It assumes the definition of a predicate perform(A,Y) or perform(A,Y,E),
%% which actually performs primitive action A and returns a list Y of
%% sensing results and, in the ternary variant, a set of exogenous actions E.
%%
%% It also assumes the definition of a predicate state_update(Z1,A,Z2,Y),
%% which specifies the effect of an action A with sensing result Y as
%% update of state Z1 to state Z2. The effects of exogenous actions are assumed
%% to be specified by a predicate state_update(Z1,A,Z2) without sensing result.
%%
%% The clauses for state_update must NOT allow for backtracking for fixed sensor result.
%%
%% For troubleshooting (qualification problem), it is assumed that accidental effects
%% of non-exogenous actions are described by a predicate ab_state_update(Z1,A,Z2,Y).
%% The initial state is assumed to be specified by init(Z0).
%%
%% The clauses for ab_state_update may allow for backtracking.
%%
%% The execution of non-primitive actions A is defined as the predicate
%% complex_action(A,Z1,Z2) such that the final result of doing A in state Z1 is
%% state Z2.
%%
%% A can be a primitive action, a list, a conditional of the form if(F,A1,A2)
%% where F fluent, or the name of a complex action. The clause should be used
%% for non-primitive A only if its executability is guaranteed also in case
%% of possible accidents.
%%
execute(A,Z1,Z2) :-
   current_predicate(perform/2),
   perform(A,Y)    -> ( current_predicate(ab_state_update/4)
                        ->
                           ( Z1=[sit(S)|Z], ! ; S=[], Z=Z1 ),
                           ( state_update(Z,A,Z3,Y)
                             ; ab_res([[A,Y]|S],Z3) ),
                           !, Z2=[sit([[A,Y]|S])|Z3]
                        ;
                        state_update(Z1,A,Z2,Y) ) ;

   current_predicate(perform/3),
   perform(A,Y,E)  -> ( current_predicate(ab_state_update/4)
                        ->
                           ( Z1=[sit(S)|Z], ! ; S=[], Z=Z1 ),
                           ( state_update(Z,A,Z3,Y), state_updates(Z3,E,Z4)
                             ; ab_res([[A,Y,E]|S],Z4) ),
                           !, Z2=[sit([[A,Y,E]|S])|Z4]
                        ;
                        state_update(Z1,A,Z,Y), state_updates(Z,E,Z2) ) ;

   A = [A1|A2]     ->
                      execute(A1,Z1,Z), execute(A2,Z,Z2) ;

   A = if(F,A1,A2) ->
                      (holds(F,Z1) -> execute(A1,Z1,Z2)
                                    ; execute(A2,Z1,Z2)) ;

   A = []          ->
                      Z1=Z2 ;

   complex_action(A,Z1,Z2).

ab_res([],Z) :- init(Z).
ab_res([S1|S],Z) :-
   ab_res(S,Z1),
   ( S1=[A,Y] -> ( state_update(Z1,A,Z,Y) ; ab_state_update(Z1,A,Z,Y) )
     ;
     S1=[A,Y,E], ( state_update(Z1,A,Z2,Y) ; ab_state_update(Z1,A,Z2,Y) ),
                 state_updates(Z2, E, Z) ).

state_updates(Z, [], Z).
state_updates(Z1, [A|S], Z2) :-
   state_update(Z1, A, Z), state_updates(Z, S, Z2).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Planning
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%
%% plan(PlanningProblemName,Z,P)
%%
%% P is an optimal plan for PlanningProblemName with starting state Z
%%
%% It assumes the definition of a predicate PlanningProblemName(Z0,P,Z)
%% describing the search space such that plan P executed in starting
%% state Z0 results in state Z which satisfies the planning goal,
%% and the definition of plan_cost(PlanningProblemName,P,Z,C) such that
%% C is the cost of plan P resulting in state Z; or
%%
%% the definition of a predicate is PlanningProblemName(Z0,P)
%% describing the search space such that conditional plan P executed in
%% starting state Z0 necessarily results in a state in which the planning
%% goal is satisfied, and the definition of plan_cost(PlanningProblemName,P,C)
%% such that C is the cost of plan P.
%%
%% For the definition of the search space, the predicates for knowledge
%% described below can be used.
%%
:- dynamic plan_search_best/2.

plan(Problem, Z, P) :-
   assertz(plan_search_best(_,-1)),
   plan_search(Problem, Z),
   retract(plan_search_best(P,C)),
   C =\= -1.

plan_search(Problem, Z) :-
   current_predicate(Problem/2) ->
      ( PlanningProblem =.. [Problem,Z,P],
        call(PlanningProblem),
        plan_cost(Problem, P, C),
        plan_search_best(_,C1),
        ( C1 =< C, C1 =\= -1 -> fail
                              ; retract(plan_search_best(_,C1)),
                                assertz(plan_search_best(P,C)), fail )
        ;
        true ) ;
   PlanningProblem =.. [Problem,Z,P,Zn],
   call(PlanningProblem),
   plan_cost(Problem, P, Zn, C),
   plan_search_best(_,C1),
   ( C1 =< C, C1 =\= -1 -> fail
                         ; retract(plan_search_best(_,C1)),
                           assertz(plan_search_best(P,C)), fail )
   ;
   true.

%%
%% knows(F,S,Z0)
%%
%% ground fluent F is known to hold after doing S in state Z0
%%
%% S ::= [] | do(A,S)
%% A ::= primitive action | if_true(F)| if_false(F)
%% F ::= fluent
%% 
knows(F, S, Z0) :- \+ ( res(S, Z0, Z), not_holds(F, Z) ).

%%
%% knows_not(F,S,Z0)
%%
%% ground fluent F is known not to hold after doing S in state Z0
%%
knows_not(F, S, Z0) :- \+ ( res(S, Z0, Z), holds(F, Z) ).

%%
%% knows_val(X,F,S,Z0)
%%
%% there is an instance of the variables in X for which
%% non-ground fluent F is known to hold after doing S in state Z0
%%
knows_val(X, F, S, Z0) :-
   res(S, Z0, Z) -> findall(X, knows_val(X,F,Z), T),
                    assertz(known_val(T)),
                    false.
knows_val(X, F, S, Z0) :-
   known_val(T), retract(known_val(T)), member(X, T),
   \+ ( res(S, Z0, Z), not_holds_all(F, Z) ).

res([], Z0, Z0).
res(do(A,S), Z0, Z) :-
   A = if_true(F)  -> res(S, Z0, Z), holds(F, Z) ;
   A = if_false(F) -> res(S, Z0, Z), not_holds(F, Z) ;
   res(S, Z0, Z1), state_update(Z1, A, Z, _).


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Ramification Problem
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                  
%%
%% causes(Z1,P,N,Z2)
%%
%% state Z2 is the result of applying causal relationships to
%% state Z1 wrt. positive effects P and negative effects N
%%
%% It is assumed that the causal relationships of a domain are specified
%% by clauses for the predicate causes(Z1,P1,N1,Z2,P2,N2) such that an
%% indirect effect causes an automatic state transition from
%% state Z1 with positive effects P1 and negative effects N1 to
%% state Z2 with positive effects P2 and negative effects N2
%%
causes(Z,P,N,Z2) :-
   causes(Z,P,N,Z1,P1,N1) -> causes(Z1,P1,N1,Z2)
                           ; Z2=Z.

%%
%% ramify(Z1,ThetaP,ThetaN,Z2)
%%
%% state Z2 is the result of applying causal relationships after
%% removing the negative direct effects ThetaN and adding the
%% positive direct effects ThetaP to state Z2
%%
ramify(Z1,ThetaP,ThetaN,Z2) :-
   update(Z1,ThetaP,ThetaN,Z), causes(Z,ThetaP,ThetaN,Z2).
   
   
%% additional predicates necessary for Sicstus

nonground(X):- \+ ground(X).

instance1(X,Y):- subsumes_chk(Y,X).


