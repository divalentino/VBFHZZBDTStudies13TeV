#!/bin/bash

# Use sig. calculation macro to impose different cuts.

vbf_file="$1"
ggf_file="$2"
cut=""

root -l -b -q 'macros/SignificanceScan.C+("'${vbf_file}'","'${ggf_file}'","'"${cut}"'")'
