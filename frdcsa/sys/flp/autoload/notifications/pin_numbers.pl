generatePinForRequest(Pin) :-
	random_between(100,999,Integer),
	safeInteger(Integer),
	atom_chars(Integer,Sequence),
	atomic_list_concat(Sequence,' ',Pin).

hasSubstring(String,Substring) :-
	(   findall(Before,sub_string(String, Before, _, After, Substring),Befores),not(length(Befores,0))).

safeInteger(Integer) :-
	unsafeIntegers(UnsafeIntegerList),
	forall(member(UnsafeInteger,UnsafeIntegerList),not(hasSubstring(Integer,UnsafeInteger))).

unsafeIntegers([66,666,6666,616,676]).
