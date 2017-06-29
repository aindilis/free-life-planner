%% as often as feasible
%% on certain days
%% constantly
%% as directed
%% after
%% three times a day
%% twice a day
%% daily
%% every other day
%% three times a week
%% biweekly/twice a week
%% weekly/at least once per week
%% twice a month
%% at least once per month
%% monthly
%% on certain day of week

everyOther(StartDate,EndDate,everyOther(week),Results) :-
	true.

everyOther(StartDate,EndDate,everyOther(month),Results) :-
 	setof([Y3,M3,D3],
	      (
	       julian:form_time([StartDate,[Y1-M1-D1]]),
	       julian:form_time([EndDate,[Y2-M2-D2]]),
	       beforeDateTime([Y3-M3-D3,12:0:0],[Y1-M1-D1,12:0:0]),
	       afterDateTime([Y2-M2-D2,12:0:0],[Y3-M3-D3,12:0:0]),
	       label([Y3,M3,D3])
	      ),Dates),
	view(Dates).

%% sameDayOfMonthNextMonth([YYYY-MM-DD],[YYYY2-MM2-DD2]) :-
%% 	DD2 = DD,
%% 	((MM == 12) -> (MM2 = 1,YYYY2 is YYYY + 1) ; (MM2 is MM + 1, YYYY2 = YYYY)).
%% julian_daysUntilDate([Y1-M1-D1,_:_:_],[Y2-M2-D2,_:_:_],Days)
%% afterDateTime([Y1-M1-D1,H1:Mi1:S1],[Y2-M2-D2,H2:Mi2:S2])
%% beforeDateTime([Y1-M1-D1,H1:Mi1:S1],[Y2-M2-D2,H2:Mi2:S2])
