genlsDirectly(Subclass,Superclass) :-
 	genlsDirectlyList(H,Superclass),
 	member(Subclass,H).

genls(Subclass,Superclass) :-
	genlsDirectly(Midclass,Superclass),
	genls(Subclass,Midclass).
genls(Subclass,Superclass) :-
	genlsDirectly(Subclass,Superclass).

isa(Object2,SuperClass) :-
	findall(Object,(genls(SubClass,SuperClass),isa(Object,SubClass)),Objects),
	member(Object2,Objects).

isa(Object,Class) :-
	are(ObjectList,Class),
	member(Object,ObjectList).

:- dynamic argIsa/3.

argIsa(PredicateAndArity,1,Type) :-
	arg1Isa(PredicateAndArity,Type).
argIsa(PredicateAndArity,2,Type) :-
	arg2Isa(PredicateAndArity,Type).
argIsa(PredicateAndArity,3,Type) :-
	arg3Isa(PredicateAndArity,Type).
argIsa(PredicateAndArity,4,Type) :-
	arg4Isa(PredicateAndArity,Type).
