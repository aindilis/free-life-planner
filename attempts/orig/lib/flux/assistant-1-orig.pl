%% first task is to convert assistant program to use julian

%% FIXME: finish conversion to use julian


:- [swipl_flux/flux_swi].

:- ['../calendaring/calendaring'].
:- multifile julian:form_time/3.


time_add_julian(T1,ns(Nanoseconds),T2) :-
	%% write_term([1,T1],[quoted(true)]),nl,
	form_time(mjn(MjnT1),T1),
	%% write_term([2,MjnT1],[quoted(true)]),nl,
	MjnT2 #= MjnT1 + Nanoseconds,
	%% write_term([3,MjnT2,Nanoseconds],[quoted(true)]),nl,
        julian:form_time(mjn(MjnT2),T2).
	%% write_term([4,T2],[quoted(true)]),nl.

time_diff_julian(T1,T2,Duration) :-
	Duration = ns(Nanoseconds),
	delta_time(T1, Duration, T2).

%% time_diff(T1,T2,T) :-
%%    H1 is T1 // 100, M1 is T1 mod 100,
%%    H2 is T2 // 100, M2 is T2 mod 100,
%%    M is (H1-H2)*60 + M1-M2,
%%    T is (M//60)*100 + M mod 60.

%% time_add(T1,T2,T) :-
%%    H1 is T1 // 100, M1 is T1 mod 100,
%%    H2 is T2 // 100, M2 is T2 mod 100,
%%    H is H1+H2+(M1+M2)//60,
%%    T is H*100+(M1+M2) mod 60.


dist(1,2,900000000000). %% 15 * 60 * 1000000000.
dist(1,3,1200000000000). %% 20 * 60 * 1000000000.
dist(2,3,300000000000). %% 5 * 60 * 1000000000.

distance(X,Y,D) :- dist(X,Y,D) ; dist(Y,X,D).


poss_julian(go(X,T),Z) :-
	knows_val([X1,T1],at(X1,T1),Z),
	X1 \= X,
	(julian:compare_time(<,T1,T) ; julian:compare_time(=,T1,T)).

%% poss(go(X,T),Z) :-
%%    knows_val([X1,T1],at(X1,T1),Z),
%%    X1 \= X, T1 =< T.


poss_julian(meet(P,T),Z) :-
	knows_val([P,X,T],available(P,X,T),Z),
	knows_val([T1],at(X,T1),Z),
	(julian:compare_time(<,T1,T) ; julian:compare_time(=,T1,T)).

%% poss(meet(P,T),Z) :-
%%    knows_val([P,X,T],available(P,X,T),Z),
%%    knows_val([T1],at(X,T1),Z),
%%    T1 =< T.


poss_julian(go_meet(P,X,T),Z) :-
	knows_val([P,X,T],available(P,X,T),Z),
	knows_val([X1,T1],at(X1,T1),Z),
	(
	 X1 = X -> (julian:compare_time(<,T1,T) ; julian:compare_time(=,T1,T))
	 ;
	 distance(X1,X,D),
	 time_add_julian(T1,ns(D),T2),
	 (julian:compare_time(<,T2,T) ; julian:compare_time(=,T2,T))
	).

%% poss(go_meet(P,X,T),Z) :-
%%    knows_val([P,X,T],available(P,X,T),Z),
%%    knows_val([X1,T1],at(X1,T1),Z),
%%    ( X1 = X -> T1 =< T
%%      ;
%%      distance(X1,X,D), time_add(T1,D,T2), T2 =< T ).


state_update(Z1,go(X,T),Z2,[]) :-
	holds(at(X1,T1),Z1),
	distance(X1,X,D),
	time_add_julian(T,ns(D),T2),
	update(Z1,[at(X,T2)],[at(X1,T1)],Z2).

state_update(Z1,meet(P,T),Z2,[]) :-
	holds(request(P,D),Z1),
	holds(at(X,T1),Z1),
	time_add_julian(T,ns(D),T2),
	update(Z1,[at(X,T2)],[at(X,T1),request(P,D)],Z2).

state_update(Z1,go_meet(P,X,T),Z2,[]) :-
	holds(request(P,D),Z1),
	holds(at(X1,T1),Z1),
	time_add_julian(T,ns(D),T2),
	update(Z1,[at(X,T2)],[at(X1,T1),request(P,D)],Z2).

%% atTime is specific to the julian version

%% requestTime is specific to the julian version

requestTime_julian(M1,Time) :-
	Time is (M1 * 60) * 1000000000.

atTime_julian(Orig,Time) :-
	Hour1 is Orig // 100, Minute1 is Orig mod 100,
	%% julian:form_time([now,[Y1-M1-D1]]),
	Y1 = 2016, M1 = 2, D1 = 22,
	julian:form_time([[Y1-M1-D1,Hour1:Minute1:0],datetime(Day1,Ns1)]),
	Time = datetime(Day1,Ns1).



init(Z0) :-
	atTime_julian(930,Time1),
	atTime_julian(1000,Time2),
	atTime_julian(1200,Time3),
	atTime_julian(1045,Time4),
	atTime_julian(1245,Time5),
	atTime_julian(1030,Time6),
	atTime_julian(1245,Time7),
	requestTime_julian(30,Time8),
	requestTime_julian(45,Time9),
	requestTime_julian(60,Time10),
	Z0 = [at(1,Time1),
	      request(1,Time8),request(2,Time9),request(3,Time10),
	      available(1,1,Time2),available(1,2,Time3),
	      available(2,3,Time4),available(2,3,Time5),
	      available(3,2,Time6),available(3,3,Time7)].

main :- init(Z0), see(Z0), plan(schedule_plan, Z0, S),
	writeln(S).

schedule_plan(Z,[],Z) :- not_holds_all(request(_,_),Z).

schedule_plan(Z,[A|P],Zn) :-
	A = go_meet(_,_,_), poss_julian(A,Z), state_update(Z,A,Z1,[]),
	schedule_plan(Z1,P,Zn).

plan_cost(schedule_plan, P, Z, C) :-
	knows_val([C1],at(_,C1),Z),
	travel_time(P,1,C2),
	C is C1+C2.

travel_time([],_,0).
travel_time([go_meet(_,X,_)|P],X1,C) :-
	( X1=X -> C1=0 ; distance(X1,X,C1) ),
	travel_time(P,X,C2),
	C is C1+C2.

test_orig :-
	form_time([2016-02-28,11:00:00],datetime(Day1,Ns1)),
	form_time([2016-02-28,12:00:00],datetime(Day2,Ns2)),
	time_diff_julian(datetime(Day1,Ns1),datetime(Day2,Ns2),Duration),
	write(Duration),nl.

test_time :-
	julian:form_time([2016-02-28,11:00:00],datetime(Day1,Ns1)),
	julian:form_time([2016-02-28,13:00:00],datetime(Day2,Ns2)),
	% write_term(datetime(Day1,Ns1),[quoted(true)]),nl,
	% write_term(datetime(Day2,Ns2),[quoted(true)]),nl,
	time_diff_julian(datetime(Day1,Ns1),datetime(Day2,Ns2),Duration),
	% write_term(Duration,[quoted(true)]),nl,
	requestTime_julian(30,Time2000),
	% write_term(Time2000,[quoted(true)]),nl,
	time_add_julian(datetime(Day1,Ns1),ns(Time2000),datetime(Day3,Ns3)),
	form_time(unix(T), datetime(Day1,Ns1)),
	write_term(datetime(Day3,Ns3),[quoted(true)]),nl.
	% form_time([unix(T), [Y-M-D,H:M:S]]),
	% write_term(T,[quoted(true)]),nl.

				
see(X) :-
	write_term(X,[quoted(true)]),nl.