#!/bin/bash

if [[ -d xml ]]; then
  rm -rf xml
fi

mkdir xml
cp macros/*.C ./xml/
cd xml
root -l -b -q 'sigScan.C' | tee sig_scan.out