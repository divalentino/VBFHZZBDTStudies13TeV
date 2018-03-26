#!/bin/bash

#Total of variables to use, of which a subset 'k' is chosen.
nvars=13

#Note that BDT training doesn't work with only 1 variable.
for (( NUMVARS=2; NUMVARS < $(($nvars+1)); NUMVARS++ )); do
  root -l -b -q 'macros/PrintBDTVarLists.C('${nvars}','${NUMVARS}')'
done