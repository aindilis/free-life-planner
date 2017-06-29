/*********************************************************************
 * (C) Copyright 2000 Albert Ludwigs University Freiburg
 *     Institute of Computer Science
 *
 * All rights reserved. Use of this software is permitted for 
 * non-commercial research purposes, and it may be copied only 
 * for that use.  All copies must include this copyright message.
 * This software is made available AS IS, and neither the authors
 * nor the  Albert Ludwigs University Freiburg make any warranty
 * about the software or its performance. 
 *********************************************************************/


 




%{
#include "ff.h"
#include "parse.h"

/* default yywrap function - always treat EOF as an EOF  */
#define ops_pddlwrap() 1 

%}

a [Aa]
b [Bb]
c [Cc]
d [Dd]
e [Ee]
f [Ff]
g [Gg]
h [Hh]
i [Ii]
j [Jj]
k [Kk]
l [Ll]
m [Mm]
n [Nn]
o [Oo]
p [Pp]
q [Qq]
r [Rr]
s [Ss]
t [Tt]
u [Uu]
v [Vv]
w [Ww]
x [Xx]
y [Yy]
z [Zz]

digit [0-9]
int -?{digit}*
float -?{digit}+(\.{digit}*)?

%x COMMENT OVERREAD

%%

"("  {  return(OPEN_PAREN); }

")"  {  return(CLOSE_PAREN); }

"-" {return (MINUS_TOK);} 
"("[ \t]*{i}{n}"-"{p}{a}{c}{k}{a}{g}{e}  {  gbracket_count = 1;
 BEGIN OVERREAD; }

{d}{e}{f}{i}{n}{e}  {  return(DEFINE_TOK); }

{d}{o}{m}{a}{i}{n}  {  return(DOMAIN_TOK); }

"("[ \t]*":"{e}{x}{t}{e}{n}{d}{s}  {  gbracket_count = 1;
 BEGIN OVERREAD; }

":"{r}{e}{q}{u}{i}{r}{e}{m}{e}{n}{t}{s}  {  return(REQUIREMENTS_TOK); }

":"{t}{y}{p}{e}{s}  {  return(TYPES_TOK); }

{a}{t}" "{s}{t}{a}{r}{t} {return AT_START;}
{a}{t}" "{e}{n}{d} {return AT_END;}
{o}{v}{e}{r}" "{a}{l}{l} {return OVER_ALL;}


":"{c}{o}{n}{s}{t}{a}{n}{t}{s}  {  return(CONSTANTS_TOK); }

{i}{n}{c}{r}{e}{a}{s}{e}     {  return(INCREASE_TOK); }

{d}{e}{c}{r}{e}{a}{s}{e}     {  return(DECREASE_TOK); }

{a}{s}{s}{i}{g}{n}           {  return(  ASSIGN_TOK); }

{s}{c}{a}{l}{e}"-"{u}{p}     {  return(SCALE_UP_TOK); }

{s}{c}{a}{l}{e}"-"{d}{o}{w}{n} {  return(SCALE_DOWN_TOK); }

"#"{t}  { return (TIME_TOK);}

"?"{d}{u}{r}{a}{t}{i}{o}{n}     {  return(DURATION_VAR_TOK); }

"("[ \t]*":"{d}{o}{m}{a}{i}{n}"-"{v}{a}{r}{i}{a}{b}{l}{e}{s}  {  gbracket_count = 1;
 BEGIN OVERREAD; } 

">=" { return(GREATER_OR_EQUAL_TOK);}

"<=" { return(LESS_THAN_OR_EQUAL_TOK);}

{float} {strupcase(yytext); strcpy(yylval.string, yytext);/*yylval.fval = atof(yytext);*/return (FLOATVAL);}
 
{int} {strupcase(yytext); strcpy(yylval.string, yytext);/*yylval.ival = atoi(yytext);*/return (INTVAL);} 

":"{p}{r}{e}{d}{i}{c}{a}{t}{e}{s}  { return(PREDICATES_TOK); }

":"{f}{u}{n}{c}{t}{i}{o}{n}{s}  { return (FUNCTIONS_TOK);}

"("[ \t]*":"{t}{i}{m}{e}{l}{e}{s}{s} {  gbracket_count = 1;
 BEGIN OVERREAD; }

":"{a}{c}{t}{i}{o}{n}  {  return(ACTION_TOK); }

":"{d}{u}{r}{a}{t}{i}{v}{e}"-"{a}{c}{t}{i}{o}{n}  { return (DURATIVE_ACTION_TOK);}


"+" {return (PLUS_TOK);}



"*" {return (MUL_TOK);}

"/" {return (DIV_TOK);}

">" {return (GREATER_TOK);}

"<" {return (LESS_THAN_TOK);}

"=" {return (EQUAL_TOK);}

":"{p}{a}{r}{a}{m}{e}{t}{e}{r}{s}  {  return(PARAMETERS_TOK); }

":"{v}{a}{r}{s}  { return(VARS_TOK); }

":"{p}{r}{e}{c}{o}{n}{d}{i}{t}{i}{o}{n}  {  return(PRECONDITION_TOK); }

":"{c}{o}{n}{d}{i}{t}{i}{o}{n}  {  return(CONDITION_TOK); }

":"{d}{u}{r}{a}{t}{i}{o}{n}   { return (DURATION_TOK); }


":"{e}{f}{f}{e}{c}{t}  {  return(EFFECT_TOK); }


{a}{n}{d}  {  return(AND_TOK); }

{n}{o}{t}  {  return(NOT_TOK); }

{w}{h}{e}{n}  {  return(WHEN_TOK); }

{i}{m}{p}{l}{y} {  return(IMPLY_TOK); }

{o}{r} {  return(OR_TOK); }

{f}{o}{r}{a}{l}{l} {  return(FORALL_TOK); }

{e}{x}{i}{s}{t}{s} {  return(EXISTS_TOK); }

{e}{i}{t}{h}{e}{r} { return(EITHER_TOK); }

:?[a-zA-Z][a-zA-Z0-9\-_]* { strupcase(yytext); strcpy(yylval.string, yytext); 
 return(NAME); }

\?[a-zA-Z][a-zA-Z0-9\-_\[\]]* { strupcase(yytext); strcpy(yylval.string, yytext); 
 return(VARIABLE); }




\;(.)*\n  {  lineno++; } 
\;(.)*  {  /* this will hold only in files that end with
		   a comment but no linefeed */ } 

<COMMENT>(.^\")*\n    {  lineno++; }  ;

<INITIAL>\" { BEGIN COMMENT;}

<COMMENT>\" { BEGIN INITIAL;}

\n    {  lineno++; } 

<OVERREAD>(.^\(\))*\n  {  lineno++; }

<OVERREAD>[^\(\)]  {  }

<OVERREAD>\(  {  BEGIN OVERREAD; gbracket_count++; }

<OVERREAD>\)  {  BEGIN OVERREAD; gbracket_count--; 
  if (!gbracket_count) BEGIN INITIAL; }

. {}
%%
