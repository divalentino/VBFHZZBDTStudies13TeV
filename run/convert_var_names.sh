#!/bin/bash

filename="$1"

root -l -b -q 'macros/ConvertVariableNames.C("'${filename}'")'