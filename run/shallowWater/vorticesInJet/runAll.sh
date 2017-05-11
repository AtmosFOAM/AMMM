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
# Create a jet and calculate its voriticty
setPlaneJet initDict -region rMesh
postProcess -func rMesh/vorticity2D -time $time -region rMesh
# Create some Gaussian patches of vorticity
setGaussians initDict -region rMesh
# Add the two types of vorticity together and invert to get velocity
sumFields $time vorticity $time vorticity2D $time vorticityGaussians \
          -region rMesh
rm $time/rMesh/vorticity2D

invertVorticity -time $time initDict -region rMesh
gmtFoam -time $time vorticity -region rMesh
evince $time/vorticity.pdf &

setBalancedHeight -region rMesh
gmtFoam -time $time hU -region rMesh
evince $time/hU.pdf &

# Solve the SWE
movingshallowWaterFoamH >& log & sleep 0.01; tail -f log

time=100000
gmtFoam -time $time hU -region rMesh
evince $time/hU.pdf &

postProcess -func rMesh/vorticity2D -time $time -region rMesh
gmtFoam -time $time vorticity -region rMesh
evince $time/vorticity.pdf &

# Animation of vorticity
postProcess -func rMesh/vorticity2D -region rMesh
gmtFoam vorticity -region rMesh
eps2gif vorticity.gif 0/vorticity.pdf ??????/vorticity.pdf ???????/vorticity.pdf
