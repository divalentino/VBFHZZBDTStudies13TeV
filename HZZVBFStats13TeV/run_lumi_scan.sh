#!/bin/bash

echo "" &> output.txt

for LUMI in $(seq 50 50 500); do

    echo "Doing scan for "${LUMI}" fb-1"
    
    cat ./scripts/do_hzz_stats.sh | sed 's,int_lumi=.*,int_lumi='${LUMI}',g' &> ./test.sh
    chmod +x ./test.sh
    cp test.sh ./scripts/do_hzz_stats.sh
    echo "LUMI: "${LUMI} >> output.txt
    ./do_10_bin_calculation.sh >> output.txt

done
