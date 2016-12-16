#!/bin/bash
set -o nounset
#set -o errexit
set -o verbose 
set -o xtrace

here=$(pwd)
cd results
cd bell
cd NEWTON2D-vectorGradc_m
#for N in 50 60 100 150 200 250 300; do
for N in 100; do
    cd $N
#    for k in 3 5 10 15 20 40 60 80 100; do
    for k in inf; do
        mkdir -p k$k
        
        sed -i "s/^nSmooth.*/nSmooth 999999;/g" system/NEWTON2D-vectorGradc_mDict
        
        NEWTON2D-vectorGradc_m > k$k/log 2> k$k/err
	    grep PABe k$k/log | cut -d= -f3 > k$k/equi
	    grep ExecutionTime k$k/log | cut -d= -f2 | cut -ds -f1 > k$k/time
        grep -A 1 'Time =' k$k/log | grep -B 1 Minimum | grep Time | cut -f2 -d'=' > k$k/reg
        
	done
	sed -i 's/^nSmooth.*/nSmooth 0;/g' system/NEWTON2D-vectorGradc_mDict
        
        
done



exit 0

