#!/bin/bash

cd /var/lib/myfrdcsa/sandbox/mprolog-2.0/mprolog-2.0
swipl -g "consult('mprolog.pl'),swi,consult_calculi('groups.cal'),mconsult('wise_men_n.mpl'),compile_loaded_program,halt."
swipl -g "consult('compile_temp.pl'), swi, set_number_of_men(3),write_term(mcall([bel([1])]:white([X])),[quoted(true)]),write('.'),nl."

