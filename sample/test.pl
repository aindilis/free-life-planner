:- use_module(library(julian)).
:- use_module(library(clpfd)).

%% findall([Year-Month-Day],(julian:form_time([dow(sunday), [Year-Month-Day]]), Year = 2016),List).
%% findall([Year-Month-Day],(julian:form_time([dow(sunday), Year-Month-Day]), Year in 2016..2026),List).
%% form_time([dow(sunday), Year-Month-Day]), Year in 2016 .. 2026.

see(Item) :-
	write_term(Item,[quoted(true)]),nl.

:- julian:form_time([dow(sunday), [Y-M-D]]),Y in 2016..2026, label([Y,M,D]), see([Y-M-D]), fail.