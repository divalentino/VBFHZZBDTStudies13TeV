#!/bin/bash

if [[ -d xml ]]; then
  rm -rf xml
fi

mkdir xml
cp macros/*.C ./xml/
cd xml
root -l -b -q 'muScan.C' | tee mu_scan.out
