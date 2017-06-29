%% swipl -s convert.pl -g 'convert(a(b(c,d),e,f(g,h,i(j,k,l(m,n)))),1,Result),write(Result),nl.'
%% swipl -s convert.pl -g 'convert(a(b,c(d)),1,Result),write(Result),nl.'

:- ensure_loaded('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl').

:- dynamic assertions/1.
:- dynamic counter/2.

setCounter(Name,Id) :-
	counter(Name,_),
	retractall(counter(Name,_)),
	assertz(counter(Name,Id)),
	view(counter(Name,Id)).
setCounter(Name,Id) :-
	not(counter(Name,_)),
	assertz(counter(Name,Id)),
	view(counter(Name,Id)).

incrCounter(Name,Id) :-
	counter(Name,PreviousId),
	retract(counter(Name,PreviousId)),
	Id is PreviousId + 1,
	assertz(counter(Name,Id)),!.
incrCounter(Name,Id) :-
	not(counter(Name,_)),
	Id = 1,
	assertz(counter(Name,Id)).

nonEmptyList(List) :-
	not(emptyList(List)).

emptyList(List) :-
	length(List,0).

convert(Term,AssertionId,PVar) :-
	Term =.. [P|B],
	findall(Var,
		(member(SubTerm,B),
		 incrCounter(predicateId,PredicateId),
		 atomic_list_concat(['v',AssertionId,'_',PredicateId],'',Var)),
		Args),
	view([pvar,PVar,args,Args]),
	append([PVar],Args,NewArgs),
	Assertion =.. [P|NewArgs],
	assertz(assertion(Assertion)),
	view([assertion,Assertion]),
	setCounter(i,0),
	member(SubTerm,B),
	incrCounter(i,I),
	nth1(I,Args,NextPVar),
	view([convert,convert(SubTerm,AssertionId,NextPVar)]),
	((atom(SubTerm)) -> (
			     incrCounter(predicateId,PredicateId2),
			     atomic_list_concat(['v',AssertionId,'_',PredicateId2],'',Var2),
			     Assertion2 =.. [SubTerm,Var2],
			     assertz(assertion(Assertion2))
			    ) ;
	 (convert(SubTerm,AssertionId,NextPVar))),
	fail.
convert(Term,AssertionId).

testCode(Term,ExpectedResult,Pass) :-
	setCounter(predicateId,1),
	findall(counter(predicateId,Y),counter(predicateId,Y),Z),
	view([z,Z]),
	convert(Term,1,'v1_1'),
	findall(Assertion,assertion(Assertion),Assertions),
	view([assertions,Assertions]),
	retractall(assertion(_)).

:- testCode(a(b(c,d),e,f(g,h,i(j,k,l(m,n)))),
	    and([a(v1_1,v1_2,v1_5,v1_6),b(v1_2,v1_3,v1_4),c(v1_3),d(v1_4),e(v1_5),f(v1_6,v1_7,v1_8,v1_9),g(v1_7),h(v1_8),i(v1_9,v1_10,v1_11,v1_12),j(v1_10),k(v1_11),l(v1_12,v1_13,v1_14),m(v1_13),n(v1_14)]),
	    Pass).

:- testCode(a(b,c(d)),
	    X,
	    Pass).


%% thing about how to convert

%% before((shopAt(Agent,Store),partOfStoreFranchise(Store,aldiGroceryStores)),hasOnPerson(Agent,and(empty(groceryBags),quarterDollarCoin))).
