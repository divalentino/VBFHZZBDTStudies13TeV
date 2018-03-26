#!/bin/bash

message="${1}"

# Do cleanup.
# rm *~
# rm -rf pyspark/metastore_db
# rm -rf pyspark/spark-warehouse
# rm -rf pyspark/*.pyc
# rm -rf pyspark/*.log

# #Add new changes.
# git add *.sh
# git add *.py
# git add README.md
# git add output
# git add pyspark

rm *~
git add HZZVBFStats13TeV
git add *.C
git add *.cxx
git add *.sh
git add *.pdf
git add macros/*.C
git add macros/*.h
git add run/*.sh
git add README.md
git commit -m "${message}"
git push