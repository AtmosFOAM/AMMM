#!/bin/bash
set -o nounset
set -o errexit
set -o verbose 
set -o xtrace


echo a program to extract the points at which BL2D regularised the Monge Ampere iterative solution
echo it will read the reg file and output a regs file that tikz can print



wd=$(pwd)


for method in AFP Newton2d-vectorGradc_m_reconstruct; do

    #ring:
    cd /home/pbrowne/OpenFOAM/results/ring/$method
    for N in */; do
        cd "$N"
        rm -rf regs
        while read line; do
            echo $line
            let a=$line+1
            echo $line $(sed -n "$a"p equi) >> regs
        done < reg
        cd -
    done

    #bell:
    cd /home/pbrowne/OpenFOAM/results/bell/$method
    for N in */; do
        cd "$N"
        rm -rf regs
        while read line; do
            echo $line
            let a=$line+1
            echo $line $(sed -n "$a"p equi) >> regs
        done < reg
        cd -
    done

done



