#!/bin/bash

##########################################
# Filter out lists of training variables
# we're not interested in. Meant to red-
# uce overall combinatorics.
##########################################

folder=${1:-"./vars/"}
prefix=${2:-"BDT_vars_10_"}

# Loop over all desired BDT files, remove the ones
# that don't contain the desired variables.
ls -1 ${folder}"/"${prefix}* | while read LINE;
do

    for VAR in "Mjj" "DelEta_jj"; do
	var_exist=`grep ${VAR} ${LINE}`
	if [[ "$var_exist" == "" ]]; then
	    rm $LINE;
	    break;
	fi
    done

done

