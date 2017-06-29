/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Turing machine simulator, written in Prolog
   (C) 2007 Markus Triska markus.triska@gmx.at
   Adapted from our solution to the 2005 Prolog contest.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

:- use_module(etrace).

turing(Tape0, Tape) :-
    perform(q0, [], Ls, Tape0, Rs),
    reverse(Ls, Ls1),
    append(Ls1, Rs, Tape).

perform(qf, Ls, Ls, Rs, Rs) :- !.
perform(Q0, Ls0, Ls, Rs0, Rs) :-
    (  Rs0 == [] ->
       Sym = b, RsRest = []
    ;  Rs0 = [Sym|RsRest]
    ),
    once(rule(Q0, Sym, Q1, NewSym, Action)),
    action(Action, Ls0, Ls1, [NewSym|RsRest], Rs1),
    perform(Q1, Ls1, Ls, Rs1, Rs).

action(left, Ls0, Ls, Rs0, Rs) :-
    (  Ls0 == [] ->
       Ls = [], Rs = [b|Rs0]
    ;  Ls0 = [L|Ls], Rs = [L|Rs0]
    ).
action(stay, Ls, Ls, Rs, Rs).
action(right, Ls0, [Sym|Ls0], [Sym|Rs], Rs).

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Example TM computing (x + 1) of a number x in unary encoding.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

rule(q0, 1, q0, 1, right).
rule(q0, b, qf, 1, stay).


tm(Ts) :-
    turing([1,1,1], Ts).

test(X) :-
    member(X, [a,b,c]).
