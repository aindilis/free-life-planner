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
(FreeLifePlanner use case: surviving a war)
