#!/bin/bash -e

# Post process
case=.
postProcess -func vorticity -case $case
writeuvw vorticity -case $case
for time in [0-9]*; do
    mv $case/$time/vorticityz $case/$time/vorticity
    rm $case/$time/vorticity?
done
gmtFoam vorticityMesh -case $case

for time in [0-9]*; do
    gmtFoam -time $time hU
    gmtFoam -time $time mesh
done

# Animattion of vorticity
eps2gif vorticityMesh.gif 0/vorticityMesh.pdf ??????/vorticityMesh.pdf \
                                      ???????/vorticityMesh.pdf

# Make links for animategraphics
mkdir -p animategraphics
for field in vorticityMesh vorticity hU mesh; do
    for time in [0-9]*; do
        t=`echo $time | awk {'print $1/100000'}`
        ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done

