#!/bin/bash

line=""
for VAL in $(seq 1 -0.2 -0.8); do
    line=${line}" ${VAL} " 
done

echo $line

./scripts/do_hzz_stats.sh $line