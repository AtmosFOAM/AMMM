#!/bin/bash -e

# Plot results
gmtPlot plots/plotMass.gmt

time=0
for field in Tmesh monitor uniT; do
    gmtFoam $field -region pMesh -time $time':'
done

# Make links for animategraphics
mkdir -p animategraphics
for field in T UT mesh monitor uniT; do
    for time in [0-9]*; do
        t=`echo $time | awk {'print $1/50'}`
        ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
