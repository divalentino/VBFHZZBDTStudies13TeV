#!/bin/bash

ls -1 sigscan_* | while read LINE; do 
  grep "Best" $LINE; 
  grep "Optimal sig" $LINE;
  grep "Optimal B" $LINE;
file=`grep 'Best' $LINE | sed -e 's,Best significance given by file: /afs/cern.ch/user/d/ddivalen/eos/atlas/user/d/ddivalen/VBFH_TMVA_training_files/16-11-14/output/TMVA_App_opt_VBF_VBF_v_ggH,,g' | sed -e 's,root,txt,g'`
file="vars/BDT_vars_13"${file}
cat $file
echo ""
done