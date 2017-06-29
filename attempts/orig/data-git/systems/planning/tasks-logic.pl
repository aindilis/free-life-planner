test(Constant,Description,Days) :-
	assertz(appointment(appointment1000,us,them)),
	julian:form_time([now,[Y1-M1-D1]]),
	calendaring:daysUntilDate(dateFn(Y1,M1,D1),Date,3),
	dateFn(Y2,M2,D2) = Date,
	assertz(deadline(appointment1000,[Y2-M2-D2,12:00:00])),
	calendaring:upcomingEvents(Constant,Description,Days).

newTask(AgentName,FormalogName,Task,Importance,Result) :-
	getNewAssertableTaskID(Task,TaskID),
	Assertion = task(TaskID,Task,Importance),
	fassert(AgentName,FormalogName,Assertion,Result1),
	Result = [Assertion,Result1].

getNewAssertableTaskID(Task,TaskID) :-
	TaskID = assertedTask1.
