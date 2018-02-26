#!/bin/bash

line=""
for VAL in $(seq 1 -0.1 -0.9); do
    line=${line}" ${VAL} " 
done

echo $line

./scripts/do_hzz_stats.sh $line