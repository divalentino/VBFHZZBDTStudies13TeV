#!/bin/bash

infile="$1"
outfile="$2"
num_entries=${3:-0}

if [[ "$infile" != "" ]] && [[ "$outfile" != "" ]]; then
  root -l -b -q 'macros/CopyTree.C+("'${infile}'","Train","'$outfile'.HiggsTree.Skim.Train.root",'${num_entries}')'
  root -l -b -q 'macros/CopyTree.C+("'${infile}'","Test","'$outfile'.HiggsTree.Skim.Test.root",'${num_entries}')'
else
  echo "Must specify input filename/folder and output prefix!"
fi
