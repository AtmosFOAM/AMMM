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
gmtFoam -time $time meshunder -case ../advectionOT
gmtFoam -time $time mountainOver
cat ../advectionOT/0/meshunder.ps 0/mountainOver.ps > initial_mesh.ps
ps2pdf initial_mesh.ps
pdfCrop initial_mesh.pdf
gv initial_mesh.pdf &
cp initial_mesh.pdf ~/Dropbox/HiroeHilaryPhilPaperFigures
