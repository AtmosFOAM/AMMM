#!/bin/bash -ve

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/polyMesh constant/*/polyMesh core log* legends gmt* *.dat

# Create initial mesh
blockMesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh

# Createt initial conditions on the rMesh and plot h and the mesh
cp -r init0 0
setBalancedHeight -region rMesh
gmtFoam -time 0 -region rMesh hMesh
evince 0/hMesh.pdf &

# Solve the shallow water equations
movingshallowWaterFoamH -fixedMesh >& log & sleep 0.01; tail -f log

# Difference between solutions
time=500
sumFields $time UDiff $time Uf 0 Uf -scale1 -1 -region rMesh
sumFields $time hDiff  $time h  0 h  -scale1 -1 -region rMesh
gmtFoam -time $time -region rMesh hUDiff
evince $time/hUDiff.pdf &
