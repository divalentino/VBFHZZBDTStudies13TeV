#!/bin/bash

bdt_cut_upper="$1"
bdt_cut_lower="$2"
sig_file="$3"
bkg_file="$4"
cuts="${5:-1}"

root -l -b -q 'macros/ContaminationCalc.C('${bdt_cut_lower}','${bdt_cut_upper}',"'${sig_file}'","'${bkg_file}'","'${cuts}'")'
