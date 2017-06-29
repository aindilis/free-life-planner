(07:06:44 <aindilis> dmiles: I wrote the program for translating between nested
      Prolog terms and logic forms.
07:07:38 <dmiles> which dirrection?
07:07:47 <aindilis> just finished it.  I'm not sure it will be suitable, but I
      believe PDDL and FLUX are limited to Fluents, which are nonnested.
      terms -> LFs
07:08:20 <aindilis> I need to fix it up of course.  It's really ugly but can
      always be rewritten later.
07:10:19 <dmiles> i've stopped worring about ugly 
07:10:40 *** minsky (minsky@231-242-183-107.Static.PanicBNC.us) has quit: Ping
      timeout: 258 seconds
07:11:01 <dmiles> oh cool.. terms to LF would be nice for times we are trying
      to show the user what prolog has in it
07:11:32 <aindilis> %% swipl -s convert.pl -g
      'convert(a(b(c,d),e,f(g,h,i(j,k,l(m,n)))),1,Result),see(Result)'
07:11:40 <dmiles> i like the idea that if a person is working from LF that
      they get to see LF as well
07:12:45 <aindilis> %%
      and(a(V1_1,V1_2,V1_5,V1_6),b(V1_2,V1_3,V1_4),c(V1_3),d(V1_4),e(V1_5),f(V1_6,V1_7,V1_8,V1_9),g(V1_7),h(V1_8),i(V1_9,V1_10,V1_11,V1_12),j(V1_10),k(V1_11),l(V1_12,V1_13,V1_14),m(V1_13),n(V1_14)).
07:14:24 *** nomicflux (~nomicflux@cpe-174-103-183-220.wi.res.rr.com) has
      joined channel ##prolog
07:14:48 <dmiles> is g V1_6 or V1_7?
07:15:03 <aindilis> V1_7
07:15:14 *** minsky (minsky@231-242-183-107.Static.PanicBNC.us) has joined
      channel ##prolog
07:15:18 <aindilis> the first arg is a "self"
07:15:54 <dmiles> ah... i see yeah f/3 is now f/4 .. i like that btw
07:18:20 <aindilis> still need to handle a(B), also, need to convert between
      'V1_3' and V1_3 (atom to var), have modes where f(g(h)) is represented
      and(f(V1_1,V1_2),g(V1_2,h)). and so on
07:19:29 <aindilis> also handle []
07:20:24 <aindilis> also need to know if it will have any use in PDDL, cause I
      can't think it all the way through
07:20:42 <aindilis> just going to start playing around and see if I can make
      use of it
07:21:01 <aindilis> maybe try to implement something of an interpretter in
      pddl or flux
##prolog> )

(? means: This variable can be either instantiated or not. Both ways are possible.
 + means: This variable is an input to the predicate. As such it must be instantiated.
 - means: This variable is an output to the predicate. It is usually non-instantiated, but may be if you want to check for a specific "return value".
 )

(it is logistics on top of logistics.  it is thinking up every
 contigency in advance and ...)

(could in theory parse all files first for predicates, and then
 write and load a file declaring them (if they haven't been
 already) discontiguous or dynamic or what not, before loading
 them)

(sometimeBetween([2017-01-05,11:54:22],[2017-01-05,11:54:23]))

(need to have the ability to say that an appointment is going to
 be schedule between a certain time, so we have to keep that
 empty during planning, usually)

(wopr should plan for what if someone leaves early during an
 appointment or such)
(make appointments and other things be able to have durations)

(make it so that when we query for an atom (such as with
 constant_apropos) it doesn't create it thereafter)

(we might not know when something like going to sleep is going to
 happen, but we can put a high confidence interval, and plan with
 different incremements)

(integrate the ability to load PrologMUD back into the main FLP,
 and load it in order to do things like parse sentences from
 texts for formalization)

(need to get this to work:
 search_sentences_of_text(<BOOKIDENTIFIERCONSTANT>,<SEARCH>,Sentences).
 (wtf
  (Prolog?: 
   > search_sentences_of_text(<BOOKIDENTIFIERCONSTANT>,<SEARCH>,Sentences).
   Warning, sender <<<Agent1>>> not using updated protocol
   $VAR1 = bless( {
                 'DataFormat' => 'Perl',
                 'Receiver' => 'temp-agent-0.489693685059564',
                 'MyXMLDumper' => bless( {
					'perldata' => {},
					'xml_parser_params' => {},
					'xml' => {}
					}, 'XML::Dumper' ),
                 'Data' => {
		 '_AgentName' => 'Agent1',
		 '_FormalogName' => 'Yaswi1',
		 'Result' => [
			      'error',
			      'instantiation_error',
			      [
			       'context',
			       [
				':',
				'system',
				[
				 '/',
				 'atom_string',
				 2
				 ]
				],
			       \*{'::?122'}
			       ]
			      ]
		 },
                 'ID' => '0',
                 'Date' => 'Tue Oct 25 13:56:32 CDT 2016',
                 'Contents' => '',
                 'Sender' => 'Agent1'
		 }, 'UniLang::Util::Message' );
   Prolog?: 
   > )
  ))

(on columcille
 (fix
  (http://127.0.0.1:3000/frdcsa/planning_systems/conditional_plans)
  (depends
   (http://127.0.0.1:3000/frdcsa/planning_systems/dates)
   (install julian))
  (progn
   (boss updatelinks formalog)
   (boss updatelinks guardian)
   )
  
  )
 )

(figure out how to assert prolog rules into the free life planner
 as it is running, ie. something like:
 assert(:-(cap(Pattern),findall(Result,constant_apropos('has',Result),Results),see(Results))).)

(Sign up for an account on here and you can get your social
 security information without having to go in and wait in line:
 https://www.ssa.gov/myaccount/)

(We're going to have to figure out what to do when the model
 exceeds the available RAM.  we can have loadable and unloadable
 modules.  we can have distributed systems.  etc.  Look into what
 is already done with this for prolog.)

(add info about what to shred and what to keep to FLP,
 via paperless-office)

(FIXME: http://dev.freelifeplanner.org/frdcsa/planning_systems/dates
 2016-7-8,12:0:0 (in 2 days)
 task(event2,decideOnTimeToMeet(andrewDougherty,jessBalint))
 task(event2,null))

(add a sanity checker for dates to make sure the activity is
 appropriate to the time, so that if there is an error, such as
 entering 4:0:0 when 16:0:0 is meant.)

(Put conditionals in life planner, if you haven't heard back from
 so and so by sunday, do X)

(when it's time to send the message, send with: echo "Reminder:
 You have an appointment at 6:30 PM on Tuesday September 15th,
 2016 with Dr. So and So..  Click here to acknowledge:
 http://freelifeplanner.org/link" | mail -r <EMAILADDRESS>
 <SMSRELAYEMAILADDRESS>)

(For the next part, have it text a link along with a message,
 using that method of texting we had.  With the link, have it
 prompt the user for all the stuff.  The message in the text
 should indicate the upcoming event and so on.  Don't text for
 events super far in the future, prefer instead to remind them to
 log in and check their events.  Only if the event far in the
 future has tasks whose critical path backs up, although keep in
 mind that maybe such tasks exist and we don't have them stored
 yet in the system.  Maybe this is like working with partial
 information games.)

(have an action type in the free life planner which responds to
 events.  like if internet isn't turned on by 9:00 am, call
 Comcast)

(as part of the audience subsystem, have it keep a record of the
 status of all asked questions to and from others.)

(Upgrades to life planner

 (have (contextFn ?X ?Y ?Z ...) so that we can do complex
  contexts without filling up the FreeKBS2 Context list)

 (convert it from asserting things into the KB into using the
  state variables)

 (make a web-based suppositional reasoner using flux state, gdl
  or what not)

 (create a thing of upcoming deadlines for us to keep an eye on,
  like a calendar, but better, with failure conditions in case we
  don't including costs, etc.  Use FLUX nondeterminism to plan
  for contingencies, get suppositional reasoner going and have
  export to it for reasoning with.  Finish life planner)

 (Add a life planner rules editor, so that people can edit, in
  English, various rules.  Have them mined potentially from texts
  and have them edit them.  Have the ability to submit texts for
  analysis to the life planner.)

 (Make it so that it has a log-in for anyone who wishes to use
  it, based on lists of who I know exists and such, send them
  email with credentials to their account.)

 (Make everything classified by default and have a
  declassification program written in prolog/perl.)

 (Add default due dates.)

 (Create a KB Editor and Prolog Rule/Fact Editor - make them
  reusable)

 (Need to enforce good boundaries)

 (Add to the free life planner a urls to visit as plan steps, and
  what to do at each of those.  use c->uri_for or something
  wrapping it to generate the appropriate url)

 (Akahige
  (add the ability to add a new question for any of the doctors)
  (add the ability to record the answer to a question, thus
   removing the question from the pending questions list)
  )

 (Gourmet/WSM
  (add the ability to request a new food pantry inventory item)
  (add the ability to say that we've found a new product acquired that satisfies a particular inventory requirement)
  (add the ability to add a new instance of that product to our
   inventory, and to make notes as to the condition of that
   individual item)
  )

 (Spending
  (add the ability to log loans and gifts, also expenses)
  )

 (Inventory Manager
  (have an inventory interface which lists types of objects and
   instances of those types, with the ability to increment and
   decrement count etc.)
  (Allow to list all the objects in a tree view consisting of
   containers and supporters such as rooms, tables, boxes and bags))
 
 (Add a QnA for asking people questions, e.g. Doug Miles: do you
  know of a way to do microtheories using modules)

 (Add the ability for a UniLang like (but private) system for
  routing messages to the appropriate agents.  Maybe have a
  completion grammar as with Dasher.)

 (Add a system for asserting appointments and tasks with
  deadlines - have it be able to edit the task/appointment as
  well, using an edit all kind of feature.)
 
 )

(see
/var/lib/myfrdcsa/codebases/minor/utility-maximization-system/data
and action-planner stuff on food-security etc)

(write tests to ensure that everything is loaded and functional)

(use qsave to save the whole thing and load in seconds with a
 qlf, and then use an updater that updates any changes.  This
 makes it releasable.)

(use this, if possible, to convert things to integers for CLPFD
 etc for FLUX
 /var/lib/myfrdcsa/codebases/minor/free-life-planner/doc/serpro.pl)

(should know typically where I store certain items from history,
 and be able to suggest locations for them defeasibly)

(as part of the pantry manager, have a system for estimating
 based on historical trends and different factors what remains of
 the inventory.)

(there are many todo items for the free life planner in the main
 ~/to.do file)

(noted elsewhere
 (progn
  (completed (add better handling of logic of recurring tasks for
	      planForContigency(neg(eventually(completed())))))
  (add multiuser)
  (add interface for doing multiple actions on individual
   assertions, both a low level admin and high level user
   interface)
  (remove test entries from KB)
  (get non atom quoting working)
  (develop FLP with an idea to enabling SAFE)
  (add a values interface, for what we value as humans)
  (develop the ability )
  (develop a problem database)
  (move over critical tasks from development to production, devel a synchronization mechanism)) (~/to.do))

(put the full force of the FRDCSA as an agent at the disposal of
 the life planner, within reason for security)

(Make use of mechanical turk for sourcing free life planner
 logic)

(reread normal form documentation)

(formalize the way Flora2 interfaces with FLP and Formalog)

(the kmax-command-log should be capable of scrolling up to view
 it instead of jumping to the end. should use save-excursion or
 something to jump to the end.)

(noted elsewhere (the situated-agent/packager-agent for emacs
 usage from kmax should be able to do Perl's Expect like
 reasoning from the CLI as input to FLUX for decision making.
 For instance, it should conceivably be able to execute commands,
 hrm forget which) (packager-agent))

(make lookahead user configurable, etc)
(when browsing the calendar, have it do a query for just that
 month's date ranges for recurrences)

(have it store the previous window configuration to register if
 it doesn't already when it loads these window configurations.
 write a general protocol for handling loading window
 configuration, and easy to remember key-bindings for loading
 others, and reloading what was there previously, like a kill
 ring for window configurations)

(Account for items that are exact repeats (and also inexact
 repeats) in multiple sources, such as unilang and to.do files)

(noted elsewhere
 (WORK ON THE FOLLOWING:
  (all previous life planner desiderata)
  (having the ability to assert into KBS2::Reasoner::Prolog)
  (making packager agent package files. have it have the ability
   to execute linux commands)
  (kmax-start-kmax-command-log-mode for programming by
   demonstration, situated agent attempt controlling Emacs)
  (make KBFS have persistence, and work against git)
  (extend NLU notes-axiomatized.pro.  see UIMA Prolog mode)
  (process the .bash_history using the bash grammar and Prolog
   NLP, and use NLU to represent it and generate it to and from
   the KB, using a planning process.  see older book on prolog
   planning, try to get code from it.)
  (finish recurrences, and have it plan out activities to a
   certain depth, marking which ones have been generated, and which
   ones have been acted on, etc
   (implement dmiles version of skolems using thereExists)
   )
  ) (~/to.do))

(it occurs to me now that the AI system must have ways of
 extending its KB for different common things, such as for
 learning new affordances for objects.  It shouldn't be given a
 static list but a dynamic one with the means to augment it.  This
 is true for all learnable things.
 (solution
  (Maybe Cyc has some notion of learnab-{le,ility})
  (constant-apropos learnab and learnib return nothing))
 )

(In the course of my readings I came across a book on
 transactional psychotherapy, which looked at emotional games
 people play.  it was informative, but in particular their theory
 of games I thought would suit audience better than other things.
 Maybe there is a corpora for pscyhoanalyzed speech transcripts,
 which we could use to train such a system.  Still it seemed
 silly the way in which in another book on pscyhoanalysis the
 content was fixed and always a function of the dream and the
 person's history and not the person themself, who might have a
 different theory.  It seemed dictatorial in that regard.)

(develop a system which summarizes and records the state of the
 system, and the priorities it has, at each point, in order to
 know what used to be important so we can know what should be
 important in the future.)

(add the capability to search deal sites for items that match
 shopping list items and provide notifications when there is a
 good match)

(have an accounting for all the facts in different queries and
 find out which facts aren't being utilized or fully utilized.)

(use the inventory manager to denote which of other peoples stuff
 is with us and to ask them to pick it up)

(write an SKSI system for accessing FreeKBS2 from within Cyc)

(track in the system all the calls and verify their usage of
 secure instances)

(write a function which allows me to search for a predicate using
 completion or some other methods, and then lists the files that
 define or contain that predicate.)

(xdg-open /var/lib/myfrdcsa/codebases/minor/free-life-planner/data/12799150_10206183076956265_3729390916409242300_n.jpg)

(integrate ball in court)

(develop a file which lists which predicates belong to which files)

(write a system for slurping to.do and notes.notes files into the
 free life planner.  for instance could load this file
 /var/lib/myfrdcsa/codebases/minor/frdcsa-2.0/requirements.notes
 as a bunch of assertions
 requirementForProject(frdcsa2,task('talk to Jess/Duke about
 ideas')) or something.  also have it load kbs files like this:
 /var/lib/myfrdcsa/codebases/minor/software-ontology/project-information/desired-capability.kbs
 )

(
 sub FetchUserActualName {
 my ( $self, $c ) = @_;
 my $result = $c->model( 'FRDCSA' )->Query
 (
  $c,
  Eval => [
	   ['_prolog_list',
	    Var('?Agent'),
	    ['catalystUserNameResolvesToAgent',$c->user->username,Var('?Agent')]
	    ],
	   ],
  );
 if (exists $result->[1]) {
 return $result->[1];
 }
 }
 )

(<pre>
 Add Honey Do List Here...

 Assign Task to (make a drop down here of all the regular contacts of a
		 person).

 Also come up with deadline information for the task.

 Hard or soft deadline.  Explanation of what these are.

 Look into information on organization and management.
 </pre>
 )

(make an inventory type a collection of all instances of that
 inventory item)

(start coming up with a documentation predicate, which explains things)

(have to fix our declassification procedure because it doesn't
 allow scry to see the results for this unless Season is made
 ground (because it's binding to something in the
	 isDeclassified) :
 
 ask(freeLifePlanner,andrewDougherty,hasSeason(Item,Season)) :-
 hasShoppingListItem(Person,Item).)

(shopping rules
 (prioritize items that pertain to the upcoming season,
  i.e. rocksalt for winter, if needed) 
 (have something that juggles deals with needs)
 (tag luxury items as such, i.e. oral-b toothbrush)
 (make shopping analysis start now, rather than reviewing
  existing items, so that it actually gets done.)  

 )

(detect if the user is using an android browser and if so include this for the possibility of scanning things:
 (zxing://scan/?ret=http://foo.com/products/{CODE}/description&SCAN_FORMATS=UPC_A,EAN_13)
 (zxing://scan/?ret=[%- c.uri_for('scan/{CODE}/description&SCAN_FORMATS=UPC_A,EAN_13') -%])

 )

(completed (add a debug feature which allows us to see the $c and so on))

(integrate paperless office)

(Develop audience module for logging phone calls to people, what
 is the force of the communication, what is the result, etc.
 have a plan ready for all communications or most.)

(have a whereis functionality that we can search for files and
 stuff that we need for the life planner.  whereis atom, whereis
 fact, whereis rule (regex-search atom/fact/rule) context search
 etc)

(ask Questions of ##prolog
 (rather create a log in and have the questions there, in audience)
 (how to check which file a particular rule/fact comes from
  %% predicate_property(Predicate, imported_from(Module))
  setof(Module,Predicate^predicate_property(Predicate, imported_from(Module)),Result),see(Result).
  )
 (how to list which files have been loaded)
 (I use a lot of perl programs so all my programs end in .pl.
  I'd like to differentiate them.  should I label them .P or .pro
  or something? or .swipl  Plz advise.)
 )

(rename everything to .pro or something so that it's more easily
 understandable, can differentiate with a locate. (frdcsa.flp? -
 seems flora2 already uses this extension))

(figure out which file contains the stuff for resources)

(come up with a set of features)

(have it be aware of what files it is loading)
(make an integrated search faculty)

(add multiuser support)

(add to audience the ability to verify the state of mail.  tie
 into paperless office.  Have a paperless office module for Free
 Life Planner.)

(in progress (make a dashboard for audience))

(make an NLU interface for editting annotations of a document)

(make a unilang interface, for adding new unilang messages, for
 searching existing ones, etc)

(completed (rename all the assertedTask1 items to differentiated
 names i.e. assertedTask135 etc))

(make the life planner https)

(create a realistic 3d model of the house, with the persons
 location, and relative locations and sizes etc of all the items)

(make a simulated house with connections between the house, that
 is navigable in the editor)

(create an interface that represents a particular room, with
 affordances.  for instance, if the room is a bathroom, make it
 easy to record that the room is getting low on handsoap)

(integrate the planning domain with the world state monitor)

(after putting a load into the laundry, put the load over later)

(after going to check the email, enter the data into Audience
 about who sent what messages)
(check email messages into audience - annotate what they require
 us to do)

(Create a standard template system for tasks and events and such
 that gives the ability to edit them and so on, with affordances
 from the object affordance system.)

(figure out what is causing it to have difficulty restarting)

(use a link checking program to make sure everything works)

(incorporate a thorough system of self-documentation and a help
 system looking up answers plus ways to record complaints about
 the way the system works to propagate to us so we can better
 implement it)

(add a rule to the life planner when purchasing cell phone and
 laptop batteries that there are many fakes from China and such.)

(when I start working on Free Life Planner, work on
 prioritize-features (see its to.do))

(solution (find the code for tab completion of atoms in FLP,
 change to use current_atom/1) (mapped to C-c f f t, but make it
 so that it also does atoms, and can restrict only to predicates
 that we have defined.))

(create MVC facsimile in Prolog)

(add feature to prioritize features to record comparisons, and
 reload when reviewing or adding new ones)

(have the system track the variable names

 (findall(Result,(current_predicate(_,M:P),catch(clause(M:P,Result),_,true)),Z),see(Z).)
 (with_output_to(string(Str), listing(member/2)),term_string(Term, Str).)

 (as this example shows they are helpful for understanding the assertions

  (implies
   (nthLargestElement ?DUR 1
    (TheSetOf ?DIFF
     (thereExists ?DATE1
      (thereExists ?DATE2
       (and
	(evaluate ?DIFF
	 (TimeElapsedFn ?DATE1 ?DATE2))
	(thereExists ?Y
	 (thereExists ?Z
	  (and
	   (isa ?Y ?COLLECTION)
	   (isa ?Z ?COLLECTION)
	   (dateOfEvent ?Y ?DATE1)
	   (dateOfEvent ?Z ?DATE2)
	   (laterThan ?DATE1 ?DATE2)
	   (unknownSentence
	    (thereExists ?MIDDLE
	     (thereExists ?EVT
	      (and
	       (isa ?EVT ?COLLECTION)
	       (dateOfEvent ?EVT ?MIDDLE)
	       (laterThan ?DATE1 ?MIDDLE)
	       (laterThan ?MIDDLE ?DATE2)))))))))))) AbsoluteValueFn)
   (specifiedPropertyOfCollection Longest-Between ?COLLECTION ?DUR))

  )
 )

(figure out which files have loaded which assertions.  maybe
 write our own inclusion system which wraps the others, but logs
 that info)

(add something to requestsLoanToCover to identify from whom we
 are asking, for instance, some people you would not ask for
 certain things from.  Have the life planner review this
 information, present in audience tab for the person)

(have the ability to know if a fact is inferred or asserted, etc
 (and which rules inferred it if it is inferred) from the web
 interface as well)

(have almost everything configurable from the web interface)

(have bills understand different things like who pays for certain
 bills usually, splitting bills, different properties etc)

(have the system know which facts and rules should be on an
 airgapped system and enforce that.  have it set up airgapped
 systems.)

(with these queries that go through Formalog but contain keys,
 figure out how best to do complex links and such as with object
 oriented items, and the correct mapping between Prolog and Perl
 data structures)

(reimplement Free-WOPR controller so it sends out one query to
 Guardian/Agent1 which gets generated from a predicate in FLP and
 has a hash like data structure of keys which are mapped to
 c.stash.<X> and contain the items.  So that it loads quicker
 than having two separate queries through the slow Formalog
 interface.)

(develop a sort of templating system for Prolog output using TT
 that sends data to the stash by generating prolog "hashes" which
 contain the values for the template)

(add different controls that make it easier to control, i.e. we
 don't have to write all the custom commands for Guardian, we can
 send that in from the model or the controller, depending.  add a
 view for Guardian CLI output)

(have inventory reordering, have resource-manager tab, predict
 how much left we have.)

(locate people via facebook who make good counterpoints, train a
 learning algorithm to identify good counterarguments to apply.
 E.g.  Craig A Bingham responded to a post that said:
 "Tom Brokaw: When Brussels Was Attacked, Obama ‘Stayed at the Baseball Game All Day Long’"
 with an image/meme that said
 "GUESS WHERE RONALD REAGAN WAS WHEN 241 AMERICAN SOLDIERS WERE
  KILLED IN BEIRUT" "ON VACATION AT THE AUGUSTA NATIONAL GOLF
  CLUB.  HE STAYED THERE.")

(completed(task5).

 hasCompletionTime(task5,[2016-...]).
 hasCostOfCompletion(task5,dollars(31.26)).
 costOfPlanThroughDate(plan1,[2016-...],between(dollars(340.00),dollars(360.00))).
 costOfPlanThroughStep(...).
 
 )

(have the ability to run whatIf scenarios out, such as, what if
 we stopped ordering jugs5Gallon and used tap water.  get a list
 of salient improvements/detriments)

(have the inventory manager page.  have it tell us what
 consumables we are out of and should reorder, e.g. jugs5Gallon)

(Have it have the ability to tell us, with the upcoming debts
 that we have, how much money we need to have by such and such a
 day.  have it automatically ask for loans as needed, refering to
 the page about what I need the loan for and the consequences of
 not getting the loan.  Enumerate the consequences, have
 invariants expressed in LTL/CTL* etc formulae.)

(use adminlte invoice template  for loan requests)

(For instance with putMoneyOn(debitCard), we should have a
 subsystem that looks at these and determines to what extent
 they've been grounded in the ontology.  Imagine that putMoneyOn
 is already defined in the ontology, but we're not using it
 correctly.  Or that it hasn't been defined, or that debitCard
 doesn't refer to anything.  The complete status of the
 translation from the pseudo-prolog-code into normalized/grounded
 PrologCYC/SUMO etc should be known, and an editor page in the
 system to assist with this process created.  Should probably be
 under frdcsa/codebases/internal/formalize{,2,3})

(add a feature to contigencies that let us mark them as obsolete etc)

(contingency)

(look into the temporal war-gaming Doug mentioned: this seems to
 be the planning system Douglas was referring to:
 /var/lib/myfrdcsa/repositories/external/git/PrologVirtualWorlds/JPrologCafe/testing/start_swiprolog_unixbcd.pl)
(/var/lib/myfrdcsa/repositories/external/git/PrologVirtualWorlds/JPrologCafe/testing/logicmoo_tap_plan.pl)

(((((
 channel ##prolog
11:59:00 <hppavilion[2]> I'm trying to make a Modal Logic Programming Language
      (temporal)
11:59:20 <dmiles> why is that?
11:59:26 <hppavilion[2]> But I don't know what the constraints on a temporal
      horn clause are, nor any useful algorithms for dealing with them. Can
      anyone help?
12:00:23 <hppavilion[2]> dmiles: Because it would be cool
12:01:16 <dmiles> i know a small bit about it.  I am hoping that this is a
      long term serious effort 
12:01:32 <hppavilion[2]> dmiles: I am too
12:01:42 <hppavilion[2]> dmiles: But knowing me, I'm going to get distracted
      in a week at most
12:01:48 <hppavilion[2]> dmiles: I'm compiling it, BTW :)
12:02:33 <hppavilion[2]> Because I recently started studying how compilers
      work, and it's easier than I expected (at least to compile C-like
      imperative language to my IL, which is ASMy)
12:02:41 <dmiles> compiling it is fine .. you'll probly still want it to run
      in a debugger someimes uncompiled
12:02:54 <hppavilion[2]> dmiles: Yes, I was thinking I would do that
12:03:15 <hppavilion[2]> dmiles: The AST will be interpretable AND compilable,
      which shouldn't be too much more difficult
12:04:17 <dmiles> well i'd start out witha  good constraint based horn clause
      solver impl.. then add the temoral side as 2 new arguments
12:04:49 <hppavilion[2]> Good idea
12:05:11 <hppavilion[2]> dmiles: What are the common algorithms for solving
      horn clauses?
12:05:25 <dmiles> one method i know of uses a sort of generic modality.. when
      i started adding the termporal in it came accrosed almost just like
      quantification of a diamond and box ( poss & nesc)
12:05:41 <dmiles> ?pttp
12:05:41 <yesbot> Prolog Technology Theorem Prover
12:05:42 <yesbot> http://www.ai.sri.com/~stickel/pttp.html
12:06:02 <hppavilion[2]> Oooh :)
12:06:08 <dmiles> that is a naive interative deeping method to solve horn
      clauses and present proofs
12:06:45 <hppavilion[2]> dmiles: Which TL should I use as a basis? LTL, CTL,
      or CTL*?
12:07:11 <hppavilion[2]> CTL* is the most powerful of the three AFAWPTM, but
      it'd also be the most complicated
12:07:16 <dmiles> i've always just use CTL but i want to leanr and use CTL* 
12:07:48 <hppavilion[2]> dmiles: I'm thinking of going with the Open World
      Assumption. Is that a good idea?
12:08:05 <dmiles> actualyl maybe i only used LTL.. i'll check in a bit
12:08:55 *** dos (~DevourerO@58-0-174-206.gci.net) has joined channel ##prolog
12:08:58 <dmiles> i have a tough time knowing when people say CWA vs OWA which
      they meant :)
12:09:19 *** dos (~DevourerO@58-0-174-206.gci.net) is now known as
      hppavilion[1]
12:09:43 <dmiles> i did it the simpler of the two ways a couple times and it
      turns out to do it better the correct way
12:10:03 <hppavilion[1]> dmiles: I like it, because it doesn't mean that
      something which cannot be proven is false (which is probably a good
      thing for debugging), but it also likely has some downsides (namely,
      things you want to be false have to be explicit, I think)
12:10:27 <dmiles> ah yeas then OWA it is
12:10:42 <dmiles> so i have come to a conclusion that i need to compile in OWA
      even thugh CWA seems enough
12:10:53 <dmiles> seemed*
12:11:29 <dmiles> what i mean by that is i have to  query the negation as well
      as the postive and get results back to trust either
12:12:22 *** hppavilion[2] (~DevourerO@58-0-174-206.gci.net) has quit: Ping
      timeout: 244 seconds
12:12:30 <dmiles> meaning   "dogs my dog like Alpo"  +  "is it false that my
      dog doesnt like Aplo"
12:13:32 <hppavilion[1]> Ah
12:13:33 <dmiles> well more like this:  "does my dog like Alpo?"  +  "is it
      imposible that my dog doesnt like Alpo?"
12:13:37 <hppavilion[1]> That does look inefficient
12:13:52 <dmiles> yes but in the mid term its way way more effecient
12:14:31 <hppavilion[1]> Ah
12:14:48 <dmiles> though this means you have to cache and optimize the
      imposible with lots of singlton vars
12:16:35 <dmiles> example:    impossibleDurring(Fact,When):-
      factStartsToHappen(Fact,Start),When<Start.
12:16:56 <dmiles> oops that not exmaple of singleton vars
12:17:31 <dmiles> more like:  impossible(mother(_,M)):- father(M).
12:18:14 *** Riviera- (Riviera@2a03:b0c0:1:d0::10:b001) has quit: Ping
      timeout: 260 seconds
12:19:01 <dmiles> so the way you are thinking termproal come into modally.. I
      have leanrd that posibility is just as important
12:19:25 <dmiles> to be "compiled in"
12:19:42 <dmiles> but luckily this come in the same ways
12:20:44 <dmiles> that sesc/poss logic is just as important as temporl logic
12:21:24 <dmiles> to neglect either is full of lots of dangers 
12:21:42 *** q_bert_ (~hubert@186.77.161.228) has joined channel ##prolog
12:23:29 *** q_bert (~hubert@186.77.173.246) has quit: Ping timeout: 260
      seconds
12:24:30 <dmiles> i mean it might be obvious that without OWA the system would
      be unreliable
12:25:43 <dmiles> just like the examples where the lack of termporal logic
      make a system silly and toy
12:27:28 <dmiles> a few years ago i extended pttp to deal with this data:
      https://github.com/opensim4opencog/PrologVirtualWorlds/blob/master/ontologies/kif/DesertShieldMt.kif
12:27:32 <yesbot> Title: PrologVirtualWorlds/DesertShieldMt.kif at master ·
      opensim4opencog/PrologVirtualWorlds · GitHub (3.93s)
12:27:32 <yesbot> http://tinyurl.com/h2npucw (0.26s)
12:28:57 <dmiles> as you see, quite a bit of temporal reasoning was
      required.. the system job was to predict and simulate outcomes if
      diffenrt world actors behaved ifferntly
12:30:22 <dmiles> i compiled that data to horn clauses
12:30:45 *** q_bert (~hubert@186.77.164.115) has joined channel ##prolog
12:30:50 *** Riviera- (Riviera@2a03:b0c0:1:d0::10:b001) has joined channel
      ##prolog
12:33:04 *** q_bert_ (~hubert@186.77.161.228) has quit: Ping timeout: 240
      seconds
12:33:08 <dmiles> then the i'd ask it several questions.. and it would reply
      back with results.. if the results seemed absurd.. i might teach it
      something more common sensical and ask again 
12:34:05 <dmiles> oh the replies are accompanied by "proofs" of why it can
      belive what it believed .. so sometimes one can refine the axioms at
      that level
12:35:27 *** Zekka (~zekka@ip24-251-69-145.ph.ph.cox.net) has quit: Ping
      timeout: 276 seconds
12:35:36 <dmiles> i tried the type of horn clause resolution methods they
      teach in school today and i also tried methods that Stickle used
12:36:08 *** hppavilion[1] (~DevourerO@58-0-174-206.gci.net) has quit: Ping
      timeout: 244 seconds
12:39:38 *** RLa (~RL@82.131.127.254.cable.starman.ee) has quit: Quit: Leaving
12:40:23 <dmiles> stickle compiles in "sanity clauses" .. nowadays those
      sanity clauses are left out in favor of smaller simpler (faster?)
      resolution.  but in practice the "sanity clauses" allow the system to
      avoid absurd routes and thus enable the system to search a wider range
      of possiblities and explore (and construct) more interesting conjectures 
12:41:23 <dmiles> and also deal with us humans proposed absurd routes
12:41:37 *** xyh (~xieyuheng@111.50.70.142) has joined channel ##prolog
12:42:51 <dmiles> which are usually sylligisms created by underspecification
      that comes from trying to ontologise and generalize our knowlege 
12:43:43 * dmiles knows he left but feels lucid and finaly like he knows how
      to explain the software P
12:45:32 <dmiles> "explore (and construct) more interesting conjectures " <- i
      said the software does that?  Yes, not that it is creative.. it is that
      by selecting a single axiom of choice at each step.. it might start to
      build a construction that we had not intended to be made
12:46:30 *** hppavilion[1] (~DevourerO@74-114-87-90.dynamic.asdk12.org) has
      joined channel ##prolog
12:46:52 <dmiles> hppavilion[1]: let me know the last comment by me was
12:47:00 <dmiles> (that you saw)
12:47:15 <hppavilion[1]> dmiles: "i compiled that data to horn clauses"
12:48:26 <dmiles>  then the i'd ask it several questions.. and it would reply
      back with results accompanied by "proofs" of why it can belive what it
      believed .. so sometimes one can refine the axioms at that level
12:49:18 <dmiles>  i tried the type of horn clause resolution methods they
      teach in school today and i also tried methods that Stickle used
      .. stickle compiles in "sanity clauses" .. nowadays those sanity clauses
      are left out in favor of smaller simpler (faster?) resolution.  but in
      practice the "sanity clauses" allow the system to avoid absurd routes
      and thus enable the system to search a wider range of 
12:49:19 <dmiles> possiblities and explore (and construct) more interesting
      conjectures  
12:50:00 <dmiles>  the software does that?  Yes, not that it is creative.. it
      is that by selecting a single axiom of choice at each step.. it might
      start to build a construction that we had not intended to be made
12:51:35 <dmiles> "sanity clauses" avoid these sylligisms created by
      underspecification that comes from trying to ontologise and generalize
      our knowlege 
12:52:02 *** Zekka (~zekka@150.135.210.122) has joined channel ##prolog
12:52:38 <dmiles> not only that .. some of the data/knowledge we put into a
      system might be wrong
12:53:17 <dmiles> so compiling for the OWA helps detect that
12:55:10 <hppavilion[1]> dmiles: Wait. Could we just do LTL logic programming
      by having a hidden variable for every predicate called "time"?
12:55:19 <dmiles> yes
12:55:31 <hppavilion[1]> So OOOp(x) becomes p(x, 3)
12:55:38 <hppavilion[1]> (O is NEXT)
12:55:48 *** xyh (~xieyuheng@111.50.70.142) is now known as xdf
12:56:03 <dmiles> that is probably exactly what we do.. for some reason i was
      aying 2 vars .. but 1 actualyl works.
12:56:16 <dmiles> aying/saying
12:58:13 *** xdf (~xieyuheng@111.50.70.142) is now known as xyh
12:59:37 <dmiles> here is the impl of a termproal reasoner i did..
      https://github.com/opensim4opencog/PrologVirtualWorlds/blob/dmiles/logicmoo/src/logicserver/moo_sentence.pl#L17  
12:59:38 <yesbot> Title: PrologVirtualWorlds/moo_sentence.pl at dmiles ·
      opensim4opencog/PrologVirtualWorlds · GitHub (0.91s)
12:59:40 <yesbot> http://tinyurl.com/hnnxfdz (2.14s)
13:00:19 <hppavilion[1]> dmiles: So I got it to compile expressions
      (arithmetical, that is), but I don't know the first thing to do for
      compiling relations and facts
13:00:31 *** xyh (~xieyuheng@111.50.70.142) has quit: Remote host closed the
      connection
13:00:42 <hppavilion[1]> dmiles: I'm trying to read papers on it, but I'm
      having trouble understanding them
13:00:58 <dmiles> the pttp is a good introduction to what it looks and feels
      like codewise
13:01:06 <hppavilion[1]> OK
13:01:17 <hppavilion[1]> dmiles: I'm writing the compiler in Python, BTW xD
13:01:39 <dmiles> the first i did when i got ahold of pttp.. is i converted it
      from a compiler to a interpretor :P
13:01:43 <hppavilion[1]> dmiles: Dammit, it's all PS
13:02:20 <dmiles> ghostview
13:02:42 <hppavilion[1]> dmiles: That's what I was thinking, but I don't know
      if I have it installed, and I don't feel like installing anything new
      right now. But I'll check.
13:02:50 <qu1j0t3> or, convert to pdf. OS X Preview can do it, most likely
      there's something like that on Windows.
13:02:50 <hppavilion[1]> Yep, I have it
13:03:33 *** T_K (~Tambet@82.131.60.239.cable.starman.ee) has joined channel
      ##prolog
13:04:52 <dmiles> i was thinki ng the NLTK had theorem rovers in it
13:05:00 <dmiles> provers*
13:05:46 <dmiles> funny i found some old papers/emails of my coworkers
      discussing cyc.. and we call it no more thna a theorem prover
13:06:13 <dmiles> for lack of a better name
13:06:44 <dmiles> yet it has nothing to do with mordern theorem proving :) 
13:07:43 <dmiles> what the software does is "gives a proof" based on a users
      "theory"
13:08:02 <dmiles> but in order to do that it has to theorize quite a bit for
      itself
13:08:37 <dmiles> and it constructs programs (called proofs) that make the
      theory possible
13:09:26 <dmiles> i am so confused to what software out there exists besides
      Cyc that is supposed to do the same thing
13:10:46 <dmiles> (and besides what i am trying to make  and made at my last
      URL)
13:12:04 <dmiles> the funny part is 30% of my code and impl is borrowed from
      modern theorem proving techniques 
13:12:15 <dmiles> (and Cyc's code)
13:12:33 <dmiles> (i mean Cyc's code is 30% the modern theorem provers code)
13:16:14 <dmiles> the 70% of outr codeebase are anchient theorem proving code
13:16:49 *** hppavilion[1] (~DevourerO@74-114-87-90.dynamic.asdk12.org) has
      quit: Ping timeout: 252 seconds
13:20:08 <dmiles> i guess no one is going to make this software for me
13:21:12 <dmiles> unless it is about number theory 
13:30:58 *** hppavilion[1] (~DevourerO@74-114-87-90.dynamic.asdk12.org) has
      joined channel ##prolog
ERC>
)

(get RCyc-1.0 and RCyc-4.0q running at the same time, and then
 use the RCyc-1.0 to bootstrap translations to Cyc)

(have the ability for people to suggest courses of action, and
 the AI to evaluate them)

(add something that estimates what the epistemic effect of
 omission of classified info will have in different contexts,
 such as nonmonotonic linguistic contexts)

(look into the commands that Doug told us about for generating
 English glosses)

(improve our notion of classified predicate to be classified to
 certain defined groups, such as all friends of a certain set of
 people, etc)

(implement a bunch of conditions for what happens if we don't
 maintain the resource-manager conditions - write it as flux
 state updates perhaps)

(write a function called term_contains_subterm which we can use
 to make sure that subterms don't unify with a given term, which
 we can use to screen the results of scry to prevent it from
 listing terms that have been marked as classified, etc.)

((solution (translate this to English, then bootstrap conversion
	    to Cyc ontology using RCyc 1.0:
	    unpreparedForContingency(neg(eventually(completedTask(developResponsePlanFor(whatIf(loses(andrewDougherty,eyesight))))))).) (We
	    are unprepared for the contingency that we never
	    complete the task of developing a response plan for
	    what to do if Andrew loses his eyesight.))

 (We are unprepared for the contingency that we never complete the task of developing a response plan for what to do if Andrew loses his eyesight.)

 ((#$thereExists ?WE2
   (#$and
    (#$isa ?WE2
     (#$ThingDescribableAsFn
      (#$WordFn "unprepared")
      #$Adjective))
    (#$isa ?WE2
     (#$ThingDescribableAsFn
      (#$WordFn "we")
      #$Noun))
    (#$thereExists ?COMPLETE15
     (#$and
      (#$isa ?COMPLETE15
       (#$ThingDescribableAsFn
	(#$WordFn "complete")
	#$Verb))
      (#$isa ?COMPLETE15 #$Situation)
      (#$doneBy ?COMPLETE15 ?WE12)
      (#$patient-Generic ?COMPLETE15 ?TASK19))))))

 (I am unprepared for that.)

 CYC(1): (cyclify "We are unprepared for the contingency that we never complete the task of developing a response plan for what to do if Andrew loses his eyesight.")
 ((#$thereExists ?WE2 (#$and (#$isa ?WE2 (#$ThingDescribableAsFn (#$WordFn "unprepared") #$Adjective)) (#$isa ?WE2 (#$ThingDescribableAsFn (#$WordFn "we") #$Noun)) (#$thereExists ?COMPLETE15 (#$and (#$isa ?COMPLETE15 (#$ThingDescribableAsFn (#$WordFn "complete") #$Verb)) (#$isa ?COMPLETE15 #$Situation) (#$doneBy ?COMPLETE15 ?WE12) (#$patient-Generic ?COMPLETE15 ?TASK19))))))
 (NIL) 
 CYC(2): (cyclify I am unprepared for that.)

 Error: I does not have a value.
 [Switching to single-threaded mode ....]

 Select a restart:
 0: Return to top-level read loop.
 1: Recursive read loop.
 2: Signal the debugger.
 3: Exit program.
 ? 0
 [Resuming multi-threaded mode ....]
 CYC(2): (cyclify "I am unprepared for that.")
 ((#$thereExists ?I2 (#$and (#$isa ?I2 (#$ThingDescribableAsFn (#$WordFn "unprepared") #$Adjective)) (#$isa ?I2 (#$ThingDescribableAsFn (#$WordFn "i") #$Noun)))))
 (NIL) 
 CYC(3): (cyclify "I am unprepared.")
 ((#$thereExists ?I2 (#$and (#$isa ?I2 (#$ThingDescribableAsFn (#$WordFn "unprepared") #$Adjective)) (#$isa ?I2 (#$ThingDescribableAsFn (#$WordFn "i") #$Noun)))))
 (NIL) 
 CYC(4): (cyclify "I am a dog.")
 ((#$thereExists ?I2 (#$and (#$isa ?I2 (#$ThingDescribableAsFn (#$WordFn "i") #$Noun)) (#$isa ?I2 #$Dog))) (#$thereExists ?I2 (#$and (#$isa ?I2 (#$ThingDescribableAsFn (#$WordFn "i") #$Noun)) (#$isa ?I2 #$HotDog))))
 (NIL NIL) 
 CYC(5): (cyclify "The person went to the store.")
 ((#$and (#$isa (#$InstanceFn "?PERSON4") #$Person) (#$thereExists ?WENT5 (#$and (#$isa ?WENT5 #$Movement-TranslationEvent) (#$toLocation ?WENT5 (#$InstanceFn "?STORE10")) (#$primaryObjectMoving ?WENT5 (#$InstanceFn "?PERSON4")) (#$isa (#$InstanceFn "?STORE10") #$RetailStore)))) (#$and (#$isa (#$InstanceFn "?PERSON4") #$Person) (#$thereExists ?WENT5 (#$and (#$isa ?WENT5 #$Movement-TranslationEvent) (#$toLocation ?WENT5 (#$InstanceFn "?STORE10")) (#$primaryObjectMoving ?WENT5 (#$InstanceFn "?PERSON4")) (#$isa (#$InstanceFn "?STORE10") #$RetailStoreSpace)))) (#$and (#$isa (#$InstanceFn "?PERSON4") #$Person) (#$isa (#$InstanceFn "?STORE10") #$StorageOfSomethingForFutureUse) (#$isa (#$InstanceFn "?STORE10") #$CourseOfStudy) (#$beneficiary (#$InstanceFn "?STORE10") (#$InstanceFn "?PERSON4"))))
 (NIL NIL NIL) 
 CYC(6): (cyclify "The person wasn't ready.")
 ((#$and (#$isa (#$InstanceFn "?PERSON4") #$Person) (#$not (#$isa :NOUN #<(#$ThingDescribableAsFn #$Ready-TheWord #$Adjective)>))))
 (NIL) 
 CYC(7): (cyclify "The person was not ready.")
 ((#$and (#$isa (#$InstanceFn "?PERSON4") #$Person) (#$not (#$isa :NOUN #<(#$ThingDescribableAsFn #$Ready-TheWord #$Adjective)>))))
 (NIL) 
 CYC(8): (cyclify "The plan failed.")
 ((#$and (#$isa (#$InstanceFn "?PLAN4") #$ArchitecturalPlan) (#$failureForAgents (#$InstanceFn "?PLAN4") ?AGENT)))
 (NIL) 
 CYC(9): (cyclify "The plan failed.")
 ((#$and (#$isa (#$InstanceFn "?PLAN4") #$ArchitecturalPlan) (#$failureForAgents (#$InstanceFn "?PLAN4") ?AGENT)))
 (NIL) 
 CYC(10): (cyclify "I am unprepared.")
 ((#$thereExists ?I2 (#$and (#$isa ?I2 (#$ThingDescribableAsFn (#$WordFn "unprepared") #$Adjective)) (#$isa ?I2 (#$ThingDescribableAsFn (#$WordFn "i") #$Noun)))))
 (NIL) 
 CYC(11): (cyclify "I never finished.")
 ((#$thereExists ?I2 (#$and (#$isa ?I2 (#$ThingDescribableAsFn (#$WordFn "i") #$Noun)) (#$thereExists ?FINISHED6 (#$and (#$isa ?FINISHED6 (#$ThingDescribableAsFn (#$WordFn "finished") #$Verb)) (#$isa ?FINISHED6 #$Situation) (#$doneBy ?FINISHED6 ?I2) (#$isa ?FINISHED6 (#$ThingDescribableAsFn (#$WordFn "never") #$Adverb)))))))
 (NIL) 
 CYC(12): (cyclify "The task is not complete.")
 ((#$and (#$isa (#$InstanceFn "?TASK4") #$AssignedTaskType) (#$not (#$isa :NOUN #<(#$ThingDescribableAsFn #$Complete-TheWord #$Adjective)>))) (#$and (#$isa (#$InstanceFn "?TASK4") (#$SubcollectionOfWithRelationToTypeFn #$Assignment-Specification #$performerInSpec #$IntelligentAgent)) (#$not (#$isa :NOUN #<(#$ThingDescribableAsFn #$Complete-TheWord #$Adjective)>))) (#$and (#$isa (#$InstanceFn "?TASK4") #$Undertaking-WorkingEvent) (#$not (#$isa :NOUN #<(#$ThingDescribableAsFn #$Complete-TheWord #$Adjective)>))))
 (NIL NIL NIL) 
 CYC(13): 

 ;; (#$SubcollectionOfWithRelationToTypeFn #$Assignment-Specification #$performerInSpec #$IntelligentAgent)
 ;; #$Undertaking-WorkingEvent
 )   


(alter the hostname on freelifeplanner.org's VM from
 panoply2.frdcsa.org to something like flp.frdcsa.org/or
 something less conflicting)

(solution (Develop a method whereby users can have money sent to
 them via the ShinyCMS shoppingcart, and a microloan system for
 people in impoverished countries.  Have a shopping cart utility
 for purchasing things for the user.) (use the shiny_cms
 shop_item table for this.  for recurring purchases, look into
 keeping the item in the DB.  for single purchases, look into
 pulling it from the DB as soon as complete))

(figure out how prologcyc handles the values of functions)

(implement english glosses, translate to prologcyc terms)

(see if there isn't a newer catalyst book)
(read the catalyst book)
(complete the model)
(parameterize the interface)

(make a backup of the previous version of
 free-fluxplayer/attempts/3, make the current 3 into 3b)

(the freelifeplanner should have a checkin feature where you
 check in after traveling somewhere, and if you don't it tries to
 get a hold of you and if not gets ahold of emergency contacts)

(run M-x free-life-planner from within X windows in order for
 planning to work, detach this dependency eventually)

(sudo cpanm Storable)

(add support to life planner for honey do and other lists)

(read a book on Catalyst, and Prolog)

(completed (Have the ability to add contigencies to plan for via
 the life planner.))

(do text processing of email to try to identify named entites and
 such that identifies possessions and world state, and have the
 user annotate them in the My->Self part.  Have this hook up
 through audience)

(integrate temporal planning capabilities.  make a separate
 system for that.)

(have a template for checklists and quizzes to ask the user)

(have a property for determining whether a given predicate is
 stored in flux state variable/domain or regularly included)

(reintegrate the loading of the facts)

(troubleshoot why FLUX-SWI is not working right now)

(for our controllers for existing systems, be sure to also put
 the menu item describing it in the title, so people don't become
 confused)

(we can have the life planner read out a state variable to a
 module by first clearning the module with retractall, then
 adding with asserts, and then read it out using the tools we
 written so far)

(Added extraction of fluents from module.  Be sure to also later
 get rules, as well as make the code, which I seem to have to
 insert directly into the module itself, be loadable using an
 include in the module.)

(
pred_for_m(M,IntoVar):- findall((P :- B) ,(
current_predicate(_,M:P), \+ predicate_property(M:P,imported_from(_)),
predicate_property(M:P,number_of_clauses(_)), clause(M:P,B)),IntoVar).
% the  number_of_clauses/1 wil avoid an error
)

(put reminder to vacuum floor, go to food pantry)

(life planner
 (first rule:
  (consequence of staying up too late the night before, too tired))
 (second rule: at least once a day, check the bank balance, might
  end up having less money than I think, which might cause an
  emergency)
 (have the ability to say that something was done at least once
  during a day, for variable definitions of day, such as waking
  cycle, versus actual hours, etc.)
 (for it to know when my library books are due, what library they
  are from, and for it to remind me continually before they are
  due, like 6 days before 4, 2, day of etc.))

(Take the different papers that the government gives us and
 formalize them into the system.  try to have all alethic
 possibilities covered.)

(translate only as needed between the term representation and the
 fluents representation)

(Is there a way convert between nested fluents and logic forms)

(try to figure out if flux can do nested fluents)

(PrologMUD can have timeouts set with call_with_depth_limit/3
 call_with_timelimit/2)

(FreeLifePlanner: come up with dashboard for bills and show all
 things.  also make crises first class objects and treated, like
 contingencies.)
(look into Service Contracts in Logic Programming formalisms for
 Resource-Manager e.g. RBSLA)

(noted elsewhere (FreeLifePlanner use case: surviving a
 war) (fluents_data.pl))
