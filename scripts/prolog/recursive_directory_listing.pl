:- consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl').

:- use_module(library(regex)).

load_all_files_in_subdirectories_with_matching_names(Dir,Regex) :-
	nl,write('LOADING DIR RECURSIVELY: '),write(Dir),nl,
	sorted_directory_files_no_hidden(Dir,Files),
	member(File,Files),
	atomic_list_concat([Dir,'/',File],'',FullFile),
	(exists_file(FullFile) ->
	 (string_match_p(FullFile,Regex) -> (tab(4),write('LOADING FILE: '),write(FullFile),nl %%,prolog_consult(FullFile)
					    )) ;
	  load_all_files_in_subdirectories_with_matching_names(FullFile,Regex)),
	fail.
load_all_files_in_subdirectories_with_matching_names(Dir,Regex).

%% load_all_prolog_files_in_subdirectories_with_matching_names(Dir) :-
%% 	nl,write('LOADING DIR RECURSIVELY: '),write(Dir),nl,
%% 	sorted_directory_files_no_hidden(Dir,Files),
%% 	member(File,Files),
%% 	atomic_list_concat([Dir,'/',File],'',FullFile),
%% 	(exists_file(FullFile) -> (atom_(Basename,'.pl',FullFile) -> tab(4),write('LOADING FILE: '),write(FullFile),nl) ; load_all_prolog_files_in_subdirectories_with_matching_names(FullFile)),
%% 	fail.
%% load_all_prolog_files_in_subdirectories_with_matching_names(Dir).
