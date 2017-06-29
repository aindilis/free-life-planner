:- dynamic prolog_files/2.
:- dynamic prolog_files/3.

:- assert(prolog_files(flp,'/var/lib/myfrdcsa/codebases/minor/free-life-planner/free_life_planner.pl')).

:- ensure_loaded('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/util/util.pl').

:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/multifile-and-dynamic-directives.pl').
%% :- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/calendaring/calendaring.pl').
:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/calendaring/calendaring-new.pl').
:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/flux/swipl_flux/flux_swi.pl').
:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/data.pl').
:- prolog_consult('/var/lib/myfrdcsa/codebases/internal/cso/comprehensive-software-ontology.pl').

:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/t/test.pl').

:- log_message('DONE LOADING FREE-LIFE-PLANNER.').

%% getCurrentDateTimeForTimeZone(TimeZone,DateTime) :-
%% 	squelch(TimeZone),
%% 	DateTime = 'Mon Jan 23 11:59:49 CST 2017'.

%% PrologMUD can have timeouts set with call_with_depth_limit/3  call_with_timelimit/2

:- module(user).
