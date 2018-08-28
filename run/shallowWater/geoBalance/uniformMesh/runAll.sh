#!/bin/bash -ve

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh core log legends gmt.history

# Create initial mesh
blockMesh
mkdir -p constant/cMesh
cp -r constant/polyMesh constant/cMesh
ln -sf ../system/dynamicMeshDict constant/dynamicMeshDict

# Create initial conditions
rm -rf [0-9]* core
cp -r init_0 0
time=0
setBalancedHeightRC
gmtFoam -time 0 hMesh
evince 0/hMesh.pdf &

# Solve the SWE
shallowWaterOTFoam -fixedMesh >& log & sleep 0.01; tail -f log

# Difference between solutions
time=10000
sumFields $time UDiff $time U 0 U -scale1 -1
sumFields $time hDiff  $time h  0 h  -scale1 -1
gmtFoam -time $time hUDiff
evince $time/hUDiff.pdf &
