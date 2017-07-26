#!/bin/bash -e

# Remove old stuff
rm -rf [0-9]* constant/polyMesh

# Generate the mesh
blockMesh

# Calculate the initial conditions and the fluxes before imposing the terrain
cp -r init0 0
solidBodyRotationOnPlaneSetup

# Put in a mountain
terrainFollowingMesh

# Write out cell volumes for post-processing
time=0
postProcess -func writeCellVolumes -time $time
gmtFoam -time $time Tunder
gmtFoam -time $time mountainOver
cat $time/Tunder.ps $time/mountainOver.ps > $time/Tmountain.ps
ps2pdf $time/Tmountain.ps
mv Tmountain.pdf $time/Tmountain.pdf
#rm $time/*.ps
evince $time/Tmountain.pdf &

rm 0/U 0/Uf

# Run
scalarTransportFoam >& log & sleep 0.001; tail -f log
