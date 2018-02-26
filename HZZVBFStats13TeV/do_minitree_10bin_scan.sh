#!/bin/bash

line=""
#for VAL in $(seq -15 -2 -35); do
for VAL in $(seq 1 -0.2 -0.8); do
    line=${line}" ${VAL} " 
done

echo $line

./scripts/do_hzz_stats_minitree.sh $line
