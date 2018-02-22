#!/bin/bash

exec=create_TMVAClassificationApplicationMinitree.exe

####################
# I/O INFORMATION. #
####################

BDTBaseFile=${3:-"./configs/BDT_vars.txt"}
suffix=${4:-""}

#################
# 14 TeV files. #
#################

#New test samples.
infile=$1
outFile=`echo $infile | sed 's,\.root,\.BDT_Tree.root,g'`

if [[ "$suffix" != "" ]]; then
    outFile=`echo $outFile | sed 's,BDT_Tree,'${suffix}'.BDT_Tree,g'`
fi

echo "////////////////////////////////////////////////////////"
echo "Producing output file: "$outFile

HName='BDTscore'

##################
# COMPILE & RUN. #
##################

BDTFile=${BDTBaseFile}

WeightFile_BDT=${2:-"weights/MyTestNewFiles_BDTG.weights.xml"}
ScoreName=${HName}

#Run over input file.
[[ -e $exec ]] && {
    ./$exec $outFile $infile $ScoreName $WeightFile_BDT $BDTFile
}
