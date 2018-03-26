#!/bin/bash

bdt_cut="$1"
sig_file="$2"
bkg_file="$3"
cuts="${4:-1}"

root -l -b -q 'macros/SignificanceCalc.C('${bdt_cut}',"'${sig_file}'","'${bkg_file}'","'"${cuts}"'")'
