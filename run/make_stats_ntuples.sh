#!/bin/bash

##############################
#Configuration.
##############################

do_jes=false

#File I/O.
vars=${1:-"test"}
in_folder=${2:-"13-03-15-smear"}

#Binning settings.
do_bin=${3:-false}
bin_threshold=${4:-40}
bin_txt=""

suffix=${5:-"HSOnly"}
out_folder="ValerioHistograms/${in_folder}/${suffix}/"

if [[ $do_bin == *true* ]]; then
  bin_txt=".bin.PtHjj."${bin_threshold}
  #vars="${vars}.bin.PtHjj.${bin_threshold}"
fi

echo "Writing histograms to $out_folder"
mkdir -p $out_folder

##############################
#Make nominal samples.
##############################

in_file="output/${in_folder}/TMVA_App_opt_VBF_VBF_v_ggH_withPU_${vars}${bin_txt}_${suffix}.root"
out_file="${out_folder}/VBF125_BDT_Tree.${vars}${bin_txt}.root"
tree_name="BDT_Tree"
sf_name="sf_to_orig_weight"

root -l -b -q 'macros/MakeStatsNtuple.C+("'${in_file}'","'${out_file}'","'${tree_name}'","'${sf_name}'")'

in_file="output/${in_folder}/TMVA_App_opt_ggH_VBF_v_ggH_withPU_${vars}${bin_txt}_${suffix}.root"
out_file="${out_folder}/ggF125_BDT_Tree.${vars}${bin_txt}.root"
tree_name="BDT_Tree"
sf_name="sf_to_orig_weight"

root -l -b -q 'macros/MakeStatsNtuple.C+("'${in_file}'","'${out_file}'","'${tree_name}'","'${sf_name}'")'

##############################
#Make JES-smeared samples.
##############################

#VBF up/down.
in_file="HiggsNtuplesLETFSmall/ScaledSamples/"${in_folder}/${suffix}"/mc15_14TeV.Powheg.VBF.02-06-15.ScaledSamples.${vars}${bin_txt}_${suffix}.root"
out_file="${out_folder}/VBF125_BDT_Tree_JESup.${vars}${bin_txt}.root"
tree_name="BDT_Tree_up"
sf_name="sf_to_orig_weight_JESup"

root -l -b -q 'macros/MakeStatsNtuple.C+("'${in_file}'","'${out_file}'","'${tree_name}'","'${sf_name}'")'

in_file="HiggsNtuplesLETFSmall/ScaledSamples/"${in_folder}/${suffix}"/mc15_14TeV.Powheg.VBF.02-06-15.ScaledSamples.${vars}${bin_txt}_${suffix}.root"
out_file="${out_folder}/VBF125_BDT_Tree_JESdown.${vars}${bin_txt}.root"
tree_name="BDT_Tree_down"
sf_name="sf_to_orig_weight_JESdown"

root -l -b -q 'macros/MakeStatsNtuple.C+("'${in_file}'","'${out_file}'","'${tree_name}'","'${sf_name}'")'

#ggF up/down.
in_file="HiggsNtuplesLETFSmall/ScaledSamples/"${in_folder}/${suffix}"/mc15_14TeV.Powheg.HJJ.02-06-15.ScaledSamples.${vars}${bin_txt}_${suffix}.root"
out_file="${out_folder}/ggF125_BDT_Tree_JESup.${vars}${bin_txt}.root"
tree_name="BDT_Tree_up"
sf_name="sf_to_orig_weight_JESup"

root -l -b -q 'macros/MakeStatsNtuple.C+("'${in_file}'","'${out_file}'","'${tree_name}'","'${sf_name}'")'

in_file="HiggsNtuplesLETFSmall/ScaledSamples/"${in_folder}/${suffix}"/mc15_14TeV.Powheg.HJJ.02-06-15.ScaledSamples.${vars}${bin_txt}_${suffix}.root"
out_file="${out_folder}/ggF125_BDT_Tree_JESdown.${vars}${bin_txt}.root"
tree_name="BDT_Tree_down"
sf_name="sf_to_orig_weight_JESdown"

root -l -b -q 'macros/MakeStatsNtuple.C+("'${in_file}'","'${out_file}'","'${tree_name}'","'${sf_name}'")'
