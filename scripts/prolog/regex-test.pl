:- use_module(library(regex)).

%% regex('(.+)-(([0-9]|\\.)+[a-zA-Z]?)$', [], Atom, [Tmp1,Tmp2,_]),

test :-
	regex('\\.pl$', [], '/tmp/tmp.pl', []),
	write('ok'),nl,
	fail.
test :-
	regex('\\.pl$', [], '/tmp/tmp.txt', []),
	write('nok'),nl.

:- test.