#!/bin/bash -e
#
set -o pipefail

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh core log legends gmt.history

cp -r init_0 0

# Create initial mesh
blockMesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh

# Createt initial conditions on the rMesh
time=0
# Create Gaussian patches of voriticty
setGaussians initDict -region rMesh
# Invert to find the wind field
invertVorticity -time $time initDict -region rMesh
gmtFoam -time $time vorticity -region rMesh
gv $time/vorticity.pdf &

# Calculate the height in balance and plot
setBalancedHeight -region rMesh
gmtFoam -time $time hU -region rMesh
evince $time/hU.pdf &

# Solve the SWE
movingshallowWaterFoamH >& log & sleep 0.01; tail -f log

# Post process
time=100000
gmtFoam -time $time hU -region rMesh
evince $time/hU.pdf &

postProcess -func rMesh/vorticity2D -time $time -region rMesh
gmtFoam -time $time vorticity -region rMesh
evince $time/vorticity.pdf &

# Only re-calcualte and re-plot recent times
time=200000
postProcess -func rMesh/vorticity2D -region rMesh -time $time':'
gmtFoam vorticity -region rMesh -time $time':'


# Animation of vorticity
postProcess -func rMesh/vorticity2D -region rMesh
gmtFoam vorticity -region rMesh
eps2gif vorticity.gif 0/vorticity.pdf ?????/vorticity.pdf ??????/vorticity.pdf ???????/vorticity.pdf

