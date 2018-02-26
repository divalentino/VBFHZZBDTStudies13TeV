#!/bin/bash

#############################################
# Variable initialization.
#############################################

vars="test"
in_folder="bdt_skims/26-04-15/HSOnly/"
int_lumi="300"

do_bin=true
bin_threshold=MYCUT

#Some variable configurations for systematics.
use_jes=true
use_stat=true
use_theo=true
float_mu_ggf=false

if [[ $do_bin == *true* ]]; then
  use_st=true
else
  use_st=false
fi


ext=${vars}
if [[ $do_bin == *true* ]]; then
  ext=${ext}".bin.PtHjj.${bin_threshold}"
  pt_bin_threshold=$bin_threshold  
else
  pt_bin_threshold=-1
fi

#Sort in descending order, so that cat "1" is tightest, etc.
#Default case is single cut on [0.55, 1]. Number of arguments
#can now come from input argument(s).
if [ "$#" -eq 0 ]; then
  category_cuts=( 1 0.35 )
else
  category_cuts=( $@ )
fi
nCats=`echo ${#category_cuts[@]}`
nCats=$(($nCats-1))

#############################################
# Create histograms.
#############################################

if [[ ! -d "histos" ]]; then
    mkdir ./histos/
fi
mkdir ./xml/

for (( icat=1; icat <= $nCats; ++icat)); do

    cat_high=${category_cuts[$(($icat-1))]}    
    cat_low=${category_cuts[${icat}]}

    echo "Making histograms for BDT category in: "${cat_low}" to "${cat_high}

    #Create nominal histograms.
    root -l -b -q 'macros/create_input_tree.C+("'${in_folder}'/VBF125_BDT_Tree.'${ext}'.root",  "BDT_Tree", "histos/output_BDT_category'${icat}'_vbf_nominal.root", "vbf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${icat}'")'
    root -l -b -q 'macros/create_input_tree.C+("'${in_folder}'/ggF125_BDT_Tree.'${ext}'.root",  "BDT_Tree", "histos/output_BDT_category'${icat}'_ggf_nominal.root", "ggf_BDTG",'${cat_low}', '${cat_high}', '${int_lumi}', "'${icat}'",'${pt_bin_threshold}')'

    #Create JES scaled trees.
    root -l -b -q 'macros/create_input_tree.C+("'${in_folder}'/VBF125_BDT_Tree_JESdown.'${ext}'.root", "BDT_Tree_down", "histos/output_BDT_category'${icat}'_vbf_jesdown.root", "vbf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${icat}'")'
    root -l -b -q 'macros/create_input_tree.C+("'${in_folder}'/VBF125_BDT_Tree_JESup.'${ext}'.root",   "BDT_Tree_up",   "histos/output_BDT_category'${icat}'_vbf_jesup.root",   "vbf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${icat}'")'

    root -l -b -q 'macros/create_input_tree.C+("'${in_folder}'/ggF125_BDT_Tree_JESdown.'${ext}'.root",  "BDT_Tree_down", "histos/output_BDT_category'${icat}'_ggf_jesdown.root", "ggf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${icat}'",'${pt_bin_threshold}')'
    root -l -b -q 'macros/create_input_tree.C+("'${in_folder}'/ggF125_BDT_Tree_JESup.'${ext}'.root",  "BDT_Tree_up", "histos/output_BDT_category'${icat}'_ggf_jesup.root", "ggf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${icat}'",'${pt_bin_threshold}')'

#         root -l -b -q 'macros/create_input_tree.C+("'${in_folder}'/ggF125_BDT_Tree.'${ext}'.root",  "BDT_Tree", "histos/output_BDT_category'${icat}'_ggf_jesdown.root", "ggf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${icat}'")'
#     root -l -b -q 'macros/create_input_tree.C+("'${in_folder}'/ggF125_BDT_Tree.'${ext}'.root",  "BDT_Tree", "histos/output_BDT_category'${icat}'_ggf_jesup.root", "ggf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${icat}'")'

    #Have to make some dummy histograms for data.
    cp ./histos/data.root ./histos/output_BDT_category${icat}_data.root
    
done


#############################################
# Create XML files.
#############################################

./scripts/make_driver_xml.sh $nCats

#Input arguments:
#1.  Histo folder, 2. # systs,   3. # samples,    4. # categories,  5. Higgs mass,
#6.  # masses,     7. Use systs, 8. Use stat err, 9. Float mu_ggf, 10. Use theo systs
#11. Remove gen. theo systs
root -l -b -q 'macros/writeXML_vbf.C+("./histos/", 1, 2, '${nCats}', 126, 1, '${use_jes}', '${use_stat}', '${float_mu_ggf}', '${use_theo}', true, '${use_st}','${bin_threshold}')'

#############################################
# Run the Z0 calculation.
#############################################

cd xml
cp ../driver${nCats}.xml .
cp -r ../histos .
hist2workspace -standard_form driver${nCats}.xml

#root -l -b -q 'p0_calculation.C("output_combined_datastat_model.root" , "combined" , "ModelConfig" ,"asimovData", true)' | tee test.out
