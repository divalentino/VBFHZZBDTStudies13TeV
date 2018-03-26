#!/bin/bash

# filename=${1:-"HiggsNtuplesLETFSmall/26-04-15/HSOnly/mc15_14TeV.Powheg.HJJ.02-06-15.HSOnly.HiggsTree.root"}
# treename="HiggsTree"
# root -l -b -q 'macros/GetDijetCutsEfficiency.C+("'${filename}'","'${treename}'")'

root -l -b -q 'macros/GetDijetCutsEfficiency.C("'${filename}'","'${treename}'")'
