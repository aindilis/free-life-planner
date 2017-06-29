/*
% ===================================================================
% File 'mpred_db_preds.pl'
% Purpose: Emulation of OpenCyc for SWI-Prolog
% Maintainer: Douglas Miles
% Contact: $Author: dmiles $@users.sourceforge.net ;
% Version: 'interface.pl' 1.0.0
% Revision:  $Revision: 1.9 $
% Revised At:   $Date: 2002/06/27 14:13:20 $
% ===================================================================
% File used as storage place for all predicates which change as
% the world is run.
%
%
% Dec 13, 2035
% Douglas Miles
*/
%:- if(( ( \+ ((current_prolog_flag(logicmoo_include,Call),Call))) )).
:- module(mpred_at_box,[
         assert_setting01/1,
         create_predicate_istAbove/3,
         make_module_name_local/2,
         make_module_name_local0/2,
         (make_shared_multifile)/3,
         (make_shared_multifile)/4,
         (shared_multifile)/1,
         add_import_predicate/3,
         autoload_library_index/4,
         ensure_abox/1,
         baseKB_hybrid_support/2,
         uses_predicate/2,
         uses_predicate/5,
         correct_module/3,
         correct_module/5,
         defaultAssertMt/1,
         ensure_imports/1,
         is_parent_goal/1,
         is_parent_goal/2,
          has_parent_goal/1,
          has_parent_goal/2,
         fileAssertMt/1,
          setup_module_ops/1,

         in_mpred_kb_module/0,
         istAbove/2,
         make_as_dynamic/4,
         makeConstant/1,
         mtCanAssert/1,
         %registerCycPred/4,
         %registerCycPred/5,
         retry_undefined/3,
         set_defaultAssertMt/1,
         set_fileAssertMt/1,
         transitive_path/3,
         which_file/1,
         user_m_check/1,

         add_abox_module/1,

         ensure_tbox/1,
          get_file_type_local/2,

         fixup_modules/0,
         import_predicate/2,
         skip_user/1,
         inherit_into_module/2,
         box_type/3,
         make_reachable/2,
         % clause_b/1,
         fixup_module/2,
         is_undefaulted/1,
         ensure_imports_tbox/2,
         map_inheritance/1,


         which_file/1
    ]).



%:- endif.

:- module_transparent((
     baseKB_hybrid_support/2,
         uses_predicate/2,
         uses_predicate/5,
         correct_module/3,
         correct_module/5,
         defaultAssertMt/1,
         ensure_imports/1,
                                is_parent_goal/1,
                                is_parent_goal/2,
                                has_parent_goal/1,
                                has_parent_goal/2,
         fileAssertMt/1,

         in_mpred_kb_module/0,
         istAbove/2,
         make_as_dynamic/4)).

:-dynamic(unused_predicate/4).

:- flag_call(logicmoo_debug=false).

:- set_prolog_flag(logicmoo_virtualize,true).

user_m_check(_Out).

:- meta_predicate make_shared_multifile(+,+,+), mpred_op_each(3).
:- meta_predicate make_shared_multifile(*,*,*,*).

:- meta_predicate with_no_retry_undefined(:).
:- meta_predicate shared_multifile(:).

:- meta_predicate transitive_path(2,*,*).

% add_abox_module(baseKB):-!.
add_abox_module(ABox):- must(atom(ABox)),
  must(mtCanAssert(ABox)),
  ain(baseKB:mtCycL(ABox)).

:- dynamic(baseKB:mtProlog/1).
:- dynamic(baseKB:mtNoPrologCode/1).
:- multifile(baseKB:mtProlog/1).
:- dynamic(baseKB:mtProlog/1).

baseKB:mtNoPrologCode(mpred_userkb).

baseKB:mtProlog(Mt):- var(Mt),!,current_module(Mt),\+ clause_b(mtCycL(Mt)).
baseKB:mtProlog(Mt):- \+ atom(Mt),!,fail.
baseKB:mtProlog(Mt):- \+ current_module(Mt),!,fail.
baseKB:mtProlog(Mt):- clause_b(mtCycL(Mt)),!,fail.
baseKB:mtProlog(Mt):- module_property(Mt,class(library)).
baseKB:mtProlog(Mt):- arg(_,v(lmcache,t_l,system,user),Mt).

:- multifile(lmcache:has_pfc_database_preds/1).
:- dynamic(lmcache:has_pfc_database_preds/1).


%% assert_setting01( ?X) is semidet.
assert_setting01(M:P):-functor(P,_,A),duplicate_term(P,DP),setarg(A,DP,_),system:retractall(M:DP),system:asserta(M:P).


%% which_file( ?F) is semidet.
%
% Which File.
%
which_file(F):- prolog_load_context(source,F) -> true; once(loading_source_file(F)).

:- module_transparent

         assert_setting01/1,
         make_module_name_local/2,
         make_module_name_local0/2,

         defaultAssertMt/1,
         set_defaultAssertMt/1,
         with_no_retry_undefined/1,
         which_file/1,
         fileAssertMt/1,
         set_fileAssertMt/1,

         correct_module/3,
         correct_module/5,
         ensure_imports/1,
         in_mpred_kb_module/0,
         which_file/1,
         user_m_check/1 .


%% in_mpred_kb_module is semidet.
%
% In Managed Predicate Knowledge Base Module.
%
in_mpred_kb_module:- source_context_module(MT),defaultAssertMt(MT2),!,MT==MT2.


map_inheritance(Child):-forall(import_module(Child,Parent),inherit_into_module(Child,Parent)).


%% box_type( ?F, ?A, ?VALUE3) is semidet.
%
% Datalog Type.
%
box_type(F,A,tbox):-current_predicate(baseKB:F/A).
box_type(_,_,abox).




:- thread_local(t_l:current_defaultAssertMt/1).
:- dynamic(baseKB:file_to_module/2).


%:- multifile(get_current_default_tbox/1).
%:- dynamic(get_current_default_tbox/1).
%get_current_default_tbox(baseKB).


%% defaultAssertMt(-Ctx) is det.
%
% ABox is an "assertion component" Prolog Module
% within a knowledge base.
%
% not just user modules
defaultAssertMt(ABox):- (t_l:current_defaultAssertMt(BBox);find_and_call(fileAssertMt(BBox)))->ABox=BBox.




% :- '$hide'(defaultAssertMt(_)).


%% get_file_type_local( ?File, ?Type) is det.
%
% Get File Type.
%
get_file_type_local(File,Type):-var(File),!,quietly_must(loading_source_file(File)),get_file_type_local(File,Type),!.
get_file_type_local(File,pfc):-file_name_extension(_,'.pfc.pl',File),!.
get_file_type_local(File,Type):-file_name_extension(_,Type,File),!.
get_file_type_local(File,Type):-clause(lmcache:mpred_directive_value(File,language,Type),true).

mtCanAssert(baseKB).
mtCanAssert(user):- !,fail.
mtCanAssert(abox):- !,dumpST,fail.
mtCanAssert(Module):- clause_b(mtCycL(Module)).
mtCanAssert(Module):- clause_b(mtExact(Module)).
mtCanAssert(Module):-  module_property(Module,file(_)),!,fail.
mtCanAssert(Module):- (loading_source_file(File),get_file_type_local(File,pfc),prolog_load_context(module,Module)).
mtCanAssert(Module):- clause_b(mtProlog(Module)),!,fail.
mtCanAssert(_).


% :- ensure_loaded(mpred_loader).

%% fileAssertMt(-ABox) is det.
%
% Gets ABox is an "assertion component" Prolog Module
% within a knowledge base.
%
% not just user modules
fileAssertMt(ABox):- nonvar(ABox), fileAssertMt(ABoxVar),!,ABox=@=ABoxVar.
fileAssertMt(Module):- (loading_source_file(File),get_file_type_local(File,pfc)),prolog_load_context(module,Module),Module\==user.
fileAssertMt(ABox):-
  (t_l:current_defaultAssertMt(ABox);
    ((('$current_source_module'(ABox);'$current_typein_module'(ABox)),mtCanAssert(ABox)))),!.
fileAssertMt(ABox):-
   which_file(File)->current_module(ABox),module_property(ABox,file(File)),File\==ABox,
   mtCanAssert(ABox).
fileAssertMt(ABox):-
  which_file(File)->make_module_name_local(File,ABox),current_module(ABox),File\==ABox,
   mtCanAssert(ABox).
fileAssertMt(baseKB).


% baseKB:mtGlobal
% mtCore



makeConstant(_Mt).


:- module_transparent((ensure_abox)/1).
:- multifile(lmcache:has_pfc_database_preds/1).
:- volatile(lmcache:has_pfc_database_preds/1).
:- dynamic(lmcache:has_pfc_database_preds/1).
ensure_abox(M):- sanity(atom(M)), lmcache:has_pfc_database_preds(M),!.
ensure_abox(user):- setup_module_ops(user),!,ensure_abox(baseKB),!.
ensure_abox(M):-
   asserta(lmcache:has_pfc_database_preds(M)),
   assert_if_new(baseKB:mtCycL(M)),
   retractall(baseKB:mtProlog(M)),
   setup_module_ops(M),
   set_prolog_flag(M:unknown,error),
   forall(mpred_database_term(F,A,_),
       (((
        M:multifile(M:F/A),
        M:dynamic(M:F/A),
        M:discontiguous(M:F/A),
        create_predicate_istAbove(M,F,A),
        M:module_transparent(M:F/A))))),!.

setup_module_ops(M):- mpred_op_each(mpred_op_unless(M)).

mpred_op_unless(M,A,B,C):- op_safe(A,B,M:C).

mpred_op_each(OpEach):-
            call(OpEach,1199,fx,('==>')), % assert
            call(OpEach,1199,fx,('?->')), % ask
            call(OpEach,1190,xfy,('::::')), % Name something
            call(OpEach,1180,xfx,('==>')), % Forward chaining
            call(OpEach,1170,xfx,('<==>')), % Forward and backward chaining
            call(OpEach,1160,xfx,('<==')), % backward chain PFC sytle
            call(OpEach,1160,xfx,('<-')), % backward chain PTTP sytle (currely really PFC)
            call(OpEach,1160,xfx,('<=')), % backward chain DRA sytle
            call(OpEach,1150,xfx,('=>')), % Logical implication
            call(OpEach,1130,xfx,('<=>')), % Logical bi-implication
            call(OpEach,600,yfx,('&')), 
            call(OpEach,600,yfx,('v')),
            call(OpEach,400,fx,('~')),
            % call(OpEach,300,fx,('-')),
            call(OpEach,350,xfx,('xor')).





%:- (system:dtrace, rtrace, dtrace,cls ).
%:- (dbreak,cnotrace,nortrace).


:- if(current_predicate(system:get_current_default_tbox/1)).
:- redefine_system_predicate(system:get_current_default_tbox/1).
:- endif.
:- module_transparent(system:get_current_default_tbox/1).
system:get_current_default_tbox(TBox):- defaultAssertMt(ABox)->current_module(ABox)->clause(ABox:defaultTBoxMt(TBox),B),call(B),!.
system:get_current_default_tbox(baseKB).

%% set_defaultAssertMt( ?ABox) is semidet.
%
% Sets Current Module.
%
set_defaultAssertMt(ABox):-
  sanity(mtCanAssert(ABox)),
  must(((
    get_current_default_tbox(TBox),
    asserta_new(TBox:mtCycL(ABox)),
    asserta_new(ABox:defaultTBoxMt(TBox)),
    assert_setting(t_l:current_defaultAssertMt(ABox)),
    ensure_abox(ABox),
    '$set_source_module'(ABox),
    '$set_typein_module'(ABox),
    nop(inherit_into_module(ABox,TBox))))),
  ain(baseKB:mtCycL(ABox)).

% :- '$hide'(set_defaultAssertMt(_)).

%% set_fileAssertMt( ABox) is semidet.
%
% Sets the File''s Module.
%
set_fileAssertMt(ABox):-
 '$current_typein_module'(CM),
 '$current_source_module'(SM),
 sanity(mtCanAssert(ABox)),
 (((
   fileAssertMt(Was),
   % get_current_default_tbox(TBox),

   set_defaultAssertMt(ABox),
   which_file(File),
   assert_setting(baseKB:file_to_module(File,ABox)),
   assert_setting(lmcache:mpred_directive_value(File,module,ABox)),
   % MAYBE? '$set_typein_module'(TBox),
   call_on_eof(set_defaultAssertMt(Was)),
   call_on_eof('$set_source_module'(SM)),
   call_on_eof('$set_typein_module'(CM))))).



make_module_name_local(A,B):- make_module_name_local0(A,B), \+ exists_file(B),!.

make_module_name_local0(Source,KB):- clause_b(mtProlog(Source)),t_l:current_defaultAssertMt(KB),!.
make_module_name_local0(Source,KB):- clause_b(mtGlobal(Source)),t_l:current_defaultAssertMt(KB),!.
make_module_name_local0(Source,SetName):- baseKB:file_to_module(Source,SetName),!.
make_module_name_local0(Source,Source):- lmcache:has_pfc_database_preds(Source).
make_module_name_local0(Source,Source):- clause_b(mtCycL(Source)),!.
make_module_name_local0(user,KB):- t_l:current_defaultAssertMt(KB),!.
make_module_name_local0(user,baseKB):-!.
make_module_name_local0(Source,GetName):- make_module_name(Source,GetName).


ensure_tbox(_ABox).


%% mtCore( ?VALUE1) is semidet.
%
% If Is A Datalog System Core Microtheory.
%
:- dynamic(baseKB:mtCore/1).
baseKB:mtCore(baseKB).




%% baseKB:mtGlobal(M,Box).
%
% Boot Modules.
%
%baseKB:mtGlobal(mpred_loader).

:- dynamic(baseKB:mtGlobal/1).
baseKB:mtGlobal(boot_system).
baseKB:mtGlobal(system_markers).
baseKB:mtGlobal(system_singleValued).
baseKB:mtGlobal(system_genls).
baseKB:mtGlobal(system_if_missing).
baseKB:mtGlobal(common_logic_clif).
baseKB:mtGlobal(system_mdefault).

:- dynamic(baseKB:mtCycLBroad/1).

baseKB:mtCycLBroad(baseKB).

is_undefaulted(user).




%% ensure_imports( ?M) is semidet.
%
% Ensure Imports.
%
ensure_imports(baseKB):-!.
ensure_imports(M):- ain(genlMt(M,baseKB)).

:-multifile(lmcache:is_ensured_imports_tbox/2).
:-dynamic(lmcache:is_ensured_imports_tbox/2).


%% skip_user( ?M) is semidet.
%
% Skip over 'user' module and still see 'system'.
%
skip_user(Mt):- import_module(Mt,system), \+ default_module(Mt,user), !.
skip_user(Mt):- !, add_import_module(Mt,system,start),ignore(delete_import_module(Mt,user)),
  forall((import_module(Mt,X),default_module(X,user)),skip_user(X)).

inherit_into_module(Child,Parent):- ==(Child,Parent),!.
inherit_into_module(Child,Parent):-ain(baseKB:genlMt(Child,Parent)).

%% ensure_imports_tbox( ?M, ?TBox) is semidet.
%
% Ensure Imports Tbox.
%
ensure_imports_tbox(M,TBox):-M==TBox,!.
ensure_imports_tbox(M,TBox):-
  lmcache:is_ensured_imports_tbox(M,TBox),!.
ensure_imports_tbox(M,TBox):-
  asserta(lmcache:is_ensured_imports_tbox(M,TBox)),

  must((
   skip_user(TBox),
   ignore(maybe_delete_import_module(M,TBox)),
   ignore(maybe_delete_import_module(TBox,M)),
   forall((user:current_predicate(_,TBox:P),
      \+ predicate_property(TBox:P,imported_from(_))),
      add_import_predicate(M,P,TBox)),
   inherit_into_module(M,user),
   skip_user(M),
   ignore(maybe_delete_import_module(M,user)),
   inherit_into_module(user,M),
   ignore(maybe_delete_import_module(user,system)), % gets from M now
   !)).



% :- inherit_into_module(logicmoo_user,system).

fixup_module(_,_):-!.
fixup_module(system,_).
fixup_module(M,_L):- clause_b(tGlobal(M)),skip_user(M).
fixup_module(system,_L):-skip_user(system).
fixup_module(_,[user]).
fixup_module(M,_L):- skip_user(M).


fixup_modules:-
   doall((current_module(M),once((findall(I,import_module(M,I),L))),once(fixup_module(M,L)))).

% :- autoload([verbose(false)]).
:- flag_call(logicmoo_debug=true).

% :- fixup_modules.







% ============================================

%% correct_module( ?M, ?X, ?T) is semidet.
%
% Correct Module.
%
correct_module(M,G,T):-functor(G,F,A),quietly_must(correct_module(M,G,F,A,T)),!.

%% correct_module( ?M, ?Goal, ?F, ?A, ?T) is semidet.
%
% Correct Module.
%
correct_module(abox,G,F,A,T):- !, defaultAssertMt(M),correct_module(M,G,F,A,T).
correct_module(tbox,G,F,A,T):- !, get_current_default_tbox(M),correct_module(M,G,F,A,T).
correct_module(user,G,F,A,T):- fail,!,defaultAssertMt(M),correct_module(M,G,F,A,T).

correct_module(HintMt,Goal,F,A,OtherMt):-var(Goal),functor(Goal,F,A),!,correct_module(HintMt,Goal,F,A,OtherMt).
correct_module(HintMt,Goal,_,_,OtherMt):- predicate_property(HintMt:Goal,imported_from(OtherMt)).
correct_module(_,Goal,_,_,OtherMt):- predicate_property(Goal,imported_from(OtherMt)).
correct_module(HintMt,_,_,_,HintMt):- call_u(mtExact(HintMt)).
correct_module(HintMt,Goal,_,_,HintMt):- predicate_property(HintMt:Goal,exported).
correct_module(_,Goal,_,_,OtherMt):- var(OtherMt),!, predicate_property(OtherMt:Goal,file(_)).
correct_module(_,Goal,_,_,OtherMt):- clause_b(mtGlobal(OtherMt)), predicate_property(OtherMt:Goal,file(_)).
correct_module(MT,_,_,_,MT):-!.



:- dynamic(lmcache:how_registered_pred/4).
:- module_transparent(lmcache:how_registered_pred/4).

add_import_predicate(Mt,Goal,OtherMt):- fail,
   clause_b(mtGlobal(Mt)),
   clause_b(mtGlobal(OtherMt)),
   \+ import_module(OtherMt,Mt),
   catch(add_import_module(Mt,OtherMt,end),
       error(permission_error(add_import,module,baseKB),
       context(system:add_import_module/3,'would create a cycle')),fail),
   must(predicate_property(Mt:Goal,imported_from(OtherMt))),!.


add_import_predicate(Mt,Goal,OtherMt):- catch(Mt:import(OtherMt:Goal),_,fail),!.
add_import_predicate(Mt,Goal,OtherMt):-
   functor(Goal,F,A),
   make_as_dynamic(imported_from(OtherMt),Mt,F,A),
   assert_if_new(( Mt:Goal :- OtherMt:Goal)).

make_as_dynamic(Reason,Mt,F,A):-
 must((
   multifile(Mt:F/A),
   discontiguous(Mt:F/A),
   dynamic(Mt:F/A),
   module_transparent(Mt:F/A),
   public(Mt:F/A),
   on_f_throw( (Mt:F/A)\== (baseKB:loaded_external_kbs/1)),
   functor(Goal,F,A),
   assert_if_new(( Mt:Goal :- (fail,infoF(Reason)))))).


transitive_path(F,[Arg1,Arg2],Arg2):-
  dif(Arg1,Arg2),call(F,Arg1,Arg2),!.
transitive_path(F,[Arg1,SecondNodeMt|REST],Arg2):-
  dif(Arg1,Arg2),dif(Arg1,SecondNodeMt),
  call(F,Arg1,SecondNodeMt),sanity(stack_check),
  transitive_path(F,[SecondNodeMt|REST],Arg2).



autoload_library_index(F,A,PredMt,File):- functor(P,F,A),'$autoload':library_index(P,PredMt,File).


:- multifile(baseKB:hybrid_support/2).
:- dynamic(baseKB:hybrid_support/2).
baseKB_hybrid_support(F,A):-baseKB:wrap_shared(F,A,_).
baseKB_hybrid_support(F,A):-clause_b(hybrid_support(F,A)).

baseKB:hybrid_support(predicateConventionMt,2).

baseKB:hybrid_support(functorDeclares,1).
baseKB:hybrid_support(arity,2).

baseKB:hybrid_support(spft,3).

baseKB:hybrid_support(mtCycL,1).
baseKB:hybrid_support(mtCycLBroad,1).
baseKB:hybrid_support(genlMt,2).

%predicateConventionMt(genlMt,baseKB).

% baseKBOnly mark_mark/3 must be findable from every module (dispite the fact that baseKB is not imported)
:- dynamic baseKB:mpred_mark/3.

% hybrid_support (like spft/3) must be defined directly in every module and then aggregated thru genlMts (thus to baseKB)

:- dynamic(lmcache:tried_to_retry_undefined/4).


istAbove(Mt,Query):- Mt \== baseKB, genlMt(Mt,MtAbove),MtAbove:Query.


% make sure we ignore calls to predicate_property/2  (or thus '$define_predicate'/1)
uses_predicate(M:F/A,R):- !, '$current_source_module'(SM), uses_predicate(SM,M,F,A,R).
uses_predicate(F/A,R):- '$current_source_module'(SM),'$current_typein_module'(M),uses_predicate(M,SM,F,A,R).



is_parent_goal(G):- prolog_current_frame(F),prolog_frame_attribute(F,parent_goal, G).
is_parent_goal(F,G):-prolog_frame_attribute(F,parent_goal, G).


has_parent_goal(G):- prolog_current_frame(F),prolog_frame_attribute(F,parent, PF),has_parent_goal(PF,G).
has_parent_goal(F,G):-prolog_frame_attribute(F,goal, G);(prolog_frame_attribute(F,parent, PF),has_parent_goal(PF,G)).


uses_predicate(_,CallerMt,'$pldoc',4,retry):- multifile(CallerMt:'$pldoc'/4),discontiguous(CallerMt:'$pldoc'/4),dynamic(CallerMt:'$pldoc'/4),!.

uses_predicate(_,M,F,A,R):-
  prolog_current_frame(FR), functor(P,F,A),(prolog_frame_attribute(FR,parent_goal,predicate_property(M:P,_))),!,R=error.
uses_predicate(_,Module,Name,Arity,Action) :-
      current_prolog_flag(autoload, true),
	'$autoload'(Module, Name, Arity), !,
	Action = retry.

uses_predicate(BaseKB,System, F,A,R):-  System\==BaseKB, call_u(mtCycL(BaseKB)),\+ call_u(mtCycL(System)),!,
   must(uses_predicate(System,BaseKB,F,A,R)),!.

uses_predicate(_,_, (:-), 1, error) :- !,dumpST,dbreak.

uses_predicate(user,user, isa, 2, retry) :- call(call,assert((user:isa(I,C):-call_u(isa(I,C))))),!.
uses_predicate(user,user, tRRP, 1, retry) :- call(call,assert((user:tRRP(I):-call_u(isa(I,tRRP))))),!.
uses_predicate(user,user, F, A, retry) :- clause_b(arity(F,A)),functor(P,F,A),
   call(call,assert((user:P :- call_u(P)))),
   user:compile_predicates([F/A]),!.

uses_predicate(_,_, (:-), _, error) :- !,dumpST,dbreak.
uses_predicate(_,_, (/), _, error) :- !,dumpST,dbreak.
uses_predicate(_,_, (//), _, error) :- !,dumpST,dbreak.
uses_predicate(_,_, (:), _, error) :- !,dumpST,dbreak.
uses_predicate(_,myMt,mtExact,1, retry):- myMt:import(baseKB:mtExact/1),!.
% uses_predicate(SM,_, '>>',  4, error) :- !,dumpST,dbreak.
uses_predicate(_,_, '[|]', _, error) :- !,dumpST,dbreak.
uses_predicate(User, User, module, 2, error):-!.

% make sure we ignore calls to predicate_property/2  (or thus '$define_predicate'/1)
uses_predicate(_,_,_,_,error):-prolog_current_frame(F),
  (is_parent_goal(F,'$define_predicate'(_));
   is_parent_goal(F,'assert_u'(_));
   has_parent_goal(F,'$syspreds':property_predicate(_,_))),!.


uses_predicate(CallerMt,CallerMt,predicateConventionMt,2,retry):-
  create_predicate_istAbove(CallerMt,predicateConventionMt,2),!.

uses_predicate(_CallerMt,baseKB,predicateConventionMt,2,retry):-
  create_predicate_istAbove(baseKB,predicateConventionMt,2).


uses_predicate(BaseKB,System, F,A,R):-  System\==BaseKB, call_u(mtCycL(BaseKB)),\+ call_u(mtCycL(System)),
   loop_check_term(must(uses_predicate(System,BaseKB,F,A,R)),
                   term(uses_predicate(System,BaseKB,F,A,R)),fail),!.

uses_predicate(_CallerMt, baseKB, F, A,retry):-
  create_predicate_istAbove(baseKB,F,A),
   nop(system:import(baseKB:F/A)),!.

uses_predicate(System, BaseKB, F,A, retry):-  System\==BaseKB, call_u(mtCycL(BaseKB)),\+ call_u(mtCycL(System)),!,
   create_predicate_istAbove(BaseKB,F,A),
    nop(system:import(BaseKB:F/A)),!.

% keeps from calling this more than once
uses_predicate(SM,M,F,A,error):-
  lmcache:tried_to_retry_undefined(SM,M,F,A),!,
  wdmsg(unused_predicate(SM,M,F,A)),backtrace(800),dbreak.

uses_predicate(SM,CallerMt,F,A,_):-
   wdmsg(uses_predicate(SM,CallerMt,F,A)),
   assert(lmcache:tried_to_retry_undefined(SM,CallerMt,F,A)),fail.

uses_predicate(_,Module, Name, Arity, Action) :- fail,
	current_prolog_flag(autoload, true),
	'$autoload'(Module, Name, Arity), !,
	Action = retry.

uses_predicate(_,System, _,_, error):- module_property(System,class(system)),!.
uses_predicate(_,System, _,_, error):- module_property(System,class(library)),!.

uses_predicate(SM,CallerMt,F,A,R):-
    loop_check_term(retry_undefined(CallerMt,F,A),dump_break_loop_check_uses_predicate(SM,CallerMt,F,A,retry),dump_break),
    R=retry.

%% create_predicate_istAbove(+ChildDefMt,+F,+A) is semidet.
%
% Ensure istAbove/2 stub is present in ChildDefMt.
%
create_predicate_istAbove(Nonvar,F,A):- sanity(ground(create_predicate_istAbove(Nonvar,F,A))),fail.
% TODO unsuspect the next line (nothing needs to see above baseKB)


create_predicate_istAbove(baseKB,F,A):- !,
  make_as_dynamic(create_predicate,baseKB,F,A), 
     ignore((( \+ (defaultAssertMt(CallerMt),CallerMt\==baseKB,create_predicate_istAbove(CallerMt,F,A) )))).
create_predicate_istAbove(abox,F,A):-  must(defaultAssertMt(CallerMt)),sanity(CallerMt\=abox),!,create_predicate_istAbove(CallerMt,F,A).
%create_predicate_istAbove(_, do_and_undo, 2):-dtrace.
create_predicate_istAbove(CallerMt,F,A):- clause_b(mtProlog(CallerMt)), must(\+ clause_b(mtCycL(CallerMt))) ,!,wdmsg(warn(create_predicate_istAbove_mtProlog(CallerMt,F,A))),dtrace.
create_predicate_istAbove(CallerMt,F,A):-
   make_as_dynamic(create_predicate_istAbove,CallerMt,F,A),
   functor(Goal,F,A),
   assert_if_new(( CallerMt:Goal :- istAbove(CallerMt,Goal))).



with_no_retry_undefined(Goal):- w_tl(flag_call(logicmoo_debug=false),Goal).


% Every module has it''s own
retry_undefined(CallerMt,'$pldoc',4):- multifile(CallerMt:'$pldoc'/4),discontiguous(CallerMt:'$pldoc'/4),dynamic(CallerMt:'$pldoc'/4),!.

% 3 very special Mts
% Module defines the type
% retry_undefined(baseKB,F,A):- make_as_dynamic(retry_undefined(baseKB),baseKB,F,A),!.
retry_undefined(lmcache,F,A):- volatile(lmcache:F/A),make_as_dynamic(retry_undefined(lmcache),lmcache,F,A),!.
retry_undefined(t_l,F,A):- thread_local(t_l:F/A),!,make_as_dynamic(retry_undefined(t_l),t_l,F,A),!.

% adult-like Mt
retry_undefined(Mt, F, A):-  clause_b(mtCycLBroad(Mt)), baseKB_hybrid_support(F,A),
   make_as_dynamic(mtCycLBroad(Mt),Mt,F,A).

% child-like Mt
retry_undefined(CallerMt,F,A):- baseKB_hybrid_support(F,A),
   clause_b(mtGlobal(CallerMt)),
   % find_and_call(baseKB:mtGlobal(CallerMt)),
   create_predicate_istAbove(CallerMt,F,A).

% import built-ins ?
retry_undefined(CallerMt,F,A):- current_predicate(system:F/A), current_module(M),M\=system,
  current_predicate(M:F/A),functor(P,F,A),predicate_property(M:P,defined),\+predicate_property(M:P,imported_from(_)),
  CallerMt:import(M:F/A).

% our autoloader hacks
retry_undefined(CallerMt,F,A):-
   autoload_library_index(F,A,_PredMt,File),
   use_module(CallerMt:File),!.

% Autoloads importing the entire other module
retry_undefined(CallerMt,F,A):- fail,
       autoload_library_index(F,A,PredMt,File),
       asserta(lmcache:how_registered_pred(PredMt:use_module(CallerMt:File),CallerMt,F,A)),
       use_module(system:File),!.
       % system:add_import_module(CallerMt,system,start).



% System-like Autoloads (TODO: confirm these can be removed)
retry_undefined(CallerMt,debug,1):- use_module(CallerMt:library(debug)),!.
retry_undefined(CallerMt,debugging,1):- use_module(CallerMt:library(debug)),!.
retry_undefined(CallerMt,member,2):- use_module(CallerMt:library(lists)),!.
retry_undefined(CallerMt,directory_file_path,3):- use_module(CallerMt:library(filesex)),!.


retry_undefined(CallerMt,F,A):- fail,
       autoload_library_index(F,A,_,File),
       load_files(CallerMt:File,[if(true),imports([F/A]),register(false),silent(false)]),!.

% Autoloads importing the entire other module
retry_undefined(CallerMt,F,A):-
       autoload_library_index(F,A,PredMt,File),
       asserta(lmcache:how_registered_pred(PredMt:use_module(CallerMt:File),CallerMt,F,A)),
       use_module(CallerMt:File),!.

/*
retry_undefined(CallerMt,F,A):-
      autoload_library_index(F,A,PredMt,File),
      ((current_module(PredMt),current_predicate(PredMt:F/A))
       -> add_import_module(CallerMt,PredMt,start) ;
       (PredMt:ensure_loaded(PredMt:File),add_import_module(CallerMt,PredMt,start))),!.
*/

retry_undefined(CallerMt,F,A):-functor(P,F,A),find_module(P,M),show_call(CallerMt:import(M:F/A)),!.


%retry_undefined(PredMt:must/1) % UNDO % :- add_import_module(PredMt,logicmoo_util_catch,start),!.
%retry_undefined(PredMt:debugm/2) % UNDO % :- add_import_module(PredMt,logicmoo_util_dmsg,start),!.


:- module_transparent(make_shared_multifile/3).
:- export((make_shared_multifile)/3).




:- module_transparent((shared_multifile)/1).
:- meta_predicate((shared_multifile(+))).

%=

%% shared_multifile( +PI) is semidet.
%
% Shared Multifile.
%
shared_multifile(PredMt:MPI):-
   context_module_of_file(CallerMt),!,
   with_pfa_group(make_shared_multifile,CallerMt,PredMt, MPI),!.

shared_multifile(PI):- kb_dynamic(PI).


%% make_shared_multifile( ?CallerMt, ?PredMt, :TermPI) is semidet.
%
% Make Shared Multifile.
%
make_shared_multifile(CallerMt, PredMt,FA):- get_fa(FA,F,A), make_shared_multifile(CallerMt, PredMt,F,A),!.

make_shared_multifile(CallerMt,    t_l,F,A):-!,CallerMt:thread_local(t_l:F/A),CallerMt:multifile(t_l:F/A).
% make_shared_multifile(CallerMt,baseKB ,F,A):-!,CallerMt:multifile(baseKB:F/A),CallerMt:dynamic(baseKB:F/A),!.
make_shared_multifile(CallerMt,lmcache,F,A):-!,CallerMt:multifile(lmcache:F/A),CallerMt:volatile(lmcache:F/A),CallerMt:dynamic(lmcache:F/A),!.

make_shared_multifile(CallerMt,PredMt,F,A):-
  functor(Goal,F,A),
  correct_module(PredMt,Goal,F,A,HomeM),
  HomeM\==PredMt,!,
  make_shared_multifile(CallerMt,HomeM,F,A).

make_shared_multifile(CallerMt,Home,F,A):- clause_b(mtProlog(Home)),!,
     wdmsg(mtSharedPrologCodeOnly_make_shared_multifile(CallerMt,Home:F/A)),!.

make_shared_multifile(_CallerMt, baseKB,F,A):-  kb_dynamic(F,A),!.

make_shared_multifile(_CallerMt,PredMt,F,A):-!,
 dmsg(make_shared_multifile(PredMt:F/A)),
 w_tl(set_prolog_flag(access_level,system),
  PredMt:(
   sanity( \+ ((PredMt:F/A) = (qrTBox:p/1))),
   PredMt:check_never_assert(declared(PredMt:F/A)),
   decl_mpred(PredMt:F/A))).




%% make_reachable( ?UPARAM1, ?Test) is semidet.
%
% Make Reachable.
%
make_reachable(_,Test):- \+ \+ ((Test= (_:F/_), is_ftVar(F))),!.
make_reachable(CM,M:F/A):-  atom(CM),ignore(CM=M),quietly_must(atom(CM)),quietly_must(atom(M)),
   functor(G,F,A),
   correct_module(M,G,F,A,TT), !,import_predicate(CM,TT:F/A).



%% import_predicate( ?CM, :TermM) is semidet.
%
% Import Predicate.
%
import_predicate(CM,M:_):- CM==M,!.
import_predicate(CM,M:_):- default_module(CM,M),!.
import_predicate(CM,M:F/A):- show_call(nop(CM:z333import(M:F/A))),CM:multifile(M:F/A),
  icatch(CM:discontiguous(M:F/A)).


:- include('mpred_header.pi').

system:call_expansion(T,(mpred_at_box:defaultAssertMt(NewVar),NewT)):- current_predicate(_,get_lang(pfc)), compound(T),
   subst(T,abox,NewVar,NewT),NewT\=@=T.

system:body_expansion(T,(mpred_at_box:defaultAssertMt(NewVar),NewT)):- current_predicate(_,get_lang(pfc)), compound(T),
   subst(T,abox,NewVar,NewT),NewT\=@=T.

