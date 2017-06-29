:- use_module(library(regex)).

:- dynamic
	pattern_cache/3.		% Pattern, Flags, Regex

regex_replace(Input, Pattern, Replace0, Flags, Result) :-
	dollar_replace(Replace0, Replace),
	with_mutex(sparql_regex,
		   locked_replace(Input, Pattern, Replace, Flags, Result)).

dollar_replace(Replace0, Replace) :-
	sub_atom(Replace0, _, _, _, $), !,
	regex_replace(Replace0, '\\$([0-9])', '\\\\1', '', Replace).
dollar_replace(Replace, Replace).

locked_replace(Input, Pattern, Replace, Flags, Result) :-
	regex_obj(Pattern, Flags, Regex),
	new(S, string('%s', Input)),
	send(Regex, for_all, S,
	     message(@arg1, replace, @arg2, Replace)),
	get(S, value, Result).

regex_obj(Pattern, Flags, Regex) :-
	pattern_cache(Pattern, Flags, Regex), !.
regex_obj(Pattern, Flags, Regex) :-
	make_regex(Pattern, Flags, Regex),
	asserta(pattern_cache(Pattern, Flags, Regex)).

make_regex(Pattern, i, Regex) :- !,
	new(Regex, regex(Pattern, @(off))).
make_regex(Pattern, _, Regex) :- !,
	new(Regex, regex(Pattern)).
