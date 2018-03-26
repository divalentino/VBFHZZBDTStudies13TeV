#!/bin/bash

suffix=${1:-"default"}

./run/run_classification.sh ${suffix}
./run/run_classification_application.sh ${suffix}
#./run/sig_scan.sh output/TMVA_App_opt_VBF_VBF_v_ggH_rescan.root output/TMVA_App_opt_ggH_VBF_v_ggH_rescan.root

