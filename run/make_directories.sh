#!/bin/bash

folder=${1:-"./"}

layouts=( gold silver bronze HSOnly )
mu=( 200 140 )
pueffs=( 0.02 0.05 )

for LAYOUT in 0 1 2; do
    for PUEFF in 0 1; do
        for MU in 0; do

            layout=${layouts[$LAYOUT]}
            avgmu=${mu[$MU]}
            pueff=${pueffs[$PUEFF]}

	    mkdir ${folder}"/"${layout}.mu${avgmu}.pueff${pueff}

	done
    done
done