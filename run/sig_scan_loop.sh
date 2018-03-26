#!/bin/bash

for NVARS in {2..13}; do
  grep "_${NVARS}_" configs/sig_files.txt &> sig_files_${NVARS}.txt
  grep "_${NVARS}_" configs/bkg_files.txt &> bkg_files_${NVARS}.txt
  nohup ./run/sig_scan.sh sig_files_${NVARS}.txt bkg_files_${NVARS}.txt &> sigscan_${NVARS}.out &
done