#!/bin/bash

exec=create_TMVAClassificationApplication.exe
code=TMVAClassificationApplication.cxx
rm -f $exec

CMD="$(root-config --cxx --cflags) -lMinuit -lTMVA -o $exec $code $(root-config --libs)"

result=$($CMD)

[[ -e $exec ]] && {

#weight_file="weights/OptimizedVars13TeVNoPtHjjJetPt30GeV_BDTG.weights.xml"
#weight_file="weights/DefaultThorJetPt30GeVMjj130GeV_BDTG.weights.xml"
#weight_file="weights/MyNewTestBinned50GeV_BDTG.weights.xml"

#weight_file="weights/OptimizedVars13TeVPtHjjBin50GeVJetPt30GeV_BDTG.weights.xml"

# Optimized variables, no Pt(H+jj).
weight_file="weights/OptimizedVars13TeVNoPtHjjJetPt30GeV_BDTG.weights.xml"
suffix="OptimizedVars13TeVNoPtHjjJetPt30GeV"
BDT_vars="configs/BDT_vars_nopt.txt"

#Default Run-I variables.
#weight_file="weights/DefaultVars13TeVJetPt30GeV_BDTG.weights.xml"
#suffix="DefaultVars13TeVJetPt30GeV"
#BDT_vars="configs/BDT_vars_default.txt"

ls -1 higgs_signal_mc_125_nominal/* | grep Higgs | grep bin | grep -v thresh | while read LINE; do
 ./apply_bdt.sh $LINE ${weight_file} ${BDT_vars} ${suffix}
done

ls -1 higgs_backgrounds/* | grep Higgs | grep bin | grep -v thresh | while read LINE; do
 ./apply_bdt.sh $LINE ${weight_file} ${BDT_vars} ${suffix}
done
exit

for BIN in $(seq 70 10 90); do

    weight_file="weights/OptimizedVars13TeVPtHjjBin"${BIN}"GeVJetPt30GeV_BDTG.weights.xml"
    #weight_file="weights/fine_scan/OptimizedVars13TeVPtHjjFineScanBin"${BIN}"GeVJetPt30GeV_BDTG.weights.xml"

    cat configs/BDT_vars.txt | sed 's,pT_Hjj,pT_Hjj_bin_'${BIN}',g' &> configs/BDT_vars_bin_${BIN}.txt
    if [[ `grep "pT_Hjj_bin_"${BIN} configs/TMVA_vars.txt` == "" ]]; then
	echo "pT_Hjj_bin_"${BIN} >> configs/TMVA_vars.txt
    fi

    suffix="thresh."${BIN}
    BDT_vars="configs/BDT_vars_bin_${BIN}.txt"

    while [[ `ps -u ddivalen | grep apply | wc -l` -ge 10 ]]; do
        sleep 5;
    done

    ls -1 higgs_signal_mc_125_nominal/* | grep Higgs | grep bin | while read LINE; do
	nohup ./apply_bdt.sh $LINE ${weight_file} ${BDT_vars} ${suffix} &
    done

    ls -1 higgs_backgrounds/* | grep Higgs | grep bin | while read LINE; do
	nohup ./apply_bdt.sh $LINE ${weight_file} ${BDT_vars} ${suffix} &
    done
    
done

}
