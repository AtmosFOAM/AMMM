#!/bin/bash -e
#
set -o pipefail

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh core log legends gmt.history

cp -r init_0 0

# Create initial mesh adapted based on mag(grad(U))
blockMesh
setGaussians initDict
invertVorticity -time 0 initDict
# Create monitor function from grad(Uf)
postProcess -field Uf -func monitorFromGradU -time 0
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh
MAmesh_AFP | grep --invert-match AMI

# Create initial conditions on the rMesh
# First initialise vorticity as 2 Gaussian blobs on the pMesh
setGaussians -region rMesh initDict
invertVorticity -time 0 -region rMesh initDict
gmtFoam -time 0 -region rMesh streamFunctionVorticity
gv 0/streamFunctionVorticity.pdf &

# Solve incompressible Euler equations on a moving mesh
movingIcoFoamH >& log &
sleep 0.1
tail -f log

# What to plot and when
time=5000
field=vorticity2D

# Calculate the vorticity
postProcess -field Uf -func $field -time $time -region rMesh

# Plot field $field at time $time
gmtFoam -time $time $field -region rMesh
gv $time/$field.pdf &

# Plots for all time
postProcess -field Uf -func $field -region rMesh
gmtFoam $field -region rMesh
pdfjam --outfile $field.pdf --landscape 0/$field.pdf ???/$field.pdf ????/$field.pdf
evince $field.pdf &

