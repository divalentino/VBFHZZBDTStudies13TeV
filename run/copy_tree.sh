#!/bin/bash

infile="$1"
outfile=${2:-""}
num_entries=${3:-"-1"}

if [[ "$infile" != "" ]]; then
  root -l -b -q 'macros/CopyTree.C+("'${infile}'","Cuts","'$outfile'",'${num_entries}')'
else
  echo "Must specify input filename/folder and output prefix!"
fi
