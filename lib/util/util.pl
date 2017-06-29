:- dynamic prolog_files/3, prolog_files/2.
:- multifile classifiedTerm/1.

:- use_module(library(regex)).

:- assert(prolog_files(util,'/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl')).

:- dynamic planStepNumber/2.

%% FIXME: decrease counter for declassfied scry results that

%% change this to ensure loaded, or whatever is necessary so it
%% doesn't load more than once, i.e. end_of_file :-
%% prolog_files(util,'...'). or whatever

%% radarDirs(['/var/lib/myfrdcsa','/var/lib/myfrdcsa/codebases','/var/lib/myfrdcsa/codebases/internal','/var/lib/myfrdcsa/codebases/external','/var/lib/myfrdcsa/packages/binary','/var/lib/myfrdcsa/codebases/minor','/var/lib/myfrdcsa/codebases/work','/var/lib/myfrdcsa/codebases/work/clients','/var/lib/myfrdcsa/codebases/work/work','/var/lib/myfrdcsa/datasets','/var/lib/myfrdcsa/codebases/internal/source-hatchery','/var/lib/myfrdcsa/collections','/var/lib/myfrdcsa/repositories/external','/var/lib/myfrdcsa/repositories/external/cvs','/var/lib/myfrdcsa/repositories/external/git','/var/lib/myfrdcsa/repositories/external/svn','/var/lib/myfrdcsa/repositories/internal','/var/lib/myfrdcsa/repositories/internal/cvs','/var/lib/myfrdcsa/repositories/internal/git','/var/lib/myfrdcsa/repositories/internal/svn','/var/lib/myfrdcsa/capabilities','/var/lib/myfrdcsas/versions/myfrdcsa-1.1/vagrant-machines']).

view(Item) :-
	write_term(Item,[quoted(true)]),nl,!.

seer(Item) :-
	write_term(Item,[quoted(true)]),nl,!.


scry(Item) :-
	findall(Item,(Item,declassified(Item)),Result),
	write_header(Item,Result),nl,
	write_list(Result),
	write_footer(Item,Result),nl,nl.

tsScry(Item) :-
	findall(Item,Item,Result),
	write_header(Item,Result),nl,
	sec_write_list(Result),
	write_footer(Item,Result),nl,nl.

scryList(X,List) :-
	findall(X,X,List).

write_header(Item,Result) :-
	write_header_footer(Item,Result).

write_footer(Item,Result) :-
	write_header_footer(Item,Result).

write_header_footer(Item,Result) :-
	write_result_length(Result),write(' for Scrying for: '),
	write_term(Item,[quoted(true)]).

write_result_length(Result) :-
	length(Result,Length),
	write(Length),write(' results').

write_list(List) :-
	member(Term,List),
	isDeclassified(Term),
	tab(4),write_term(Term,[quoted(true)]),write('.'),nl,
	fail.
write_list(_).

sec_write_list(List) :-
	member(Term,List),
	tab(4),write_term(Term,[quoted(true)]),write('.'),nl,
	fail.
sec_write_list(_).


%%% COMPLETIONS FOR TAB COMPLETIONS

				% listCompletions(Completions) :-
				% 	findall(X,current_atom(X),Completions).

listCompletions(Completions) :-
	setof(X,M^P^current_predicate(X,M:P),Completions).

listCompletionsForModule(M,Completions) :-
	setof(X,M^P^current_predicate(X,M:P),Completions).

%% declassified(Goal) :-
%% 	Goal,
%% 	forall(occurs:sub_term(X,Goal),isDeclassified(X)).

declassified_orig(Goal) :-
	Goal,
	forall(occurs:sub_var(X,Goal),isDeclassified(X)).

declassified(Goal) :-
	Goal,
	forall(term_contains_subterm(X,Goal),isDeclassified(X)).

isDeclassified(Term) :-
	forall(classifiedTerm(Sub),free_of_term(Sub, Term)).

%% See /var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/trouble.txt

%% FIXME: implement this
%% reviewAllTermsForClassificationStatus.

%% findAllTermsInListingMatchingSubterm(M,Term,Matches) :-
%% 	findall(Result,(current_predicate(_,M:P),catch(clause(M:P,Result),_,true)),Z),
%% 	findall(Clause,(member(Clause,Z),occurs:sub_term(Term,Clause)),Matches).

%% findAllFactsInListingMatchingSubterm(Term,Matches) :-
%% 	findall(Result,(current_predicate(_,M:P),catch(clause(M:P,true),_,true)),Z),
%% 	findall(Clause,(member(Clause,Z),occurs:sub_term(Term,Clause)),Matches).

listFiles(Files) :-
	listFilesA(FilesA),
	listFilesB(FilesB),
	setof(File,FilesA^FilesB^(member(File,FilesA) ; member(File,FilesB)),Files).

listFilesA(Files) :-
	setof(File,Predicate^(predicate_property(Predicate,file(File)),not((File =~ "^/usr/lib/swi-prolog");(File =~ "lib/swipl/pack"))),Files).

listFilesB(Files) :-
	setof(File,Project^Mode^Exported^(prolog_files(Mode,File);prolog_files(Mode,File,Exported)),Files).

				% grep_atoms(Search) :-
				% 	setof(Atom,current_atom(Atom),List),
				% 	member(Atom,List),

prolog_record_load(Type,File) :-
	write('Prolog-Loading-'),write(Type),write(': '),write(File),nl,
	assert(prolog_files(Type,File)).

prolog_record_load(Type,File,X) :-
	write('Prolog-Loading-'),write(Type),write(': '),write(File),write(' :: '),write(X),nl,
	assert(prolog_files(Type,File,X)).

prolog_ensure_loaded(File) :-
	prolog_record_load(ensure_loaded,File),
	ensure_loaded(File).

prolog_consult(File) :-
	prolog_record_load(consult,File),
	consult(File).

prolog_include(File) :-
	prolog_record_load(include,File).

prolog_use_module(File) :-
	prolog_record_load(use_module,File),
	use_module(File).

prolog_use_module(File,X) :-
	prolog_record_load(use_module,File,X),
	use_module(File,X).

prolog_import(File) :-
	prolog_record_load(import,File),
	import(File).


%% prolog_ensure_loaded(File) :-
%% 	prolog_record_load(ensure_loaded,File),
%% 	load_qlf_but_failed(File) -> ensure_loaded(File).

%% prolog_consult(File) :-
%% 	prolog_record_load(consult,File),
%% 	load_qlf_but_failed(File) -> consult(File).

%% prolog_include(File) :-
%% 	prolog_record_load(include,File).

%% prolog_use_module(File) :-
%% 	prolog_record_load(use_module,File),
%% 	load_qlf_but_failed(File) -> use_module(File).

%% prolog_use_module(File,X) :-
%% 	prolog_record_load(use_module,File,X),
%% 	load_qlf_but_failed(File) -> use_module(File,X).

%% prolog_import(File) :-
%% 	prolog_record_load(import,File),
%% 	load_qlf_but_failed(File) -> import(File).

directory_files_no_hidden(Dir,Files) :-
	directory_files(Dir,TmpFiles),
	findall(File,(member(File,TmpFiles),not(atom_concat('.',_,File))),Files).

sorted_directory_files(Dir,Files) :-
	directory_files(Dir,TmpFiles),
	sort(TmpFiles,Files).

sorted_directory_files_no_hidden(Dir,Files) :-
	directory_files_no_hidden(Dir,TmpFiles),
	sort(TmpFiles,Files).

load_all_prolog_files_in_subdirectories_with_matching_names(Dir) :-
	nl,write('LOADING DIR(1): '),write(Dir),nl,
	sorted_directory_files_no_hidden(Dir,Files),
	member(File,Files),
	atom_concat(File,'.pl',PrologFile),
	atomic_list_concat([Dir,'/',File,'/',PrologFile],'',FullFileName),
	write(FullFileName),nl,
	exists_file(FullFileName),
	tab(4),write('Loading Prolog File: '),write(FullFileName),nl,
	prolog_consult(FullFileName),
	fail.
load_all_prolog_files_in_subdirectories_with_matching_names(Dir).

load_frdcsa_project_files(Project,Dir) :-
	%% nl,
	exists_directory(Dir),
	sorted_directory_files_no_hidden(Dir,Files),
	member(File,Files),
	split_string(File, "-", "", SplitFile),
	atomic_list_concat(SplitFile, "_",FileProlog),
	%% write('Checking '),write(FileProlog),nl,
	load_frdcsa_project_files_helper(Project,Dir,File,FileProlog),
	fail.
load_frdcsa_project_files(Project,Dir) :-
	%% nl,
	not(exists_directory(Dir)),
	write('WARNING: Directory does not exist: '),write(Dir),nl,
	fail.
load_frdcsa_project_files(Project,Dir).

load_myfrdcsa1p1_project_files(Project,Dir) :-
	%% nl,
	exists_directory(Dir),
	sorted_directory_files_no_hidden(Dir,Files1),
	member(File1,Files1),
	atomic_list_concat([Dir,'/',File1],NewDir),
	exists_directory(NewDir),
	sorted_directory_files_no_hidden(NewDir,Files2),
	member(File2,Files2),
	split_string(File1, "-", "", SplitFile),
	atomic_list_concat(SplitFile, "_",FileProlog),
	%% write('Checking '),write(FileProlog),nl,
	atomic_list_concat([File1,'/',File2],NewFile),
	atomic_list_concat([Dir,'/',NewFile],TmpFullFileName),	
	exists_directory(TmpFullFileName),
	load_frdcsa_project_files_helper(Project,Dir,NewFile,FileProlog),
	fail.
load_myfrdcsa1p1_project_files(Project,Dir) :-
	%% nl,
	not(exists_directory(Dir)),
	write('WARNING: Directory does not exist: '),write(Dir),nl,
	fail.
load_myfrdcsa1p1_project_files(Project,Dir).

load_frdcsa_project_files_helper(Project,Dir,File,FileProlog) :-
	atomic_list_concat([Dir,'/',File,'/frdcsa/sys/',Project,'/',FileProlog,'.pl'],'',FullFileName),
	exists_file(FullFileName),
	write('Loading Source File: '),write(FullFileName),nl,
	prolog_consult(FullFileName),
	fail.
load_frdcsa_project_files_helper(Project,Dir,File,FileProlog) :-
	atomic_list_concat([Dir,'/',File,'/frdcsa/sys/',Project,'/autoload'],'',FullDirName),
	%% tab(4),view(FullDirName),nl,
	exists_directory(FullDirName),
	tab(4),write('Loading Directory Files: '),write(FullDirName),nl,
	load_all_files_in_subdirectories_with_matching_names(FullDirName,'\\.pl$').
%% load_frdcsa_project_files_helper(Project,Dir,File,FileProlog) :-
%% 	atomic_list_concat([Dir,'/',File,'/frdcsa/sys/',Project,'/autoload'],'',FullDirName),
%% 	%% tab(4),view(FullDirName),nl,
%% 	exists_directory(FullDirName),
%% 	tab(4),write('Loading Directory Files: '),write(FullDirName),nl,
%% 	load_all_prolog_files_in_directory(FullDirName).

load_all_files_having_extension_in_directory(Dir,Extension) :-
	nl,write('LOADING DIR(2): '),write(Dir),nl,
	sorted_directory_files(Dir,Files),
	member(File,Files),
	atom_concat(Thing,Extension,File),
	atomic_list_concat([Dir,'/',File],'',FullFileName),
	exists_file(FullFileName),
	tab(4),write('Loading Prolog File: '),write(FullFileName),nl,
	prolog_consult(FullFileName),
	fail.
load_all_files_having_extension_in_directory(Dir,Extension).

load_all_prolog_files_in_directory(Dir) :-
	load_all_files_having_extension_in_directory(Dir,'.pl').

%% load_all_prolog_files_in_directory_and_all_subdirectories(Dir) :-
%%    load_all_files_having_extension_in_directory_and_all_subdirectories(Dir,'.pl').

load_all_files_having_extension_in_directory_and_all_subdirectories(Dir,Extension) :-
	exists_directory(Dir),
	nl,write('LOADING DIR(3): '),write(Dir),nl,
	sorted_directory_files(Dir,Files),
	member(File,Files),
	atom_concat(Thing,Extension,File),
	atomic_list_concat([Dir,'/',File],'',FullFileName),
	exists_file(FullFileName),
	tab(4),write('Loading Prolog File: '),write(FullFileName),nl,
	prolog_consult(FullFileName),
	fail.
load_all_files_having_extension_in_directory_and_all_subdirectories(Dir,Extension).

:- include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/shell-utils.pl').
:- prolog_include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/shell-utils.pl').

				% make :-
				% 	listFiles(Files),
				% 	member(File,Files),
				% 	qcompile(File),
				% 	fail.
				% make.

load_qlf(File) :-
	atom_concat(Tmp1,'.pl',File),
	atom_concat(Tmp1,'.qlf',File2),
	consult(File2).

load_qlf_but_failed(File) :-
	true.
%% not(load_qlf(File)).

nextCounter(Task,ID) :-
	\+ counter(Task,_),
	NewID is 1,
	atom_concat(Task,NewID,ID),
	assert(counter(Task,NewID)).
nextCounter(Task,ID) :-
	counter(Task,OrigID),
	retractall(counter(Task,OrigID)),
	NewID is OrigID + 1,
	atom_concat(Task,NewID,ID),
	assert(counter(Task,NewID)).

resetCounter(Task) :-
	retractall(counter(Task,_)).

string_match_p(String,Regex) :-
	regex(Regex, [], String, []).

load_all_files_in_subdirectories_with_matching_names(Dir,Regex) :-
	nl,write('LOADING DIR RECURSIVELY: '),write(Dir),nl,
	sorted_directory_files_no_hidden(Dir,Files),
	member(File,Files),
	atomic_list_concat([Dir,'/',File],'',FullFile),
	(exists_file(FullFile) ->
	 (string_match_p(FullFile,Regex) -> (tab(4),write('LOADING FILE: '),write(FullFile),nl,prolog_consult(FullFile))) ;
	 load_all_files_in_subdirectories_with_matching_names(FullFile,Regex)),
	fail.
load_all_files_in_subdirectories_with_matching_names(Dir,Regex).

load_all_prolog_files_in_subdirectories_with_downcased_matching_names(Dir) :-
	nl,write('LOADING DIR(4): '),write(Dir),nl,
	sorted_directory_files_no_hidden(Dir,Files),
	member(File,Files),
	atom_concat(File,'.pl',Tmp),
	view(1117),
	downcase_atom(Tmp,PrologFile),
	atomic_list_concat([Dir,'/',File,'/',PrologFile],'',FullFileName),
	write(FullFileName),nl,
	exists_file(FullFileName),
	tab(4),write('Loading Prolog File: '),write(FullFileName),nl,
	prolog_consult(FullFileName),
	fail.
load_all_prolog_files_in_subdirectories_with_downcased_matching_names(Dir).

getListing(M,Listing) :-
 	findall(Result,(current_predicate(_,M:P),catch(clause(M:P,Result),_,true)),Listing).

%% getFullListing(Listing) :-
%% 	findall(Atom,(findall(Term,predicate_property(Term,file('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl')),List),member(Term,List),Term =.. [P|_],with_output_to(atom(Atom),listing(P)),view(Atom)),Listing).

%% findAllTermsInListingMatchingSubterm(M,Term,Matches) :-
%% 	getListing(M,Listing),
%% 	findall(Clause,(member(Clause,Listing),occurs:sub_term(Term,Clause)),Matches).

%% findAllFactsInListingMatchingSubterm(Term,Matches) :-
%%  	findall(Result,(current_predicate(_,M:P),catch(clause(M:P,true),_,true)),Z),
%%  	findall(Clause,(member(Clause,Z),occurs:sub_term(Term,Clause)),Matches).

%% http://www.swi-prolog.org/pldoc/man?section=consulting
%% source_location().
%% prolog_load_context().

%% decamelcase(Input,Output) :-
%% 	.

findDefinitionInformation(Predicate/ArityArg,DefinitionInformation) :-
	(number(ArityArg) -> (Arity = ArityArg) ; (atom_number(ArityArg,Arity))),
	repl(_,Arity,ArgList),
	Term =.. [Predicate|ArgList],
	predicate_property(Term,file(Filename)),
	predicate_property(Term,line_count(LineNumber)),
	DefinitionInformation = [Filename,LineNumber].

%% findall(Atom,(findall(Term,predicate_property(Term,file('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl')),List),member(Term,List),Term =.. [P|_],with_output_to(atom(Atom),listing(P)),view(Atom)),Atoms),view(Atoms).

%% findall(Atom,(findall(Term,predicate_property(Term,file('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl')),List),member(Term,List),clause(Term,true),Atoms),view(Atoms).

%% predicate_property(Term,file('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl')),Term =.. [P|_],current_predicate(_,M:P),catch(clause(M:P,Result),_,true).

%% findall(Init,(findall(Atom,(findall(Term,predicate_property(Term,file('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl')),List),member(Term,List),Term =.. [P|_],with_output_to(atom(Atom),listing(P))),Atoms),member(Atom,Atoms),atom_concat(Init,'.\n\n',Atom)),Inits),view(Inits).

%% findall([P,B],(predicate_property(user:P,file('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl')),clause(P,B,R)),Items),view(Items).

%% F = '/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl', findall(C,(predicate_property(user:P,file(F)),clause(P,B,R),C = :-(P,B)),Cs),view(Cs).

%% allTermAssertions(Term,File,Assertions) :-
%% 	findall(C,(predicate_property(user:P,file(File)),clause(P,B,R),C = :-(P,B)),Cs),
%% 	findall(Clause,(member(Clause,Cs),occurs:sub_term(Term,Clause)),Assertions).





%% cap(Term) :-
%% 	allTermAssertions(Term,Assertions),
%% 	write_list_no_tabs(Assertions).

%% write_list_no_tabs(List) :-
%% 	member(Term,List),
%% 	isDeclassified(Term),
%% 	write_term(Term,[quoted(true)]),write('.'),nl,
%% 	fail.
%% write_list_no_tabs(_).

%% allTermAssertions(Term,Assertions) :-
%% 	listFiles(Files),
%% 	findall(C,(member(File,Files),atom(File),predicate_property(user:P,file(File)),clause(P,B,R),C = :-(P,B)),Cs),
%% 	findall(Output,(member(Clause,Cs),occurs:sub_var(Term,Clause),Clause = (:-(Head,Body)),((Body = true) -> (Output = Head) ; (Output = :-(Head,Body)))),Assertions).

%% allTermAssertionsNoFactForm(Term,Assertions) :-
%% 	listFiles(Files),
%% 	findall(C,(member(File,Files),atom(File),predicate_property(user:P,file(File)),clause(P,B,R),C = :-(P,B)),Cs),
%% 	findall(Clause,(member(Clause,Cs),occurs:sub_term(Term,Clause)),Assertions).

%% getFullListing(Assertions) :-
%% 	listFiles(Files),
%% 	findall(C,(member(File,Files),atom(File),predicate_property(user:P,file(File)),clause(P,B,R),C = :-(P,B)),Cs),
%% 	findall(Output,(member(Clause,Cs),Clause = (:-(Head,Body)),((Body = true) -> (Output = Head) ; (Output = :-(Head,Body)))),Assertions).



c(Term) :-
	allTermAssertions(Term,Assertions),
	write_list_no_tabs(Assertions).

write_list_no_tabs(List) :-
	member(Term,List),
	isDeclassified(Term),
	write_term(Term,[quoted(true)]),write('.'),nl,
	fail.
write_list_no_tabs(_).

allTermAssertionsNoPredicates(Term,Assertions) :-
	listFiles(Files),
	findall(C,(member(File,Files),atom(File),predicate_property(user:P,file(File)),clause(P,B,R),C = :-(P,B)),Cs),
	findall(Output,(member(Clause,Cs),occurs:sub_var(Term,Clause),Clause = (:-(Body,Head)),((Head = true) -> (Output = Body) ; (Output = :-(Body,Head)))),Assertions).

%% allTermAssertionsOrig(Term,Assertions) :-
%% 	listFiles(Files),
%% 	findall(C,(member(File,Files),atom(File),predicate_property(M:P,file(File)),clause(P,B,R),C = :-(P,B)),Cs),
%% 	findall(Output,(member(Clause,Cs),term_contains_subterm(Term,Clause),Clause = (:-(Body,Head)),((Head = true) -> (Output = Body) ; (Output = :-(Body,Head)))),Assertions).

allTermAssertionsNoPred(Term,Assertions) :-
	listFiles(Files),
	setof(C,File^Files^M^P^B^R^(member(File,Files),atom(File),(predicate_property(M:P,file(File));predicate_property(M:P,dynamic)),clause(P,B,R),C = :-(P,B)),Cs),
	setof(Output,Clause^Cs^Term^Body^Head^(member(Clause,Cs),occurs:sub_var(Term,Clause),Clause = (:-(Body,Head)),((Head = true) -> (Output = Body) ; (Output = :-(Body,Head)))),Assertions).

allTermAssertions(Term) :-
	allTermAssertions(Term,Assertions),
	write_list(Assertions).

allTermAssertions(Term,Assertions) :-
	listFiles(Files),
	setof(C,File^Files^M^P^B^R^(member(File,Files),atom(File),(predicate_property(M:P,file(File));predicate_property(M:P,dynamic)),clause(P,B,R),C = :-(P,B)),Cs),
	setof(Output,Clause^Cs^Term^Body^Head^(member(Clause,Cs),term_contains_subterm_nonvar(Term,Clause),Clause = (:-(Body,Head)),((Head = true) -> (Output = Body) ; (Output = :-(Body,Head)))),Assertions).

allTermAssertionsSource(Term,Results) :-
	listFiles(Files),
	setof([C,File],File^Files^M^P^B^R^(member(File,Files),atom(File),(predicate_property(M:P,file(File));predicate_property(M:P,dynamic)),clause(P,B,R),C = :-(P,B)),Cs),
	setof(Output,Clause^Cs^Term^Body^Head^(member([Clause,File],Cs),term_contains_subterm_nonvar(Term,Clause),Clause = (:-(Body,Head)),((Head = true) -> (Output = [Body,File]) ; (Output = [:-(Body,Head),File]))),Results).


%% allTermAssertionsNoFactForm(Term,Assertions) :-
%% 	listFiles(Files),
%% 	findall(C,(member(File,Files),atom(File),predicate_property(user:P,file(File)),clause(P,B,R),C = :-(P,B)),Cs),
%% 	findall(Clause,(member(Clause,Cs),occurs:sub_var(Term,Clause)),Assertions).

getFullListing(Assertions) :-
	listFiles(Files),
	findall(C,(member(File,Files),atom(File),predicate_property(user:P,file(File)),clause(P,B,R),C = :-(P,B)),Cs),
	findall(Output,(member(Clause,Cs),Clause = (:-(Body,Head)),((Head = true) -> (Output = Body) ; (Output = :-(Body,Head)))),Assertions).

dump :-
	getFullListing(Assertions),write_list(Assertions).

%% allTermAssertionsTemp(Term,Assertions) :-
%% 	listFiles(Files),
%% 	findall(C,(member(File,Files),atom(File),predicate_property(user:P,file(File)),clause(P,B,R),C = :-(P,B)),Cs),
%% 	findall(Output,(member(Clause,Cs),Clause = (:-(Body,Head)),((Head = true) -> (Output = Body) ; (Output = :-(Body,Head)))),Assertions).

%% get_all_terms_with_leading_predicate(Predicate,Matches) :-
%% 	findall(C,(predicate_property(user:P,file(_)),clause(P,B,R),C = :-(P,B)),Clauses),
%% 	findall(Term,(member(Clause,Clauses),nonvar(Clause),Clause =.. [Predicate|B]),Matches).

get_all_subterms_with_leading_predicate_orig(Predicate,Matches) :-
	findall(C,(predicate_property(user:P,file(_)),clause(P,B,R),C = :-(P,B)),Clauses),
	findall(Term,(member(Clause,Clauses),occurs:sub_term(Term,Clause),nonvar(Term),Term =.. [Predicate|B]),Matches).

%% get_all_subterms_with_leading_predicate(Predicate,Matches) :-
%% 	findall(C,(predicate_property(user:P,file(_)),clause(P,B,R),C = :-(P,B)),Clauses),
%% 	findall(Term,(member(Clause,Clauses),term_contains_subterm(Term,Clause),nonvar(Term),Term =.. [Predicate|B]),Matches).

get_all_subterms_with_leading_predicate(Predicate,Matches) :-
	findall(C,(predicate_property(user:P,file(_)),clause(P,B,R),C = :-(P,B)),Clauses),
	findall(Term,(member(Clause,Clauses),occurs:sub_term(Term,Clause),nonvar(Term),((compound(Term)) -> (compound_name_arguments(Term,Predicate,B)) ; (Term = Predicate))),Matches).

%% %% <borrowed system="LogicMOO" from="/var/lib/myfrdcsa/sandbox/logicmoo-20141002/logicmoo-20141002/src/logicmoo_util/logicmoo_util_strings.pl">

%% % ===========================================================
%% % CASE CHANGE
%% % ===========================================================

%% noCaseChange(NC):-noCaseChange(NC,_),!.
%% noCaseChange([],[]):-!.
%% noCaseChange(VAR,VAR):-var(VAR),!.
%% noCaseChange(MiXed,MiXed):-atom(MiXed),atom_concat('#$',_,MiXed),!.
%% noCaseChange(c(VAR),c(VAR)):-!.

%% toUppercase(MiXed,MiXed):-noCaseChange(MiXed),!.
%% toUppercase(V,V2):-string(V),!,atom_codes(V,VC),toUppercase(VC,CVC),string_to_atom(V2,CVC),!.
%% toUppercase(95,45):-!.
%% toUppercase(I,O):-integer(I),!,to_upper(I,O).
%% toUppercase([A|L],[AO|LO]):-
%%    toUppercase(A,AO),!,
%%    toUppercase(L,LO),!.
%% toUppercase(MiXed,CASED):-atom(MiXed),upcase_atom(MiXed,CASED),!.
%% toUppercase(MiXed,CASED):-atom(MiXed),!,
%%    atom_codes(MiXed,Codes),
%%    toUppercase(Codes,UCodes),
%%    atom_codes(CASED,UCodes),!.
%% toUppercase(MiXed,CASED):-compound(MiXed),MiXed=..MList,toUppercase(MList,UList),!,CASED=..UList.
%% toUppercase(A,A).

%% toLowercase(MiXed,MiXed):-noCaseChange(MiXed),!.
%% toLowercase(V,V2):-string(V),!,atom_codes(V,VC),toLowercase(VC,CVC),string_to_atom(V2,CVC),!.
%% toLowercase(95,45):-!.
%% toLowercase(I,O):-integer(I),!,to_lower(I,O).
%% toLowercase([A|L],[AO|LO]):-
%%    toLowercase(A,AO),!,
%%    toLowercase(L,LO),!.
%% toLowercase(MiXed,CASED):-atom(MiXed),downcase_atom(MiXed,CASED),!.
%% toLowercase(MiXed,CASED):-atom(MiXed),!,
%%    atom_codes(MiXed,Codes),
%%    toLowercase(Codes,UCodes),
%%    atom_codes(CASED,UCodes),!.
%% toLowercase(MiXed,CASED):-compound(MiXed),MiXed=..MList,toLowercase(MList,UList),!,CASED=..UList.
%% toLowercase(A,A).

%% toPropercase(VAR,VAR):-var(VAR),!.
%% toPropercase(Left,VAR):-nonvar(VAR),!,toPropercase(Left,New),!,VAR=New.
%% toPropercase([],[]):-!.
%% toPropercase([CX|Y],[D3|YY]):- must_det(toPropercase(CX,D3)),must_det(toPropercase(Y,YY)).
%% toPropercase(MiXed,UPPER):-compound(MiXed),MiXed=..MList,toPropercase(MList,UList),!,UPPER=..UList.
%% toPropercase(D3,D3):-not(atom(D3)),!.
%% toPropercase('',''):-!.
%% toPropercase(A,U):-atom_length(A,1),toUppercase(A,U),!.
%% toPropercase('_','_'):-!.
%% toPropercase('_','-'):-!.
%% toPropercase(D3,DD3):- camelSplitters(V),concat_atom([L,I|ST],V,D3),L \='',I \='',toPropercase([L,I|ST],LIST2),toPropercase(V,VV),concat_atom(LIST2,VV,DD3),!.
%% toPropercase(CX,Y):- name(CX,[S|SS]),char_type(S,to_lower(NA)),name(NA,[N]),name(Y,[N|SS]),!.
%% toPropercase(A,A).


%% toCamelcase(VAR,VAR):-var(VAR),!.
%% toCamelcase([],[]):-!.
%% toCamelcase([CX|Y],[D3|YY]):-!,toCamelcase(CX,D3),toCamelcase(Y,YY).
%% %% toCamelcase(D3,DD3):-atom(D3),camelSplitters(V),concat_atom([L,I|ST],V,D3),toCamelcase([L,I|ST],LIST2),toCamelcase(V,VV),concat_atom(LIST2,VV,DD3).
%% toCamelcase(CX,Y):-atom(CX),name(CX,[S|SS]),char_type(S,to_upper(NA)),name(NA,[N]),name(Y,[N|SS]),!.
%% toCamelcase(MiXed,UPPER):-compound(MiXed),MiXed=..MList,toCamelcase(MList,UList),!,UPPER=..UList.
%% toCamelcase(A,A).

%% %% </borrowed>

%% %% camelSplitters(V):-member(V,[' ','-','_',':','mt','doom','Mt','Doom']).


%% (defun kmax-decamelcase (string)
%%  (join " " (mapcar #'capitalize (kmax-decamelcase-into-list string))))

capitalize(WordLC, WordUC) :-
	atom_chars(WordLC, [FirstChLow|LWordLC]),
	atom_chars(FirstLow, [FirstChLow]),
	upcase_atom(FirstLow, FirstUpp),
	atom_chars(FirstUpp, [FirstChUpp]),
	atom_chars(WordUC, [FirstChUpp|LWordLC]),!.

kifToPrologCamelCase(Input,Output) :-
	atomic_list_concat([Head|Rest],'-',Input),
	downcase_atom(Head,DowncasedHead),
	findall(CapitalizedWord,(member(Word,Rest),capitalize(Word,CapitalizedWord)),RestWords),
	append([DowncasedHead],RestWords,Words),
	atomic_list_concat(Words,'',Output).

prologCamelCaseToKIF(Input,Output) :-
	decamelcaseIntoList(Input,List),
	findall(DowncasedAtom,(member(Codes,List),atom_codes(Atom,Codes),capitalize(Atom,CapitalizedAtom),downcase_atom(Atom,DowncasedAtom)),Words),
	atomic_list_concat(Words,'-',Output).

decamelcase(Input,Output) :-
	decamelcaseIntoList(Input,List),
	view([list,List]),
	findall(CapitalizedAtom,(member(Codes,List),atom_codes(Atom,Codes),capitalize(Atom,CapitalizedAtom)),Words),
	view([words,Words]),
	atomic_list_concat(Words,' ',Output).

%% decamelcaseIntoList(Input,['this','is','a','test']).

decamelcaseIntoList(Input,List) :-
	getNextWord(Input,Word,New),
	view([input,Input,word,Word,new,New]),
	decamelcaseIntoList(New,List2),
	List = [Word|List2].

decamelcaseIntoList('',[]).

%%  ""
%%  (interactive)
%%  (let ((words nil))
%%   (while (kmax-non-empty-string string)
%%    (let* ((word (kmax-get-next-word string))
%% 	  (word-length (length word))
%% 	  (string-length (length string)))
%%     ;; (see word 0.2)
%%     (unshift word words)
%%     (setq string (substring string word-length string-length))))
%%   words))

:- use_module(library(regex)).

regex_atom(Regex,Unknown,Input,OutputList) :-
	atom_string(Input,String),
	regex(Regex,Unknown,String,List),
	findall(Item,(member(Item,List),atom_string(Output,Item)),OutputList).

getNextWord(Atom,Word,NewAtom) :-
	%% regex('(.+)-(([0-9]|\\.)+[a-zA-Z]?)$', [], Atom, [Tmp1,Tmp2,_]),
	regex_atom("^([A-Za-z][a-z]*)",[],Atom,[Word]),
	atom_length(Word,WordLength),
	atom_length(Atom,AtomLength),
	view([atom,Atom,word,Word,atomLength,AtomLength,wordLength,WordLength]),
	SecondLength is AtomLength - WordLength,
	sub_atom(Atom,WordLength,SecondLength,_,NewAtom).

%% (defun kmax-get-next-word (string)
%% ""
%% (interactive)
%% (let ((original-value case-fold-search))
%%  (setq case-fold-search nil)
%%  (string-match "^\\([A-Za-z][a-z]*\\)" string)
%%  (let ((result (match-string 1 string)))
%%   (setq case-fold-search original-value)
%%   result)))

list_terms_starting_with_predicate(Predicate,Terms) :-
	findall(P,(predicate_property(user:P,file(_)),clause(P,B,R),C = :-(P,B),P =.. [Predicate|A]),Terms).

log_message(Message) :-
	getCurrentDateTime(DateTime),
	generateGlossFor(DateTime,Gloss),
	atomic_list_concat(['On',Gloss,':',Message],' ',LogMessage),
	write(LogMessage),nl,nl.

test_multiarity_predicate.
test_multiarity_predicate(_).
test_multiarity_predicate(_,_).

term_contains_subterm(SubTerm, Term) :-
	not(compound(Term)),
	SubTerm=Term.
term_contains_subterm(SubTerm, Term) :-
	compound(Term),
	compound_name_arguments(Term, SubTerm, _).
term_contains_subterm(SubTerm, Term) :-
	compound(Term),
	arg(_, Term, SubSubTerm),
	term_contains_subterm(SubTerm, SubSubTerm).

term_contains_subterm_nonvar(SubTerm, Term) :-
	not(compound(Term)),
	nonvar(Term),
	SubTerm=Term.
term_contains_subterm_nonvar(SubTerm, Term) :-
	compound(Term),
	nonvar(Term),
	compound_name_arguments(Term, SubTerm, _).
term_contains_subterm_nonvar(SubTerm, Term) :-
	compound(Term),
	nonvar(Term),
	arg(_, Term, SubSubTerm),
	term_contains_subterm_nonvar(SubTerm, SubSubTerm).


%% term_contains_subterm(SubTerm, Term) :-
%% 	SubTerm==Term.
%% term_contains_subterm(SubTerm, Term) :-
%% 	%% nonvar(Term),
%% 	compound(Term),
%% 	(compound_name_arguments(Term, SubTerm, _)) ;
%% 	(compound(Term),arg(_, Term, SubSubTerm),term_contains_subterm(SubTerm, SubSubTerm)).


term_contains_variable(1,A).

write_data_to_file(Data,Filename) :-
	open(Filename, write, S),
	write(S,Data),
	close(S).

writeq_data_to_file(Data,Filename) :-
	open(Filename, write, S),
	writeq(S,Data),
	close(S).

read_all([])     --> [].
read_all([L|Ls]) --> [L], read_all(Ls).

read_data_from_file(File,Data) :-
	once(phrase_from_file(read_all(Ls), File)),
	atom_string(Data,Ls).

typeHasMainType(consult,consult).
typeHasMainType(ensure_loaded,ensure_loaded).
typeHasMainType(flp,consult).
typeHasMainType(include,include).
typeHasMainType(use_module,use_module).
typeHasMainType(util,ensure_loaded).

getMainTypeForFilename(Filename,MainType) :-
	prolog_files(Type,Filename),
	typeHasMainType(Type,MainType).

sublist([X|_],1,1,[X]).
sublist([],_,_,[]).		% I use this one for the case bases
sublist([X|Xs],1,K,[X|Ys]):-
	K>1,
	K1 is K-1,
	sublist(Xs,1,K1,Ys).
sublist([_|Xs],I,K,Ys):-
	I > 1,
	I1 is I-1,
	sublist(Xs,I1,K,Ys).

kmax_get_index_of_first_item_in_list(Item,List,Index) :-
	nth1(Index,List,Item),!.

chomp(String,Atom) :-
	regex_atom('^(.+)\n*$',[s],String,[Output]),
	atom_string(Atom,Output).

%% allAssertedKnowledge(Assertions),write_list(Assertions).

%% allAssertedKnowledge(Assertions),length(Assertions,X),view(X).

allAssertedKnowledge(Assertions) :-
	listFiles(Files),
	setof(C,File^Files^M^P^B^R^(member(File,Files),atom(File),(predicate_property(M:P,file(File));predicate_property(M:P,dynamic)),clause(P,B,R),C = :-(P,B)),Cs),
	setof(Clause,Cs^member(Clause,Cs),Assertions).

allFacts(Facts) :-
	allAssertedKnowledge(Assertions),
	findall(Fact,
		(   member(Assertion,Assertions),
		    Assertion = (:-(Body,Head)),
		    (	(   Head = true) ->
			(   Fact = Body) ;
			fail)),
		Facts).

%% arity(P,Arity) :-
%% 	arities(P,Arities),
%% 	member(Arity,Arities).

arities(P,Arities) :-
	setof(Arity,X^Y^B^P^A^(current_predicate(X,Y:B),B =.. [P|A],length(A,Arity)),Arities).

planStepNumber(planFn(1),1).

setSingleValued(Assertion,Result) :-
	Assertion =.. [P|Args],
	reverse(Args,[Last|Rest]),
	reverse(Rest,Tmp),
	append(Tmp,[A],NewArgs),
	NewAssertion =.. [P|NewArgs],
	findall(NewAssertion,NewAssertion,NewAssertions),
	%% unassert(
	view([newAssertions,NewAssertions]).
	%% fretractall(NewAssertion),
	%% fassert('Agent1','Yaswi1',Assertion,Result).

%% predicate_has_arities(P,Arities) :-
%% 	setof(Arity,X^Y^B^P^A^(current_predicate(X,Y:B),B =.. [P|A],length(A,Arity)),Arities).

%% START: CONSIDER ADDING THESE FUNCTIONS

%% squelch(_) :-
%% 	true.

%% END: CONSIDER ADDING THESE FUNCTIONS

frdcsaElConcatDir(List,FileName) :-
	%% make this better, catching leading and trailing /
	atomic_list_concat(List,'/',FileName).

kbs2_import_export([input(Input),inputType(InputType),outputType(OutputType),results(Results)]) :-
	%% Generate it and if we have a connection to formalog use it,

	%% otherwise, write it to a file, and get the output.
	InputFile = '/tmp/kbs2-import-export-input.txt',
	OutputFile = '/tmp/kbs2-import-export-output.txt',
	writeq_data_to_file(Input,InputFile),
	atomic_list_concat([
			    'import-export -i \'',
			    InputType,
			    '\' -o \'',
			    OutputType,
			    '\' -f \'',
			    InputFile,'\' > ',
			    OutputFile
			   ],'',Command),
	shell(Command),
	read_data_from_file(OutputFile,Results).

timestamp(TimeStamp) :-
	shell_command_to_string('timestamp',TimeStamp).

shell_command_to_string(Command,String) :-
	File = '/tmp/shell_command_to_string.txt',
	atomic_list_concat(['rm -f ',File],'',Command1),
	shell(Command1),
	atomic_list_concat([Command,' > ',File],'',Command2),
	shell(Command2),
	read_data_from_file(File,String),
	shell(Command1).

shell_quote_term(Term,Atom) :-
	with_output_to(atom(Atom),write_term(Term,[quoted(true)])).

shell_command_async(Command,STDOUT) :-
	File = '/tmp/flp-util.sh',
	write_data_to_file(Command,File),
	atomic_list_concat(['/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/shell-command-async-wrapper.sh',File],' ',ShellCommand),
	view(['shell_command_async/2',ShellCommand]),
	shell_command_to_string(ShellCommand,STDOUT),
	view([stdout2,STDOUT]).

shell_command_async(Command) :-
	view(['shell_command_async/1',Command]),
	shell_command_async(Command,STDOUT),
	view([stdout1,STDOUT]).

replaceArgPos(L, I, X, R) :-
	%% this substitutes an item in a list at a specified position
	Dummy =.. [dummy|L],
	J is I + 1,
	setarg(J, Dummy, X),
	Dummy =.. [dummy|R].

repl(X, N, L) :-
	length(L, N),
	maplist(=(X), L).

replaceArgPositions(List,[Position|RestPositions],[Replacement|RestReplacements],NewList) :-
 	replaceArgPos(List,Position,Replacement,TmpNewList),
 	replaceArgPositions(TmpNewList,RestPositions,RestReplacements,NewList).
replaceArgPositions(List,[Position],[Replacement],NewList) :-
	replaceArgPos(List,Position,Replacement,NewList).

my_set_of(X,Y,C) :-
	findall(X,Y,Z),
	view([Z]),
	setof(A,(member(B,Z),nonvar(B)),C).

getFirst(Var,Query,Result) :-
	findall(Var,Query,Results),
	nth0(0,Results,Result),
	!.

executeCommandOnSystem(loginFn(UserName,System),TmpCommand) :-
	view([executeCommandOnSystem,[loginFn(UserName,System),TmpCommand]]),
	hasIPAddress(System,IP),
	atomic_list_concat(TmpCommand,'',Command),
	atomic_list_concat([
			    '/var/lib/myfrdcsa/codebases/minor/prolog-agent/scripts/remote-execution.pl -s ',
			    IP,
			    ' -u ',
			    UserName,
			    ' -c "',
			    Command,
			    '"'
			   ],'',ShellCommand),
	shell_command_async(ShellCommand).

%% executeCommandOnSystem(loginFn(UserName,System),TmpCommand) :-
%% 	view([executeCommandOnSystem(loginFn(UserName,System),TmpCommand)]),
%% 	hasIPAddress(System,IP),
%% 	atomic_list_concat(TmpCommand,'',Command),
%% 	atomic_list_concat([
%% 			    '/var/lib/myfrdcsa/codebases/minor/prolog-agent/scripts/remote-execution.pl -s ',
%% 			    IP,
%% 			    ' -u ',
%% 			    UserName,
%% 			    ' -c "',
%% 			    Command,
%% 			    '"'
%% 			   ],'',ShellCommand),
%% 	shell(ShellCommand).

executeCommandOnSystem(loginFn(UserName,System),TmpCommand,Result) :-
	view([executeCommandOnSystem(loginFn(UserName,System),TmpCommand,Result)]),
	hasIPAddress(System,IP),
	atomic_list_concat(TmpCommand,'',Command),
	atomic_list_concat([
			    '/var/lib/myfrdcsa/codebases/minor/prolog-agent/scripts/remote-execution.pl -s ',
			    IP,
			    ' -u ',
			    UserName,
			    ' -c "',
			    Command,
			    '"'
			   ],'',ShellCommand),
	shell_command_to_string(ShellCommand,Result).

%% executeCommandOnSystemAsync(loginFn(UserName,System),TmpCommand) :-
%% 	view([executeCommandOnSystemAsync(loginFn(UserName,System),TmpCommand)]),
%% 	hasIPAddress(System,IP),
%% 	atomic_list_concat(TmpCommand,'',Command),
%% 	atomic_list_concat([
%% 			    '/var/lib/myfrdcsa/codebases/minor/prolog-agent/scripts/remote-execution.pl -s ',
%% 			    IP,
%% 			    ' -u ',
%% 			    UserName,
%% 			    ' -c "',
%% 			    Command,
%% 			    '"'
%% 			   ],'',ShellCommand),
%% 	shell_command_async(ShellCommand).

hasTruthValue(Expression,Value) :-
	(   Expression -> (Value = true) ; (Value = fail)),
	view([expression,Expression,value,Value]).

%% seq(Start,End,List) :-
%% 	numlist(Start,End,List).

%% seq(Start,End,List) :-
%% 	findall(N, between(Start,End,N), List).

%% seq(Start,Step,End,List) :-
%%      %% FIXME: completed gibberish
%% 	findall(N, (between(Start,End,N),zeroP(%((N - Start),Step)), List).

:- prolog_include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/regex.pl').
:- include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/regex.pl').



