%% see /var/lib/myfrdcsa/codebases/minor/free-wopr/data-git/wopr.pl
%% see /var/lib/myfrdcsa/codebases/minor/free-wopr/data-git/wopr-orig.pl
%% see /var/lib/myfrdcsa/codebases/minor/free-wopr/data-git/wopr_data.pl

counter(woprTask,0).

unpreparedForContingency(Contingency) :-
	planForContingency(Contingency),
	\+ possibleResponse(Contingency,Response).
unpreparedForContingency(Contingency) :-
	planForContingency1(Contingency),
	\+ possibleResponse(Contingency,Response).

unpreparedForContingency1(Contingency) :-
	planForContingency(Contingency).

planForContingency1(neg(inTime(completed(Task)))) :-
	task(TaskID,Task,Criticality),
	\+ completed(TaskID).
planForContingency1(neg(inTime(completed(Task)))) :-
	task1(TaskID,Task,Criticality),
	\+ completed(TaskID).

task1(TaskID,developResponsePlanFor(andrewDougherty,Contingency),Criticality) :-
	\+ ground(TaskID),
	unpreparedForContingency1(Contingency),
	\+ contingencyOccurred(Contingency),
	worstCaseDamageCriticality(Contingency,Criticality),
	\+ task(Tmp,developResponsePlanFor(andrewDougherty,Contingency),Criticality),
	nextCounter(woprTask,TaskID),
	assertAndPrint(task(TaskID,developResponsePlanFor(andrewDougherty,Contingency),Criticality)).

assertAndPrint(Item) :-
	view(Item),nl,
	assertz(Item).

completedTask(developResponsePlanFor(andrewDougherty,Contingency)) :-
	possibleResponse(Contingency,_).

eventually(completedTask(Task)) :- completedTask(Task).

completed(TaskID) :-
	completedTask(Task),
	task(TaskID,Task,_).

%% see /var/lib/myfrdcsa/codebases/minor/free-wopr/data-git/wopr.pl
%% see /var/lib/myfrdcsa/codebases/minor/free-wopr/data-git/wopr-orig.pl
%% see /var/lib/myfrdcsa/codebases/minor/free-wopr/data-git/wopr_data.pl
