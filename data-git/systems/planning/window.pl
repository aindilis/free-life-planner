%% findall([Year-Month-Day],(julian:form_time([dow(sunday), [Year-Month-Day]]), Year = 2016),List).
%% findall([Year-Month-Day],(julian:form_time([dow(sunday), Year-Month-Day]), Year in 2016..2026),List).
%% form_time([dow(sunday), Year-Month-Day]), Year in 2016 .. 2026.

%% :- julian:form_time([dow(sunday), [Y-M-D]]),Y in 2016..2026, label([Y,M,D]), view([Y-M-D]), fail.

:- dynamic time_frame/2.

time_frame(short,years(1)).
time_frame(medium,years(5)).
time_frame(long,years(10)).

%% See 'Artificial Intelligence: Strategies, Applications, and Models
%% Through SEARCH' and other more recent books.

%% Need to do a quiescence search to avoid horizon effect, the
%% deliberate postponing of bad results over the edge of the horizon,
%% possibly by unnecessary bad moves which are less bad than the thing
%% pushed over the horizon

%% Quiescence Search can be postponed for now, just use fixed horizons.


% %% form_time([2016-1-1],Start), julian:delta_time(Start, days(365), Finish), form_time([YF-MF-DF],Finish), view([YF-MF-DF]).
% form_time(now,Start), julian:delta_time(Start, days(365), Finish), form_time([YF-MF-DF],Finish), julian:form_time([dow(sunday), [Y-M-D]]), julian:compare_time(>,[Y-M-D],Start), julian:compare_time(<,[Y-M-D],[YF-MF-DF]), label([Y,M,D]), view([Y-M-D]), fail.

% %% form_time(now,Start), julian:delta_time(Start, days(365), Finish), form_time([YF-MF-DF],Finish), julian:form_time([dow(sunday), [Y-M-D]]), julian:compare_time(>,[Y-M-D],Start), julian:compare_time(<,[Y-M-D],[YF-MF-DF]), label([Y,M,D]), view([Y-M-D]), fail.

% form_time(now,Start), form_time([Y0-M0-D0],Start), julian:delta_time(Start, days(365), Finish), form_time([YF-MF-DF],Finish), julian:form_time([dow(sunday), [Y-M-D]]), julian:compare_time(>,[Y-M-D],[Y0-M0-D0]), julian:compare_time(<,[Y-M-D],[YF-MF-DF]), label([Y,M,D]), view([Y-M-D]), fail.

% form_time(now,Start), form_time([Y0-M0-D0,H0:Mi0:S0],Start), julian:delta_time(Start, days(365), Finish), form_time([YF-MF-DF:0,0,0],Finish), julian:form_time([dow(sunday), [Y-M-D,0:0:0]]), julian:compare_time(>,[Y-M-D,0:0:0],[Y0-M0-D0,0:0:0]), julian:compare_time(<,[Y-M-D],[YF-MF-DF,0:0:0]), label([Y,M,D,0:0:0]), view([Y-M-D,0:0:0]), fail.

%% form_time(now,Start), julian:delta_time(Start, days(365), Finish), form_time([YF-MF-DF,0:0:0],Finish), view([YF-MF-DF]), julian:form_time([dow(sunday), [Y-M-D,0:0:0]]), julian:compare_time(>,[Y-M-D,0:0:0],Start), julian:compare_time(<,[Y-M-D,0:0:0],[YF-MF-DF,0:0:0]), label([Y,M,D]), view([Y-M-D]), fail.


%% julian:form_time([dow(sunday), [Y-M-D]]), julian:compare_time(>,[Y-M-D],[2016-1-1]), julian:compare_time(<,[Y-M-D],[2016-7-1]), label([Y,M,D]), view([Y-M-D]), fail.

%% setof([Y,M,D],(julian:form_time([dow(sunday), [Y-M-D]]), Y #>= 2016, Y #=< 2017, label([Y,M,D])),Result), view(Result).


%% hasRecurrence(TaskID,([dow(sunday)])).

%% REDACTED

%% getClosestOneOrTwoDatesToDayOfWeek()

%% habitual(TaskID)

%% julian:form_time([dow(saturday), [Y-3-D]]), D  #=< 17, Y #>= 2016, Y #=< 2021, label([Y,3,D])),

%% setof([Y,3,D],(julian:form_time([dow(saturday), [Y-3-D]]), D  #>= 17, Y #>= 2016, Y #=< 2021, label([Y,3,D])),Result), view(Result).
%% setof([Y,3,D],(julian:form_time([dow(saturday), [Y-3-D]]), D  #=< 17, Y #>= 2016, Y #=< 2021, label([Y,3,D])),Result), view(Result).

%% setof([Y,M,D],(julian:form_time([dow(saturday), [Y-M-D]]), M #= 3, D  #=< 17, Y #>= 2016, Y #=< 2021, label([Y,M,D])),Result), view(Result).

%% setof(Y,(Y in 2016..2021,label([Y])),Result).

%% setof([Y,M,D],(julian:form_time([dow(saturday), [Y-M-D]]), M #=> MI - 1, M #=< MI + 1, , Y #>= 2016, Y #=< 2021, label([Y,M,D])),Result), setof(D,(member([Y,M,D],Result),

%% form_time([2016-3-17,0:0:0],Start),(julian:delta_time(Start, days(-7), Finish), form_time([YF-MF-DF,0:0:0],Finish)),label([YF,MF,DF]),view([YF-MF-DF]), fail.

getWindow([Y0-M0-D0],Result) :-
	setof(
	      [YF-MF-DF],
	      Start^Finish^(
			    form_time([Y0-M0-D0,0:0:0],Start),
			    (
			     (julian:delta_time(Start, days(-7), Finish) ; julian:delta_time(Start, days(7), Finish)),
			     form_time([YF-MF-DF,0:0:0],Finish)
			    ),
			    label([YF,MF,DF])
			   ),
	      Result).

getNearestDOWToDate(DOW,[_-M0-D0],StartYear,EndYear,Results) :-
	setof(Y,(Y #>= StartYear, Y #=< EndYear),Years),
	member(Y0,Years),
	getWindow([Y0-M0-D0],[Start,Finish]),
	setof([Y,M,D],(julian:form_time([dow(DOW), [Y-M-D]]), julian:compare_time(>,[Y-M-D],Start), julian:compare_time(<,[Y-M-D],Finish), label([Y,M,D])),Results).

%% %% :- setof(Results,Y^getNearestDOWToDate(saturday,[Y-3-17],2016,2116,Results),AllResults),view([results,AllResults]).

%% :- setof(Results,Y^getNearestDOWToDate(saturday,[Y-3-17],2016,2021,Results),AllResults),view([results,AllResults]).



%% For the calendar we would want to set a long of date on it, but for normal operation we wouldn't