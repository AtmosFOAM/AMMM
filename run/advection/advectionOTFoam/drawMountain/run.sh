#!/bin/bash -e

# Remove old stuff
rm -rf [0-9]* constant/polyMesh

# Generate the mesh
blockMesh

# Calculate the initial conditions and the fluxes before imposing the terrain
cp -r init0 0

# Put in a mountain
terrainFollowingMesh

# Write out cell volumes for post-processing
time=0
postProcess -func writeCellVolumes -time $time
gmtFoam -time $time Tunder
gmtFoam -time $time mountainOver
