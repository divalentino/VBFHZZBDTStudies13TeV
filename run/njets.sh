#!/bin/bash

file="$1"
inclusive=${2:-"1"}

root -l 'macros/DrawNJets.C+("'${file}'",'${inclusive}')'
