#!/bin/bash

infile="$1"

if [[ "$infile" != "" ]]; then
  root -l -b -q 'macros/BinHiggsPt.C+("'${infile}'")'
else
  echo "Must specify input filename/folder!"
fi
