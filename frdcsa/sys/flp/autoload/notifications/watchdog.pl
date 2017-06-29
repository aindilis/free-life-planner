processThatShouldBeRunning(['flp-cron-like.pl']).

%% if the process that runs the heart beat is monitored by the
%% heartbeat, if it stops it won't detect itself.  Fix that.

watchdogHeartbeat :-
	processThatShouldBeRunning(ProcessPatternList),
	hasTheCon(Agent),
	member(ProcessPattern,ProcessPatternList),
	(   not(isRunningP(ProcessPattern,Result)) ->
	    (	tell(freeLifePlanner,Agent,neg(isRunningP(ProcessPattern)))) ;
	    (	true)).
