#!/bin/bash

if [[ -d xml ]]; then
  rm -rf xml
fi

mkdir xml
cp macros/*.C ./xml/
cd xml
root -l -b -q 'sig2DScan.C' | tee ../sig_2D_scan.out