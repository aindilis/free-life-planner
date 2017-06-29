bash_command(Command, Output) :-
	process_create(path(bash),
		       ['-c', Command],
		       [stdout(pipe(Out))]),
	read_string(Out, _, Output),
	close(Out).

%% process_create(path(ls), ['-l'], []).

grep(File, Pattern, Lines) :-
	process_create(path(grep), [ Pattern, file(File) ],
		       [ stdout(pipe(Out))
		       ]),
	read_lines(Out, Lines).

read_lines(Out, Lines) :-
	read_line_to_codes(Out, Line1),
	read_lines(Line1, Out, Lines).

read_lines(end_of_file, _, []) :- !.
read_lines(Codes, Out, [Line|Lines]) :-
	atom_codes(Line, Codes),
	read_line_to_codes(Out, Line2),
	read_lines(Line2, Out, Lines).
