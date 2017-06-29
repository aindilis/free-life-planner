

% nutcracker.pl, by Johan Bos

/*========================================================================
   File Search Paths
========================================================================*/

file_search_path(semlib,     'src/prolog/lib').
file_search_path(nutcracker, 'src/prolog/nutcracker').
file_search_path(knowledge,  'src/prolog/boxer/knowledge').


/*========================================================================
   Load other libraries
========================================================================*/

:- use_module(library(lists),[member/2]).

:- use_module(semlib(drs2fol),[drs2fol/2,symbol/4]).
:- use_module(semlib(errors),[error/2,warning/2]).
:- use_module(semlib(options),[option/2,parseOptions/2,setOption/3,
                               showOptions/1,setDefaultOptions/1]).

:- use_module(nutcracker(version),[version/1]).
:- use_module(nutcracker(input),[openInput/0,tDRS/1,hDRS/1]).
:- use_module(nutcracker(callInference),[callTPandMB/5]).
:- use_module(nutcracker(fol2bliksem),[fol2bliksem/2]).
:- use_module(nutcracker(miniWordNet),[compConcepts/1,compISA/0,
                                       clearMWN/0,cutDownMWN/0,
                                       addTopMWN/0,graphMWN/0,
                                       outputMWN/0,axiomsBK/1]).


/*========================================================================
   Main
========================================================================*/

main:-
   option(Option,do), 
   member(Option,['--version','--help']), !, 
   version,
   help.

main:-
   checkDir,
   meta,
   parse, 
   box,
   openInput,
   nc,
   !.

main:-
   setOption(nutcracker,'--help',do), !,
   help.


/*------------------------------------------------------------------------
   Check Dir
------------------------------------------------------------------------*/

checkDir:-
   checkDir1,
   checkDir2.

checkDir1:-
   option('--dir',Dir),
   atom_chars(Dir,Chars),
   append(NewChars,['/'],Chars), !,
   atom_chars(NewDir,NewChars),
   setOption(nutcracker,'--dir',NewDir),
   checkDir1.

checkDir1.

checkDir2:-
   option('--dir',Dir),
   exists_directory(Dir), 
   access_file(Dir,write), !.

checkDir2:-
   option('--dir',Dir),
   \+ exists_directory(Dir), 
   access_file(Dir,write), !,
   make_directory(Dir).

checkDir2:-
   option('--dir',Dir),
   error('cannot access or create directory ~p',[Dir]), 
   !, fail.


/*------------------------------------------------------------------------
   File preparation (adding META markup)
------------------------------------------------------------------------*/

meta:-
   option('--dir',Dir),
   concat_atom([Dir,'/','t'],TFile),
   concat_atom([Dir,'/','h'],HFile),
   access_file(TFile,read),
   access_file(HFile,read), 
   concat_atom([Dir,'/','t.met'],TFileMET),
   concat_atom([Dir,'/','h.met'],HFileMET),
   access_file(TFileMET,read),
   access_file(HFileMET,read), 
   time_file(TFile,TT1), time_file(TFileMET,TT2), TT1 < TT2,
   time_file(HFile,TH1), time_file(HFileMET,TH2), TH1 < TH2, !.

meta:-
   option('--dir',Dir),
   concat_atom([Dir,'/','t'],TFile),
   concat_atom([Dir,'/','h'],HFile),
   access_file(TFile,read),
   access_file(HFile,read), !,
   concat_atom([Dir,'/','t.met'],TFileMET),
   concat_atom([Dir,'/','h.met'],HFileMET),
   concat_atom([Dir,'/','i.met'],IFileMET),
   concat_atom([echo,'"<META>rte"','>',IFileMET],' ',Shell1), shell(Shell1),
   concat_atom([cat,IFileMET,TFile,'>',TFileMET],' ',Shell2), shell(Shell2),
   concat_atom([cat,IFileMET,TFile,HFile,'>',HFileMET],' ',Shell3), shell(Shell3).

meta:-
   option('--dir',Dir),
   error('directory ~p does not contain files named t and h',[Dir]), 
   !, fail.   


/*------------------------------------------------------------------------
   Parse (init)
------------------------------------------------------------------------*/

parse:-
   option('--dir',Dir),
   concat_atom([Dir,'/','t.met'],TFileMET),
   concat_atom([Dir,'/','h.met'],HFileMET),   
   access_file(TFileMET,read),
   access_file(HFileMET,read), !,
   concat_atom([Dir,'/','t.ccg'],TFileCCG),
   concat_atom([Dir,'/','h.ccg'],HFileCCG),
   parse(TFileMET,TFileCCG),
   parse(HFileMET,HFileCCG).

parse:-
   option('--dir',Dir),
   error('directory ~p does not contain files named t.met and h.met',[Dir]), 
   !, fail.   


/*------------------------------------------------------------------------
   Parse
------------------------------------------------------------------------*/

parse(In,Out):-
   exists_file(In),
   time_file(In,T1),
   exists_file(Out),
   time_file(Out,T2),
   T1 < T2, !.

parse(In,Out):-
   concat_atom(['bin/candc',
                '--input',In,
                '--output',Out,
                '--models models/boxer',
                '--candc-printer boxer'],' ',Shell),
   write(Shell), nl,
   shell(Shell,_).


/*------------------------------------------------------------------------
   Boxer (init)
------------------------------------------------------------------------*/

box:-
   option('--dir',Dir),
   concat_atom([Dir,'/','t.ccg'],TFileCCG),
   concat_atom([Dir,'/','h.ccg'],HFileCCG),
   access_file(TFileCCG,read),
   access_file(HFileCCG,read), !,
   concat_atom([Dir,'/','t.drs'],TFileDRS),
   concat_atom([Dir,'/','h.drs'],HFileDRS),
   box(TFileCCG,TFileDRS),
   box(HFileCCG,HFileDRS).

box:-
   option('--dir',Dir),
   error('directory ~p does not contain files named t.ccg and h.ccg',[Dir]), 
   !, fail.   


/*------------------------------------------------------------------------
   Boxer (init)
------------------------------------------------------------------------*/

box(In,Out):-
   exists_file(In),
   time_file(In,T1),
   exists_file(Out),
   time_file(Out,T2),
   T1 < T2, !.

box(In,Out):-
   concat_atom(['bin/boxer',
                '--input',In,
                '--output',Out,
                '--resolve',
                '--vpe',
                '--box'],' ',Shell),
   write(Shell), nl,
   shell(Shell,_).


/* =======================================================================
   Textual Entailment
========================================================================*/

nc:-
   tDRS(TXDRS), TXDRS=xdrs(_,_,_,TDRS),
   hDRS(HXDRS), HXDRS=xdrs(_,_,_,HDRS),
   mwn(HXDRS),
   drs2fol(TDRS,TFOL),
   drs2fol(HDRS,HFOL),
   bk(TFOL,TFOLandBK),
   proof(TFOLandBK,HFOL).


/* =======================================================================
   Get Background Knowledge
========================================================================*/

bk(OldF,NewF):-
   option('--bk',true),
   option('--domsize',DomSize),
   axiomsBK(BK), 
   NewF = and(OldF,BK),
   callTPandMB(not(NewF),NewF,DomSize,Model,Engine),
   warning('~p found first result for -(T & BK)',[Engine]),
   \+ Model = model([],[]), !, 
   outputModel(Model,t).

bk(F,F):-
   option('--bk',true), !,   %%% BK inconsistent
   warning('background knowledge inconsistent',[]),
   option('--domsize',DomSize),
   callTPandMB(not(F),F,DomSize,Model,Engine),
   warning('~p found first result for -T',[Engine]),
   outputModel(Model,t).

bk(F,F):-
   option('--domsize',DomSize),
   callTPandMB(not(F),F,DomSize,Model,Engine),
   warning('~p found first result for -T',[Engine]),
   outputModel(Model,t).


/* =======================================================================
   Do the proofs
========================================================================*/

proof(TFOL,HFOL):-
   option('--domsize',DomSize),
   F1 = and(TFOL,not(HFOL)),
   F2 = and(TFOL,HFOL),
   callTPandMB(not(F1),F1,DomSize,Model1,Engine1),
   warning('~p found first result for -(T & BK & -H)',[Engine1]),
   callTPandMB(not(F2),F2,DomSize,Model2,Engine2),
   warning('~p found first result for -(T & BK & H)',[Engine2]),
   outputModel(Model2,th),
   prediction(Model1,Model2,Prediction),
   outputPrediction(Prediction).


/* =======================================================================
   Prediction
========================================================================*/

prediction(model([],[]),_,'entailed'):- !.
prediction(_,model([],[]),'inconsistent'):- !.
prediction(_,_,           'informative').



/* =======================================================================
   Output Model
========================================================================*/

outputModel(Model,Name):-
   option('--dir',Dir),
   concat_atom([Dir,'/',Name,'.mod'],File),
   open(File,write,Stream),
   write(Stream,Model), 
   write(Stream,'.'), 
   nl(Stream),
   close(Stream),
   outputModelSize(Model,Name).


/* =======================================================================
   Output Model Size
========================================================================*/

outputModelSize(Model,Name):-
   Model = model(Dom,_),
   length(Dom,Size),
   option('--dir',Dir),
   concat_atom([Dir,'/',Name,'.dom'],File),
   open(File,write,Stream),
   write(Stream,Size), 
   write(Stream,'.'), 
   nl(Stream),
   close(Stream).


/* =======================================================================
   Output Prediction
========================================================================*/

outputPrediction(Prediction):-
   outputPrediction(user_output,Prediction),
   option('--dir',Dir),
   concat_atom([Dir,'/','prediction.txt'],File),
   open(File,write,Stream),
   outputPrediction(Stream,Prediction),
   close(Stream).

outputPrediction(Stream,Prediction):-
   write(Stream,Prediction), 
   nl(Stream).


/* =======================================================================
   MiniWordNet
========================================================================*/

mwn(XDRS):-   
   option('--wordnet',true), !,
   clearMWN,
   compConcepts(XDRS),
   compISA,
   cutDownMWN,   
   addTopMWN,                   %%% this can cause inconsistencies!
   outputMWN,
   graphMWN.

mwn(_).

   
/* =======================================================================
   Version
========================================================================*/

version:-
   option('--version',do), !,
   version(V),
   format(user_error,'~p~n',[V]).

version.


/* =======================================================================
   Help
========================================================================*/

help:-
   option('--help',do), !,
   format(user_error,'usage: nc [options]~n~n',[]),
   showOptions(nutcracker).

help:-
   option('--help',dont), !.


/* =======================================================================
   Definition of start
========================================================================*/

start:-
   prolog_flag(argv,[_Comm|Args]),
   set_prolog_flag(float_format,'%.20g'),
   setDefaultOptions(nutcracker), 
   parseOptions(nutcracker,Args),
   main, !,
   halt.

start:- 
   error('nc failed',[]), 
   halt.
