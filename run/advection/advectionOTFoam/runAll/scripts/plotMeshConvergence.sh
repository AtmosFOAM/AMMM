#!/bin/bash -e

# Plot the initial mesh convergence for each resolution in movingOverMountainsA

dir=movingOverMountainsA

# Assemble stats to plot
for res in 50 100 200 400; do
    grep 'Initial residual' $dir/n$res/meshLogs/log0 | \
        awk 'BEGIN{i=0;}{i=i+1; print i, $8, $15}' | \
        awk -F ',' '{print $1, $2}' \
        > $dir/n$res/meshConverge.dat
done

gmtPlot plots/meshResiduals.gmt
gmtPlot plots/meshIters.gmt

for res in 50 100 200 400; do
    dt=`echo $res | awk '{print 50/'$res'}'`
    grep meshPot $dir/n$res/log | \
        awk 'BEGIN{t=0.5*'$dt';}{print t, $8, $15; t=t+0.25*'$dt'}' | \
        awk -F ',' '{print $1, $2}' \
        > $dir/n$res/meshAdaptConverge.dat
done

gmtPlot plots/meshAdaptResiduals.gmt
gmtPlot plots/meshAdaptIters.gmt

