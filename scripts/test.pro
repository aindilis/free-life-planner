save(Data,Filename) :-
	open(Filename, write, S),
	write(S,Data),
	close(S).
