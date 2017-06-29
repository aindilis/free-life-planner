listCompletions(Completions) :-
	setof(X,M^P^current_predicate(X,M:P),Completions).

get_predicates_for_file(File,Result) :-
	listCompletions(InitialPredicates),
	consult(File),
	listCompletions(FinalPredicates),
	subtract(FinalPredicates, InitialPredicates, Result).
