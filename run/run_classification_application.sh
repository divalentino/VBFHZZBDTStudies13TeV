#!/bin/bash

exec=create_TMVAClassificationApplication.exe
code=TMVAClassificationApplication.cxx
rm -f $exec

doPileup="1"

####################
# I/O INFORMATION. #
####################

suffix=${1:-"default"}
if [[ "$doPileup" != "0" ]]; then
  suffix="withPU_"${suffix}
fi
weightdir=./weights/

BDTBaseFile="./vars/BDT_vars_${suffix}.txt"

# weight files
BaseWeightFile_BDT1_VBF_v_ggH=${weightdir}/training_VBF_v_ggH_${suffix}

# output files
BaseoutFile_ggH_VBF_v_ggH=output/TMVA_App_opt_ggH_VBF_v_ggH_${suffix}
BaseoutFile_VBF_VBF_v_ggH=output/TMVA_App_opt_VBF_VBF_v_ggH_${suffix}

#################
# 14 TeV files. #
#################

if [[ "$doPileup" == "0" ]]; then
  #No pileup samples.
  infile_VBF="bdt_train_test/mc12_14TeV.Powheg.VBFH.3Mevts.HiggsTree.Skim.Test.root"
  infile_ggH="bdt_train_test/mc12_14TeV.Powheg.HJJ.10Mevts-08-10-14.HiggsTree.Skim.Test.root"
else
  #Pileup samples.
  infile_VBF="bdt_train_test_pu/mc14_14TeV.Powheg.VBFH.3Mevts-21-01-15.withPU.HiggsTree.Skim.Test.root"
  infile_ggH="bdt_train_test_pu/mc14_14TeV.Powheg.HJJ.10Mevts-21-01-15.withPU.HiggsTree.Skim.Test.root"
fi

HName='BDTscore'

CMD="$(root-config --cxx --cflags) -lMinuit -lTMVA -o $exec $code $(root-config --libs)"

result=$($CMD)

##################
# COMPILE & RUN. #
##################

[[ -e $exec ]] && {

  BDTFile=${BDTBaseFile}

  WeightFile_BDT_VBF_v_ggH=${BaseWeightFile_BDT1_VBF_v_ggH}_BDTG.weights.xml
  #WeightFile_BDT_VBF_v_ggH="../../CategoriesMVA/weights/VBF_Discriminant.weights.xml"
  ScoreName=${HName}

  #Run over ggH file.
  outFile_ggH_VBF_v_ggH=${BaseoutFile_ggH_VBF_v_ggH}.root
  ./$exec $outFile_ggH_VBF_v_ggH $infile_ggH $ScoreName $WeightFile_BDT_VBF_v_ggH $BDTFile

  #Run over VBF file.
  outFile_VBF_VBF_v_ggH=${BaseoutFile_VBF_VBF_v_ggH}.root
  ./$exec $outFile_VBF_VBF_v_ggH $infile_VBF $ScoreName $WeightFile_BDT_VBF_v_ggH $BDTFile

}
