%% include all of this:

%% /var/lib/myfrdcsa/repositories/external/git/RosettaCodeData/Lang/Prolog

levenshtein(S, T, R) :-
	length(S, M),
	M1 is M+1,
	length(T, N),
	N1 is N+1,
	length(Lev, N1),
	maplist(l_init(M1), Lev),
	numlist(0, N, LN),
	maplist(l_init_n, LN, Lev),
	nth0(0, Lev, Lev0),
	numlist(0, M, Lev0),

	% compute_levenshtein distance
	numlist(1, N, LN1),
	maplist(work_on_T(Lev, S), LN1, T),
	last(Lev, LevLast),
	last(LevLast, R).


work_on_T(Lev, S, J, TJ) :-
	length(S, M),
	numlist(1, M, LM),
	maplist(work_on_S(Lev, J, TJ), LM, S).

work_on_S(Lev, J, C, I, C) :-
	% same char
	!,
	I1 is I-1, J1 is J-1,
	nth0(J1, Lev, LevJ1),
	nth0(I1, LevJ1, V),
	nth0(J, Lev, LevJ),
	nth0(I, LevJ, V).


work_on_S(Lev, J, _C1, I, _C2) :-
	I1 is I-1, J1 is J - 1,
	% compute the value for deletion
	nth0(J, Lev, LevJ),
	nth0(I1, LevJ, VD0),
	VD is VD0 + 1,

	% compute the value for insertion
	nth0(J1, Lev, LevJ1),
	nth0(I, LevJ1, VI0),
	VI is VI0 + 1,

	% compute the value for substitution
	nth0(I1, LevJ1, VS0),
	VS is VS0 + 1,

	% set the minimum value to cell(I,J)
	sort([VD, VI, VS], [V|_]),

	nth0(I, LevJ, V).


l_init(Len, C) :-
	length(C, Len).

l_init_n(N, L) :-
	nth0(0, L, N).

%% test :-
%%   time(lcs("thisisatest", "testing123testing", Lcs)),
%%   writef('%s',[Lcs]).


%%   lcs([ H|L1],[ H|L2],[H|Lcs]) :- !,
%%   lcs(L1,L2,Lcs).

%%   lcs([H1|L1],[H2|L2],Lcs):-
%%   lcs(    L1 ,[H2|L2],Lcs1),
%%   lcs([H1|L1],    L2 ,Lcs2),
%%   longest(Lcs1,Lcs2,Lcs),!.

%%   lcs(_,_,[]).


%%   longest(L1,L2,Longest) :-
%%   length(L1,Length1),
%%   length(L2,Length2),
%%   ((Length1 > Length2) -> Longest = L1; Longest = L2).


%% %%declare that we will add lcs_db facts during runtime
%% :- dynamic lcs_db/3.

%% test :-
%%   retractall(lcs_db(_,_,_)), %clear the database of known results
%%   time(lcs("thisisatest", "testing123testing", Lcs)),
%%   writef('%s',[Lcs]).


%%   % check if the result is known
%%   lcs(L1,L2,Lcs) :-
%%   lcs_db(L1,L2,Lcs),!.

%%   lcs([ H|L1],[ H|L2],[H|Lcs]) :- !,
%%   lcs(L1,L2,Lcs).

%%   lcs([H1|L1],[H2|L2],Lcs) :-
%%   lcs(    L1 ,[H2|L2],Lcs1),
%%   lcs([H1|L1],    L2 ,Lcs2),
%%   longest(Lcs1,Lcs2,Lcs),!,
%%   assert(lcs_db([H1|L1],[H2|L2],Lcs)).

%%   lcs(_,_,[]).


%%   longest(L1,L2,Longest) :-
%%   length(L1,Length1),
%%   length(L2,Length2),
%%   ((Length1 > Length2) -> Longest = L1; Longest = L2).

%% %% ?- time(lcs("beginning-middle-ending","beginning-diddle-dum-ending", Lcs)),writef('%s', [Lcs]).
%% %% 				% 10,875,184 inferences, 1.840 CPU in 1.996 seconds (92% CPU, 5910426 Lips)
%% %% beginning-iddle-ending

%% %% Second version which is much faster :

%% %% ?- time(lcs("beginning-middle-ending","beginning-diddle-dum-ending", Lcs)),writef('%s', [Lcs]).
%% %% 				% 2,376 inferences, 0.010 CPU in 0.003 seconds (300% CPU, 237600 Lips)
%% %% beginning-iddle-ending