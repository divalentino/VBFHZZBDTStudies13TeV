#!/bin/bash

bdt_cut=${1:-"0"}
vbf_file=${2:-"output/TMVA_App_opt_VBF_VBF_v_ggH_default.root"}
ggf_file=${3:-"output/TMVA_App_opt_ggH_VBF_v_ggH_default.root"}

root -l -b -q 'macros/ComputeSensitivities.C+("'${vbf_file}'","'${ggf_file}'",'${bdt_cut}')'