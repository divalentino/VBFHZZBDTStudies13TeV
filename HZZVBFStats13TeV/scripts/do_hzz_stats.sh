#!/bin/bash

#############################################
# Variable initialization.
#############################################

int_lumi=100
tree_name="BDT_Tree"
do_bin=true
pt_threshold=20

#Our new discriminant.
discriminant="BDTscore"

#Run-I discriminant.
#discriminant="BDT_discriminant_VBF"

#Some variable configurations for systematics.
use_flat_jes=false
use_syst=false
use_stat=true
use_theo=true
float_mu_ggf=true
num_syst=4

if [[ $do_bin == *true* ]]; then
    use_st=true
else
    use_st=false
fi

if [[ $do_bin == *false* ]]; then
    pt_threshold=90
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

if [[ $do_bin == *true* ]]; then

    #suffix="OSXTestBinned50GeV"
    suffix="thresh.${pt_threshold}"

    vbf_file="higgs_signal_mc_125_nominal/mc15_13TeV.341518.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_VBFH125_ZZ4lep_noTau.1fb-1.bin.PtHjj.${suffix}.BDT_Tree.root"
    ggf_file="higgs_signal_mc_125_nominal/mc15_13TeV.341505.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_ggH125_ZZ4lep_noTau.1fb-1.bin.PtHjj.${suffix}.BDT_Tree.root"
    qqzz_file="higgs_backgrounds/mc15_13TeV.342556.PowhegPy8EG_CT10nloME_AZNLOCTEQ6L1_ZZllll_mll4_m4l_100_150.1fb-1.bin.PtHjj.${suffix}.BDT_Tree.root"

else

    #suffix="OSXDefaultVarsJetPt30GeV"
    suffix="OptimizedVars13TeVNoPtHjjJetPt30GeV"

    vbf_file="higgs_signal_mc_125_nominal/mc15_13TeV.341518.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_VBFH125_ZZ4lep_noTau.1fb-1.bin.PtHjj.${suffix}.BDT_Tree.root"
    ggf_file="higgs_signal_mc_125_nominal/mc15_13TeV.341505.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_ggH125_ZZ4lep_noTau.1fb-1.bin.PtHjj.${suffix}.BDT_Tree.root"
    qqzz_file="higgs_backgrounds/mc15_13TeV.342556.PowhegPy8EG_CT10nloME_AZNLOCTEQ6L1_ZZllll_mll4_m4l_100_150.1fb-1.bin.PtHjj.${suffix}.BDT_Tree.root"

fi

for (( icat=1; icat <= $nCats; ++icat)); do

    cat_high=${category_cuts[$(($icat-1))]}    
    cat_low=${category_cuts[${icat}]}

    echo "Making histograms for BDT category in: "${cat_low}" to "${cat_high}

    #Create nominal histograms.
    root -l -b -q 'macros/create_input_tree.C+("'${vbf_file}'",  "'$tree_name'", "histos/output_BDT_category'${icat}'_vbf_nominal.root", "vbf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${discriminant}'",'${pt_threshold}',"'${icat}'")'

    root -l -b -q 'macros/create_input_tree.C+("'${ggf_file}'",  "'$tree_name'", "histos/output_BDT_category'${icat}'_ggf_nominal.root", "ggf_BDTG",'${cat_low}', '${cat_high}', '${int_lumi}', "'${discriminant}'",'${pt_threshold}',"'${icat}'")'

    #Create background histograms.
    #root -l -b -q 'macros/create_input_tree.C+("'${qqzz_file}'",  "'$tree_name'", "histos/output_BDT_category'${icat}'_qqzz_nominal.root", "qqzz_BDTG",'${cat_low}', '${cat_high}', '${int_lumi}', "'${discriminant}'",'${pt_threshold}',"'${icat}'")'

    if [[ $use_syst == *true* ]]; then

	for NAME in "JET_GroupedNP_1__1" "JET_GroupedNP_2__1" "JET_GroupedNP_3__1" "JET_JER_SINGLE_NP__1"; do

	    vbf_syst_name_up=`echo $vbf_file | sed 's,bdt_skims,bdt_skims_JES_JER,g' | sed 's,BDT_Tree,'${NAME}'up.BDT_Tree,g'`
	    vbf_syst_name_down=`echo $vbf_file | sed 's,bdt_skims,bdt_skims_JES_JER,g' | sed 's,BDT_Tree,'${NAME}'down.BDT_Tree,g'`
	    
	    root -l -b -q 'macros/create_input_tree.C+("'${vbf_syst_name_up}'",  "'$tree_name'", "histos/output_BDT_category'${icat}'_vbf_'${NAME}'up.root", "vbf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${discriminant}'",'${pt_threshold}',"'${icat}'")'

	    if [[ -f $vbf_syst_name_down ]]; then
		root -l -b -q 'macros/create_input_tree.C+("'${vbf_syst_name_down}'",  "'$tree_name'", "histos/output_BDT_category'${icat}'_vbf_'${NAME}'down.root", "vbf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${discriminant}'",'${pt_threshold}',"'${icat}'")'
	    fi


	    ggf_syst_name_up=`echo $ggf_file | sed 's,bdt_skims,bdt_skims_JES_JER,g' | sed 's,BDT_Tree,'${NAME}'up.BDT_Tree,g'`
	    ggf_syst_name_down=`echo $ggf_file | sed 's,bdt_skims,bdt_skims_JES_JER,g' | sed 's,BDT_Tree,'${NAME}'down.BDT_Tree,g'`

	    root -l -b -q 'macros/create_input_tree.C+("'${ggf_syst_name_up}'",  "'$tree_name'", "histos/output_BDT_category'${icat}'_ggf_'${NAME}'up.root", "ggf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${discriminant}'",'${pt_threshold}',"'${icat}'")'

	    if [[ -f $ggf_syst_name_down ]]; then
		root -l -b -q 'macros/create_input_tree.C+("'${ggf_syst_name_down}'",  "'$tree_name'", "histos/output_BDT_category'${icat}'_ggf_'${NAME}'down.root", "ggf_BDTG",'${cat_low}', '${cat_high}' , '${int_lumi}', "'${discriminant}'",'${pt_threshold}',"'${icat}'")'
	    fi

	done
	
    fi
    
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

root -l -b -q 'macros/writeXML_vbf.C+("./histos/", '${num_syst}', 3, '${nCats}', 126, 1, '${use_syst}', '${use_stat}', '${float_mu_ggf}', '${use_theo}', true, '${use_st}','${use_flat_jes}')'

#############################################
# Run the Z0 calculation.
#############################################

cd xml
cp ../driver${nCats}.xml .
cp -r ../histos .

hist2workspace -standard_form driver${nCats}.xml
root -l -b -q 'p0_calculation.C("output_combined_datastat_model.root" , "combined" , "ModelConfig" ,"asimovData", false)' | tee test.out
