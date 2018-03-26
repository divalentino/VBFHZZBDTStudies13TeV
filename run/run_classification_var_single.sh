#!/bin/bash

exec=create_TMVAClassification.exe
code=TMVAClassification.cxx
rm -f $exec

eos='/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select'

numvar="$1"
numfile="$2"
inFolder=""

#Set I/O parameters.
if [[ $HOME == *afs* ]]; then

    #inFolder="/afs/cern.ch/user/d/ddivalen/public/VBFH4lAnalysis/EvtTreeFiles/"
    #inFolder="/afs/cern.ch/user/d/ddivalen/public/bdt_train_test/"

  inFolder="/afs/cern.ch/work/d/ddivalen/VBFZPlusJetsStudies/H4lBDTAnalysis/TMVAscripts/"

  export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
  . ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
  #source $AtlasSetup/scripts/asetup.sh 18.9.0,here
  localSetupROOT --rootVersion=5.34.21-x86_64-slc6-gcc48-opt
  

elif [[ $HOME == *home* ]]; then

  inFolder="/home/ddivalen/Research/VBFZPlusJetsStudies/H4lBDTAnalysis/TMVAscripts/scan_test/"

  export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
  . ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
  #source $AtlasSetup/scripts/asetup.sh 18.9.0,here
  localSetupROOT --rootVersion=5.34.21-x86_64-slc6-gcc48-opt

else
  inFolder="/Users/daviddivalentino/Research/VBFZPlusJetsStudies/evttree_files/"
fi

#Define inputs.
# inFileTrainVBFSig=${inFolder}/"mc12_14TeV.PowhegPythia8.VBFH.455kevts.EvtTree.Skim.Train.root"
# inFileTestVBFSig=${inFolder}/"mc12_14TeV.PowhegPythia8.VBFH.455kevts.EvtTree.Skim.Test.root"

# inFileTrainggHSig=${inFolder}/"mc12_14TeV.PowhegPythia8.HJJ.10Mevts.merged.EvtTree.Skim.Train.root"
# inFileTestggHSig=${inFolder}/"mc12_14TeV.PowhegPythia8.HJJ.10Mevts.merged.EvtTree.Skim.Test.root"

################
# 14 TEV SCANS #
################

inFileTestggHSig=${inFolder}/"bdt_train_test/mc12_14TeV.Powheg.HJJ.10Mevts-08-10-14.HiggsTree.Skim.Test.root"
inFileTrainggHSig=${inFolder}/"bdt_train_test/mc12_14TeV.Powheg.HJJ.10Mevts-08-10-14.HiggsTree.Skim.Train.root"

inFileTestVBFSig=${inFolder}/"bdt_train_test/mc12_14TeV.Powheg.VBFH.3Mevts.HiggsTree.Skim.Test.root"
inFileTrainVBFSig=${inFolder}/"bdt_train_test/mc12_14TeV.Powheg.VBFH.3Mevts.HiggsTree.Skim.Train.root"

# inFileTestVBFSig=${inFolder}/"mc12_14TeV.Powheg.VBFH.455kevts.HiggsTree.Skim.Test.root"
# inFileTrainVBFSig=${inFolder}/"mc12_14TeV.Powheg.VBFH.455kevts.HiggsTree.Skim.Train.root"

# inFileTestggHSig=${inFolder}/"mc12_14TeV.Powheg.HJJ.10Mevts-08-10-14.HiggsTree.Skim.Test.root"
# inFileTrainggHSig=${inFolder}/"mc12_14TeV.Powheg.HJJ.10Mevts-08-10-14.HiggsTree.Skim.Train.root"

###############
# 8 TEV SCANS #
###############

# inFileTestVBFSig=${inFolder}/"VBF.merged.8TeV.HiggsTree.Skim.Test.root"
# inFileTrainVBFSig=${inFolder}/"VBF.merged.8TeV.HiggsTree.Skim.Train.root"

# inFileTestggHSig=${inFolder}/"ggH.merged.8TeV.HiggsTree.Skim.Test.root"
# inFileTrainggHSig=${inFolder}/"ggH.merged.8TeV.HiggsTree.Skim.Train.root"

BDTBaseFile="./vars/BDT_vars_13"
TreeName="HiggsTree"

rm -f $exec
CMD="$(root-config --cxx --cflags) -lMinuit -lTMVA -o $exec $code $(root-config --libs)"

result=$($CMD)

#Run TMVA code.
[[ -e $exec ]] && {

    BDTFile=${BDTBaseFile}_${numvar}_${numfile}.txt

    ./$exec training/training_VBF_v_ggH_${numvar}_${numfile}.root $inFileTrainVBFSig $inFileTestVBFSig $inFileTrainggHSig $inFileTestggHSig $BDTFile "" "" --WFN training_VBF_v_ggH_${numvar}_${numfile} --treeName ${TreeName}

    #If using batch, copy everything to EOS. Otherwise, data065.
    dd=`date +%d`; mm=`date +%m`; yy=`date +%y`
    outfolder="$dd-$mm-$yy"
    if [[ $HOME == *afs* ]]; then
      ${eos} mkdir /eos/atlas/user/d/ddivalen/VBFH_TMVA_training_files/${outfolder}/
      #${eos} cp training/training_VBF_v_ggH_${numvar}_${numfile}.root /eos/atlas/user/d/ddivalen/VBFH_TMVA_training_files/${outfolder}/
      ${eos} cp weights/*training_VBF_v_ggH_${numvar}_${numfile}* /eos/atlas/user/d/ddivalen/VBFH_TMVA_training_files/${outfolder}/
    else
      mkdir -p /data/data065/ddivalen/VBFH_TMVA_training_files/${outfolder}/training/
      mkdir -p /data/data065/ddivalen/VBFH_TMVA_training_files/${outfolder}/weights/
      #cp training/training_VBF_v_ggH_${numvar}_${numfile}.root /data/data065/ddivalen/VBFH_TMVA_training_files/${outfolder}/training/
      cp weights/*training_VBF_v_ggH_${numvar}_${numfile}* /data/data065/ddivalen/VBFH_TMVA_training_files/${outfolder}/weights/
    fi

}
