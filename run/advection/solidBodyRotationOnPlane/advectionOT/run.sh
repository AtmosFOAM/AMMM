#!/bin/bash -e
# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh constant/dynamicMeshDict core log legends gmt.history

# Create initial mesh
blockMesh
mkdir -p constant/cMesh
cp -r constant/polyMesh constant/cMesh
ln -sf ../system/dynamicMeshDict constant/dynamicMeshDict

# Create initial conditions
rm -rf [0-9]* core
cp -r init_0 0
time=0
# Create Gaussian patches of voriticty
setGaussians initDict
# Invert to find the wind field
invertVorticity -time $time initDict
gmtFoam -time $time vorticityMesh

# Calculate the height in balance and plot
setBalancedHeightRC
gmtFoam -time $time hUmesh

# Solve the SWE
advectionOTFoam -fixedMesh >& log & sleep 0.01; tail -f log
