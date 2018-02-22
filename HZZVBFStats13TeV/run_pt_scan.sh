#!/bin/bash

for PT in 20 30 40 50 60 70; do

    echo "************ RUNNING PT SCAN FOR THRES. "${PT}" GeV ************"
    cat scripts/do_hzz_stats.sh | sed 's,pt_threshold=.*,pt_threshold='${PT}',g' &> test.sh
    cp test.sh scripts/do_hzz_stats.sh

    ./do_10_bin_calculation.sh &> lh_scan_result_pt_${PT}.out

done
