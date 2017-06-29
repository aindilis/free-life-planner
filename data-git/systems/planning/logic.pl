:- prolog_ensure_loaded('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/counter2.pl').
:- prolog_ensure_loaded('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/kbs.pl').

test(Constant,Description,Days) :-
	assertz(appointment(appointment1000,us,them)),
	julian:form_time([now,[Y1-M1-D1]]),
	calendaring:daysUntilDate([Y1-M1-D1],Date,3),
	[Y2-M2-D2] = Date,
	assertz(deadline(appointment1000,[Y2-M2-D2,12:00:00])),
	calendaring:upcomingEvents(Constant,Description,Days).

newTask(AgentName,FormalogName,Task1,Importance1,TaskID,Result) :-
	getNewCounter(assertedTask,ID,(task(ID,Task,Importance),atom_concat(assertedTask,N,ID)),TaskID),
	fassert(AgentName,FormalogName,task(TaskID,Task1,Importance1),Result1),
	Result = [task(TaskID,Task1,Importance1),Result1],
	!.

log(SubHabit) :-
	genls(SubHabit,habit).

cents(X) :-
	dollars(Y),
	X is Y * 100.0.

nearest(cents(X)) :-
	cents(Y),
	X is round(Y).

nearest(dollars(X)) :-
	dollars(Y),
	X is round(Y).

newAssertion(AgentName,FormalogName,Assertion,Result) :-
	fassert(AgentName,FormalogName,Assertion,Result1),
	Result = [Assertion,Result1],
	!.

% newAssertionWithCounter(AgentName,FormalogName,Assertion,Type,AssertionID,Result) :-
% 	getNewCounter(Type,ID,(task(ID,Task,Importance),atom_concat(Type,N,ID)),AssertionID),
% 	Assertion =.. [P|Args],
% 	fassert(AgentName,FormalogName,task(AssertionID,Task1,Importance1),Result1),
% 	Result = [task(AssertionID,Task1,Importance1),Result1],
% 	!.

taskIsAssignedToAgent(Agent,Task) :-
	taskIsAssignedToSelf(Agent,Task).
taskIsAssignedByAgent(Agent,Task) :-
	taskIsAssignedToSelf(Agent,Task).

taskIsAssignedOrOfferedToAgent(Agent,Task) :-
	(taskIsAssignedToAgent(Agent,Task) ;
	 taskIsOfferedToAgent(Agent,Task)).

taskIsAssignedOrOfferedByAgent(Agent,Task) :-
	(taskIsAssignedByAgent(Agent,Task) ;
	 taskIsOfferedByAgent(Agent,Task)).

taskAssignedByTo(TaskID,Description,AssignedByAgent,AssignedToAgent) :-
	taskIsAssignedOrOfferedToAgent(AssignedToAgent,TaskID),
	taskIsAssignedOrOfferedByAgent(AssignedByAgent,TaskID),
	task(TaskID,Description,_).

habitual(Item) :-
	habituals(List),
	member(Item,List).

maybe(interestedIn(Person,makingPackagesOfAISoftware)) :-
	isa(Person,debianDeveloper).

taskIsAssignedToAgent(Agent,Task) :-
	tasksAreAssignedToAgent(Agent,List),
	member(Task,List).

taskIsAssignedByAgent(Agent,Task) :-
	tasksAreAssignedByAgent(Agent,List),
	member(Task,List).

taskIsOfferedToAgent(Agent,Task) :-
	tasksAreOfferedToAgent(Agent,List),
	member(Task,List).

taskIsOfferedByAgent(Agent,Task) :-
	tasksAreOfferedByAgent(Agent,List),
	member(Task,List).

taskInvolvesAgent(Agent,Task1) :-
	setof(Task,Agent^(taskIsAssignedToAgent(Agent,Task) ;
		    taskIsAssignedByAgent(Agent,Task) ;
		    taskIsOfferedToAgent(Agent,Task) ;
		    taskIsOfferedByAgent(Agent,Task) ;
		    taskIsOfferedToAgent1(Agent,Task)),List),
	member(Task1,List).

taskIsOfferedToAgent1(andrewDougherty,TaskID) :-
	task(TaskID,Description,Importance),
	\+ taskIsAssignedToAgent(_,TaskID).

taskIsOfferedByAgent1(andrewDougherty,TaskID) :-
	task(TaskID,Description,Importance),
	\+ taskIsAssignedByAgent(_,TaskID).
