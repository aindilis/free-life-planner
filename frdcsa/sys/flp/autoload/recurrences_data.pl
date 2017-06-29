hasRecurrence(do(task),
		[
		 [dow(sunday),[12:0:0]],
		 [dow(monday),[12:0:0]],
		 [dow(tuesday),[12:0:0]],
		 [dow(wednesday),[12:0:0]],
		 [dow(thursday),[12:0:0]],
		 [dow(friday),[12:0:0]],
		 [dow(saturday),[12:0:0,15:0:0]]
		]).

hasRecurrenceSpec(doOtherTask,through(sunday,saturday),[12:0:0]).

hasRecurrenceSpec3(evalProlog(regenerateFLPInternalSchedules(recurrences)),through(sunday,saturday),hourly).

hasRecurrenceSpec(testRecurrences,through(sunday,saturday),[12:0:0]).

%% hasRecurrenceSpec(do(<PERSON>,exercises),today,TimeAfter) :-
%% 	lastTimeOfEvent(hasGoneToSleep(Agent),DateTime),
%% 	member(Delay,[30,60,120,180]),
%% 	getTimeAfter(DateTime,minutes(Delay),TimeAfter).

hasTemporaryRecurrence(measure(hasWeight(<PERSON>,newBathroomScale,pounds(P))),
		[
		 [dow(monday),[12:0:0]],
		]).


