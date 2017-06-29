%% implement code that calls this file from here:
%% /var/lib/myfrdcsa/codebases/minor/formalog/scripts/cron-5-mins.pl

%% /var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/window.pl

contactHasSMSGatewayAddress(andrewDougherty,'<PHONENUMBERSMSEMAIL>').
%% contactHasSMSGatewayAddress(<PERSON>,Unknown).

%% window(months(1)).


%% Fix timezone issue


%% Nightly reminders to lower windows.  FIXME: Make this take
%% eventually place around sundown.

%% Weekly reminders to take trash out

%% getDateCertainTimeFromNow(now, [months(1),days(15)]).

%% setof([Y,M,D],(julian:form_time([dow(DOW), [Y-M-D]]), julian:compare_time(>,[Y-M-D],Start), julian:compare_time(<,[Y-M-D],Finish), label([Y,M,D])),Results).

%% Write a function for getting the days of the months in the year, and use that when adding months, do this later though

%% Daily reminders at night and morning to brush teeth


%% see /var/lib/myfrdcsa/codebases/minor/free-life-planner/release/input/var/lib/myfrdcsa/codebases/minor/resource-manager/bill_payment.pl

%% hasRecurrence(pay(Bill)) :-
%% 	hasBill(Bill).



%% write agent thinking system, load and integrate prolog agent

%% dailyUpdateOfRecurrences(Day) :-
%%      true.

%% updateRecurrences(Day) :-
%%      true.

%% hasRecurrenceSpec(do(and(brushTeeth,floss)),through(sunday,saturday),[9:0:0,21:0:0]).

hasRecurrence(Name,DayOfWeek,Hours) :-
	hasRecurrenceSpec(Name,DaysOfWeekFormula,Hours),
	ourEval(DaysOfWeekFormula,DayOfWeekList),
	member(DayOfWeek,DayOfWeekList).

%% REDACTED

%% %% FIXME: add these
%% hasRecurrence('replace furnace filter',[]).
%% hasRecurrence('vacuum floor',[twicePer(week)]).
%% hasRecurrence('wash dishes',[daily]).

%% Join the FSF and friends Friday, MONTH DAY, from 12pm to 3pm EDT
%% (16:00 to 19:00 UTC) to help improve the Free Software Directory. 

calculateTimestampsForReminder(Duration,DOW,H:Mi:S,[Y0-M0-D0,H0:Mi0:S0],Timestamps,[Y1-M1-D1,H1:Mi1:S1]) :-
	julian:form_time(now,Start),
	julian:form_time([Y0-M0-D0,H0:Mi0:S0],Start),
	julian:delta_time(Start,Duration,Finish),
	julian:form_time([Y1-M1-D1],Finish),
 	setof([Y,M,D,H,Mi,S],
	      (
	       julian:form_time([DOW,[Y-M-D]]),
	       julian:compare_time(>,[Y-M-D,H:Mi:S],Start),
	       julian:compare_time(<,[Y-M-D,H:Mi:S],Finish),
	       label([Y,M,D])
	      ),Timestamps).

%% For the trash reminder, it makes sense to tell us when we can, not
%% warn us ahead of time, unless we are going somewhere else, and want
%% to incorporate this into our plans

calculateUpcomingAlerts(Duration,Alerts) :-
	setof([Name,SortedAllTimestamps],
	      Name^AllTimestamps^DOW^Times^Occurrences^Time^Duration^A^B^
	     (
	      hasRecurrence(Name,Occurrences),
	      setof(Timestamps,
		    DOW^Times^Occurrences^Time^Duration^A^B^
		   (
		    member([DOW,Times],Occurrences),
		    member(Time,Times),
		    calculateTimestampsForReminder(Duration,DOW,Time,A,Timestamps,B)
		   ),AllTimestamps),

	      %% FIXME: now actually sort all timestamps lexigraphically by start date, end date
	      SortedAllTimestamps = AllTimestamps

	     ),Alerts).

cronEvery5Minutes :-
	sendOutAlerts.

sendOutAlerts :-
	%% Find all upcoming events,

	%% Ensure that they have had appropriate warning messages sent to the user.

	%% If not, send a message

	%% FIMXE: make it a window: calculateUpcomingAlerts([days(30),days(60)],Alerts),
	calculateUpcomingAlerts(days(30),Alerts),
	view(Alerts).

%% FIXME: Implement these later, too complex for initial
%% implementation.  For now, just have it send a reminder when the
%% window is invoked.  For now, just run every 5 minutes.

%% some things to note.  there are numerous social rules that should be followed by this system.

%% if the recipient is expected to be asleep, don't send unless
%% emergency or important

%% if the user 



%% % Borrowed from elsewhere as an example

%% sortDeadlines(Deadlines) :-
%% 	findall([X,Y],(deadline(X,Z),julian:form_time(Z,Y)),List),
%% 	%% view([list,List]),
%% 	setof(Date,Key^member([Key,Date],List),Dates),
%% 	%% view([dates,Dates]),
%% 	predsort(julian:compare_time,Dates,SortedDates),
%% 	%% view([sortedDates,SortedDates]),
%% 	findall([DateTime,Tasks],
%% 		(
%% 		 member(Date,SortedDates),
%% 		 julian:form_time([Date,[Y-M-D,H:Mi:S]]),
%% 		 DateTime = [Y-M-D,H:Mi:S],
%% 		 findall(TaskID2,(member([TaskID2,Date2],List),Date=Date2),Tasks)
%% 		),
%% 		Deadlines).

hasRecurrence(Name,Spec) :-
	setof(Name,X^Y^hasRecurrence(Name,X,Y),Names),
	member(Name,Names),
	findall([dow(DayOfWeek),Hours],hasRecurrence(Name,DayOfWeek,Hours),Spec).

hasTimes(hourly,Times) :-
	numlist(0,23,Hours),
	findall(Time,(member(Hour,Hours),Time = Hour:0:0),Times).

hasRecurrenceSpec(evalProlog(regenerateFLPInternalSchedules(recurrences)),X,Times) :-
	hasRecurrenceSpec3(evalProlog(regenerateFLPInternalSchedules(recurrences)),X,TimesSpec),
	hasTimes(TimesSpec,Times).

hasRecurrence(X,Y) :-
	hasTemporaryRecurrence(X,Y).
