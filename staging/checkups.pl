% %% Record St. Patrick's day here

% defeasibly(ask(frdcsaAgentFn(freeLifePlanner),Agent,obtainAnswerOfType(unknown)) :-
% 	   isa(Agent,legalAdult),
% 	   not(has(Agent,disability)).

% defeasibly(formFn(tax(federal)),formFn(tax(state)),formFn(tax(city))) :-
% 	yearFn(Year),
% 	alive(Agent,Year).

% defeasibly(renewalFn(foodStamps)) :-
% 	yearFn(Year),
% 	alive(Agent,Year).

% % Birthdays of people I know
% %   Tax forms for federal, state, and city taxes
% %   Food stamp semi-annual form every six months (May and November)
% %   Health insurance semi-annual form every six months (April and October)
% %   Yearly health exams:

% yearlyHealthExams(Person,[examFn(mammography),examFn(dental),examFn(dentalCleaning)]).
% regularMeetings(Person,[
% 			meetingFn(work),
% 			meetingFn(communityEvents),
% 			]).

% %% eventFn(due(billFn(X))).
% %% eventFn(due(billFn(cellPhone))).
