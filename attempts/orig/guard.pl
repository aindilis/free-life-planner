%%



login :-
	printStatus,
	checkTimes,

checkTimes :-
	load('/var/lib/myfrdcsa/codebases/minor/guardian/data-git/appointments.pl'),
	checkItems(tardy),
	checkItems(dueNow),
	checkItems(dueToday),
	checkItems(dueTomorrow).

%% now, seconds, minutes, hours, {morning,afternoon,evening,night}, days, weeks, months, years, decades
		%% negative now, seconds, minutes, hours, {morning,afternoon,evening,night}, days, weeks, months, years, decades


checkItems(Interval) :-
	true.


