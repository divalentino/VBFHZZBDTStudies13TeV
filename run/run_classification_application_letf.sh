#!/bin/bash

exec=create_TMVAClassificationApplication.exe
code=TMVAClassificationApplication.cxx
rm -f $exec

####################################
# Samples with fwd. tracking.
####################################

testVBFFwdTracking="HiggsNtuplesLETF/FwdTracking/mc15_14TeV.Powheg.VBF.3Mevts.FwdTracking.HiggsTree.Skim.Test.root"
testggFFwdTracking="HiggsNtuplesLETF/FwdTracking/mc14_14TeV.Powheg.HJJ.10Mevts.FwdTracking.HiggsTree.Skim.Test.root"


####################################
# Samples without fwd. tracking.
####################################

testVBFNoFwdTracking="HiggsNtuplesLETF/NoFwdTracking/mc15_14TeV.Powheg.VBF.3Mevts.NoFwdTracking.HiggsTree.Skim.Test.root"
testggFNoFwdTracking="HiggsNtuplesLETF/NoFwdTracking/mc14_14TeV.Powheg.HJJ.10Mevts.NoFwdTracking.HiggsTree.Skim.Test.root"

####################
# I/O INFORMATION. #
####################

weightdir=./weights/

BDTBaseFile="./vars/BDT_vars_test.txt"

# weight files
BaseWeightFile_BDT1_VBF_v_ggH=${weightdir}/

# output files
BaseoutFile_ggH_VBF_v_ggH=output/LETF/TMVA_App_opt_ggH_VBF_v_ggH
BaseoutFile_VBF_VBF_v_ggH=output/LETF/TMVA_App_opt_VBF_VBF_v_ggH

HName='BDTscore'

CMD="$(root-config --cxx --cflags) -lMinuit -lTMVA -o $exec $code $(root-config --libs)"

result=$($CMD)

##################
# COMPILE & RUN. #
##################

[[ -e $exec ]] && {

  BDTFile=${BDTBaseFile}


  ###########################
  # Apply fwd. tracking BDT.
  ###########################

  WeightFile_BDT_VBF_v_ggH="weights/FwdTracking_BDTG.weights.xml"
  #WeightFile_BDT_VBF_v_ggH="../../CategoriesMVA/weights/VBF_Discriminant.weights.xml"
  ScoreName=${HName}

  #Run over ggH file.
  outFile_ggH_VBF_v_ggH=${BaseoutFile_ggH_VBF_v_ggH}"_FwdTracking.root"
  ./$exec $outFile_ggH_VBF_v_ggH $testggFFwdTracking $ScoreName $WeightFile_BDT_VBF_v_ggH $BDTFile

  #Run over VBF file.
  outFile_VBF_VBF_v_ggH=${BaseoutFile_VBF_VBF_v_ggH}"_FwdTracking.root"
  ./$exec $outFile_VBF_VBF_v_ggH $testVBFFwdTracking $ScoreName $WeightFile_BDT_VBF_v_ggH $BDTFile

  ###############################
  # Apply non-fwd. tracking BDT.
  ###############################

  WeightFile_BDT_VBF_v_ggH="weights/NoFwdTracking_BDTG.weights.xml"
  #WeightFile_BDT_VBF_v_ggH="../../CategoriesMVA/weights/VBF_Discriminant.weights.xml"
  ScoreName=${HName}

  #Run over ggH file.
  outFile_ggH_VBF_v_ggH=${BaseoutFile_ggH_VBF_v_ggH}"_NoFwdTracking.root"
  ./$exec $outFile_ggH_VBF_v_ggH $testggFNoFwdTracking $ScoreName $WeightFile_BDT_VBF_v_ggH $BDTFile

  #Run over VBF file.
  outFile_VBF_VBF_v_ggH=${BaseoutFile_VBF_VBF_v_ggH}"_NoFwdTracking.root"
  ./$exec $outFile_VBF_VBF_v_ggH $testVBFNoFwdTracking $ScoreName $WeightFile_BDT_VBF_v_ggH $BDTFile

}
