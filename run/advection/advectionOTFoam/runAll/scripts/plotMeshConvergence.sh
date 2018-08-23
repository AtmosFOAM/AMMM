#!/bin/bash -e

# Plot the initial mesh convergence for each resolution in movingOverMountainsA

dir=movingOverMountainsA
res=50

# Assemble stats to plot
for res in 50 100 200 400; do
    grep 'Initial residual' $dir/n$res/meshLogs/log0 | \
        awk 'BEGIN{i=0;}{i=i+1; print i, $8, $15}' | \
        awk -F ',' '{print $1, $2}' \
        > $dir/n$res/meshConverge.dat
done

