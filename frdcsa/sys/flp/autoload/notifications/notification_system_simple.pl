%% okay this is the simplest system

%% it periodically recalculates recurrences, then updates the schedules.

%% Otherwise it just checks the scheduler periodically to see if
%% anything is coming up.  Will use an external script to avoid having
%% to write all the complicated formalog timing and loops logic that
%% eventually needs to be written.  Borrow from
%% launch-climate-monitor.sh

:- dynamic flpInternalSchedules/2.

%% mainLoopHeartbeat(_) :-
%% 	watchdogHeartbeat,
%% 	fail.
mainLoopHeartbeat(AllResults) :-
	getCurrentDateTime(CurrentDateTime),
	view([currentDateTime,CurrentDateTime]),
	findall(Results,
		(
		 member(ScheduleType,[plans,notifications,recurrences]),
		 handleFLPInternalSchedules(ScheduleType,CurrentDateTime,Results)
		),
		AllResults).

handleFLPInternalSchedules(ScheduleType,CurrentDateTime,Results) :-
	view([scheduleType,ScheduleType]),
	flpInternalSchedules(ScheduleType,Schedule),
	argt(Schedule,[pending(ScheduleType,Recurrences)]),
	view([[recurrences,Recurrences]]),
	findall(Result,(
			member([Action,ItemDateTime,RecurrenceDateTime,Type],Recurrences),
			view([wtf,Action,CurrentDateTime,RecurrenceDateTime]),
			afterDateTime(CurrentDateTime,RecurrenceDateTime),
			processPendingRecurrence(Action,ItemDateTime,RecurrenceDateTime,Type,Result)
		       ),Results).

processPendingRecurrence(Action,ItemDateTime,RecurrenceDateTime,Type,Results) :-
	(   (	Action = evalProlog(Prolog)) ->
	    (	
		not(wsmHoldsWrapper(completed(evalProlog(recurrenceFn([
								       action(Action),
								       itemDateTime(ItemDateTime),
								       recurrenceDateTime(RecurrenceDateTime),
								       type(Type)
								      ]))),Context)),
		call(Prolog),
		fassert('Agent1','Yaswi1',completed(evalProlog(recurrenceFn([
									     action(Action),
									     itemDateTime(ItemDateTime),
									     recurrenceDateTime(RecurrenceDateTime),
									     type(Type)
									    ]))),Results)
	    ) ;
	    (	
		%% hasParticipants(_,Participants),
		Participants = [andrewDougherty],
		generateReminder([action(Action),itemDateTime(ItemDateTime),recurrenceDateTime(RecurrenceDateTime),type(Type),participants(Participants)])
	    )).

generateReminder(Arguments) :-
	view([generateReminder]),
	argt(Arguments,[action(Action),itemDateTime(ItemDateTime),recurrenceDateTime(RecurrenceDateTime),type(Type),participants(Participants)]),
	%% have to create a pin for this request
	shell_quote_term(Action,ActionDescription),
	view([description,ActionDescription,pendingRecurrence,[Action,ItemDateTime,RecurrenceDateTime,Type]]),
	generateGlossFor(RecurrenceDateTime,RecurrenceDateTimeGloss),
	view([recurrenceDateTime,RecurrenceDateTime,recurrenceDateTimeGloss,RecurrenceDateTimeGloss]),
	shell_quote_term(Type,QuotedType),
	generatePinForRequest(Pin),
	getCurrentDateTime(Now),
	englishDescriptionOfTimeUntil(Now,RecurrenceDateTime,DurationEnglishGloss),
	(   julian:compare_time(Comparison,Now,RecurrenceDateTime) ->
	    (	
		SubList = [an,upcoming]
	    ) ;
	    (	
		SubList = [a,past,due]
	    )),

	member(Participant,Participants),
	append([[
		 Participant,
		 has
		],
		SubList,
		[
		 QuotedType,
		 DurationEnglishGloss,
		 at,
		 RecurrenceDateTimeGloss,
		 to,
		 ActionDescription,
		 '.',
		 'Please',
		 acknowledge,
		 with:,
		 'Alice,',
		 confirm,
		 with,
		 pin,
		 Pin
		]],
	       MessageList),
	atomic_list_concat(MessageList,' ',Message),
	view([message,Message]),
	%% notifyParticipants2([
	%% 		     participants([Participant]),
	%% 		     message(Message),
	%% 		     action(Action),
	%% 		     eventDateTime(ItemDateTime),
	%% 		     recurrenceDateTime(RecurrenceDateTime),
	%% 		     type(Type)
	%% 		    ]),
	fail.
generateReminder(_).

notifyParticipants2(Arguments) :-
	argt(Arguments,[
			participants(Participants),
			message(Message),
			action(Action),
			eventDateTime(ItemDateTime),
			recurrenceDateTime(RecurrenceDateTime),
			type(Type)
		       ]),
	member(Participant,Participants),
	getCurrentDateTime(CurrentDateTime),
	Context = z,

	not(wsmHoldsWrapper(completed(tell(freeLifePlanner,Participant,recurrenceFn([
										     action(Action),
										     itemDateTime(ItemDateTime),
										     recurrenceDateTime(RecurrenceDateTime),
										     type(Type)
										    ]))),Context)),
	%% not(wsmHoldsWrapper(completed(acknowledged(Participant,
	%% 					   completed(tell(freeLifePlanner,Participant,recurrenceFn([
	%% 												    action(Action),
	%% 												    itemDateTime(ItemDateTime),
	%% 												    recurrenceDateTime(RecurrenceDateTime),
	%%												    type(Type)
	%% 												   ]))))),Context)),

	queueAction(tell(Participant,Message)),

	%% getCurrentDateTime(DateTime1),
	%% Participant = andrewDougherty,
	%% tell(Participant,[Message]),
	%% getCurrentDateTime(DateTime2),

	view([atTime(CurrentDateTime,completed(tell(freeLifePlanner,Participant,recurrenceFn([
											      action(Action),
											      itemDateTime(ItemDateTime),
											      recurrenceDateTime(RecurrenceDateTime),
											      type(Type)
											     ]))))]),
	fassert('Agent1','Yaswi1',
		atTime(CurrentDateTime,
		       completed(tell(freeLifePlanner,Participant,recurrenceFn([
										action(Action),
										itemDateTime(ItemDateTime),
										recurrenceDateTime(RecurrenceDateTime),
										type(Type)
									       ])))),Results),
	%% fassert('Agent1','Yaswi1',
	%% 	betweenTimes([DateTime1,DateTime2],
	%% 		     completed(tell(freeLifePlanner,Participant,recurrenceFn([
	%% 									      action(Action),
	%% 									      itemDateTime(ItemDateTime),
	%% 									      recurrenceDateTime(RecurrenceDateTime),
	%%									      type(Type)
	%% 									     ])))),Results),

	%% sleep(15),

	%% fassert([assertion(told(Participant,Message))),
	%% fassert([assertion(sentNotification(NotificationID))),
	true.


nextUpcomingRecurrenceOrNotification :-
	true.

listNotifications :-
	flpInternalSchedules(recurrences,[pending(plans,PendingPlans),pending(notifications,Notifications),pending(recurrences,Recurrences)]),
	member(Recurrence,Recurrences),
	view(Recurrence).

%% skip this case for now.  have to at least record that
%% it was acted on, so it doesn't continually become sent
%% to us

%% this needs to be recorded in the KB, but can later be withdrawn and cleaned up

%% what is the difference between kassert and fassert?

%% go ahead and have something for generating the recurrence schedule
regenerateFLPInternalSchedules(recurrences) :-
	getCurrentDateTime(CurrentDateTime),
	CurrentDateTime = [Y-M-D,H:Mi:S],
	StartDateTime = [Y-M-D,0:0:0],	
	consecutiveDates([Y-M-D],[Y1-M1-D1]),
	EndDateTime = [Y1-M1-D1,23:59:59],	
	view([times,[CurrentDateTime,StartDateTime,EndDateTime]]),
	%% pending(notifications,CurrentDateTime,StartDateTime,EndDateTime,PendingNotifications),
	pending(recurrences,CurrentDateTime,StartDateTime,EndDateTime,PendingRecurrences),
	retractall(flpInternalSchedules(recurrences,_)),
	assert(flpInternalSchedules(recurrences,
				    [
				     pending(notifications,PendingNotifications),
				     pending(recurrences,PendingRecurrences)
				    ])),
	flpInternalSchedules(recurrences,Results),
	view([flpInternalSchedules(recurrences,Results)]).

updateFLPInternalSchedule(plans,[transformed(Transformed)]) :-
	view([updateFLPInternalSchedule(plans,_)]),
	retractall(flpInternalSchedules(plans,_)),
	Transformed = [_,_,Steps],
	view([steps,Steps]),
	findall([[Action,PlanStepStartDateTime,planStep(start)],[Action,PlanStepEndDateTime,planStep(end)]],
		(   
		    member([StepNumber,StepData],Steps),
		    nth0(3,StepData,[Action,_]),
		    nth0(4,StepData,Data),
		    Data = [_,_,PlanStepStartDateTime,_,PlanStepEndDateTime,_]
		),
		ActionDateTimes),
	view([actionDateTimes,ActionDateTimes]),
	findall(Result,(
			member(Items,ActionDateTimes),
			member([Action,ItemDateTime,Type],Items),
			hasAdvanceDateTime(ItemDateTime,StartDateTime,EndDateTime,CurrentTime,TmpResult1),
			append([Action],TmpResult1,TmpResult2),
			append(TmpResult2,[Type],Result)
		       ),
		PendingPlans),
	view([pendingPlans,PendingPlans]),
	assert(flpInternalSchedules(plans,
				    [
				     pending(plans,PendingPlans)
				    ])),
	view([flpInternalSchedules,flpInternalSchedules(plans,
							[
							 pending(plans,PendingPlans)
							])]).

%% write some stuff for controlling processes here

%% ensureRunning('/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/flp-cron-like.pl -i 300').

%% [checkInventoryOfLifeCriticalItems,[2017-5-29,14:0:0],[2017-5-28,14:0:0]]

%% listAllItemsBetweenNowAndQualitativeTimeOfDay(QualitativeTimeOfDay) :-
%% 	[earlyMorning,morning,lateMorning,noon,earlyAfternoon,lateAfternoon,earlyEvening,evening,lateEvening,night,lateNight]

%% listAllItemsBetweenNowAndDateTime(DateTime) :-
%% 	getCurrentDateTime(Now),
%% 	listAllItemsBetweenDateTimes(Now,DateTime).

listAllItemsBetweenDateTimes(StartDateTime,EndDateTime) :-
	pending(notifications,StartDateTime,StartDateTime,EndDateTime,PendingNotifications),
	pending(recurrences,StartDateTime,StartDateTime,EndDateTime,PendingRecurrences),
	pending(plans,StartDateTime,StartDateTime,EndDateTime,PendingPlans).

%% I want to come up with a clean way for logging all of this.  I also
%% want to get it to query us.  Maybe I can integrate prolog agent at
%% this point.  And make a domain.  That's overkill, and likely to
%% fail.  Our system had us do several such implementations, of
%% increasing ability.  Need to find those notes.  All this is one,
%% since it has a _simple postfix.

%% there was a problem that it would go through all of the
%% notifications from the beginning of the day.  Is that a bug or a
%% feature.  When multiple messages are queued, there should be a way
%% to abort them.  There should be a web interface for interacting.

%% need ways to group multiple messages together, provided privacy
%% holds, etc.
