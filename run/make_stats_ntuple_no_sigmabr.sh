#!/bin/bash

in_file="$1"
out_file="$2"
tree_name="BDT_Tree"
sf_name="sf_to_orig_weight"
use_br=false

root -l -b -q 'macros/MakeStatsNtupleNoSigmaBR.C+("'${in_file}'","'${out_file}'","'${tree_name}'","'${sf_name}'",'${use_br}')'
