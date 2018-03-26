#!/bin/bash

in_file="$1"
out_file="$2"
tree_name="$3"
sf_name="$4"

root -l -b -q 'macros/MakeStatsWorkspace.C+("'${in_file}'","'${out_file}'","'${tree_name}'","'${sf_name}'")'
