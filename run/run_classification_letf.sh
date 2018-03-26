#!/bin/bash

exec=create_TMVAClassification.exe
code=TMVAClassification.cxx
rm -f $exec

####################################
# Samples with fwd. tracking.
####################################

testVBFFwdTracking="HiggsNtuplesLETF/FwdTracking/mc15_14TeV.Powheg.VBF.3Mevts.FwdTracking.HiggsTree.Skim.Test.root"
trainVBFFwdTracking="HiggsNtuplesLETF/FwdTracking/mc15_14TeV.Powheg.VBF.3Mevts.FwdTracking.HiggsTree.Skim.Train.root"

testggFFwdTracking="HiggsNtuplesLETF/FwdTracking/mc14_14TeV.Powheg.HJJ.10Mevts.FwdTracking.HiggsTree.Skim.Test.root"
trainggFFwdTracking="HiggsNtuplesLETF/FwdTracking/mc14_14TeV.Powheg.HJJ.10Mevts.FwdTracking.HiggsTree.Skim.Train.root"


####################################
# Samples without fwd. tracking.
####################################

testVBFNoFwdTracking="HiggsNtuplesLETF/NoFwdTracking/mc15_14TeV.Powheg.VBF.3Mevts.NoFwdTracking.HiggsTree.Skim.Test.root"
trainVBFNoFwdTracking="HiggsNtuplesLETF/NoFwdTracking/mc15_14TeV.Powheg.VBF.3Mevts.NoFwdTracking.HiggsTree.Skim.Train.root"

testggFNoFwdTracking="HiggsNtuplesLETF/NoFwdTracking/mc14_14TeV.Powheg.HJJ.10Mevts.NoFwdTracking.HiggsTree.Skim.Test.root"
trainggFNoFwdTracking="HiggsNtuplesLETF/NoFwdTracking/mc14_14TeV.Powheg.HJJ.10Mevts.NoFwdTracking.HiggsTree.Skim.Train.root"


####################################
# Other I/O info.
####################################

BDTFile="vars/BDT_vars_test.txt"
TreeName="HiggsTree"

####################################
# RUN THE CODE!
####################################

rm -f $exec
CMD="$(root-config --cxx --cflags) -lMinuit -lTMVA -o $exec $code $(root-config --libs)"

result=$($CMD)

[[ -e $exec ]] && {
 

 #Train with fwd. tracking.
 training_prefix="FwdTracking"
 ./$exec training/${training_prefix}.root $trainVBFFwdTracking $testVBFFwdTracking $trainggFFwdTracking $testggFFwdTracking $BDTFile "" "" --WFN ${training_prefix} --treeName ${TreeName}

 #Train without fwd. tracking.
 training_prefix="NoFwdTracking"
 ./$exec training/${training_prefix}.root $trainVBFNoFwdTracking $testVBFNoFwdTracking $trainggFNoFwdTracking $testggFNoFwdTracking $BDTFile "" "" --WFN ${training_prefix} --treeName ${TreeName}

}
