hasCapability(freeLifePlanner,systemsIntegration).
hasCapability(cyc,systemsIntegration).

addTestCase('/var/lib/myfrdcsa/codebases/internal/cso/data-git/comprehensive-software-ontology.flr').
addTestCase('/var/lib/myfrdcsa/codebases/internal/cso/comprehensive-software-ontology.pl').

addFeature('what about variable deadlines, such as, when this package arrives').
addFeature('remind us to check up on packages that have been shipped within a reasonable time frame').
addFeature('have almost everything editable from the web interface').

hasUseCases(freeLifePlanner,['surviving a war']).

lifePlannerLifeImprovementIntakeQuestion('Are you listening to books at night via computer?').

allowed(visit(andrewDougherty,facebook),between([2016-07-09,12:0:0],[2016-07-09,17:0:0])).

socialRules('if someone is doing a favor for you, and you need to ask them to do another, wait to ask them').

featureRequest(featureRequest1,'make webbased reward system',critical).

periodically(sync([devFreeLifePlannerOrg,freeLifePlannerOrg])).

suitableRewardsFor(andrewDougherty,[postStatusUpdateTo(facebook),listenToShuffledPlaylistOneTime,listenToOneSong,listenToShuffledPlaylistForDuration]).

desires(andrewDougherty,obtainPaper('Automated Deductive Synthesis of Lisp Programs in the System ALISA')).

addFeature('for every financial problem a person has, there may be a social service associated with that that we could find').
addFeature('have modes for recovering from disturbances, SOPs').
addFeature('have it print out when the FLP is done loading so its clear').

%% We have to figure out the conflict between the dentist appt at 5 and the AI meeting in Chicago today the 22nd of sept. 2016

%% Need to fix the use of deadline for an appointment, doesn't make sense.

eventuallyAddResourceToFLP(fileFn('/home/andrewdo/sdf/freeshell.pl')).
eventuallyAddResourceToFLP('all FLP pl files in all dirs and subdirs of /var/lib/myfrdcsa/private').
eventuallyAddResourceToFLP(urlFn('http://www.kanecountyguide.org/index.php/component/cpx/?task=resource.view&id=115578&search_history_id=2754055')).
eventuallyAddResourceToFLP(fileFn('/home/andrewdo/.config/frdcsa/audience/las/talking-points/talking-points.pl')).

ask(frdcsaAgentFn(freeLifePlanner),andrewDougherty,haveYouEnsureThatThisResourceIsLoadedByTheFLPOrOtherwiseIntegrated(Resource)) :-
	eventuallyAddResourceToFLP(Resource).

%% (add logic To FLP
%%  task(Task),
%%  preferredTimesToBeReachedByPhone(lelandAromin,[weekdays(between([8:0:0],[9:0:0])),weekdays(between([14:0:0],[18:0:0]))]),
%%  urgent(Task) -> okayTo(call) ; call(later)).
%%  )

%% hasResponsibility(tenantsFn(House),maintain(carpetInHouseFn(House))).

defined_in(Predicate,File) :-
	predicate_property(Predicate,file(File)).

%% checkConsistencyOfKnowledgeBase().  %% check that all argIsa are in
%% fact correct