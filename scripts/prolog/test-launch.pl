launchProgram(Program) :-
	write(Program),
	exec(Program).

launchPrograms :-
	log(Date,hasSymptom(Person,Symptom)),
	launchProgram('/usr/bin/stopwatch').

:- launchPrograms.