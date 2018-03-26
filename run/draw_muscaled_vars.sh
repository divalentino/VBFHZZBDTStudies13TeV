#!/bin/bash

input_folder="/data/data065/ddivalen/ZZ4lSkims/mc/HJJ/EvtTree/27-02-15/"

nom=`ls -1 $input_folder/*nom*.root`
up=`ls -1 $input_folder/*0.5*.root`
down=`ls -1 $input_folder/*2.0*.root`

root -l 'macros/DrawMuScaledVariables.C+("'$nom'","'$up'","'$down'")'