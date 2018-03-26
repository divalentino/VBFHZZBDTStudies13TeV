#!/bin/bash

exec=create_TMVAClassification.exe
code=TMVAClassification.cxx
rm -f $exec

doPileup="1"

####################
# I/O INFORMATION. #
####################

suffix=${1:-"default"}
if [[ "$doPileup" != "0" ]]; then
  suffix="withPU_"${suffix}
fi
training_prefix="training_VBF_v_ggH_${suffix}"

inFolder="../../evttree_files/"
BDTBaseFile="./vars/BDT_vars_${suffix}.txt"

TreeName="HiggsTree"

##################
# ggF BACKGROUND.#
##################

if [[ "$doPileup" == "0" ]]; then
  #No pileup samples.
  inFileTestggHSig="bdt_train_test/mc12_14TeV.Powheg.HJJ.10Mevts-08-10-14.HiggsTree.Skim.Test.root"
  inFileTrainggHSig="bdt_train_test/mc12_14TeV.Powheg.HJJ.10Mevts-08-10-14.HiggsTree.Skim.Train.root"
else
  #Pileup samples.
  inFileTestggHSig="bdt_train_test_pu/mc14_14TeV.Powheg.HJJ.10Mevts-21-01-15.withPU.HiggsTree.Skim.Test.root"
  inFileTrainggHSig="bdt_train_test_pu/mc14_14TeV.Powheg.HJJ.10Mevts-21-01-15.withPU.HiggsTree.Skim.Train.root"
fi

##################
#   VBF SIGNAL.  #
##################

if [[ "$doPileup" == "0" ]]; then
  #No pileup samples.
  inFileTestVBFSig="bdt_train_test/mc12_14TeV.Powheg.VBFH.3Mevts.HiggsTree.Skim.Test.root"
  inFileTrainVBFSig="bdt_train_test/mc12_14TeV.Powheg.VBFH.3Mevts.HiggsTree.Skim.Train.root"
else
  #Pileup samples. 
  inFileTestVBFSig="bdt_train_test_pu/mc14_14TeV.Powheg.VBFH.3Mevts-21-01-15.withPU.HiggsTree.Skim.Test.root"
  inFileTrainVBFSig="bdt_train_test_pu/mc14_14TeV.Powheg.VBFH.3Mevts-21-01-15.withPU.HiggsTree.Skim.Train.root"
fi

##################
# COMPILE & RUN. #
##################

rm -f $exec
CMD="$(root-config --cxx --cflags) -lMinuit -lTMVA -o $exec $code $(root-config --libs)"

result=$($CMD)

[[ -e $exec ]] && {
 
 BDTFile=${BDTBaseFile}
 ./$exec training/${training_prefix}.root $inFileTrainVBFSig $inFileTestVBFSig $inFileTrainggHSig $inFileTestggHSig $BDTFile "" "" --WFN ${training_prefix} --treeName ${TreeName}

}
