#!/bin/bash

infile="$1"

pt_min=${2:-30}
pt_max=${3:-60}
pt_del=${4:-5}

if [[ "$infile" != "" ]]; then
  root -l -b -q 'macros/BinHiggsPtMiniTree.C+("'${infile}'",'${pt_min}','${pt_max}','${pt_del}')'
else
  echo "Must specify input filename/folder!"
fi
