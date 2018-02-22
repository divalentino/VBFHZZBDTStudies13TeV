#!/bin/bash

#############################################
# Variable initialization.
#############################################

int_lumi=100
#tree_name="newtree"
tree_name="tree_incl_all"
do_bin=false

#Our new discriminant.
#discriminant="BDTscore"

#Run-I discriminant.
#discriminant="eo_VBF_uux_ME"
discriminant="BDT_discriminant_VBF"

#Some variable configurations for systematics.
use_jes=false
use_stat=true
use_theo=true
float_mu_ggf=true

if [[ $do_bin == *true* ]]; then
    use_st=true
else
    use_st=false
fi

if [[ $do_bin == *true* ]]; then
    pt_threshold=50
else
    pt_threshold=-1
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

# vbf_file="bdt_skims_valerio/mc15_VBFH125_35.root"
# ggf_file="bdt_skims_valerio/mc15_ggH125_35.root"
# qqzz_file="bdt_skims_valerio/mc15_ZZllll_35.root"

vbf_file="bdt_skims_haider/mc15_13TeV.341518.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_VBFH125_ZZ4lep_noTau.root"
ggf_file="bdt_skims_haider/mc15_13TeV.341505.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_ggH125_ZZ4lep_noTau.root"
qqzz_file="bdt_skims_haider/mc15_13TeV.342556.PowhegPy8EG_CT10nloME_AZNLOCTEQ6L1_ZZllll_mll4_m4l_100_150.root"

for (( icat=1; icat <= $nCats; ++icat)); do

    cat_high=${category_cuts[$(($icat-1))]}    
    cat_low=${category_cuts[${icat}]}

    echo "Making histograms for BDT category in: "${cat_low}" to "${cat_high}

    #Create nominal histograms.
    root -l -b -q 'macros/create_input_tree_valerio.C+("'${vbf_file}'",  "'$tree_name'", "histos/output_BDT_category'${icat}'_vbf_nominal.root", "vbf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${discriminant}'",'${pt_threshold}',"'${icat}'")'

    root -l -b -q 'macros/create_input_tree_valerio.C+("'${ggf_file}'",  "'$tree_name'", "histos/output_BDT_category'${icat}'_ggf_nominal.root", "ggf_BDTG",'${cat_low}', '${cat_high}', '${int_lumi}', "'${discriminant}'",'${pt_threshold}',"'${icat}'")'

    #Create background histograms.
    root -l -b -q 'macros/create_input_tree_valerio.C+("'${qqzz_file}'",  "'$tree_name'", "histos/output_BDT_category'${icat}'_qqzz_nominal.root", "qqzz_BDTG",'${cat_low}', '${cat_high}', '${int_lumi}', "'${discriminant}'",'${pt_threshold}',"'${icat}'")'

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

root -l -b -q 'macros/writeXML_vbf.C+("./histos/", 1, 3, '${nCats}', 126, 1, '${use_jes}', '${use_stat}', '${float_mu_ggf}', '${use_theo}', true, '${use_st}')'

#############################################
# Run the Z0 calculation.
#############################################

cd xml
cp ../driver${nCats}.xml .
cp -r ../histos .

hist2workspace -standard_form driver${nCats}.xml
root -l -b -q 'p0_calculation.C("output_combined_datastat_model.root" , "combined" , "ModelConfig" ,"asimovData", false)' | tee test.out
