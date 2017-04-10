#!/bin/bash -ve

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh core log legends gmt.history

# Create initial mesh
blockMesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh

# Create initial conditions on the rMesh and plot
time=0
cp -r init_0 0

# Solve Monge-Ampere
AFP

# plot new mesh
gmtFoam -region rMesh -latestTime mesh
evince [1-9]*/mesh.pdf &

