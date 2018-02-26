#!/bin/bash

#############################################
# Variable initialization.
#############################################

vars="noPt"
in_folder="bdt_skims/26-04-15/HSOnly/"
int_lumi="300"

do_bin=false
bin_threshold=300

#Threshold for Pt cut.
nPtCats=2
ptThreshold=60

#Some variable configurations for systematics.
use_jes=true
use_stat=true
use_theo=true
float_mu_ggf=false

#These should be set by default.
use_st=true
bin_threshold=${ptThreshold}

ext=""
if [[ $do_bin == *true* ]]; then
  ext="bin.PtHjj.${bin_threshold}"
fi

if [[ "$vars" != "" ]] && [[ "$ext" != "" ]]; then
    ext=${vars}"."${ext}
else
    ext=${vars}
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

num_cats=0
for (( icat=1; icat <= $nCats; ++icat)); do
    for (( ptcat=0; ptcat < $nPtCats; ++ptcat)); do

	#Poor fix for case where we only want events PASSING
	#cut on Pt(H+jj).
	if [[ $nPtCats == 1 ]]; then
	    ptcat=1;
	fi
	
	cat_high=${category_cuts[$(($icat-1))]}    
	cat_low=${category_cuts[${icat}]}

	echo "Making histograms for BDT category in: "${cat_low}" to "${cat_high}

	num_cats=$(($num_cats+1))

	#Create nominal histograms.
	root -l -b -q 'macros/create_input_tree_ptcuts.C+("'${in_folder}'/VBF125_BDT_Tree.'${ext}'.root",  "BDT_Tree", "histos/output_BDT_category'${num_cats}'_vbf_nominal.root", "vbf_BDTG",'${cat_low}', '${cat_high}' , '${ptcat}' , '${ptThreshold}' , '${int_lumi}', "'${num_cats}'")'
	root -l -b -q 'macros/create_input_tree_ptcuts.C+("'${in_folder}'/ggF125_BDT_Tree.'${ext}'.root",  "BDT_Tree", "histos/output_BDT_category'${num_cats}'_ggf_nominal.root", "ggf_BDTG",'${cat_low}', '${cat_high}', '${ptcat}' , '${ptThreshold}' , '${int_lumi}', "'${num_cats}'")'

	#Create JES scaled trees.
	root -l -b -q 'macros/create_input_tree_ptcuts.C+("'${in_folder}'/VBF125_BDT_Tree_JESdown.'${ext}'.root", "BDT_Tree_down", "histos/output_BDT_category'${num_cats}'_vbf_jesdown.root", "vbf_BDTG",'${cat_low}', '${cat_high}' , '${ptcat}' , '${ptThreshold}' , '${int_lumi}', "'${num_cats}'")'
	root -l -b -q 'macros/create_input_tree_ptcuts.C+("'${in_folder}'/VBF125_BDT_Tree_JESup.'${ext}'.root",   "BDT_Tree_up",   "histos/output_BDT_category'${num_cats}'_vbf_jesup.root",   "vbf_BDTG",'${cat_low}', '${cat_high}' , '${ptcat}' , '${ptThreshold}' , '${int_lumi}', "'${num_cats}'")'

	root -l -b -q 'macros/create_input_tree_ptcuts.C+("'${in_folder}'/ggF125_BDT_Tree_JESdown.'${ext}'.root",  "BDT_Tree_down", "histos/output_BDT_category'${num_cats}'_ggf_jesdown.root", "ggf_BDTG",'${cat_low}', '${cat_high}' , '${ptcat}' , '${ptThreshold}' , '${int_lumi}', "'${num_cats}'")'
	root -l -b -q 'macros/create_input_tree_ptcuts.C+("'${in_folder}'/ggF125_BDT_Tree_JESup.'${ext}'.root",  "BDT_Tree_up", "histos/output_BDT_category'${num_cats}'_ggf_jesup.root", "ggf_BDTG",'${cat_low}', '${cat_high}' , '${ptcat}' , '${ptThreshold}' , '${int_lumi}', "'${num_cats}'")'

	# root -l -b -q 'macros/create_input_tree_ptcuts.C+("'${in_folder}'/ggF125_BDT_Tree.'${ext}'.root",  "BDT_Tree", "histos/output_BDT_category'${num_cats}'_ggf_jesdown.root", "ggf_BDTG",'${cat_low}', '${cat_high}' , '${ptcat}' , '${ptThreshold}' , '${int_lumi}', "'${num_cats}'")'
	# root -l -b -q 'macros/create_input_tree_ptcuts.C+("'${in_folder}'/ggF125_BDT_Tree.'${ext}'.root",  "BDT_Tree", "histos/output_BDT_category'${num_cats}'_ggf_jesup.root", "ggf_BDTG",'${cat_low}', '${cat_high}' , '${ptcat}' , '${ptThreshold}' , '${int_lumi}', "'${num_cats}'")'

	#Have to make some dummy histograms for data.
	cp ./histos/data.root ./histos/output_BDT_category${num_cats}_data.root

    done
done

#############################################
# Create XML files.
#############################################

./scripts/make_driver_xml.sh $num_cats

#Input arguments:
#1.  Histo folder, 2. # systs,   3. # samples,    4. # categories,  5. Higgs mass,
#6.  # masses,     7. Use systs, 8. Use stat err, 9. Float mu_ggf, 10. Use theo systs
#11. Remove gen. theo systs
root -l -b -q 'macros/writeXML_vbf_ptcuts.C+("./histos/", 1, 2, '${num_cats}', 126, 1, '${use_jes}', '${use_stat}', '${float_mu_ggf}', '${use_theo}', true, '${use_st}','${bin_threshold}')'

# #############################################
# # Run the Z0 calculation.
# #############################################

cd xml
cp ../driver${num_cats}.xml .
cp -r ../histos .
hist2workspace -standard_form driver${num_cats}.xml

root -l -b -q 'p0_calculation.C("output_combined_datastat_model.root" , "combined" , "ModelConfig" ,"asimovData", true)' | tee test.out
