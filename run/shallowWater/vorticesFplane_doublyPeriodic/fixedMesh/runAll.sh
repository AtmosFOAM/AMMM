#!/bin/bash -ve

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh core log legends gmt.history

# Create initial mesh from previous adapted mesh
blockMesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh
cp -r init_0 0
time=0
oldCase=../linGradU
cp -r $oldCase/$time/rMesh/polyMesh 0/rMesh
time=0
gmtFoam -time $time mesh -region rMesh
evince $time/mesh.pdf &

# Create initial conditions on the rMesh and plot
time=0
# Create Gaussian patches of voriticty
setGaussians initDict -region rMesh
# Invert to find the wind field
invertVorticity -time $time initDict -region rMesh
gmtFoam -time $time vorticity -region rMesh
evince $time/vorticity.pdf &

# Calculate the height in balance and plot
setBalancedHeight -region rMesh
gmtFoam -time $time hU -region rMesh
evince $time/hU.pdf &

# Solve the SWE
movingshallowWaterFoamH -fixedMesh >& log & sleep 0.01; tail -f log

# Post process
time=700000
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
eps2gif vorticity.gif 0/vorticity.pdf ??????/vorticity.pdf \
        ???????/vorticity.pdf

