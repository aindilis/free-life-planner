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
#define fct_pddlwrap() 1 

int gbracket_count = 0;

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

"("  { return(OPEN_PAREN); }

")"  {  return(CLOSE_PAREN); }

"-" {return (MINUS_TOK);}
\([ \t]*{i}{n}"-"{p}{a}{c}{k}{a}{g}{e}  {  gbracket_count = 1;
 BEGIN OVERREAD; }

\([ \t]*":"{l}{e}{n}{g}{t}{h}  {  gbracket_count = 1;
 BEGIN OVERREAD; }

\([ \t]*":"{r}{e}{q}{u}{i}{r}{e}{m}{e}{n}{t}{s}  {  gbracket_count = 1;
 BEGIN OVERREAD; }

{d}{e}{f}{i}{n}{e}  {  return(DEFINE_TOK); }

{p}{r}{o}{b}{l}{e}{m}  {  return(PROBLEM_TOK); }

{s}{i}{t}{u}{a}{t}{i}{o}{n}  {  return(SITUATION_TOK); }

":"{s}{i}{t}{u}{a}{t}{i}{o}{n}  {  return(BSITUATION_TOK); }

":"{o}{b}{j}{e}{c}{t}{s}  {  return(OBJECTS_TOK); }

":"{g}{o}{a}{l}  {  return(GOAL_TOK); }

":"{i}{n}{i}{t}  {  return(INIT_TOK); }

{float} {strupcase(yytext);strcpy(yylval.string, yytext);return (FLOATVAL);}
 
{int} {strupcase(yytext);strcpy(yylval.string, yytext);return (INTVAL);} 



":"{m}{e}{t}{r}{i}{c}           {  return(    METRIC_TOK); }

{m}{i}{n}{i}{m}{i}{z}{e}     {  return( MINIMIZE_TOK); }

{m}{a}{x}{i}{m}{i}{z}{e}     {  return( MAXIMIZE_TOK); }

":"{l}{e}{n}{g}{t}{h}           {  return(    LENGTH_TOK); }

":"{s}{e}{r}{i}{a}{l}           {  return(   SERIAL_TOK); }

":"{p}{a}{r}{a}{l}{l}{e}{l}     {  return(  PARALLEL_TOK); }

{t}{o}{t}{a}{l}"-"{t}{i}{m}{e} {  return(  TOTAL_TIME_TOK); }

":"{d}{o}{m}{a}{i}{n}  {  return(BDOMAIN_TOK); }

\([ \t]*":"{e}{x}{t}{e}{n}{d}{s}  {  gbracket_count = 1;
 BEGIN OVERREAD; }

{a}{n}{d}  {  return(AND_TOK); }

{i}{m}{p}{l}{y} {  return(IMPLY_TOK); }

{o}{r} {  return(OR_TOK); }

{f}{o}{r}{a}{l}{l} {  return(FORALL_TOK); }

{e}{x}{i}{s}{t}{s} {  return(EXISTS_TOK); }

{n}{o}{t}  {  return(NOT_TOK); }

"+" {return (PLUS_TOK);}

"*" {return (MUL_TOK);}

"/" {return (DIV_TOK);}

">" {return (GREATER_TOK);}

"<" {return (LESS_THAN_TOK);}

"="  {  return(EQUAL_TOK); }

">=" { return(GREATER_OR_EQUAL_TOK);}

"<=" { return(LESS_THAN_OR_EQUAL_TOK);}

:?[a-zA-Z][a-zA-Z0-9\-_]* { strupcase( yytext );  
  strcpy(yylval.string, yytext ); return(NAME); }

\?[a-zA-Z][a-zA-Z0-9\-_\[\]]* {strupcase( yytext );
 strcpy(yylval.string, yytext); return(VARIABLE); }


\;(.)*\n  {  lineno++; } 
\;(.)*  {  /* this will hold only in files that end with
		   a comment but no linefeed */ } 

<COMMENT>(.^\")*\n    {  lineno++; }  ;

<INITIAL>\" { BEGIN COMMENT;}

<COMMENT>\" { BEGIN INITIAL;}

\n    {  lineno++; } 

<OVERREAD>(.^\(\))*\n  {  lineno++; }

<OVERREAD>[^\(\)]  {  }

<OVERREAD>\(  {  gbracket_count++; }

<OVERREAD>\)  {  gbracket_count--; 
  if (!gbracket_count) BEGIN INITIAL; }

. {}
%%
