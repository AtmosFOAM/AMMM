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
rm -rf [0-9]*
cp -r init_0 0
time=0
# Create Gaussian patches of voriticty
setGaussians initDict
# Invert to find the wind field
invertVorticity -time $time initDict
gmtFoam -time $time vorticity
gv $time/vorticity.pdf &

# Solve the incompressibe Euler equations
icoOTFoam >& log & sleep 0.01; tail -f log

# Post process
time=700000
postProcess -func vorticity -time $time
writeuvw -time $time vorticity
mv $time/vorticityz $time/vorticity
rm $time/vorticity?
gmtFoam -time $time vorticity
gv $time/vorticity.pdf &

# Only re-calcualte and re-plot recent times
time=1600000
postProcess -func vorticity -time $time':'
writeuvw vorticity -time $time':'
for time in [0-9]*; do
    mv $time/vorticityz $time/vorticity
    rm $time/vorticity?
done
gmtFoam vorticity -time $time':'

# Animation of vorticity
postProcess -func vorticity
writeuvw vorticity
for time in [0-9]*; do
    mv $time/vorticityz $time/vorticity
    rm $time/vorticity?
done
gmtFoam vorticity
eps2gif vorticity.gif 0/vorticity.pdf ??????/vorticity.pdf ???????/vorticity.pdf

# Make links for animategraphics
field=vorticity
DT=100000
mkdir -p animategraphics
for time in [0-9]*; do
    let t=$time/$DT
    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
done

