login :-
	printStatus,
	checkTimes,
	true.

checkTimes :-
	prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/appointments.pl'),
	checkItems(tardy),
	checkItems(dueNow),
	checkItems(dueToday),
	checkItems(dueTomorrow).

%% now, seconds, minutes, hours, {morning,afternoon,evening,night}, days, weeks, months, years, decades
		%% negative now, seconds, minutes, hours, {morning,afternoon,evening,night}, days, weeks, months, years, decades

checkItems(_) :-
	true.

printStatus :-
	true.