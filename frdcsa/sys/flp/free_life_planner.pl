:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/admin.pl').

:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/free_life_planner_flux.pl').
:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/logic.pl').

:- prolog_use_module('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/fluents.pl',[fluents_for_fluents/1]).
% :- prolog_use_module('other-state',[fluents_for_other_state/1]).

:- include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/fluents_data.pl').
:- prolog_include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/fluents_data.pl').

:- include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/hours.pl').
:- prolog_include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/hours.pl').

:- include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/addresses.pl').
:- prolog_include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/addresses.pl').

:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/free_wopr.pl').
:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/sort.pl').

:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/chore_chart.pl').
:- prolog_consult('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/guard.pl').

:- include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/window.pl').
:- prolog_include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/window.pl').
