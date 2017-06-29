%% FIXME: replace this with setof

list_modules :-
	findall(
		M,
		(
		 current_predicate(_,M:P),
		 \+ predicate_property(M:P,imported_from(_)),
		 predicate_property(M:P, number_of_clauses(_)),
		 clause(M:P,B)
		),
		IntoVar
	       ), see(IntoVar).

% %% the number_of_clauses/1 will avoid an error
pred_for_m(M,IntoVar) :-
	findall(
		%% (P :- B),
		P,
		(
		 current_predicate(_,M:P),
		 \+ predicate_property(M:P,imported_from(_)),
		 predicate_property(M:P, number_of_clauses(_)),
		 clause(M:P,B),
		 B = true
		),
		IntoVar
	       ).
