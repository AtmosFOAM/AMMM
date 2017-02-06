#!/bin/bash -e
#
set -o pipefail

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh core log legends gmt.history

cp -r init_0 0

# Create initial mesh
blockMesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh

# Createt initial conditions on the rMesh
setGaussians initDict -region rMesh
invertVorticity -time 0 initDict -region rMesh
gmtFoam -time 0 vorticity -region rMesh
evince 0/vorticity.pdf &

cp 0/meshPhi 0/rMesh

# Solve incompressible Euler equations on a moving mesh
movingIcoFoamH -fixedMesh >& log & sleep 0.01; tail -f log
#movingIcoFoamH |&  tee log

# What to plot and when
# Approximate time for one complete revolution
time=3150

# Plot field vorticity at time $time
gmtFoam -time $time vorticity -region rMesh
gv $time/vorticity.pdf &

# Plots for all time
gmtFoam vorticity -region rMesh
#pdfjam --outfile vorticity.pdf --landscape 0/vorticity.pdf ???/vorticity.pdf ????/vorticity.pdf
eps2gif vorticity.gif ?/vorticity.pdf ??/vorticity.pdf ???/vorticity.pdf ????/vorticity.pdf

