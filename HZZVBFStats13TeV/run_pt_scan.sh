#!/bin/bash

#for PT in 20 30 40 50 60 70; do
for PT in $(seq 70 10 90); do

    echo "************ RUNNING PT SCAN FOR THRES. "${PT}" GeV ************"
    cat scripts/do_hzz_stats.sh | sed 's,pt_threshold=.*,pt_threshold='${PT}',g' &> test.sh
    cp test.sh scripts/do_hzz_stats.sh

    ./do_10_bin_calculation.sh &> lh_scan_result_pt_${PT}.out

    # cat template-run_hzz_stats.sh | sed 's,MYCUT,'${PT}',g' &> scripts/run_hzz_stats.sh
    # #./scripts/do_sig_scan.sh
    # ./scripts/do_mu_scan.sh
done
