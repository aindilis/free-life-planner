:- consult('/var/lib/myfrdcsa/codebases/minor/agent-attempts/7/parser.pl').

view(A) :-
	write_term(A, [quoted(true)]),
	nl, !.

:- parseCapsule('/var/lib/myfrdcsa/codebases/minor/agent-attempts/7/pddl/','hygiene',Parsed),
	view(Parsed).
