(
 log(iSatisfiedRequirementsFor(SubHabit,[Y-M-D],[Y-M-D,H-M-S])) :-
 genls(SubHabit,habit),
 julian:form_time([now,[Y-M-D,H-M-S]]).
 )
