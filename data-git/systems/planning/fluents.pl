%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% FLUX State Exporter
:- module(fluents,[fluents_for_fluents/1]).

:- discontiguous fluents:appointment/3, fluents:arg1Isa/2,
fluents:arg2Isa/2, fluents:arg3Isa/2, fluents:completed/1,
fluents:dateOfEvent/2, fluents:deadline/2,
fluents:dependsCapabilityOnService/2, fluents:desires/2,
fluents:desiresToWorkOn/2, fluents:event/3,
fluents:expectedTimeOfArrival/2, fluents:habitual/1,
fluents:habituals/1, fluents:hasCapability/1,
fluents:hasDistraction/2, fluents:hasFormalization/2,
fluents:hasInventory/2, fluents:hasInventory/3, fluents:hasProperty/2,
fluents:hasQuestionFor/3, fluents:hasService/2,
fluents:hasStartDate/2, fluents:hasTrackingNumber/2, fluents:log/1,
fluents:meeting/3, fluents:neg/1, fluents:questionHasNL/2,
fluents:recommendedTimeToOccurBetween/3, fluents:task/3,
fluents:taskIsAssignedByAgent/2, fluents:taskIsAssignedToAgent/2,
fluents:taskIsAssignedToSelf/2, fluents:tasksAreOfferedByAgent/2,
fluents:tasksAreOfferedToAgent/2.

:- include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/state-exporter').
:- prolog_include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/state-exporter.pl').
fluents_for_fluents(AllAssertedKnowledge) :-
	pred_for_m(fluents,AllAssertedKnowledge).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

:- include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/fluents_data').
:- prolog_include('/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/fluents_data.pl').

:- include('/var/lib/myfrdcsa/codebases/minor/resource-manager/resource_manager').
:- prolog_include('/var/lib/myfrdcsa/codebases/minor/resource-manager/resource_manager.pl').
