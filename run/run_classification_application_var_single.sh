#!/bin/bash

exec=create_TMVAClassificationApplication.exe
code=TMVAClassificationApplication.cxx
rm -f $exec

eos='/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select'

numvar="$1"
numfile="$2"
inFolder=""
outFolder=""
weightFolder=""

#I/O information.
num_vars=13
BDTBaseFile="./vars/BDT_vars_${num_vars}"

#Set up Root via CVMFS, if available.
if [[ -d "/cvmfs/" ]]; then
  export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
  . ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
  localSetupROOT --rootVersion=5.34.21-x86_64-slc6-gcc48-opt
fi

#####################
#Set I/O parameters.
#####################

dd=`date +%d`; mm=`date +%m`; yy=`date +%y`
outfolder_date="$dd-$mm-$yy"

if [[ $HOME == *afs* ]]; then
    
  outFolder="/eos/atlas/user/d/ddivalen/VBFH_TMVA_training_files/${outfolder_date}/"
  weightFolder="/eos/atlas/user/d/ddivalen/VBFH_TMVA_training_files/16-11-14/weights/"

  ################
  # 14 TEV TEST. #
  ################

  #inFolder="/afs/cern.ch/user/d/ddivalen/public/bdt_train_test/"
  inFolder="/afs/cern.ch/work/d/ddivalen/VBFZPlusJetsStudies/H4lBDTAnalysis/TMVAscripts/"

  ###############
  # 8 TEV TEST. #
  ###############

  #inFolder="/afs/cern.ch/work/d/ddivalen/VBFZPlusJetsStudies/H4lBDTAnalysis/TMVAscripts/8TeV_test/"
  
elif [[ $HOME == *home* ]]; then
    
  inFolder="/home/ddivalen/Research/VBFZPlusJetsStudies/H4lBDTAnalysis/TMVAscripts/scan_test/"
  outFolder="/data/data065/ddivalen/VBFH_TMVA_training_files/${outfolder_date}/output/"
  weightFolder="/data/data065/ddivalen/VBFH_TMVA_training_files/04-11-14/weights/"
  
else
  inFolder="/Users/daviddivalentino/Research/VBFZPlusJetsStudies/evttree_files/"
fi

#Define inputs.
# infile_VBF=${inFolder}/"mc12_14TeV.PowhegPythia8.VBFH.455kevts.EvtTree.Skim.Test.root"
# infile_ggH=${inFolder}/"mc12_14TeV.PowhegPythia8.HJJ.10Mevts.merged.EvtTree.Skim.Test.root"

################
# 14 TEV TEST. #
################

infile_VBF=${inFolder}/"bdt_train_test/mc12_14TeV.Powheg.VBFH.3Mevts.HiggsTree.Skim.Test.root"
infile_ggH=${inFolder}/"bdt_train_test/mc12_14TeV.Powheg.HJJ.10Mevts-08-10-14.HiggsTree.Skim.Test.root"

###############
# 8 TEV TEST. #
###############

#infile_VBF=${inFolder}/"VBF.merged.8TeV.HiggsTree.Skim.Test.root"
#infile_ggH=${inFolder}/"ggH.merged.8TeV.HiggsTree.Skim.Test.root"

#Define output bases.
HName='BDTscore'
TreeName="HiggsTree"
weightdir="./weights/"

BaseWeightFile_BDT1_VBF_v_ggH=${weightdir}/training_VBF_v_ggH_${numvar}_${numfile}
BaseoutFile_ggH_VBF_v_ggH=output/TMVA_App_opt_ggH_VBF_v_ggH_${numvar}_${numfile}
BaseoutFile_VBF_VBF_v_ggH=output/TMVA_App_opt_VBF_VBF_v_ggH_${numvar}_${numfile}

rm -f $exec
CMD="$(root-config --cxx --cflags) -lMinuit -lTMVA -o $exec $code $(root-config --libs)"

result=$($CMD)

#################
#Run TMVA code.
#################

[[ -e $exec ]] && {

    BDTFile=${BDTBaseFile}_${numvar}_${numfile}.txt

    #Copy over weight file.
    echo "Attempting to copy file: "${weightFolder}/*training_VBF_v_ggH_${numvar}_${numfile}*
    if [[ $HOME == *afs* ]]; then
      ${eos} cp ${weightFolder}/training_VBF_v_ggH_${numvar}_${numfile}* ${weightdir}
    else
      cp ${weightFolder}/*training_VBF_v_ggH_${numvar}_${numfile}* ${weightdir}
    fi
	
    WeightFile_BDT_VBF_v_ggH=${BaseWeightFile_BDT1_VBF_v_ggH}_BDTG.weights.xml
    ScoreName=${HName}

    #Run over ggH file.
    outFile_ggH_VBF_v_ggH=${BaseoutFile_ggH_VBF_v_ggH}.root
    ./$exec $outFile_ggH_VBF_v_ggH $infile_ggH $ScoreName $WeightFile_BDT_VBF_v_ggH $BDTFile

    #Run over VBF file.
    outFile_VBF_VBF_v_ggH=${BaseoutFile_VBF_VBF_v_ggH}.root
    ./$exec $outFile_VBF_VBF_v_ggH $infile_VBF $ScoreName $WeightFile_BDT_VBF_v_ggH $BDTFile

    #If using batch, copy everything to EOS. Otherwise, data065.
    if [[ $HOME == *afs* ]]; then
      ${eos} mkdir ${outFolder}
      ${eos} cp output/*.root ${outFolder}
    else
      mkdir -p ${outFolder}
      cp output/* ${outFolder}
    fi

}
