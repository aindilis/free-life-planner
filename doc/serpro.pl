toCanonical(P,T):-copy_term(P,CP),numbervars(CP,0,_),prolog2TermAlgebra(CP,T).

fromCanonical(T,P):-termAlgebra2prolog(T,P).

prolog2TermAlgebra(P,T):-P='$VAR'(_),!,toTypeValue(P,T). 
prolog2TermAlgebra(P,T):-atomic(P),!,toTypeValue(P,T).
prolog2TermAlgebra(P,fun(FCode,Ts)):-compound(P),P=..[F|Ps],atom2nat(F,FCode),
  maplist(prolog2TermAlgebra,Ps,Ts).

termAlgebra2prolog(leaf(0,N),P):-!,P='$VAR'(N).  
termAlgebra2prolog(T,P):-T=leaf(_,_),!,fromTypeValue(T,P).
termAlgebra2prolog(fun(FCode,Ts),P):-nat2atom(FCode,F),
  maplist(termAlgebra2prolog,Ts,Ps),P=..[F|Ps]. 

toTypeValue('$VAR'(N),TV):-!,TV=leaf(0,N).
toTypeValue(P,TV):-atom(P),!,atom2nat(P,Code),TV=leaf(1,Code).
toTypeValue(P,TV):-integer(P),!,encodeSign(P,Code),TV=leaf(2,Code).  

fromTypeValue(leaf(0,N),P):-!,P='$VAR'(N).
fromTypeValue(leaf(1,Code),P):-!,nat2atom(Code,P).
fromTypeValue(leaf(2,Code),P):-decodeSign(Code,P).

totalTypes(3). 

encodeSign(0,Code):-!,Code=0.
encodeSign(P,Code):-P>0,!,Code is 2*P+1.
encodeSign(N,Code):-N<0,Code is 2*(-N)+2.

decodeSign(0,Z):-!,Z=0.
decodeSign(Code,P):-Code>0,Code mod 2=:=1,!,P is (Code-1) // 2.
decodeSign(Code,N):-Code>0,Code mod 2=:=0,N is (2-Code) // 2.


fromBBase(_,[],0).
fromBBase(B,[D|Ds],NewN):-fromBBase(B,Ds,N),putBDigit(B,D,N,NewN).

toBBase(_,0,[]).
toBBase(B,N,[D|Ds]):-N>0,getBDigit(B,N,D,M),toBBase(B,M,Ds).

putBDigit(B,D,M,R):-R is 1+D+B*M.

getBDigit(B,N,Digit,NewN):-Q is N // B,D is N mod B,
  getBDigit1(D,Q,B,Digit,NewN).

getBDigit1(0,Q,B,Digit,NewN):-Digit is B-1,NewN is Q-1.
getBDigit1(D,Q,_B,Digit,Q):-D>0,Digit is D-1.

c0(A):-[A]="a".
c1(Z):-[Z]="z".

base(B):-c0(A),c1(Z),B is 1+Z-A.

string2nat(".",N):-!,N=0.
string2nat("[]",N):-!,N=1.
string2nat(Cs,N):-Cs=[_|_],base(B), maplist(chr2ord,Cs,Ns),
  fromBBase(B,Ns,N0),N is N0+1.

nat2string(0,Cs):-!,Cs=".".
nat2string(1,Cs):-!,Cs="[]".
nat2string(N0,Cs):-N0 > 1,N is N0-1,
  base(B),toBBase(B,N,Xs),maplist(ord2chr,Xs,Cs).

chr2ord(C,O):-c0(A),C>=A,c1(Z),C=<Z,O is C-A.
ord2chr(O,C):-O>=0,base(B),O<B,c0(A),C is A+O.

atom2nat(Atom,Nat):-atom_codes(Atom,Cs),string2nat(Cs,Nat).

nat2atom(Nat,Atom):-nat2string(Nat,Cs),atom_codes(Atom,Cs).

term2bitpars(T,Ps,As):-toCanonical(T,CT),term2bitpars(CT,Ps,[],As,[]).

term2bitpars(T,[0,1|Ps],Ps)-->{T=leaf(_,_)},!,[T].
term2bitpars(T,[0|Ps],NewPs)-->{T=fun(F,Xs)},[fun(F)],args2bitpars(Xs,Ps,NewPs).
  
args2bitpars([],[1|Ps],Ps)-->[].
args2bitpars([X|Xs],Ps,NewPs)-->term2bitpars(X,Ps,XPs),
  args2bitpars(Xs,XPs,NewPs).  

bitpars2term(Ps,As,T):-bitpars2term(CT,Ps,[],As,[]),fromCanonical(CT,T).

bitpars2term(T,[0,1|Ps],Ps)-->!,[T].
bitpars2term(T,[0|Ps],NewPs)-->[fun(F)],bitpars2args(Xs,Ps,NewPs),{T=fun(F,Xs)}.
  
bitpars2args([],[1|Ps],Ps)-->[].
bitpars2args([X|Xs],Ps,NewPs)-->bitpars2term(X,Ps,XPs),bitpars2args(Xs,XPs,NewPs).  

bijEncodeStructure(T,N,As):-term2bitpars(T,Ps,As),rankCatalan(Ps,N).

bijDecodeStructure(N,As,T):-unrankCatalan(N,Ps),bitpars2term(Ps,As,T).

fromCantorTuple(Ns,N) :- list2set(Ns,Xs),fromKSet(Xs,N).

fromKSet(Xs,N):-untuplingLoop(Xs,0,0,N).

untuplingLoop([],_L,B,B).
untuplingLoop([X|Xs],L1,B1,Bn) :- L2 is L1+1, binomial(X,L2,B), B2 is B1+B,
  untuplingLoop(Xs,L2,B2,Bn).  

toKSet(K,N,Ds):-combinatoriallDigits(K,N,[],Ds).

combinatoriallDigits(0,_,Ds,Ds).
combinatoriallDigits(K,N,Ds,NewDs):-K>0,K1 is K-1,
  upperBinomial(K,N,M),M1 is M-1,binomial(M1,K,BDigit),N1 is N-BDigit,
  combinatoriallDigits(K1,N1,[M1|Ds],NewDs).

upperBinomial(K,N,R):-S is N+K,roughLimit(K,S,K,M),L is M // 2,
  binarySearch(K,N,L,M,R).

roughLimit(K,N,I, L):-binomial(I,K,B),B>N,!,L=I.
roughLimit(K,N,I, L):-J is 2*I,roughLimit(K,N,J,L).

binarySearch(_K,_N,From,From,R):-!,R=From.
binarySearch(K,N,From,To,R):-Mid is (From+To) // 2,
  binomial(Mid,K,B),
  splitSearchOn(B,K,N,From,Mid,To,R).

splitSearchOn(B,K,N,From,Mid,_To,R):-B>N,!,binarySearch(K,N,From,Mid,R).
splitSearchOn(_B,K,N,_From,Mid,To,R):-Mid1 is Mid+1,binarySearch(K,N,Mid1,To,R).  

toCantorTuple(K,N,Ns):-toKSet(K,N,Ds),set2list(Ds,Ns).

encodeTerm(T,Code):-
  term2bitpars(T,Ps,Xs),rankCatalan(Ps,N),encodeSyms(Xs,Ns),
  fromCantorTuple(Ns,M),fromCantorTuple([N,M],Code).

decodeTerm(Code,T):-
  toCantorTuple(2,Code,[SkelCode,SymsCode]),unrankCatalan(SkelCode,Ps),
  length(Ps,L),K is L // 2,
  toCantorTuple(K,SymsCode,Ns),decodeSyms(Ns,Ps,Syms),bitpars2term(Ps,Syms,T).

encodeSyms([],[]).
encodeSyms([leaf(T,C)|Xs],[N|Ns]):-!,
  totalTypes(B),putBDigit(B,T,C,N1),N is N1-1,encodeSyms(Xs,Ns).
encodeSyms([fun(N)|Xs],[N|Ns]):-encodeSyms(Xs,Ns).

decodeSyms([],[],[]).
decodeSyms([N|Ns],[0,1|Ps],[X|Xs]):-!,
  totalTypes(B),N1 is N+1,getBDigit(B,N1,T,C),
  X=leaf(T,C),decodeSyms(Ns,Ps,Xs).
decodeSyms([N|Ns],[0|Ps],[X|Xs]):-!,X=fun(N),decodeSyms(Ns,Ps,Xs).
decodeSyms(Ns,[1|Ps],Xs):-decodeSyms(Ns,Ps,Xs).   

binomialLoop(_,K,I,P,R) :- I>=K, !, R=P.
binomialLoop(N,K,I,P,R) :- I1 is I+1, P1 is ((N-I)*P) // I1,
   binomialLoop(N,K,I1,P1,R).

binomial(_N,K,R):- K<0,!,R=0.
binomial(N,K,R) :- K>N,!, R=0.
binomial(N,K,R) :- K1 is N-K, K>K1, !, binomialLoop(N,K1,0,1,R).
binomial(N,K,R) :- binomialLoop(N,K,0,1,R).

list2set(Ns,Xs) :- list2set(Ns,-1,Xs).

list2set([],_,[]).
list2set([N|Ns],Y,[X|Xs]) :- X is (N+Y)+1, list2set(Ns,X,Xs).

set2list(Xs,Ns) :- set2list(Xs,-1,Ns).

set2list([],_,[]).
set2list([X|Xs],Y,[N|Ns]) :- N is (X-Y)-1, set2list(Xs,X,Ns).

cat(0,1).
cat(N,R):-N>0, PN is N-1, SN is N+1,cat(PN,R1),R is 2*(2*N-1)*R1//SN.

binDif(N,X,Y,R):- N1 is 2*N-X,R1 is N - (X + Y) // 2, R2 is R1-1,
  binomial(N1,R1,B1),binomial(N1,R2,B2),R is B1-B2.  

localRank(N,As,NewLo):- X is 1, Y is 0, Lo is 0,binDif(N,0,0,Hi0),Hi is Hi0-1,
  localRankLoop(As,N,X,Y,Lo,Hi,NewLo,_NewHi).
  
localRankLoop(As,N,X,Y,Lo,Hi,FinalLo,FinalHi):-N2 is 2*N,X< N2,!,
  PY is Y-1, SY is Y+1, nth0(X,As,A),
  (0=:=A->
     binDif(N,X,PY,Hi1),
     NewHi is Hi-Hi1, NewLo is Lo, NewY is SY
   ; binDif(N,X,SY,Lo1),
     NewLo is Lo+Lo1, NewHi is Hi, NewY is PY
  ),
  NewX is X+1,
  localRankLoop(As,N,NewX,NewY,NewLo,NewHi,FinalLo,FinalHi).
localRankLoop(_As,_N,_X,_Y,Lo,Hi,Lo,Hi).  

rankCatalan(Xs,R):-length(Xs,XL),XL>=2, L is XL-2, I is L // 2,
  localRank(I,Xs,N),S is 0, PI is I-1,rankLoop(PI,S,NewS),R is NewS+N.

rankLoop(I,S,FinalS):-I>=0,!,cat(I,C),NewS is S+C, PI is I-1,
  rankLoop(PI,NewS,FinalS).
rankLoop(_,S,S).

localUnrank(N,R,As):-Y is 0,Lo is 0,binDif(N,0,0,Hi0),Hi is Hi0-1, X is 1,
  localUnrankLoop(X,Y,N,R,Lo,Hi,As).

localUnrankLoop(X,Y,N,R,Lo,Hi,As):-N2 is 2*N,X=<N2,!,
   PY is Y-1, SY is Y+1,
   binDif(N,X,SY,K), LK is Lo+K,
   ( R<LK -> 
     NewHi is LK-1, NewLo is Lo, NewY is SY, Digit=0
   ; NewLo is LK, NewHi is Hi, NewY is PY, Digit=1
   ),
   nth0(X,As,Digit),NewX is X+1,
   localUnrankLoop(NewX,NewY,N,R,NewLo,NewHi,As).
localUnrankLoop(_X,_Y,_N,_R,_Lo,_Hi,_As). 

unrankCatalan(R,Xs):- S is 0, I is 0,unrankLoop(R,S,I,NewS,NewI),
  LR is R-NewS, L is 2*NewI+1,length(As,L),localUnrank(NewI,LR,As),
  As=[_|Bs], append([0|Bs],[1],Xs).

unrankLoop(R,S,I,FinalS,FinalI):-cat(I,C),NewS is S+C, NewS=<R,
   !,NewI is I+1,
   unrankLoop(R,NewS,NewI,FinalS,FinalI).
unrankLoop(_,S,I,S,I).

injEncodeStructure(T,N,As):-term2bitpars(T,Ps,As),fromBBase(2,Ps,N).

injDecodeStructure(N,As,T):-toBBase(2,N,Ps),bitpars2term(Ps,As,T).

% some tests

println(X):-writeq(X),nl.
 
go:-go(10).
 
go(N0):-
  decodeTerm(N0,T),
  println(term=T),
  encodeTerm(T,N),
  println(go(N0=N)),
  fail.

g1:-between(0,31,N),decodeTerm(N,T),encodeTerm(T,M),println(n(N)=m(M)+T),fail.

g2:-toCanonical(f(a,X,-33,44,g(X)),T),term2bitpars(T,Ps,As),bitpars2term(Ps,As,TT),fromCanonical(TT,P),
  maplist(println,[T,Ps,As,TT,P]),fail.

g:-encodeTerm(f(a,X,-33,44,g(X)),N),decodeTerm(N,T),maplist(println,[N,T]),fail.

bigt(0,_).
bigt(N,T):-N>0,N1 is N-1,bigt(N1,T1),T=f(_,T1).

bigtt(0,_).
bigtt(N,T):-N>0,N1 is N-1,bigtt(N1,T1),T=f(_,T1,T1).

btest(N):-
  bigt(N,T),
  encodeTerm(T,Code),
  println(T),
  println(Code),
  decodeTerm(Code,T1),
  println(T1),
  T1=T.

bttest(N):-
  bigtt(N,T),
  encodeTerm(T,Code),
  println(T),
  println(Code),
  decodeTerm(Code,T1),
  println(T1),
  T1=T.  
 

