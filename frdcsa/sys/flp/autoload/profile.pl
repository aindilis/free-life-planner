%% CDT will be observed in Chicago until Nov 5, 2017 at 2:00 am

currentTimeZone(centralDaylightTime).

%% currentTimeZone(TimeZone) :-
%% 	getCurrentUTCDateTime(TmpCurrentUTCDateTime),
%% 	convertUTCDateTimeToTimeZoneDateTime(TmpCurrentUTCDateTime,utc,CurrentUTCDateTime),
%% 	convertUTCDateTimeToTimeZoneDateTime(TmpNextEndOfDSTUTCDateTime,centralDaylightTime,[2017-11-5,2:0:0]),
%% 	convertUTCDateTimeToTimeZoneDateTime(TmpNextEndOfDSTUTCDateTime,utc,[Y-M-D,H:Mi:S]),
%% 	view([currentUTCDateTime,CurrentUTCDateTime,nextEndOfDSTUTCDateTime,[Y-M-D,H:Mi:S]]),
%% 	(   before(CurrentUTCDateTime,NextEndOfDSTUTCDateTime) -> 
%% 	    (	TimeZone = centralStandardTime ) ;
%% 	    (	TimeZone = centralDaylightTime )).
