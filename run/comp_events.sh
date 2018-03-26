#!/bin/bash

file="$1"
cuts=${2:-"1"}
cuts=`echo $cuts | sed 's, ,,g'`

root -l -b -q 'macros/ComputeEvents.C("'${file}'","'${cuts}'")'
