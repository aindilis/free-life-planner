:- ensure_loaded('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl').

:- dynamic counter/2.

nonEmptyList(List) :-
	not(emptyList(List)).

emptyList(List) :-
	length(List,0).

%% DOES NOT WORK FOOL!   (convert(a(b,c(d)),1,Res),write_list(Res), doens't work right)


%% swipl -s convert.pl -g 'convert(a(b(c,d),e,f(g,h,i(j,k,l(m,n)))),1,Result),write(Result),nl.'
%% swipl -s convert.pl -g 'convert(a(b,c(d)),1,Result),write(Result),nl.'


%% and(a(v1_1,v1_2,v1_5,v1_6),b(v1_2,v1_3,v1_4),c(v1_3),d(v1_4),e(v1_5),f(v1_6,v1_7,v1_8,v1_9),g(v1_7),h(v1_8),i(v1_9,v1_10,v1_11,v1_12),j(v1_10),k(v1_11),l(v1_12,v1_13,v1_14),m(v1_13),n(v1_14)).


%% swipl -s convert.pl -g 'getNthOfListOfSameLengthLists([[a,b],[b,c]],1,NewList),write_list(NewList)'

%% b.
%% c.
%% ?- 

%% exportKBAsFluents :-
%% 	findall(LF,
%% 		(
%% 		 getFullListing(Assertions),
%% 		 member(Assertion,Assertions),
%% 		 incrCounter(assertionId,AssertionId),
%% 		 convert(Assertion,AssertionId,LF)
%% 		),
%% 		LFs),
%% 	write_list(LFs).

%% :- consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl').

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

exportKBAsFluents :-
	getFullListing(Assertions),
	member(Assertion,Assertions),
	incrCounter(assertionId,AssertionId),
	convert(Assertion,AssertionId,LF),
	write(LF),nl,
	fail.
exportKBAsFluents.

getNthOfListOfSameLengthLists(List,N,NewList) :-
	findall(Item,(member(SubList,List),nth0(N,SubList,Item)),NewList).


incrCounter(Name,PredId) :-
	counter(Name,_),
	retract(counter(Name,PreviousPredId)),
	PredId is PreviousPredId + 1,
	assertz(counter(Name,PredId)),!.
incrCounter(Name,PredId) :-
	not(counter(Name,_)),
	PredId = 1,
	assertz(counter(Name,PredId)).

convert(Term,AssertionId,Result) :-
	convert_term_to_fluents(Term,AssertionId,PVarName,NewFluents),
	findall(Fluent,
		(member(Item,NewFluents),member(List1,Item),
		 ((atom(List1)) ->
		  (Fluent = List1) ;
		  (
		   %% view([b,Term]),
		   Fluent =.. List1
		  )))
	       ,Fluents),
	append([and],Fluents,List2),
	%% view([b,Result]),
	Result =.. List2.

convert_term_to_fluents(Term,AssertionId,PVar,Fluents) :-
	%% view([a,Term]),
	Term =.. [P|B],
	incrCounter(predicateId,PredicateId),
	atomic_list_concat(['v',AssertionId,'_',PredicateId],'',Var),
	term_to_atom(PVar,Var),
	%% PVar = Var,
	((emptyList(B)) ->
	 (Fluents = [[P,PVar]]) ;
	 %% (Fluents = [P]) ;
	 (
	  findall([VarName,NewFluents],
		  (member(SubTerm,B),
		   %% convert_term_to_fluents(SubTerm,AssertionId,VarName,NewFluents),
		   convert_term_to_fluents(SubTerm,AssertionId,VarName,Tmp2Fluents),
		   ((length(Tmp2Fluents,N),
		     N > 1) ->
		    (findall(Member,(member(Item,Tmp2Fluents),member(Member,Item)),NewFluents)) ;
		    (NewFluents = Tmp2Fluents))
		  ),
		  Results),
	  ((length(Results,1)) -> (Results2 = [Results]) ; (Results2 = Results)),
	  getNthOfListOfSameLengthLists(Results2,0,VarList),
	  getNthOfListOfSameLengthLists(Results2,1,FluentList),
	  %% view([fluentList,FluentList]),nl,
	  append([P,PVar],VarList,Fluent),
	  append([[Fluent]],FluentList,Fluents)
	 )).

test(a(b(c,d),e,f(g,h,i(j,k,l(m,n)))),
     and(a(V1_1,V1_2,V1_5,V1_6),b(V1_2,V1_3,V1_4),c(V1_3),d(V1_4),e(V1_5),f(V1_6,V1_7,V1_8,V1_9),g(V1_7),h(V1_8),i(V1_9,V1_10,V1_11,V1_12),j(V1_10),k(V1_11),l(V1_12,V1_13,V1_14),m(V1_13),n(V1_14))).
%% test(a(b,c(d)),
