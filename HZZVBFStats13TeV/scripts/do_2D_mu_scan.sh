#!/bin/bash

if [[ -d xml ]]; then
  rm -rf xml
fi

mkdir xml
cp macros/*.C ./xml/
cd xml
root -l -b -q 'mu2DScan.C' &> ../mu_2D_scan.out