:- use_module(library(julian)).
:- use_module(library(clpfd)).

:- multifile julian:form_time/3.
:- dynamic currentDate/1, appointment/3, deadline/2, hasStartDate/2.

julian:form_time(consecutive,Y1-M1-D1,Y2-M2-D2) :-
	form_time([Y1-M1-D1,datetime(Day1,0)]),
	form_time([Y2-M2-D2,datetime(Day2,0)]),
	1 #= Day2 - Day1.

list_consecutive_days(Y1-M1-D1) :-
	retractall(currentDate(_)),
	assert(currentDate(Y1-M1-D1)),
	list_consecutive_days.

list_consecutive_days :-
	repeat,
	currentDate(Y1-M1-D1),
	retractall(currentDate(_)),
	julian:form_time(consecutive,Y1-M1-D1,Y2-M2-D2),
	assert(currentDate(Y2-M2-D2)),
	displayDate(Y2-M2-D2),
	fail.

displayDate(Y-M-D) :-
	write(Y),write('-'),write(M),write('-'),write(D),nl.

today(Y-M-D) :-
	form_time([now,Y-M-D]).

tomorrow(Y-M-D) :-
	today(Y1-M1-D1),
	julian:form_time(consecutive,Y1-M1-D1,Y-M-D).

% dateFn(Date) :-
% 	Date = dateFn(Y,M,D),
% 	Y-M-D.

consecutiveDates(Date1,Date2) :-
	Date1 = dateFn(Y1,M1,D1),
	Date2 = dateFn(Y2,M2,D2),
	julian:form_time(consecutive,Y1-M1-D1,Y2-M2-D2).

daysUntilDate(Date1,Date2,Days) :-
	Date1 = dateFn(Y1,M1,D1),
	Date2 = dateFn(Y2,M2,D2),
	form_time([Y1-M1-D1,datetime(Day1,0)]),
	form_time([Y2-M2-D2,datetime(Day2,0)]),
	Days is Day2 - Day1.

% daysUntilDate(+Date1,-Date2,Days) :-
% 	Date1 = dateFn(Y1,M1,D1),
% 	form_time([[Y1-M1-D1],datetime(Day1,0)]),
% 	Day2 is Day1 + Days,
% 	form_time([datetime(Day2,0),[Y2-M2-D2]]),
% 	Date2 = dateFn(Y2,M2,D2).

significantDateForEvent(Constant,Date) :-
	hasStartDate(Constant,Date) ;
	deadline(Constant,Date) ;
	recommendedTimeToOccurBetween(Constant,Date,Date1) ;
	recommendedTimeToOccurBetween(Constant,Date2,Date).

daysUntil(Constant,Date,Days) :-
	significantDateForEvent(Constant,Date),
	julian:form_time([now,[A-B-C]]),
	julian:form_time([Date,[D-E-F]]),
	daysUntilDate(dateFn(A,B,C),dateFn(D,E,F),Days).

description(Constant,Description) :-
	task(Constant,Description,_) ;
	(
	 appointment(Constant,Org1,Org2),
	 Description = appointmentWith(Org1,Org2)
	).
description(Constant,Description) :-
	\+ task(Constant,Description,_),
	\+ (
	    appointment(Constant,Org1,Org2),
	    Description = appointmentWith(Org1,Org2)
	   ),
	Description = null.

daysUntilSignificantEvent(Constant,Days) :-
	julian:form_time([now,[Y1-M1-D1]]),
	significantDateForEvent(Constant,Date),
	julian:form_time([Date,[Y2-M2-D2,H:M:S]]),
	daysUntilDate(dateFn(Y1,M1,D1),dateFn(Y2,M2,D2),Days).

upcomingEvent(Constant,Description,days(Days)) :-
	daysUntilSignificantEvent(Constant,Days),
	(requiresInstruments(Constant,Instruments) -> Days < 30 ;
	 \+ requiresInstruments(Constant,Instruments) -> Days < 15),
	description(Constant,Description).

overdue(Constant) :-
	daysUntilSignificantEvent(Constant,Days),
	Days < 0.

dueNow(Constant) :-
	recommendedTimeToOccurBetween(Constant,Date1,Date2),
	julian:form_time([now,[Y1-M1-D1,H1:Mi1:S1]]),
	(compare_time(>,Date1,[Y1-M1-D1,H1:Mi1:S1]) ; compare_time(=,Date1,[Y1-M1-D1,H1:Mi1:S1])),
	(compare_time(<,Date2,[Y1-M1-D1,H1:Mi1:S1]) ; compare_time(=,Date2,[Y1-M1-D1,H1:Mi1:S1])).
