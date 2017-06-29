% task(task1,'test',critical).
% task(task2,'test2',critical).

repl(X, N, L) :-
	length(L, N),
	maplist(=(X), L).

getNewCounter(Item,Vars,Term,ID) :-
	findall(Vars,Term,List),
	length(List,Length),
	repl(Item,Length,RepeatedList),
	%% view([RepeatedList,ResultList1,List]),
	maplist(atom_concat,RepeatedList,ResultList1,List),
	maplist(atom_number,ResultList1,ResultList2),
 	incrementNewCounter(Item,Vars,Term,ResultList2,ID),!.

incrementNewCounter(Item,Vars,Term,List,ID) :-
	List \= [],
	max_list(List,Num),
	Num2 is Num + 1,
	getCounterID(Item,Vars,Term,Num2,ID).
incrementNewCounter(Item,Vars,Term,List,ID) :-
	List = [],
	getCounterID(Item,Vars,Term,1,ID).

getCounterID(Item,Vars,Term,Num,ID) :-
	%% view([items,Item,Vars,Term,Num,ID]),
	atom_concat(Item,Num,ID).

%% getNewTask(Task1,Importance1,TaskID) :-
%% 	getNewCounter(task,ID,task(ID,Task,Importance),TaskID),
%% 	assert(task(TaskID,Task1,Importance1)).





%% substitute(Term, Term, Term1, Term1):- !.
%% substitute(_, Term, _, Term):- atomic(Term), !.
%% substitute(Sub, Term, Sub1, Term1):-
%% 	Term=..[F|Args],
%% 	substlist(Sub, Args, Sub1, Args1),
%% 	Term1=..[F|Args1].
%% substlist(_, [], _, []).
%% substlist(Sub, [Term|Terms], Sub1, [Term1|Terms1]):-
%% 	substitute(Sub, Term, Sub1, Term1),
%% 	substlist(Sub, Terms, Sub1, Terms1).

