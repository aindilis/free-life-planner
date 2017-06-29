%%
%% FLUX: a Prolog library for high-level programming of cognitive agents
%% Copyright 2003, 2004  Michael Thielscher
%% This file belongs to the flux kernel package distributed at
%%   http://www.fluxagent.org
%%



:- [flux_swi].



%%% WORLD 

%%% has(1) = has(gold) ; has(2) = has(arrow)

%%%
%%% Specify range of cave
%%%

xdim(10).
ydim(12).

%%%
%%% Specify number of randomly generated pits
%%%

no_of_random_pits(12).


state_update(Z1,enter,Z2,[B,S,G]) :-
  update(Z1,[at(1,1),facing(1)],[],Z2),
  breeze_perception(1,1,B,Z2),
  stench_perception(1,1,S,Z2),
  glitter_perception(1,1,G,Z2).

state_update(Z1,exit,Z2,[]) :-
  holds(facing(D),Z1),
  update(Z1,[],[at(1,1),facing(D)],Z2).

state_update(Z1,turn,Z2,[]) :-
  holds(facing(D),Z1),
  (D#<4 #/\ D1#=D+1) #\/ (D#=4 #/\ D1#=1),
  update(Z1,[facing(D1)],[facing(D)],Z2).

state_update(Z1,grab,Z2,[]) :-
  holds(at(X,Y),Z1),
  update(Z1,[has(1)],[gold(X,Y)],Z2).

state_update(Z1,shoot,Z2,[S]) :-
  ( S=true, update(Z1,[dead],[has(2)],Z2)
    ; S=false, update(Z1,[],[has(2)],Z2) ).

state_update(Z1,go,Z2,[B,S,G]) :-
  holds(at(X,Y),Z1), holds(facing(D),Z1),
  adjacent(X,Y,D,X1,Y1),
  update(Z1,[at(X1,Y1)],[at(X,Y)],Z2),
  breeze_perception(X1,Y1,B,Z2),
  stench_perception(X1,Y1,S,Z2),
  glitter_perception(X1,Y1,G,Z2).

stench_perception(X,Y,Percept,Z) :-
  XE#=X+1, XW#=X-1, YN#=Y+1, YS#=Y-1,
  ( Percept=false, not_holds(wumpus(XE,Y),Z),
                   not_holds(wumpus(XW,Y),Z),
                   not_holds(wumpus(X,YN),Z),
                   not_holds(wumpus(X,YS),Z) ;
    Percept=true,
      or_holds([wumpus(XE,Y),wumpus(X,YN),
                wumpus(XW,Y),wumpus(X,YS)],Z) ).

breeze_perception(X,Y,Percept,Z) :-
  XE#=X+1, XW#=X-1, YN#=Y+1, YS#=Y-1,
  ( Percept=false, not_holds(pit(XE,Y),Z),
                   not_holds(pit(XW,Y),Z),
                   not_holds(pit(X,YN),Z),
                   not_holds(pit(X,YS),Z) ;
    Percept=true,
      or_holds([pit(XE,Y),pit(X,YN),
                pit(XW,Y),pit(X,YS)],Z) ).

glitter_perception(X,Y,Percept,Z) :-
  Percept=false, not_holds(gold(X,Y),Z) ;
  Percept=true,  holds(gold(X,Y),Z).

adjacent(X,Y,D,X1,Y1) :-
  xdim(XD), ydim(YD),
  X in 1..XD, X1 in 1..XD, Y in 1..YD, Y1 in 1..YD, D in 1..4,
      (D#=1) #/\ (X1#=X)   #/\ (Y1#=Y+1) % north
  #\/ (D#=3) #/\ (X1#=X)   #/\ (Y1#=Y-1) % south
  #\/ (D#=2) #/\ (X1#=X+1) #/\ (Y1#=Y)   % east
  #\/ (D#=4) #/\ (X1#=X-1) #/\ (Y1#=Y).  % west


%%% INIT

init(Z0) :- Z0 = [has(2),wumpus(WX,WY)|Z],
            xdim(XD), ydim(YD), XD1 is XD+1, YD1 is YD+1,
            WX in 1..XD, WY in 1..YD,
            not_holds(wumpus(1,1),Z0),
            not_holds_all(wumpus(_,_),Z),
            not_holds(dead,Z),
            not_holds(pit(1,1),Z),
            not_holds_all(pit(_,0),Z), %boundary
            not_holds_all(pit(_,YD1),Z),
            not_holds_all(pit(0,_),Z),
            not_holds_all(pit(XD1,_),Z),
            not_holds_all(at(_,_),Z),
            not_holds_all(facing(_),Z),
            duplicate_free(Z0).

main :- init_simulator,
        init(Z0), execute(enter,Z0,Z1),
        Cpts=[1,1,[1,2]], Vis=[[1,1]], Btr=[], 
        main_loop(Cpts,Vis,Btr,Z1).

main_loop([X,Y,Choices|Cpts],Vis,Btr,Z) :-
  Choices=[Dir|Dirs] ->
    (explore(X,Y,Dir,Vis,Z,Z1) ->
       knows_val([X1,Y1],at(X1,Y1),Z1),
       hunt_wumpus(X1,Y1,Z1,Z2),
       (knows(gold(X1,Y1),Z2) ->
          execute(grab,Z2,Z3), go_home(Z3)
        ; Cpts1=[X1,Y1,[1,2,3,4],X,Y,Dirs|Cpts],
          Vis1=[[X1,Y1]|Vis], Btr1=[X,Y|Btr],
          main_loop(Cpts1,Vis1,Btr1,Z2) )
     ; main_loop([X,Y,Dirs|Cpts],Vis,Btr,Z) )
  ; backtrack(Cpts,Vis,Btr,Z).

explore(X,Y,D,V,Z1,Z2) :-
  adjacent(X,Y,D,X1,Y1), \+ member([X1,Y1],V),
  knows_not(pit(X1,Y1),Z1),
  (knows_not(wumpus(X1,Y1),Z1);knows(dead,Z1)),
  turn_to(D,Z1,Z), execute(go,Z,Z2).

backtrack(_,_,[],Z) :- execute(exit,Z,_).
backtrack(Cpts,Vis,[X,Y|Btr],Z) :-
  go_back(X,Y,Z,Z1), main_loop(Cpts,Vis,Btr,Z1).

go_back(X,Y,Z1,Z2) :-
  holds(at(X1,Y1),Z1), adjacent(X1,Y1,D,X,Y),
  turn_to(D,Z1,Z), execute(go,Z,Z2).

turn_to(D,Z1,Z2) :-
  knows(facing(D),Z1) -> Z2=Z1
  ; execute(turn,Z1,Z), turn_to(D,Z,Z2).

hunt_wumpus(X,Y,Z1,Z2) :-
  \+ knows(dead,Z1),
  knows_val([WX,WY],wumpus(WX,WY),Z1),
  in_direction(X,Y,D,WX,WY)
  -> turn_to(D,Z1,Z), execute(shoot,Z,Z2)
   ; Z2=Z1.

in_direction(X,Y,D,X1,Y1) :-
  xdim(XD), ydim(YD),
  X in 1..XD, X1 in 1..XD, Y in 1..YD, Y1 in 1..YD, D in 1..4,
      (D#=1) #/\ (X1#=X) #/\ (Y1#>Y)  % north
  #\/ (D#=3) #/\ (X1#=X) #/\ (Y1#<Y)  % south
  #\/ (D#=2) #/\ (X1#>X) #/\ (Y1#=Y)  % east
  #\/ (D#=4) #/\ (X1#<X) #/\ (Y1#=Y). % west

go_home(Z) :- write('Planning...'),
              a_star_plan(Z,S), execute(S,Z,Z1), execute(exit,Z1,_).

%%
%% a_star_plan(Z,S)
%%
%% use A* planning to find situation S representing the shortest path to (1,1)
%%

:- dynamic visited/2.

a_star_plan(Z,S) :-
   retractall(visited(_,_)),
   knows_val([X,Y],at(X,Y),Z), assertz(visited(X,Y)),
   a_star(Z,[[],0,100000],S).

a_star(Z,[Sit,Cost,_|L],S) :-
   findall([A,H], a_star_do(Z,Sit,A,H), Actions),
   ( member([Action,0], Actions) -> S=do(Action,Sit)
     ;
     insert_all(Actions, Sit, Cost, L, L1),
     a_star(Z, L1, S) ).

insert_all([],_,_,L,L).

insert_all([[A,H]|As],S,C,L,L2) :-
   insert_all(As,S,C,L,L1),
   Cost is C+1, Heuristic is Cost+H,
   ins(do(A,S),Cost,Heuristic,L1,L2).

ins(S1,C1,H1,[S2,C2,H2|L],L2) :-
   ( H1>H2 -> ins(S1,C1,H1,L,L1), L2=[S2,C2,H2|L1]
     ;
     L2=[S1,C1,H1,S2,C2,H2|L] ).

ins(S,C,H,[],[S,C,H]).

a_star_do(Z,S,A,H) :-
  ( S=do(go_to(X,Y),_) -> true ; knows_val([X,Y],at(X,Y),Z) ),
  ( D=4 ; D=3 ; D=2 ; D=1 ),
  adjacent(X,Y,D,X1,Y1), \+ visited(X1,Y1),
  knows_not(pit(X1,Y1),Z),
  ( \+ knows(dead,Z)->knows_not(wumpus(X1,Y1),Z)
    ; true ),
  A = go_to(X1,Y1),
  assertz(visited(X1,Y1)),
  H is X1+Y1-2.



complex_action(do(A,S),Z1,Z2) :-
  execute(S,Z1,Z), execute(A,Z,Z2).

complex_action(go_to(X,Y),Z1,Z2) :-
  holds(at(X1,Y1),Z1), adjacent(X1,Y1,D,X,Y),
  turn_to(D,Z1,Z), execute(go,Z,Z2).





:- use_module( library(random)).

:- dynamic current_state/1.

init_simulator :- init_scenario,
                  retractall(current_state(_)), assertz(current_state([])).

:- dynamic wumpus/2,pit/2,gold/2.

init_scenario :-

   retractall(wumpus(_,_)), retractall(pit(_,_)), retractall(gold(_,_)),

   xdim(XD), ydim(YD),

   random(0,4294967296,N1), random(0,4294967296,N2), XW is N1 mod XD + 1, YW is N2 mod YD + 1,
   ( XW=1, YW=1 -> true ; assertz(wumpus(XW,YW)), write(wumpus(XW,YW)) ),

   random(0,4294967296,N3), random(0,4294967296,N4), XG is N3 mod XD + 1, YG is N4 mod YD + 1,
   assertz(gold(XG,YG)), write(gold(XG,YG)),

   no_of_random_pits(P), create_pits(P).

create_pits(0) :- !.
create_pits(M) :-
   xdim(XD), ydim(YD),
   random(0,4294967296,N1), random(0,4294967296,N2), XP is N1 mod XD + 1, YP is N2 mod YD + 1,
   ( XP+YP < 4 -> create_pits(M) ; assertz(pit(XP,YP)), write(pit(XP,YP)) ),
   M1 is M-1, create_pits(M1).


:- dynamic((state_update/3,state_update/4,causes/6,ab_state_update/3,ab_state_update/4,
            perform/2,perform/3,plan_cost/3,plan_cost/4)).


perform(turn, []) :-
        write('turn'), nl,
        current_state([at(X,Y),facing(D)]),
        retract(current_state([at(X,Y),facing(D)])),
        ( D < 4 -> D1 is D+1 ; D1 is 1 ),
        assertz(current_state([at(X,Y),facing(D1)])).

perform(enter, [Breeze,Stench,Glitter]) :-
        write('enter'), nl,
        current_state(Z),
        retract(current_state(Z)),
        assertz(current_state([at(1,1),facing(1)])),
        ( gold(1,1) -> Glitter = true ; Glitter = false ),
        ( (wumpus(1,2) ; wumpus(2,1)) -> Stench = true ;
            Stench = false ),
        ( (pit(2,1) ; pit(1,2)) -> Breeze = true ;
            Breeze = false ).

perform(exit, []) :-
        write('exit'), nl,
        current_state([at(X,Y),facing(D)]),
        retract(current_state([at(X,Y),facing(D)])),
        assertz(current_state([])).

perform(grab, []) :-
        write('grab'), nl.

perform(shoot, [Scream]) :-
        write('shoot'), nl,
        current_state([at(X,Y),facing(D)]),
        wumpus(WX, WY),
        ( in_direction(X, Y, D, WX, WY), Scream = true ; Scream = false ).

perform(go, [Breeze,Stench,Glitter]) :-
        write('go'), nl,
        current_state([at(X,Y),facing(D)]),
        retract(current_state([at(X,Y),facing(D)])),
        ( D = 1 -> X1 is X, Y1 is Y+1 ;
          D = 3 -> X1 is X, Y1 is Y-1 ;
          D = 2 -> X1 is X+1, Y1 is Y ;
          D = 4 -> X1 is X-1, Y1 is Y ),
        assertz(current_state([at(X1,Y1),facing(D)])),
        ( gold(X1,Y1) -> Glitter = true ; Glitter = false ),
        X_east is X1+1, X_west is X1-1, Y_north is Y1+1, Y_south is Y1-1,
        ( (wumpus(X_east,Y1) ; wumpus(X1,Y_north) ;
           wumpus(X_west,Y1) ; wumpus(X1,Y_south)) -> Stench = true ;
            Stench = false ),
        ( (pit(X_east,Y1) ; pit(X1,Y_north) ;
           pit(X_west,Y1) ; pit(X1,Y_south)) -> Breeze = true ;
            Breeze = false ).

