alsoKnownAs('President''s Day','Washington''s Birthday').

hasHoliday(state,illinois,holidayFn('New Year''s Day'),[2017-1-2]).
hasHoliday(state,illinois,holidayFn('Martin Luther King Day',[2017-1-16])).
hasHoliday(state,illinois,holidayFn('Lincoln''s Birthday',[2017-2-13])).
hasHoliday(state,illinois,holidayFn('President''s Day',[2017-2-20])).
hasHoliday(state,illinois,holidayFn('Memorial Day',[2017-5-29])).
hasHoliday(state,illinois,holidayFn('Independence Day',[2017-7-4])).
hasHoliday(state,illinois,holidayFn('Labor Day',[2017-9-4])).
hasHoliday(state,illinois,holidayFn('Columbus Day',[2017-10-9])).
hasHoliday(state,illinois,holidayFn('Veterans''s Day',[2017-11-10])).
hasHoliday(state,illinois,holidayFn('Thanksgiving',and([2017-11-23],[2017-11-24]))).
hasHoliday(state,illinois,holidayFn('Christmas',[2017-12-25])).

hasHoliday(federal,unitedStatesOfAmerica,holidayFn('New Year''s Day'),[2017-1-2]).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('New Year''s Day (observed)'),[2017-2-2]).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('Martin Luther King Day',[2017-1-16])).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('President''s Day',[2017-2-20])).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('Memorial Day',[2017-5-29])).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('Independence Day',[2017-7-4])).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('Labor Day',[2017-9-4])).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('Columbus Day',[2017-10-9])).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('Veterans''s Day (observed)',[2017-11-10])).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('Veterans''s Day',[2017-11-11])).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('Thanksgiving',[2017-11-23])).
hasHoliday(federal,unitedStatesOfAmerica,holidayFn('Christmas',[2017-12-25])).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% ourEval(through(sunday,sunday),ResultList).
%% should it be this or that?
%% ResultList = [sunday]
%% ResultList = [sunday, monday, tuesday, wednesday, thursday, friday, saturday, sunday] 

%% ourEval(through(monday,friday),[monday,tuesday,wednesday,thursday,friday]).
%% ourEval(through(monday,saturday),[monday,tuesday,wednesday,thursday,friday,saturday]).
%% ourEval(through(sunday,saturday),[sunday,monday,tuesday,wednesday,thursday,friday,saturday]).

dayOfWeekList([sunday,monday,tuesday,wednesday,thursday,friday,saturday]).

ourEval(on(DOW1),[DOW1]).
ourEval(through(DOW1,DOW2),ResultList) :-
	dayOfWeekList(DOWList),
	append(DOWList,DOWList,DoubleDOWList),
	kmax_get_index_of_first_item_in_list(DOW1,DoubleDOWList,I1),
	nth(I2,DoubleDOWList,DOW2),
	I2 > I1,
	!,
	Offset is (I2 - I1) + 1,
	sublist(DoubleDOWList,I1,Offset,ResultList).

hasHours(Organization,DayOfWeek,Hours) :-
	hasHoursSpec(Organization,DaysOfWeekFormula,Hours),
	ourEval(DaysOfWeekFormula,DayOfWeekList),
	member(DayOfWeek,DayOfWeekList).

hasHoursSpec(Organization,DateRange,Hours) :-
	hasHoursSpec2(Organization,SpecList),
	member([DateRange,Hours],SpecList).

isOpenAt(Organization,DateTime) :-
	DateTime = [Y-M-D,_],
	dayOfWeek(DateTime,DOW),
	hasHours(Organization,DOW,HoursList),
	member([StartTime,StopTime],HoursList),
	StartTime = [StartHour:StartMinute:StartSecond],
	StopTime = [StopHour:StopMinute:StopSecond],
	occursDuringInclusive(DateTime,[Y-M-D,StartHour:StartMinute:StartSecond],[Y-M-D,StopHour:StopMinute:StopSecond]).

isTrue(X,Y) :-
	((call(X)) -> (Y = true) ; (Y = fail)).

isCurrentlyOpen(Organization) :-
	getCurrentDateTime(Now),
	isOpenAt(Organization,Now).

%% opensAt(Organization,DateTime) :-
%% 	not(isCurrentlyOpen(Organization)),
%% 	true.

isOpenUntil(Organization,DateTime) :-
	isCurrentlyOpen(Organization),
	getCurrentDateTime(Now),
	dayOfWeek(Now,DOW),
	hasHours(Organization,DOW,HoursList),
	member([StartTime,StopTime],HoursList),
	StartTime = [StartHour:StartMinute:StartSecond],
	StopTime = [StopHour:StopMinute:StopSecond],
	occursDuringInclusive(Now,[Y-M-D,StartHour:StartMinute:StartSecond],[Y-M-D,StopHour:StopMinute:StopSecond]),
	DateTime = [Y-M-D,StopHour:StopMinute:StopSecond].

isClosedAllToday(Organization) :-
	getCurrentDateTime([Y-M-D,_]),	
	isClosedEntireDate(Organization,[Y-M-D]).

isClosedEntireDate(Organization,Date) :-
	dayOfWeek(Date,DOW),
	not(hasHours(Organization,DOW,_)).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

should(hasAbility(frdcsaAgentFn(freeLifePlanner),parseText('feb 24-26 and march 3-5 hours 9:00 am to 5:30 pm'))).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Add something about not having other transportation to

scheduleTripByFromTo(bus,andrewDougherty,busstopFn(cornerOf(<ROADNAME2>,<ROADNAME2>,aurora,illinois)),<BUSINESSLOCATION>) :-
	task(Task,obtainFrom(andrewDougherty,<BUSINESSLOCATION>,<SPECIFICSUPPLIES>)).

scheduleTripByFromTo(bus,andrewDougherty,CurrentLocation,Location,arrivingBy(Deadline)) :-
	appointment(ID,Agent,Location),
	deadline(ID,Deadline).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% REDACTED

fixMe('add method to handle (except state holidays) to hasHours').
%% holidays(state,2017,[]).

%% TTY: (866) 323-0616
%% Fax: (630) 844-7499
%% Hours: 8:30 a.m. - 5:00 p.m. Monday - Friday (except state holidays)
%% Receives applications for Cash, SNAP, and Medical Assistance by fax, mail or in person.
%% IDHS Human Services - (Spanish) DHS 4001 S
%% IDHS Human Services - DHS 4001


isa(target_NorthAuroraIllinois,storeFn(targetCorporation)).
hasPharmacy(target_NorthAuroraIllinois,targetPharmacy_NorthAuroraIllinois).

hasAddress(target_NorthAuroraIllinois,addressFn('1800 Orchard Gateway Blvd',northAuroraIllinois,illinois,60542)).
hasPhoneNumber(target_NorthAuroraIllinois,phoneNumberFn('(630) 518-9042')).
%% hasHoursSpec(target_NorthAuroraIllinois,through(monday,saturday),[[[8:0:0],[22:0:0]]]).
%% hasHours(target_NorthAuroraIllinois,sunday,[[[8:0:0],[21:0:0]]]).
hasHoursSpec2(target_NorthAuroraIllinois,[
					  [on(sunday),[[[8:0:0],[21:0:0]]]],
					  [through(monday,saturday),[[[8:0:0],[22:0:0]]]]
					 ]).

isa(walgreens_NorthAuroraIllinois,storeFn(walgreensCompany)).

%% 'Hours might differ' on Presidents day
hasAddress(walgreens_NorthAuroraIllinois,addressFn('1051 Oak St',northAuroraIllinois,illinois,60542)).
hasPhoneNumber(walgreens_NorthAuroraIllinois,phoneNumberFn('(630) 907-9924')).
%% hasHoursSpec(walgreens_NorthAuroraIllinois,through(sunday,saturday),[[[9:0:0],[17:0:0]]]).
%% hasHoursSpec(walgreens_NorthAuroraIllinois,through(monday,friday),[[[9:0:0],[21:0:0]]]).
hasHoursSpec2(walgreens_NorthAuroraIllinois,[
					     [through(sunday,saturday),[[[8:0:0],[22:0:0]]]]
					    ]).

hasAddress(cvs_OrchardAndIndianTrail,addressFn('2360 W Indian Trail',auroraIllinois,illinois,60506)).
hasPhoneNumber(cvs_OrchardAndIndianTrail,phoneNumberFn('(630) 907-2041')).
hasHoursSpec2(cvs_OrchardAndIndianTrail,[
					 [through(sunday,saturday),[[[8:0:0],[22:0:0]]]]
					]).

hasAddress(cvsPhoto_OrchardAndIndianTrail,addressFn('2360 W Indian Trail',auroraIllinois,illinois,60506)).
%% hasPhoneNumber(cvsPhoto_OrchardAndIndianTrail,phoneNumberFn('(630) 907-2041')).
hasHoursSpec2(cvsPhoto_OrchardAndIndianTrail,[
					      [through(sunday,saturday),[[[8:0:0],[22:0:0]]]]
					     ]).

addHoursForLocation(cvs_indian_trail_and_orchard).
renameThisConstant(cvs_indian_trail_and_orchard).

addHoursForLocation(dollarTree_NorthAurora).

%% REDACTED

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% normalBusinessHours.

%% may(useServer(Person,auroraInterfaithFoodPantry,Week)) :-
%% week(Week),
%% isInNeedOfFood(Person),
%% not(usedServiceDuringWeek(Person,auroraInterfaithFoodPantry,Week)).

%% (ADD ROLLERBLADES TO GET TO THE OTHER BUSSTOP TO OUR LIST OF
%% TRANSPORT OPTIONS)

hoursOpens([harborFreightTools],[8:0:0]).
hoursOpens([dollarTree],[9:0:0]).
hoursOpens([fiveBelow,gameStop],[10:0:0]).

%% Office Depot
%% Address: 2302 W Indian Trail, Aurora, IL 60506
%% Phone:(630) 907-9596

%% Sunday 10:00AM  - 6:00PM
%% Monday-Saturday 8:00AM - 9:00PM