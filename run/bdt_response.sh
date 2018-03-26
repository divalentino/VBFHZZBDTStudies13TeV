#!/bin/bash

sig_file="$1"
bkg_file="$2"
cuts="${3:-1}"

root -l 'macros/DrawBDTResponse.C+("'${sig_file}'","'${bkg_file}'","'${cuts}'")'