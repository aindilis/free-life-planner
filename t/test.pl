time(Call,Time) :-
	statistics(runtime,_),
	call(Call),
	statistics(runtime,[_,Time]).

test(1) :- 
	scryList(unpreparedForContingency(_),List),
	member(List,neg(inTime(completed('get reconsult to work')))),
	member(List,neg(inTime(completed(fix(neg(hasService(car))))))),
	member(List,dies(andrewDougherty)).

test(2) :- 
	true.

run_tests :-
	forall(between(1,2,X),once(test(X))).
