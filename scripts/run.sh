#!/bin/bash

swipl -s test.pl -g 'findall(M,(current_module(M)),IntoVar),see(IntoVar).'
