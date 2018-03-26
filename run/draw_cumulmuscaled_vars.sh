#!/bin/bash

input_folder=${1:-"/data/data065/ddivalen/ZZ4lSkims/mc/HJJ/EvtTree/27-02-15/"}

nom=`ls -1 $input_folder/*nom*Higgs*.root`
up=`ls -1 $input_folder/*0.5*Higgs*.root`
down=`ls -1 $input_folder/*2.0*Higgs*.root`

root -l 'macros/DrawMuScaledCumulativeDists.C+("'$nom'","'$up'","'$down'")'