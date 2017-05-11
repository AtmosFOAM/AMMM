#!/bin/bash -e

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh core log legends gmt.history

# Create initial mesh
blockMesh
mkdir -p constant/cMesh
cp -r constant/polyMesh constant/cMesh
ln -sf ../system/dynamicMeshDict constant/dynamicMeshDict

# Create initial conditions
rm -rf [0-9]* core
cp -r init_0 0
time=0
# Create Gaussian patches of voriticty
setGaussians initDict
# Invert to find the wind field
invertVorticity -time $time initDict

gmtFoam -time $time vorticityMesh
ev $time/vorticityMesh.pdf

# Calculate the height in balance and plot
setBalancedHeightRC
gmtFoam -time $time hUmesh
gv $time/hUmesh.pdf &

# Solve the SWE
shallowWaterOTFoam -fixedMesh >& log & sleep 0.01; tail -f log

# Post process
time=700000
gmtFoam -time $time hU
gv $time/hU.pdf &

time=100000
case=.
postProcess -func vorticity -time $time -case $case
writeuvw -time $time vorticity -case $case
mv $case/$time/vorticityz $case/$time/vorticity
rm $case/$time/vorticity?
gmtFoam -time $time vorticity -case $case
gv $case/$time/vorticity.pdf &

# Animation of vorticity
case=.
postProcess -func vorticity -case $case
writeuvw vorticity -case $case
for time in [0-9]*; do
    mv $case/$time/vorticityz $case/$time/vorticity
    rm $case/$time/vorticity?
done
gmtFoam vorticity -case $case
eps2gif vorticity.gif 0/vorticity.pdf ??????/vorticity.pdf \
        ???????/vorticity.pdf

# Make links for animategraphics
field=vorticity
DT=100000
mkdir -p animategraphics
for time in [0-9]*; do
    let t=$time/$DT
    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
done

