%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% FLUX State Exporter
:- module('other-state',[fluents_for_other_state/1]).
:- include('state-exporter').
:- flp_include('state-exporter.pl').
fluents_for_other-state(AllAssertedKnowledge) :-
	pred_for_m('other-state',AllAssertedKnowledge).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fluent(1).
