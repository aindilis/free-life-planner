formalizes('/var/lib/myfrdcsas/versions/myfrdcsa-1.0/codebases/minor/free-life-planner/data-git/systems/library/religion/christianity/en.wikipedia.org/wiki/Works_of_mercy').

sometimesKnownAs(worksOfMercy,actsOfMercy).

includesButIsNotLimitedTo(corporalWorksOfMercy,
			  [
			   feedTheHungry,
			   giveWaterToTheThirsty,
			   clotheTheNaked,
			   shelterTheHomeless,
			   visitTheSick,
			   visitTheImprisonedOrRansomTheCaptive,
			   buryTheDead
			  ]).

includesButIsNotLimitedTo(spiritualWorksOfMercy,
			  [
			   instructTheIgnorant,
			   counselTheDoubtful,
			   admonishSinners,
			   bearPatientlyThoseWhoWrongUs,
			   forgiveOffenses,
			   consoleTheAfflicted,
			   prayForTheLivingAndTheDead
			  ]).


%% implies(feedTheHungry(Agent1),
%% 	(findall(Agent,hungry(Agent),TheHungry),member(Agent2,TheHungry),feed(Agent1,Agent2))).

%% and(
%%     obligatedTo(Agent1,),
%%     obligatedTo(Agent1,(Agent1,Agent2)),
%%     obligatedTo(Agent1,(Agent1,Agent2)),
    
%%     ) :-
%% 	claims(Agent1,isa(Agent,christian)).
