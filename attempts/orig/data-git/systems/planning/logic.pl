cents(X) :-
	dollars(Y),
	X is Y * 100.0.

nearest(cents(X)) :-
	cents(Y),
	X is round(Y).

nearest(dollars(X)) :-
	dollars(Y),
	X is round(Y).
