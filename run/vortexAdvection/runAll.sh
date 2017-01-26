#!/bin/bash -e
#
set -o pipefail

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh core log legends gmt.history

cp -r init_0 0

# Create initial mesh adapted based on Gaussians defined in initDict
blockMesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh
MAmesh_AFP -dict initDict
cp 0/meshPhi 0/rMesh
#gmtFoam -region rMesh mesh
#gv 0/mesh.pdf &

# Create initial conditions on the rMesh
# First initialise vorticity as 2 Gaussian blobs on the pMesh
setGaussians -region rMesh initDict
# Invert the vorticity to find the wind
invertVorticity -time 0 -region rMesh initDict
gmtFoam -time 0 -region rMesh vorticity
gv 0/vorticity.pdf &

## check
#postProcess -field Uf -func rMesh/vorticity -region rMesh -time 0
#gmtFoam -time 0 -region rMesh vorticity
#gv 0/vorticity.pdf &


# Solve incompressible Euler equations on a moving mesh
#movingIcoFoamH >& log & sleep 0.01; tail -f log
movingIcoFoamH |&  tee log

# What to plot and when
time=100
field=vorticity

# Plot field $field at time $time
gmtFoam -time $time $field -region rMesh
gv $time/$field.pdf &

# Plots for all time
gmtFoam $field -region rMesh
#pdfjam --outfile $field.pdf --landscape 0/$field.pdf ???/$field.pdf ????/$field.pdf
eps2gif $field.gif ?/$field.pdf ??/$field.pdf ???/$field.pdf

