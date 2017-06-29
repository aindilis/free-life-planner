:- dynamic dynamicCounter/2.

getNewCounter(Item,ID) :-
	findall(Count,dynamicCounter(Item,Count),List),
	incrementNewCounter(Item,List,ID),!.

incrementNewCounter(Item,List,ID) :-
	List \= [],
	max_list(List,Num),
	Num2 is Num + 1,
	addNewCounter(Item,Num2,ID).
incrementNewCounter(Item,List,ID) :-
	List = [],
	addNewCounter(Item,1,ID).


addNewCounter(Item,Num,ID) :-
	assert(dynamicCounter(Item,Num)),
	atom_concat(Item,Num,ID),!.

