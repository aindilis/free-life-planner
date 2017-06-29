generatePageFor(alexaIntegration,[history,History]) :-
	findall([Query,Answer,Command,DateTime],
		(   naturalLanguageQueryHistory(Query,TmpDateTime),
		    (	naturalLanguageAnswerHistory(Query,TmpAnswer,TmpDateTime) -> Answer = TmpAnswer ; Answer = ''),
		    (	naturalLanguageCommandHistory(Query,TmpCommand,TmpDateTime) -> Command = TmpCommand ; Command = ''),
		    fixTime(TmpDateTime,DateTime)),
		TmpHistory),
	predsort(compareQNATime,TmpHistory,Tmp2History),
	findall([Query,Answer,Command,Gloss],(member([Query,Answer,Command,DateTime],Tmp2History),generateGlossFor(DateTime,Gloss)),Tmp3History),
	reverse(Tmp3History,History).

compareQNATime(Order,[_,_,_,T1],[_,_,_,T2]) :-
	compareTime(Order,T1,T2).
