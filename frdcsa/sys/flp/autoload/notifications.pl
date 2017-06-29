%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% %% notificationSystem

%% %% we can always improve this in the future for now, just generate
%% %% warnings ahead of time.

%% :- prolog_use_module(library(julian)).

%% 1: This notification is to warn you of an upcoming notification.
%% 2: This notification is just to remind you of the previous notification.

pendingItemsDateTimeRange([2017-5-19,0:0:0],[2017-5-20,0:0:0]).

mainLoopOneStep :-
	getCurrentDateTime(CurrentTime),
	%% pendingItemsDateTimeRange(StartDateTime,EndDateTime),
	CurrentTime = [Y-M-D,H:Mi:S],
	StartDateTime = [Y-M-D,0:0:0],	
	consecutiveDates([Y-M-D],[Y1-M1-D1]),
	EndDateTime = [Y1-M1-D1,23:59:59],	
	actOnAllPendingItems(CurrentTime,StartDateTime,EndDateTime),
	%% logSensorData,
	true.

%% mainLoopOneStep :-
%% 	getCurrentDateTime(CurrentTime),
%% 	pendingItemsDateTimeRange(StartDateTime,EndDateTime),
%% 	actOnAllPendingItems(CurrentTime,StartDateTime,EndDateTime).
%% 	%% logSensorData,

actOnAllPendingItems(CurrentTime,StartDateTime,EndDateTime) :-
	view([times,[CurrentTime,StartDateTime,EndDateTime]]),
	pending(notifications,CurrentTime,StartDateTime,EndDateTime,PendingNotifications),
	pending(recurrences,CurrentTime,StartDateTime,EndDateTime,PendingRecurrences),
	processPendingNotifications(PendingNotifications),
	processPendingRecurrences(PendingRecurrences).

pending(notifications,CurrentTime,StartDateTime,EndDateTime,PendingNotifications) :-
	findall(Result,(
			significantDateForEvent(Event,ItemDateTime),
			hasAdvanceDateTime(ItemDateTime,StartDateTime,EndDateTime,CurrentTime,TmpResult1),
			append([Event],TmpResult1,TmpResult2),
			append(TmpResult2,[notification(event)],Result)
		       ),
		PendingNotifications),
	view([pending(notifications,PendingNotifications)]).

processPendingNotifications(PendingNotifications) :-
	view([pending(notifications,PendingNotifications)]),
	forall(member([EventID,ItemDateTime,NotificationDateTime,Type],PendingNotifications),
	       processPendingNotificationsHelper(EventID,ItemDateTime,NotificationDateTime,Type)),
	!.

processPendingNotificationsHelper(EventID,ItemDateTime,NotificationDateTime,Type) :-
	description(EventID,TmpDescription),
	shell_quote_term(TmpDescription,Description),
	hasParticipants(EventID,Participants),
	view([description,Description,pendingNotifications,PendingNotifications]),	
	generateGlossFor(NotificationDateTime,NotificationDateTimeGloss),
	view([notificationDateTime,NotificationDateTime,notificationDateTimeGloss,NotificationDateTimeGloss]),
	shell_quote_term(Type,QuotedType),
	atomic_list_concat([
			    'Upcoming ',QuotedType,' at: ',
			    NotificationDateTimeGloss,
			    '. Description: ',
			    Description,
			    '. Please acknowledge by saying: Alice, acknowledge.'
			   ],' ',Message),
	notifyParticipants(Participants,Message).

pending(recurrences,CurrentTime,StartDateTime,EndDateTime,PendingRecurrences) :-
	julian:delta_time(CurrentTime,s(DurationSeconds),EndDateTime),
	view([durationSeconds,DurationSeconds]),
	calculateUpcomingAlerts(s(DurationSeconds),TmpPendingRecurrences),
	view([tmpPendingRecurrences,TmpPendingRecurrences]),
	findall(Result,(
			member([Description,Times2],TmpPendingRecurrences),
			member(Times1,Times2),
			member([Y,M,D,H,Mi,S],Times1),
			ItemDateTime = [Y-M-D,H:Mi:S],
			dateTimeP(ItemDateTime),
			%% view([description,Description,itemDateTime,ItemDateTime]),
			hasAdvanceDateTime(ItemDateTime,StartDateTime,EndDateTime,CurrentTime,TmpResult1),
			%% view([tmpResult,TmpResult1]),
			append([Description],TmpResult1,TmpResult2),
			append(TmpResult2,[recurrence(event)],Result)
		       ),
		PendingRecurrences),
	view([pendingRecurrences,PendingRecurrences]).

processPendingRecurrences(PendingRecurrences) :-
	view([pendingRecurrences,PendingRecurrences]),
	forall(member([TmpDescription,ItemDateTime,RecurrenceDateTime,Type],PendingRecurrences),
	       processPendingRecurrencesHelper(TmpDescription,ItemDateTime,RecurrenceDateTime,Type)),
	!.

processPendingRecurrencesHelper(TmpDescription,ItemDateTime,RecurrenceDateTime,Type) :-
	shell_quote_term(TmpDescription,Description),
	%% hasParticipants(_,Participants),
	Participants = [andrewDougherty],
	view([description,Description,pendingRecurrence,[TmpDescription,ItemDateTime,RecurrenceDateTime]]),
	generateGlossFor(RecurrenceDateTime,RecurrenceDateTimeGloss),
	view([notificationDateTime,RecurrenceDateTime,notificationDateTimeGloss,RecurrenceDateTimeGloss]),
	shell_quote_term(Type,QuotedType),
	atomic_list_concat([
			    'Upcoming ',QuotedType,' at: ',
			    RecurrenceDateTimeGloss,
			    '. Description: ',
			    Description,
			    '. Please acknowledge by saying: Alice, acknowledge.'
			   ],' ',Message),
	notifyParticipants(Participants,Message).

hasParticipants(EventID,Participants) :-
	appointment(EventID,_,_),
	meetingAgenda(EventID,Participants,_).

notifyParticipants(Participants,Message) :-
	member(Participant,Participants),
	view([tell(Participant,Message)]),
	%% tell(Participant,[Message]),
	%% fassert([assertion(told(Participant,Message))),
	%% fassert([assertion(sentNotification(NotificationID))),
	true.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Sensor Readings
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

locationHasSensor(Location,Sensor) :-
	locationHasSensors(Location,Sensors),
	member(Sensor,Sensors).

locationHasSensors(auroraIllinois,[temperature]).
locationHasSensors(<ROOM1>,[temperature]).
locationHasSensors(<ROOM2>,[temperature]).

logAllSensorData :-
	findall([Sensor,Location],locationHasSensor(Location,Sensor),SensorLocations),
	member([Sensor,Location],SensorLocations),
	logSensorData(Sensor,Location).
	
hasAssignedContext(sensorLog,'Org::FRDCSA::SVRS::SensorLog').

logSensorData(Sensor,Location) :-
	sensorHasPropertyAtTime(Location,Sensor,QueryDateTime,RecordDateTime,Value),
	hasAssignedContext(sensorLog,SensorLogContext),
	%% fassert([context(SensorLogContext),assertion(atTime(DateTime,hasProperty(Location,Sensor,Value)))]),
	true.

sensorHasPropertyAtTime(Location,temperature,QueryDateTime,RecordDateTime,Value) :-
	getCurrentDateTime(QueryDateTime),
	%% FIXME: get sensor time from sensor in the future
	RecordDateTime = QueryDateTime,
	hasClimatePropertyAt(Location,temperature,Value).

%% attendedEvent(appointment231,andrewDougherty).

%% eventHappenedOnTime(appointment231).

%% task(TaskID,completeEvent(Event),Importance) :-
%% 	squelch(TaskID),
%% 	squelch(Importance),
%% 	event(Event).

%% just use some home grown temporal semantics for now, then
%% generalize using a more complete system later



%% sendNotification(Notification) :-
%% 	notificationAgent(Notification,Agent),
%% 	notificationMessage(Notification,Message),
%% 	broadcastOnAllMediums([agent(Agent),message(Message)]),
%% 	getCurrentDateTime(Now),
%% 	notificationID(Notification,NotificationID),
%% 	fassert('Agent1','Yaswi1',atTime(Now,completed(NotificationID)),Result).


%% calculateAllUnperformedNotificationsForEvent(Event) :-
%% 	%% give a notification ahead of time.  one 
%% 	true.

%% %% typeOfEvent(Event,normal).

hasAdvanceDateTime(ItemDateTime,StartDateTime,EndDateTime,Now,Result) :-
	dateTimeP(ItemDateTime),
	%% member(AdvanceWarningInSeconds,[86400,3600,300,0]),
	member(AdvanceWarningInSeconds,[0]),
	julian:delta_time(TmpNotificationDateTime,s(AdvanceWarningInSeconds),ItemDateTime),
	%% view([tmpNotificationDateTime,TmpNotificationDateTime]),
	julian:form_time([TmpNotificationDateTime,Y-M-D,H:Mi:S]),
	%% view([ymdhms,[Y-M-D,H:Mi:S]]),
	NotificationDateTime = [Y-M-D,H:Mi:S],
	%% view([notificationDateTime,NotificationDateTime]),
	afterDateTime(NotificationDateTime,StartDateTime),
	%% Fixme: change Now to Now + one minute
	%% afterDateTime(Now,NotificationDateTime),
	afterDateTime(EndDateTime,NotificationDateTime),
	Result = [ItemDateTime,NotificationDateTime].

%% have a startDateTime before which no alarms from that time are
%% queued anymore.  alarms that have past sometimes are still queued,
%% such as if the computer was disabled or unable to communicate.q


%% %% kqmlPerformativeTell(AgentA,AgentB,Phi).

%% %% see /var/lib/myfrdcsa/codebases/minor/agent/frdcsa/sys/flp/autoload/kqml.pl

%% announced(NotificationID

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% communicationMediums([bluetoothSpeakers,computerSpeakers,headPhones,email,textMessage,instantMessage,phoneCall,irc]).

%% broadcastOnAllMediums(Arguments) :-
%% 	argt(Arguments,[agent(Agent),message(Message)]),
%% 	communicationMediums(CommunicationMediums),
%% 	forall(member(CommunicationMedium,CommunicationMediums),
%% 	       broadcastOnMedium(CommunicationMedium,[agent(Agent),message(Message)])).

%% broadcastOnMedium(bluetoothSpeakers,Arguments) :-
%% 	argt(Arguments,[message(Message)]),

%% 	%% in the future find where the user is believed to be and
%% 	%% just send there

%% 	getAllMedium(bluetoothSpeakers,BluetoothSpeakers),
%% 	member(BluetoothSpeaker,BluetoothSpeakers),
%% 	hasLogin(BluetoothSpeaker,Login),
%% 	alexaPushNotification(Login,Message).

%% broadcastOnMedium(email,Arguments) :-
%% 	argt(Arguments,[agent(Agent),message(Message)]),
%% 	forall(	hasEmail(Agent,EmailAddress),
%% 		(
%% 		 renderMessageToSubject(Message,Subject),
%% 		 renderMessageToBody(Message,Body),
%% 		 %% 'freelifeplanner@gmail.com'
%% 		 sendEmail(['adougher9@gmail.com',[EmailAddress],[],[],Subject,Body,[],''],Result)
%% 		)).
%% renderMessageToSubject(Message,'important announcement from free life planner').
%% renderMessageToBody(Message,Message).

%% broadcastOnMedium(textMessage,Arguments) :-
%% 	true.	

%% broadcastOnMedium(instantMessage,Arguments) :-
%% 	argt(Arguments,[agent(Agent),message(Message)]),
%% 	%% get other agents who should know
%% 	tell(Agent,Message).


%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%